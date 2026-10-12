/* $Id: telnetd.c,v 1.16 2000/03/06 10:39:44 johana Exp $
 *
 * Simple telnet server
 * Bjorn Wesen, Axis Communications AB (bjornw@axis.com) 
 *
 * This file is distributed under the Gnu Public License (GPL),
 * please see the file LICENSE for further information.
 * 
 * ---------------------------------------------------------------------------
 * (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
 ****************************************************************************
 *
 * The telnetd manpage says it all:
 *
 *   Telnetd operates by allocating a pseudo-terminal device (see pty(4))  for
 *   a client, then creating a login process which has the slave side of the
 *   pseudo-terminal as stdin, stdout, and stderr. Telnetd manipulates the
 *   master side of the pseudo-terminal, implementing the telnet protocol and
 *   passing characters between the remote client and the login process.
 */

/////////////////////////////////////////////////////////////////////////////////////
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//
// When client request connection, it load CLIAgent Program
// The maximum number of connection is 3(ptya0,ptya1,ptya2)
//
/////////////////////////////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <arpa/telnet.h>
#include <ctype.h>

#define BUFSIZE 4000

/* what shell should we run */
#ifndef SHELLPATH
#define SHELLPATH AGENT_DIR"/CLIAgent"
#endif

/* shell to use if the defined shell does not exist */
#define BACKUPSHELL AGENT_DIR"/CLIAgent"

#define RWDEBUG(x)
#define D(x)

static char shellpath[32] = SHELLPATH;

/* environment we give to each shell */

const char *envp_init[] = { "HOME=/", "TERM=vt100", "LD_LIBRARY_PATH=/usr/lib:/lib:/usr/local/lib",
					"PATH=/usr/bin:/usr/sbin:/bin:/sbin:/mnt:/usr/local/sbin:/usr/local/bin",
					"TZ=KST-9",
				   NULL, };

/* shell name and arguments */

const char *argv_init[] = { "bash", "1", NULL };

/* structure that describes a session */

struct tsession {
	struct tsession *next;
	int sockfd, ptyfd;
	int shell_pid;
	/* two circular buffers */
	char *buf1, *buf2;
	int rdidx1, wridx1, size1;
	int rdidx2, wridx2, size2;
};

/*

   This is how the buffers are used. The arrows indicate the movement
   of data.

   +-------+     wridx1++     +------+     rdidx1++     +----------+
   |       | <--------------  | buf1 | <--------------  |          |
   |       |     size1--      +------+     size1++      |          |
   |  pty  |                                            |  socket  |
   |       |     rdidx2++     +------+     wridx2++     |          |
   |       |  --------------> | buf2 |  --------------> |          |
   +-------+     size2++      +------+     size2--      +----------+

   Each session has got two buffers.

*/

static int maxfd;

#define MIN(a,b) ((a) > (b) ? (b) : (a))

static struct tsession *sessions;
static void
send_do(struct tsession *ts, int option)
{
	/* We rely on that there is space in the buffer for now.  */
	char *b = ts->buf2 + ts->rdidx2;
	*b++ = IAC;
	*b++ = DO;
	*b++ = option;
	ts->rdidx2 += 3;
	ts->size2 += 3;
}

static void
send_will(struct tsession *ts, int option)
{
	/* We rely on that there is space in the buffer for now.  */
	char *b = ts->buf2 + ts->rdidx2;
	*b++ = IAC;
	*b++ = WILL;
	*b++ = option;
	ts->rdidx2 += 3;
	ts->size2 += 3;
}

static struct tsession *
make_new_session(int sockfd)
{
	struct termios termbuf;
	int pty, pid;
	static char tty_name[11] = "/dev/pty";
	struct tsession *ts = (struct tsession *)malloc(sizeof(struct tsession));

	ts->buf1 = (char *)malloc(BUFSIZE);
	ts->buf2 = (char *)malloc(BUFSIZE);

	ts->sockfd = sockfd;

	ts->rdidx1 = ts->wridx1 = ts->size1 = 0;
	ts->rdidx2 = ts->wridx2 = ts->size2 = 0;

	/* Got a new connection, set up a tty and spawn a shell.  */
	if (pty > maxfd)
		maxfd = pty;

	ts->ptyfd = pty;

	/* Make the telnet client understand we will echo characters so it 
	 * should not do it locally. We don't tell the client to run linemode,
	 * because we want to handle line editing and tab completion and other
	 * stuff that requires char-by-char support.
	 */

	send_do(ts, TELOPT_ECHO);
	send_do(ts, TELOPT_LFLOW);
	send_will(ts, TELOPT_ECHO);
	send_will(ts, TELOPT_SGA);

	D(printf("Making new session, sockfd %d, ptyfd %d.\n", sockfd, pty); fflush(stdout));

	/* fork off a child. use vfork in elinux, it will suspend the parent until the child
	 * does the execve. this is good, because the compiler caches pointers to things on the
	 * stack (which moves after fork...) and breaks in tty_name below otherwise.
	 */

#ifndef ELINUX
	if ((pid = fork()) < 0) {
#else
	if ((pid = vfork()) < 0) {
#endif
		perror("fork");
	}
	
	D(printf("tty = %s \n", tty_name));
	
	if (pid == 0) {
		/* In child, open the child's side of the tty.  */
		int i;
		int t = open(tty_name, O_RDWR | O_NOCTTY);
		if (t < 0) {
			perror("Could not open tty");
			exit(-1);
		}

		/* The pseudo-terminal allocated to the client is configured to operate in
		 * cooked mode, and with XTABS CRMOD enabled (see tty(4)).
		 */

		tcgetattr(t, &termbuf);
		termbuf.c_oflag |= ONLCR|XTABS;
		termbuf.c_iflag |= ICRNL;
		termbuf.c_iflag &= ~IXOFF;
		termbuf.c_lflag &= ~ECHO;
		tcsetattr(t, TCSANOW, &termbuf);

		if (setsid() < 0) {
			perror("setsid"); /* make new process group */
			exit(-1);
		}

		/* Make sure it is on descriptors 0, 1 and 2 */

		if (t != 0) dup2(t, 0);
		if (t != 1) dup2(t, 1);
		if (t != 2) dup2(t, 2);

		/* for some reason, we have to close open descriptors here. when
		 * we're killed they are supposed to be closed but...
		 */
		for(i = 3; i <= maxfd; i++)
			close(i);

		/* exec shell, with correct argv and env */

		execve(shellpath, (char* const*)argv_init, (char* const*)envp_init);

		/* NOT REACHED */
		perror("execl");

		_exit(0);
	}

	ts->shell_pid = pid;

	D(printf("Parent: spawned shell as pid %d.\n", pid); fflush(stdout));

	return ts;
}

static void
free_session(struct tsession *ts)
{
	struct tsession *t = sessions;

	/* Unlink this telnet session from the session list.  */
	if(t == ts)
		sessions = ts->next;
	else {
		while(t->next != ts)
			t = t->next;
		t->next = ts->next;
	}

	free(ts->buf1);
	free(ts->buf2);

	kill(ts->shell_pid, SIGKILL);

	D(printf("Freeing session ptyfd %d, sockfd %d, waiting for shell %d.\n",
		 ts->ptyfd, ts->sockfd, ts->shell_pid));
	
	wait4(ts->shell_pid, NULL, 0, NULL);

	D(printf("Done waiting.\n"));

	close(ts->ptyfd);
	close(ts->sockfd);

	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;
	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;

	free(ts);
}

int main(int argc, char **argv)
{
	struct sockaddr_in sa;
	int master_fd;
	fd_set rdfdset, wrfdset;
	int selret, salen;
	int on = 1;
	int portnbr = 23;

	/* check if user supplied a port number */

	if(argc == 2)
		portnbr = atoi(argv[1]);

	D(printf("Telnetd starting, using port %d.\n", portnbr)); fflush(stdout);

	/* check which shell is available */

	if(access(shellpath, X_OK) < 0) {
		if(access(BACKUPSHELL, X_OK) < 0) {
			printf("Shell unavailable. Giving up.\n");
			exit(0);
		}
		strcpy(shellpath, BACKUPSHELL);
	}
	D(printf("  using %s as shell.\n", shellpath));
			
	sessions = 0;

	/* Grab a TCP socket.  */

	master_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (master_fd < 0) {
		perror("socket");
		return 1;
	}
	(void)setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	/* Set it to listen to specified port.  */

	memset((void *)&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(portnbr);

	if (bind(master_fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("bind");
		return 1;
	}

	if (listen(master_fd, 1) < 0) {
		perror("listen");
		return 1;
	}

	maxfd = master_fd;

	do {
		struct tsession *ts;

		FD_ZERO(&rdfdset);
		FD_ZERO(&wrfdset);

		/* select on the master socket, all telnet sockets and their
		 * ptys if there is room in their respective session buffers.
		 */

		FD_SET(master_fd, &rdfdset);

		ts = sessions;
		while (ts) {
			/* buf1 is used from socket to pty
			 * buf2 is used from pty to socket
			 */
			if (ts->size1 > 0) {
				FD_SET(ts->ptyfd, &wrfdset);  /* can write to pty */
			}
			if (ts->size1 < BUFSIZE) {
				FD_SET(ts->sockfd, &rdfdset); /* can read from socket */
			}
			if (ts->size2 > 0) {
				FD_SET(ts->sockfd, &wrfdset); /* can write to socket */
			}
			if (ts->size2 < BUFSIZE) {
				FD_SET(ts->ptyfd, &rdfdset);  /* can read from pty */
			}
			ts = ts->next;
		}

		selret = select(maxfd + 1, &rdfdset, &wrfdset, 0, 0);

		if (!selret)
			break;

		/* First check for and accept new sessions.  */
		if (FD_ISSET(master_fd, &rdfdset)) {
			int fd;

			if ((fd = accept(master_fd, (struct sockaddr *)&sa, (socklen_t *)&salen)) < 0) {
				continue;
			} else {
				/* Create a new session and link it into
				   our active list.  */
				struct tsession *ts = make_new_session(fd);
				if (ts) {
					ts->next = sessions;
					sessions = ts;
					if (fd > maxfd)
						maxfd = fd;
				} else {
					D(printf("Closing fd.\n"); fflush(stdout));
					close(fd);
				}
			}
		}

		/* Then check for data tunneling.  */

		ts = sessions;
		while (ts) { /* For all sessions...  */
			int maxlen, w, r;
			struct tsession *next = ts->next; /* in case we free ts. */

			if (ts->size1 && FD_ISSET(ts->ptyfd, &wrfdset)) {
				/* Write to pty from buffer 1.  */
				maxlen = MIN(BUFSIZE - ts->wridx1,
					     ts->size1);
				w = write(ts->ptyfd, ts->buf1 + ts->wridx1, maxlen);
				if (w < 0) {
					perror("write");
					D(printf("main(): write() to pty failed!\n"); fflush(stdout));
					free_session(ts);
					ts = next;
					continue;
				}
				RWDEBUG(printf("Wrote %d bytes to pty:\n", w); fflush(stdout));
				ts->wridx1 += w;
				ts->size1 -= w;
				if (ts->wridx1 == BUFSIZE)
					ts->wridx1 = 0;
			}

			if (ts->size2 && FD_ISSET(ts->sockfd, &wrfdset)) {
				/* Write to socket from buffer 2.  */
				maxlen = MIN(BUFSIZE - ts->wridx2,
					     ts->size2);
				w = write(ts->sockfd, ts->buf2 + ts->wridx2, maxlen);
				if (w < 0) {
					perror("write");
					D(printf("main(): write() to socket failed!\n"); fflush(stdout));
					free_session(ts);
					ts = next;
					continue;
				}
				RWDEBUG(printf("Wrote %d bytes to socket:\n", w); fflush(stdout));
				ts->wridx2 += w;
				ts->size2 -= w;
				if (ts->wridx2 == BUFSIZE)
					ts->wridx2 = 0;
			}

			if (ts->size1 < BUFSIZE && FD_ISSET(ts->sockfd, &rdfdset)) {
				/* Read from socket to buffer 1. */
				maxlen = MIN(BUFSIZE - ts->rdidx1,
					     BUFSIZE - ts->size1);
				r = read(ts->sockfd, ts->buf1 + ts->rdidx1, maxlen);
				if (!r || (r < 0 && errno != EINTR)) {
					D(perror("read"));
					D(printf("main(): read() from socket failed!\n"); fflush(stdout));
					free_session(ts);
					ts = next;
					continue;
				}
				if(!*(ts->buf1 + ts->rdidx1 + r - 1)) {
					r--;
					if(!r)
						continue;
				}
				RWDEBUG(printf("Read %d bytes from socket:\n", r); fflush(stdout));
				ts->rdidx1 += r;
				ts->size1 += r;
				if (ts->rdidx1 == BUFSIZE)
					ts->rdidx1 = 0;
			}

			if (ts->size2 < BUFSIZE && FD_ISSET(ts->ptyfd, &rdfdset)) {
				/* Read from pty to buffer 2.  */
				maxlen = MIN(BUFSIZE - ts->rdidx2,
					     BUFSIZE - ts->size2);
				r = read(ts->ptyfd, ts->buf2 + ts->rdidx2, maxlen);
				if (!r || (r < 0 && errno != EINTR)) {
					D(perror("read"));
					D(printf("main(): read() from pty failed!\n"); fflush(stdout));
					free_session(ts);
					ts = next;
					continue;
				}
				RWDEBUG(printf("Read %d bytes from pty:\n", r); fflush(stdout));
				ts->rdidx2 += r;
				ts->size2 += r;
				if (ts->rdidx2 == BUFSIZE)
					ts->rdidx2 = 0;
			}

			if (ts->size1 == 0) {
				ts->rdidx1 = 0;
				ts->wridx1 = 0;
			}
			if (ts->size2 == 0) {
				ts->rdidx2 = 0;
				ts->wridx2 = 0;
			}
			ts = next;
		}

	} while (1);

	return 0;
}
