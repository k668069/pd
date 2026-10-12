/* vFTPd (Virtual FTP daemon)
 *
 * Written by Bjorn Wesen
 * Copyright (C) 1999, 2000, 2001, 2002 Axis Communications AB
 *
 * This file is distributed under the Gnu Public License (GPL),
 * please see the file LICENSE for further information.
 *
 *
 *
 * Required commands:
 *
 * USER <username>
 * PASS <password>
 * CWD <new working directory>
 * CDUP (cd ..)
 * QUIT
 * PORT h1,h2,h3,h4,p1,p2  (host, port)
 * PASV
 * RETR
 * REST
 * STOR
 * DELE
 * RMD (remove directory)
 * MKD (make directory)
 * PWD
 * LIST <dir>
 * NOOP
 * TYPE binary/ascii
 * RNFR (rename from file)
 * RNTO (rename to file)
 *
 * Responses:
 * 220 hostname FTP server ready.
 * 221 Goodbye.
 * 331 Password required for user.
 * 350 File exists, ready for destination name
 * 230 User xxx logged in.
 * 250 Command successful
 * 257 Informative response
 * 502 Command not implemented.
 * 530 Incorrect login
 * 1yz   Positive Preliminary reply
 * 2yz   Positive Completion reply
 * 3yz   Positive Intermediate reply
 * 4yz   Transient Negative Completion reply
 * 5yz   Permanent Negative Completion reply
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include <crypt.h>
#include <fcntl.h>

#include "vftpd.h"
#include "virtual_target.h"
#include "ls.h"

#define DEBUG(x)

/* FTP commands */
const char *c_user  = "USER";
const char *c_quit  = "QUIT";
const char *c_pass  = "PASS";
const char *c_syst  = "SYST";
const char *c_port  = "PORT";
const char *c_pasv  = "PASV";
const char *c_list  = "LIST";
const char *c_nlst  = "NLST";
const char *c_retr  = "RETR";
const char *c_rest = "REST";
const char *c_stor  = "STOR";
const char *c_type  = "TYPE";
const char *c_mkd   = "MKD";
const char *c_rmd   = "RMD";
const char *c_dele  = "DELE";
const char *c_pwd   = "PWD";
const char *c_cwd   = "CWD";
const char *c_site  = "SITE";
const char *c_cdup  = "CDUP";
const char *c_rnfr  = "RNFR";
const char *c_rnto  = "RNTO";
const char *c_chmod = "CHMOD";
const char *c_reboot = "REBOOT";

/* FTP response codes */
const char *r_openingdata  = "150 Opening data connection.\r\n";
const char *r_hello        = "220 Simple FTPd welcomes you.\r\n";
const char *r_goodbye      = "221 Goodbye.\r\n";
const char *r_loggedin     = "230 User logged in, proceed.\r\n";
const char *r_virtexec     = "214-Virtual target execution.\r\n";
const char *r_virtexit     = "214 Virtual target exit.\r\n";
const char *r_system       = "215 UNIX Type: L8\r\n";
const char *r_success      = "250 Command successful.\r\n";
const char *r_okay         = "200 Command okay.\r\n";
const char *r_trcomplete   = "226 Transfer complete.\r\n";
const char *r_fileexists   = "350 File exists, ready for destination name.\r\n";
const char *r_userok       = "331 User name okay, need password.\r\n";
const char *r_cantopen     = "425 Cannot open data connection.\r\n";
const char *r_error        = "500 Syntax error.\r\n";
const char *r_badseq       = "503 Bad sequence of commands.\r\n";
const char *r_notimpl      = "502 Command not implemented.\r\n";
const char *r_nosuchfile   = "550 No such file or directory.\r\n";
const char *r_noaccess     = "550 Access denied.\r\n";
const char *r_notadir      = "550 Not a directory.\r\n";
const char *r_outofspace   = "550 Device out of space.\r\n";
const char *r_outofmem     = "550 Out of memory.\r\n";
const char *r_unknownerror = "550 Unknown error.\r\n";
const char *r_badlogin     = "530 Login incorrect.\r\n";
const char *r_use_i_mode   = "550 Must use 'binary' mode (TYPE I).\r\n";
const char *r_unknown_mode = "550 Unknown mode.\r\n";
const char *r_bad_file     = "550 Bad file.\r\n";

/* only root can login if set by command option -r */
static int root_login_only = 0;

/* default daemon mode, turn off by command option -n */
static int daemon_mode = 1;

/* binary (I) or ascii (A) */
static char data_format;

/* The port used for the control channel. Port ctrl_port-1 is used for the
   data channel */
static int ctrl_port;

/* contains the welcome banner shown each time the ftp daemon forks */
static char banner[BANNER_LEN];

/* return 0 if signal initialisation succeeds or -1 */
static int signals_init(void);

/* handle process termination, iterate and close file descriptors */
static void sigchld(int dummy);

/* abort on fatal error */
static void sigabort(int signo);

/* may want to wait for transfers in progress */
static void sigterm(int dummy);

/* step beyond whitespace in the beginning of a string and return
   first non-whitespace character or \0 */
static char *gobble_space(char *c);

/* change access permissions to what is hidden in args, 
   e.g. site chmod 775 foo */
static void do_site(int fd, char *args);

/* change current working directory to dirname without checking access
   permissions */
static void do_chdir(int fd, const char *dirname);

/* handle a ftp session where the fd is the established socket
   identified by the remote_addr */
static void ftpd_main(int fd, struct sockaddr_in *remote_addr);

/* return the default banner r_hello if the banner file does not exist */
char *set_banner(const char *r_hello);

/* read a command from the client until it ends with a newline
   if an error occurs, return -1 */
static int get_command(int fd, char *buf);

/* check if login is correct; if so return 0, uid and gid.
   on failure return -1 */
static int check_login(char* user, char* pass, uid_t *uid, gid_t *gid);

/* send an error response to the client on an open control connection */
static void send_error_resp(int fd);

/* list files in directory dir based on the content in simple */
static void do_list(const char *dir, int fd, struct s_client *client, int simple);

/* validate the client IP address and store it with the client port in
   struct s_client from the string command. Respond with okay or
   unknown error to the client */
static void handle_port_cmd(int fd, const char *command, struct s_client *client);

/* opens a passive data connection on file descriptor fd
   and listens. Returns the file descriptor or -1 on fatal
   error. */
static int open_passive_data_connection(void);

/* open a data connection to the client and return the file
   descriptor fd or -1 on fatal error */
static int open_data_connection(struct s_client *client);

/* write to control connection fd when the command buf of length len
   is not a trivial command listed */
int write_to_fd(int fd, const char *buf, int len);

/* opens a passive data connection and ignores the return value,
   closes the previously opened client connection and replaces it with
   the passive one. Respond with entering passive mode or unknown error. */
static void handle_pasv_cmd(int fd, struct s_client *client);

/* open fname as a virtual or regular file and send local file content
   to the client in binary or ASCII format. Response codes are sent
   and it returns or exits on fatal errors */
static void do_retr(char *fname, int fd, struct s_client *client, int nOffset);

static void do_rest(int fd, int nOffset);

/* open fname as a virtual or regular file and receive local file content
   from the client in binary or ASCII format. Response codes are sent
   and it returns or exits on fatal errors */
static void do_stor(char *fname, int fd, struct s_client *client);

void SysLog(int nLog, char *strFormat, ...)
{
	char strData[1024];
	va_list ap;
	int nLength;
	FILE *fp;
	int nDisplay;

	if((fp = fopen("/var/ftplog.dat", "rb")) == NULL) return;
	if(fread(&nDisplay, sizeof(int), 1, fp) != 1){
		fclose(fp);
		return;
	}
	fclose(fp);
	if(!nDisplay) return;
	
	va_start(ap, strFormat);
	vsprintf(strData, strFormat, ap);
	va_end(ap);
	
	nLength = strlen(strData);
	while(strData[nLength-1] == '\n' || strData[nLength-1] == '\r'){
		strData[nLength-1] = 0;
		nLength--;
	}
	syslog(nLog, strData);
}

/* setup the master listening socket, wait for connections and invoke
   ftpd_main in a child whenever a connection comes. */
int
main(int argc, char **argv)
{
	const char opt[] = "rdn";
	int opt_c;
	int dont_close_fd = 0;
	int ctrlfd;
	struct sockaddr_in remote_addr;
	int remote_addrlen;
	fd_set rfds;
	int true = 1;
	int syslog_option = LOG_PID;

	/* parse the command line options */
	for (;;) {
		opt_c = getopt(argc, argv, opt);
		/* last argument may be optional port number historically */
		if (opt_c == -1) {
			if (optind < argc) {
				ctrl_port = atoi(argv[argc]);
			}
			break;
		}
		/* any known arguments */
		switch (opt_c) {
		case 'r':
			root_login_only = true;
			break;
		case 'd':
			syslog_option |= LOG_PERROR;
			dont_close_fd = -1;
			break;
		case 'n':
			daemon_mode = 0;
			break;
		default:
			break;
		}
	}

	/* generic user level messages prefixed by name and pid */
	openlog(PROGRAM_NAME, syslog_option, LOG_FTP);
	SysLog(LOG_INFO, "Starting vftpd 0.01");

	/* execute as daemon */
	DEBUG(dont_close_fd = -1);
	if (daemon_mode == true) {
		if (daemon(0, dont_close_fd) < 0) {
			SysLog(LOG_EMERG, "Failed: daemon. %m");
			exit(EXIT_FAILURE);
		}
	}

	if (signals_init() < 0) {
		SysLog(LOG_EMERG, "Failed: signals_init.");
		exit(EXIT_FAILURE);
	}

	/* open a TCP socket and set it to reusable */
	ctrlfd = socket(AF_INET, SOCK_STREAM, 0);
	if (ctrlfd < 0) {
		SysLog(LOG_ERR, "Unable to get a TCP socket.");
		exit(1);
	}
	if (setsockopt(ctrlfd, SOL_SOCKET, SO_REUSEADDR, (void *)&true, 
		       sizeof(true)) < 0) {
		SysLog(LOG_EMERG, "Error setsockopt. %m");
		exit(1);
	}

	/* listen to the chosen ftp port and accept one connection only */
	if (ctrl_port <= 0 || ctrl_port >= 65536) {
		ctrl_port = IPPORT_FTP;
	}

	memset((void *)&remote_addr, 0, sizeof(struct sockaddr_in));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(ctrl_port);
	if (bind(ctrlfd, (struct sockaddr *)&remote_addr,
		sizeof(struct sockaddr_in)) < 0) {
		SysLog(LOG_EMERG, "Unable to bind addr to socket (- bind() failed). %m");
		exit(1);
	}
	if (listen(ctrlfd, 1) < 0) {
		SysLog(LOG_EMERG, "Failed to get socket to listen to port (- listen() failed). %m");
		exit(1);
	}

	for (;;) {
		FD_ZERO(&rfds);
		FD_SET(ctrlfd, &rfds);
		/* wait for any connection without timeout */
		if (select(ctrlfd + 1, &rfds, 0, 0, 0) == 0) {
			break;
		}
		/* accept the incoming connection and fork  */
		if (FD_ISSET(ctrlfd, &rfds)) {
			int fd;

			remote_addrlen = sizeof(struct sockaddr_in);
			if ((fd = accept(ctrlfd,
					 (struct sockaddr *)&remote_addr,
					 &remote_addrlen)) < 0) {
				continue;
			} else {
				int pid;

				DEBUG(fprintf(stderr, "accept from %s\n",
					      inet_ntoa(remote_addr.sin_addr)));
				pid = fork();
				if (pid < 0) {
					SysLog(LOG_EMERG, "Failed to fork. %m");
				} else if (pid == 0) {
					/* in child */
					ftpd_main(fd, &remote_addr);
					/* Exit, this will cause a SIGCHLD to 
					   the parent and we return the fd as 
					   status code so it knows which.  */
					exit(fd);
				}
				/* in parent */
			}
		}
	}

	close(ctrlfd);
	return 0;
}

/* return 0 if signal initialisation succeeds or -1 */
static int
signals_init(void)
{
	struct sigaction sa;

	sa.sa_flags = 0;
	if (sigemptyset(&sa.sa_mask) < 0) {
		SysLog(LOG_EMERG, "Error: sigemptyset %m");
		return -1;
	}
	if (sigaddset(&sa.sa_mask, SIGCHLD) < 0
	    || sigaddset(&sa.sa_mask, SIGTERM) < 0
	    || sigaddset(&sa.sa_mask, SIGSEGV) < 0
	    || sigaddset(&sa.sa_mask, SIGBUS) < 0
	    || sigaddset(&sa.sa_mask, SIGINT) < 0) {
		SysLog(LOG_EMERG, "Error: sigaddset %m");
		return -1;
	}

	sa.sa_handler = sigchld;
	if (sigaction(SIGCHLD, &sa, NULL) < 0) {
		SysLog(LOG_EMERG, "Error: sigaction %m");
		return -1;
	}

	sa.sa_handler = sigterm;
	if (sigaction(SIGTERM, &sa, NULL) < 0) {
		SysLog(LOG_EMERG, "Error: sigaction %m");
		return -1;
	}

	sa.sa_handler = sigabort;
	if (sigaction(SIGSEGV, &sa, NULL) < 0
	    || sigaction(SIGBUS, &sa, NULL) < 0
	    || sigaction(SIGINT, &sa, NULL) < 0) {
		SysLog(LOG_EMERG, "Error: sigaction %m");
		return -1;
	}

	return 0;
}

/* handle process termination, iterate and close file descriptors */
static void
sigchld(int dummy)
{
	int status;
	pid_t pid;

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		DEBUG(fprintf(stderr, "reaping child %d: status %d\n",
			      pid, status));
		/* close the socket - the child returns the fd with exit()
		 * since we also got some normal exit(0)'s, and we probably
		 * don't want to close stdin, we check for fd == 0 as well.
		 */
		if (WIFEXITED(status) && WEXITSTATUS(status)) {
			close(WEXITSTATUS(status));
		}
	}
}

/* abort on fatal error */
static void
sigabort(int signo)
{
	SysLog(LOG_EMERG, "Aborting on signal: %d", signo);
	abort();
}

/* may want to wait for transfers in progress */
static void
sigterm(int dummy)
{
	SysLog(LOG_INFO, "Exiting");
	exit(EXIT_SUCCESS);
}

/* step beyond whitespace in the beginning of a string and return
   first non-whitespace character or \0 */
static char *
gobble_space(char *c)
{
	while (*c && isspace(*c))
		c++;
	return c;
}

/* change access permissions to what is hidden in args, 
   e.g. site chmod 775 foo */
static void
do_site(int fd, char *args)
{
	if (IS_CMD(args, c_chmod)) {
		mode_t mode;
		char *filename;
		char buf[257];

		args += 5;
		mode = strtol(args, &args, 8);
		args = gobble_space(args);
		filename = args;

		if (getcwd(buf, 192) == NULL
		    || strlen(buf) + strlen(filename) + 1 > 256) {
			DO_RESP(fd, r_bad_file);
			return;
		}

		strcat(buf, "/");
		strcat(buf, filename);
		/* try to set mode for filename */
		if (chmod(filename, mode) < 0) {
			SysLog(LOG_ERR, "Failed to chmod %s: %s", filename,
			       strerror(errno));
			send_error_resp(fd);
		} else {
			DO_RESP(fd, r_okay);
		}
	}
	else if (IS_CMD(args, c_reboot)) {
		if (system("/sbin/init 6") < 0) {
			SysLog(LOG_ERR, "Failed to reboot: %m");
			send_error_resp(fd);
		} else {
			SysLog(LOG_INFO, "Rebooting.");
			DO_RESP(fd, r_okay);
		}
	} else {
		DO_RESP(fd, r_notimpl);
	}
}

/* change current working directory to dirname without checking access
   permissions */
static void
do_chdir(int fd, const char *dirname)
{
	if (chdir(dirname) < 0) {
		send_error_resp(fd);
		return;
	}
	DO_RESP(fd, r_success);
}


/* handle a ftp session where the fd is the established socket
   identified by the remote_addr */
static void
ftpd_main(int fd, struct sockaddr_in *remote_addr)
{
	uid_t uid;
	gid_t gid;
	char username[U_P_BUFLEN];
	char password[U_P_BUFLEN];
	int login;
	int state;
	struct s_client client;
	char command[MAX_CMD_SIZE + 1];
	char rnfrbuffer[MAX_CMD_SIZE + 1];
	int nOffset = 0;

	/* start by not being authenticated */
	uid = -1;
	gid = -1;

	/* expect authentication in the initial state */
	state = FST_AUTHENTICATE1;

	/* for remembering the Rename-From name */
	rnfrbuffer[0] = 0;

	client.port = remote_addr->sin_port;
	client.ip = remote_addr->sin_addr;
	client.fd = -1;

	username[0] = 0;
	SysLog(LOG_INFO, "Accepted request from %s", inet_ntoa(client.ip));
	DEBUG(fprintf(stderr, "request on fd %d\n", fd));

	/* default data format is binary */
	data_format = BINARY_MODE;

	/* read configurations  */
	set_virtual_targets();

	/* show the client what we are */
	DO_RESP(fd, set_banner(r_hello));
	for (;;) {
		if (get_command(fd, command) < 0) {
			/* if an error occured while reading the next
			 * command, this session should be purged
			 */
			break;
		}
		SysLog(LOG_INFO, "Command = %s", command);

		/* check for QUIT in all states  */
		if (IS_CMD(command, c_quit)) {
			break;
		}

		/* state machine with three states: not authenticated,
		 * entered username, and entered password (running) */
		switch (state) {
		case FST_AUTHENTICATE1:
			if (IS_CMD(command, c_user)
			    && HAS_PARAM(command, c_user)) {
				DEBUG(fprintf(stderr, "username %s\n", command + 5));
				/* copy and terminate user name */
				strncpy(username, command + 5, U_P_BUFLEN - 1);
				username[U_P_BUFLEN - 1] = 0;
				DO_RESP(fd, r_userok);
				state = FST_AUTHENTICATE2;
			}
			else {
				DO_RESP(fd, r_badseq);
			}
			break;

		case FST_AUTHENTICATE2:
			if (IS_CMD(command, c_pass)) {
			        /* reject all except root when restricted */
				if (root_login_only > 0) {
					if (strcmp(username, ROOT_USER)) {
						DO_RESP(fd, r_badlogin);
						state = FST_AUTHENTICATE1;
						SysLog(LOG_INFO, "User %s access denied.", username);
						break;
					}
				}

				/* copy and terminate password */
				strncpy( password, command + 5, U_P_BUFLEN - 1);
				password[U_P_BUFLEN - 1] = 0;
				DEBUG(fprintf(stderr, "password %s\n", password));
				login = check_login(username, password, &uid, &gid);
				if (login < 0) {
					DO_RESP(fd, r_badlogin);
					state = FST_AUTHENTICATE1;
					/* do not log the incorrect username,
					 * the user might have accidentally typed the password
					 * there instead
					 */
					SysLog(LOG_INFO, "Incorrect username/password.");
				}
				else {
					struct passwd *pw;
					struct stat mystat;

					DO_RESP(fd, r_loggedin);

					SysLog(LOG_INFO, "User %s logged in.", username);

					/* regenerate password for user flash in special
					   case SNMP handling of login */
					if ((pw = getpwuid(uid))
					    && strcmp(pw->pw_name, FLASH_USER) == 0
					    && stat(PROGRAM_FLASH_PASSWD, &mystat) >= 0) {
						system(PROGRAM_FLASH_PASSWD FLASH_USER);
					}
					
					/* authentication OK - switch to the chosen uid/gid */
					setegid(gid);
					seteuid(uid);

					state = FST_RUNNING;
				}
			}
			else {
				state = FST_AUTHENTICATE1;
				DO_RESP(fd, r_badseq);
			}
			break;

		case FST_RUNNING:
			if (IS_CMD(command, c_syst)) {
				/* client asks what kind of system we run on */
				DO_RESP(fd, r_system);
			}
			else if (IS_CMD(command, c_port)
				 && HAS_PARAM(command, c_port)) {
				/* get client IP address and port number */
				handle_port_cmd(fd, command, &client);
			}
			else if (IS_CMD(command, c_pasv)) {
				/* enter passive mode */
				handle_pasv_cmd(fd, &client);
			}
			else if (IS_CMD(command, c_list)
				 || IS_CMD(command, c_nlst)) {
				/* LIST/NLST same now */
				int len = strlen(command);

				if (len > 5 && command[5] != '-') {
					/* NLST -CF from ncftp
					   client specified a directory */
					char *c = command;
					c += 5;
					len -= 5;
					while (len && isspace(*c))
						c++;
					if (len) {
						do_list(c, fd, &client, IS_CMD(command, c_nlst));
					}
				}
				else {
					/* no dir specified, use present directory */
					do_list(".", fd, &client, IS_CMD(command, c_nlst));
				}
			}
			
			else if (IS_CMD(command, c_retr)
				 && HAS_PARAM(command, c_retr)) {
				/* client wants to retrieve a file */
				do_retr(gobble_space(command + 5), fd, &client, nOffset);
				SysLog(LOG_INFO, "RETR : FileName = %s, Offset = %d", gobble_space(command + 5), nOffset);
				nOffset = 0;
			}
			else if (IS_CMD(command, c_rest)
				 && HAS_PARAM(command, c_rest)) {
				/* client wants to retrieve a file */
				nOffset = atoi(gobble_space(command + 5));
				SysLog(LOG_INFO, "REST : Offset = %d", nOffset);
				do_rest(fd, nOffset);
			}			
			else if (IS_CMD(command, c_stor)
				 && HAS_PARAM(command, c_stor)) {
				/* client wants to store a file */
				do_stor(gobble_space(command + 5), fd, &client);
			}
			else if (IS_CMD(command, c_type)) {
				/* client wants to set binary I or ascii A mode */
				if (command[5] == ASCII_MODE
				    || command[5] == BINARY_MODE) {
					data_format = command[5];
					DO_RESP(fd, r_okay);
				}
				else {
					DO_RESP(fd, r_unknown_mode);
				}
			}
			else if (IS_CMD(command, c_mkd)
				 && HAS_PARAM(command, c_mkd)) {
				/* create directory */
				if (!mkdir(gobble_space(command + 4), 0755)) {
					DO_RESP(fd, r_success);
				}
				else {
					send_error_resp(fd);
				}
			}
			else if (IS_CMD(command, c_rmd)
				 && HAS_PARAM(command, c_rmd)) {
				/* remove directory */
				if (!rmdir(gobble_space(command + 4))) {
					DO_RESP(fd, r_success);
				}
				else {
					send_error_resp(fd);
				}
			}
			else if (IS_CMD(command, c_dele)
				 && HAS_PARAM(command, c_dele)) {
				/* remove file */
				if (!unlink(gobble_space(command + 5)))
					DO_RESP(fd, r_success);
				else
					send_error_resp(fd);
			}
			else if (IS_CMD(command, c_pwd)) {
				/* print working directory */
				char buf[200];
				char *wdir = getcwd(buf, 200);
				if (wdir) {
					if (write_to_fd(fd, "257 \"", 5)
					    || write_to_fd(fd, wdir, strlen(wdir))
					    || write_to_fd(fd, "\" is current directory.\r\n", 25)) {
						return;
					}
				}
				else {
					DO_RESP(fd, r_error);
				}
			}
			else if (IS_CMD(command, c_cwd)
				 && HAS_PARAM(command, c_cwd)) {
				/* change working directory */
				do_chdir(fd, gobble_space(command + 4));
			}
			else if (IS_CMD(command, c_site)
				 && HAS_PARAM(command, c_site)) {
				do_site(fd, gobble_space(command + 5));
			}
			else if (IS_CMD(command, c_cdup)) {
				/* change to parent directory */
				do_chdir(fd, "..");
			}
			else if (IS_CMD(command, c_rnfr)
				 && HAS_PARAM(command, c_rnfr)) {
				/* rename from: check existence of file,
				   but do nothing */
				strncpy(rnfrbuffer, gobble_space(command + 5), MAX_CMD_SIZE);
				rnfrbuffer[MAX_CMD_SIZE] = 0;
				if (access(rnfrbuffer, F_OK) == 0)
					DO_RESP(fd, r_fileexists);
				else
					send_error_resp(fd);
			}
			else if (IS_CMD(command, c_rnto)
				 && HAS_PARAM(command, c_rnto)) {
				/* Second part of a rename. Use stored RNFR
				   name and do the rename.  */
				if (rnfrbuffer[0]
				   && rename(rnfrbuffer, gobble_space(command + 5)) == 0)
					DO_RESP(fd, r_success);
				else
					send_error_resp(fd);
			}
			else {
				DO_RESP(fd, r_error);
			}
			break;

		default:
			break;
		}
	}

	DO_RESP(fd, r_goodbye);

	close(fd);

	SysLog(LOG_INFO, "Client %s disconnected.", inet_ntoa(client.ip));
}

/* return the default banner r_hello if the banner file does not exist */
char *
set_banner(const char *r_hello)
{
	FILE *f;
	char *newline;

	/* read banner if the file exists or use a default banner  */
	if ((f = fopen(BANNER_FILE, "r"))) {
		strcpy(banner, "220 ");
		/* Room for "220 "+ "\r\n\0" */
		fgets(banner + 4, BANNER_LEN-7, f);
		fclose(f);
		/* banner which is a one-liner must end with \r\n */
		newline = strchr(banner, '\n');
		if (newline == NULL) {
			DEBUG(fprintf(stderr, "set_banner: adding CRLF\n"));
			newline = banner + strlen(banner);
			newline[0]='\r';
			newline[1]='\n';
			newline[2]='\0';      
		} else if (newline && *(newline-1) != '\r') {
			DEBUG(fprintf(stderr, "set_banner: adding CR\n"));
			newline[0]='\r';
			newline[1]='\n';
			newline[2]='\0';
		}
	} else {
		strcpy(banner, r_hello);
	}

	return &banner[0];
}

/* read a command from the client until it ends with a newline
 * if an error occurs, return -1 */
static int
get_command(int fd, char *buf)
{
	int len = 0;

	while (len < MAX_CMD_SIZE && (!len || buf[len-1] != '\n')) {
		int r;
		fd_set rset;
		struct timeval tv;

		FD_ZERO(&rset);
		FD_SET(fd, &rset);

		/* Only wait up to FTPD_TIMEOUT seconds for a command */
		tv.tv_sec = FTPD_TIMEOUT;
		tv.tv_usec = 0;

		if ((r = select(fd + 1, &rset, NULL, NULL, &tv)) <= 0) {
			/* select returned error or a timeout occured
			 * in either case, we count it as an error.
			 */
			if (!r)
				SysLog(LOG_INFO, "Control-connection timed out.");
			return -1;
		}
		r = read(fd, buf + len, MAX_CMD_SIZE - len);
		if (r < 0) {
			if (errno != EAGAIN && errno != EINTR)
				return -1;
		} else if (r == 0) {
			return -1;
		} else
			len += r;
	}
	if (buf[len-2] == '\r')
		len--;
	buf[len-1] = 0;
	DEBUG(fprintf(stderr, "command: %s\n", buf));
	return 0;
}

/* check if login is correct; if so return 0, uid and gid.
   on failure return -1 */
static int
check_login(char* user, char* pass, uid_t *uid, gid_t *gid)
{
	struct passwd *pw = getpwnam(user);
	char *password = NULL;

	if (pw == NULL) {
		DEBUG(fprintf(stderr, "getpwnam: (%s) %s\n", user, 
			      strerror(errno)));
		return -1;
	}

#if SHADOW_PASSWD_SUPPORT
	if (!geteuid())
	{
		struct spwd *spwd = getspnam(user);

		if (spwd) {
			password = spwd->sp_pwdp;
		}
	}
#endif

	if (!password) {
		password = pw->pw_passwd;
	}

	/* Check against the encrypted password. */
	if (!strcmp(crypt(pass, password), password)) {
		*uid = pw->pw_uid;
		*gid = pw->pw_gid;
		return 0;
	}

	return -1;
}

/* send an error response to the client on an open control
   connection */
static void
send_error_resp(int fd)
{
	switch (errno) {
	case EACCES:
		DO_RESP(fd, r_noaccess);
		break;
	case ENOENT:
		DO_RESP(fd, r_nosuchfile);
		break;
	case ENOTDIR:
		DO_RESP(fd, r_notadir);
		break;
	case ENOMEM:
		DO_RESP(fd, r_outofmem);
		break;
	case ENOSPC:
		DO_RESP(fd, r_outofspace);
		break;
	default:
		DO_RESP(fd, r_unknownerror);
		break;
	}
}

/* list files in directory dir based on the content in simple */
static void
do_list(const char *dir, int fd, struct s_client *client, int simple)
{
	int dfd;
	DIR *dirp;
	struct dirent *dp;

	dirp = opendir(dir);
	if (!dirp) {
		send_error_resp(fd);
		return;
	}

	DO_RESP(fd, r_openingdata);

	dfd = open_data_connection(client);

	if (dfd < 0) {
		DO_RESP(fd, r_cantopen);
	} else {
		if (!simple) {
			ls(dir, dirp, dfd);
		} else {
			/* very simple ls, just lists the names. */
			while ((dp = readdir(dirp)) != NULL) {
				write(dfd, dp->d_name, strlen(dp->d_name));
				write(dfd, "\r\n", 2);
			}
		}
		closedir(dirp);
		DO_RESP(fd, r_trcomplete);
		close(dfd);
	}
}

/* validate the client IP address and store it with the client port in
   struct s_client from the string command. Respond with okay or
   unknown error to the client */
static void
handle_port_cmd(int fd, const char *command, struct s_client *client)
{
	int a1, a2, a3, a4;
	int p1, p2;

	if (sscanf(command, "PORT %d,%d,%d,%d,%d,%d",
		   &a1, &a2, &a3, &a4, &p1, &p2) == 6
	    && a1 < 256 && a2 < 256 && a3 < 256 && a4 < 256
	    && p1 < 256 && p2 < 256) {
		client->ip.s_addr = htonl((a1 << 24) + (a2 << 16) + (a3 << 8) + a4);
		client->port = htons((p1 << 8) + p2);

		if (client->ip.s_addr && client->port) {
			if (client->fd >= 0)
			{
				close(client->fd);
				client->fd = -1;
			}

			DO_RESP(fd, r_okay);
			return;
		}
	}

	DO_RESP(fd, r_unknownerror);
}

/* opens a passive data connection on file descriptor fd
   and listens, returns the file descriptor or -1 on fatal error. */
static int
open_passive_data_connection(void)
{
	struct sockaddr_in addr;
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		SysLog(LOG_EMERG, "Unable to get a TCP socket.");
		return -1;
	}

	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	
	if (listen(fd, 1) < 0) {
		SysLog(LOG_EMERG, "Failed to get socket to listen to port (- listen() failed). %m");
		close(fd);
		return -1;
	}

	return fd;
}

/* open a data connection to the client and return the file
   descriptor fd or -1 on fatal error */
static int
open_data_connection(struct s_client *client)
{
	struct sockaddr_in addr;
	int tries = 1;
	int on = 1;
	int fd = -1;

	if (client->fd != -1) {
		/* passive mode entered */

		if ((fd = accept(client->fd, NULL, 0)) < 0) {
			SysLog(LOG_EMERG, "Failed to accept connection: %m");
		}

		close(client->fd);
		client->fd = -1;

		return fd;
	}

	/* active open of data connection */
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		SysLog(LOG_EMERG, "Unable to get a TCP socket: %m");
		goto bad;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on) < 0) {
		SysLog(LOG_EMERG, "Failed to enable socket option SO_REUSEADDR: %m");
		goto bad;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(ctrl_port - 1);

	for (;; tries++) {
		if (bind(fd, (struct sockaddr *)&addr, sizeof addr) >= 0)
			break;
		if (errno != EADDRINUSE || tries > 10) {
			SysLog(LOG_EMERG, "Unable to bind data socket to port %d: %m", ctrl_port - 1);
			goto bad;
		}
		sleep(tries);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr = client->ip;
	addr.sin_port = client->port;

	if (connect(fd, (struct sockaddr *)&addr, sizeof addr) < 0) {
		SysLog(LOG_EMERG, "Failed to connect data socket: %m");
		goto bad;
	}

	DEBUG(fprintf(stderr, "data connection to 0x%x, port %d\n",
		      client->ip.s_addr, client->port));

	return fd;

bad:
	on = errno;                 /* Save errno for return */
	if (fd != -1) {
		close(fd);
	}
	errno = on;
	return -1;
}

/* write to control connection fd when the command buf of length len
   is not a trivial command listed */
int
write_to_fd(int fd, const char *buf, int len)
{
	while (len) {
		int w = write(fd, buf, len);
		if (w < 0) {
			if (errno != EINTR && errno != EAGAIN)
				return w;
		} else {
			len -= w;
			buf += w;
		}
	}
	return 0;
}

/* opens a passive data connection and FIXME ignores the return value,
   closes the previously opened client connection and replaces it with
   the passive one. Respond with entering passive mode or unknown error. */
static void
handle_pasv_cmd(int fd, struct s_client *client)
{
	int client_fd;

	if ((client_fd = open_passive_data_connection())) {
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof addr;
		unsigned int ip = 0;
		int port = 0;

		if (getsockname(fd, (struct sockaddr*)&addr, &addr_len) >= 0)
			ip = ntohl(addr.sin_addr.s_addr);

		if (getsockname(client_fd, (struct sockaddr*)&addr, &addr_len) >= 0)
			port = ntohs(addr.sin_port);

		if (ip && port) {
			char buffer[80];

			if (client->fd >= 0) {
				close(client->fd);
			}
			client->fd = client_fd;
			
			sprintf(buffer,
				"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n",
				ip >> 24, (ip >> 16) & 0xFF,
				(ip >> 8) & 0xFF, ip & 0xFF,
				port >> 8, port & 0xFF);
			DO_RESP(fd, buffer);
			return;
		}
		close(client_fd);
	}

	DO_RESP(fd, r_unknownerror);
}

/* open fname as a virtual or regular file and send local file content from nOffset
   to the client in binary or ASCII format. Response codes are sent
   and it returns or exits on fatal errors */
static void
do_retr(char *fname, int fd, struct s_client *client, int nOffset)
{
	int dfd; /* remote data connection */
	int ffd; /* local target file descriptor */
	int virtual_type; /* type of virtual target */
	int statfd; /* read from stdout in child */
	struct stat mystat;

	/* first assume it may be opened as a virtual target, secondly
	   as a regular file and send an error response if neither is true */
	ffd = open_virtual(fname, O_RDONLY, &virtual_type, &statfd);
	if (ffd == 0) {
		/* not a virtual target */
		/*  stat the requested local file or send an error response */
		if (stat(fname, &mystat) < 0) {
			send_error_resp(fd);
			return;
		}
		/* fetch directory is not supported, send an error response */
		if (S_ISDIR(mystat.st_mode)) {
			DO_RESP(fd, r_bad_file);
			return;
		}
		/* file accessible or send an error response */
		if (access(fname, R_OK) < 0) {
			send_error_resp(fd);
			return;
		}
		ffd = open(fname, O_RDONLY);
	}
	if (ffd < 0) {
		send_error_resp(fd);
		return;
	}

	/* open the remote data connection in either active or passive mode
	   or send an error response */
	DO_RESP(fd, r_openingdata);
	dfd = open_data_connection(client);
	if (dfd < 0) {
		DO_RESP(fd, r_cantopen);
		return;
	} 

	if (data_format == BINARY_MODE) {
		char *buf;
		int r,w, rtot=0;
		int nSize;

		buf = (char *)malloc(FILEBUFSIZE);
		if (!buf) {
			SysLog(LOG_EMERG, "Memory allocation failed. %m");
			send_error_resp(fd);
			return;
		}
		SysLog(LOG_DEBUG, "send %s in binary mode", fname);
		errno = 0;
		if(nOffset > 0){
			nSize = nOffset;
			while (((r = read(ffd, buf, nSize)) > 0) || (r < 0 && (errno == EINTR || errno == EAGAIN))) {
				if (r > 0) rtot += r;
				nSize = nOffset - rtot;
				if(nSize > FILEBUFSIZE) nSize = FILEBUFSIZE;
				if(nSize <= 0) break;
			}
		}
		while (((r = read(ffd, buf, FILEBUFSIZE)) > 0)
		       || (r < 0 
			   && (errno == EINTR || errno == EAGAIN))) {
			if (r > 0){
				rtot += r;
				if ((w = write_to_fd(dfd, buf, r)) < 0) {
					SysLog(LOG_DEBUG, "r %i != w %i, rtot=%i errno=%i", r, w,rtot, errno);
					break;
				}
			}
		}
		close(ffd);
		close(dfd);
		free(buf);
		DO_RESP(fd, r_trcomplete);
	} else if (data_format == ASCII_MODE) {
		FILE *filestream = fdopen(ffd, "r");
		char *buf;
		char *ok;

		buf = (char *)malloc(FILEBUFSIZE);
		if (!buf) {
			SysLog(LOG_EMERG, "Memory allocation failed. %m");
			send_error_resp(fd);
			return;
		}
		SysLog(LOG_DEBUG, "send %s in ascii mode", fname);

		/* read line-by-line, checking for LF, allow other errors
		   than EOF, e.g. EINTR */
		clearerr(filestream);
		while ((ok = fgets(buf, FILEBUFSIZE - 1, filestream))
		       || (!feof(filestream))) {

			if (ferror(filestream)){
				SysLog(LOG_INFO, "feof(): %i ferror():%i errno=%i '%s'", 
				       feof(filestream), ferror(filestream), errno, buf);
			}
			if (!feof(filestream)){
				int len = strlen(buf);

				/* insert CRLF, but only if the LF was not
				   already preceded by a CR */
				if (len && buf[len - 1] == '\n'
				    && (len < 2 || buf[len - 2] != '\r')) {
					strcpy(buf + len - 1, "\r\n");
					len++;
				}
				write_to_fd(dfd, buf, len);
			}
			*buf = '\0';
		}
		fclose(filestream); /* closes ffd as well */
		close(dfd);
		free(buf);
		DO_RESP(fd, r_trcomplete);
	} else {
		SysLog(LOG_ERR, "Unknown transfer mode.");
	}
}

static void
do_rest(int fd, int nOffset)
{
	char strResponse[100];
	
	sprintf(strResponse, "350 REST supported. Ready to resume at byte offset %d\r\n", nOffset);

	DO_RESP(fd, strResponse);
//	printf("%s", strResponse);
}

/* open fname as a virtual or regular file and receive local file content
   from the client in binary or ASCII format. Response codes are sent
   and it returns or exits on fatal errors */
static void
do_stor(char *fname, int fd, struct s_client *client)
{
	int ffd; /* local target file descriptor */
	int virtual_type; /* type of virtual target */
	int statfd; /* read from stdout in child */
	int dfd; /* remote data connection */
	struct stat mystat;
	mode_t modes = DEFAULT_MODE_REGULAR_FILES;
	int r;
	int q;
	char *p;
	char *buf;

	/* first assume it may be opened as a virtual file, secondly
	   as a regular file and send an error response if neither is true */
	ffd = open_virtual(fname, O_WRONLY, &virtual_type, &statfd);
	if (ffd == 0) {
		/* not a virtual target */
		/* get stats on the requested local file if it exists */
		if (stat(fname, &mystat) == 0) {
			/* exists */
			modes = mystat.st_mode & 0777;

			/* store directory is not supported, send an 
			   error response */
			if (S_ISDIR(mystat.st_mode)) {
				DO_RESP(fd, r_bad_file);
				return;
			}

			/* if the file cannot be accessed, send an 
			   error response */
			if (access(fname, W_OK) < 0) {
				send_error_resp(fd);
				return;
			}
		}
		ffd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, modes);
	}
	if (ffd <= 0) {
		send_error_resp(fd);
		return;
	}

	/* need a buffer for the data connection */
	if ((buf = (char *)malloc(FILEBUFSIZE)) == NULL) {
		SysLog(LOG_EMERG, "Memory allocation failed. %m");
		send_error_resp(fd);
		close(ffd);
		return;
	}
	/* open the remote data connection in either active or passive mode
	   or send an error response */
	DO_RESP(fd, r_openingdata);
	dfd = open_data_connection(client);
	if (dfd < 0) {
		DO_RESP(fd, r_cantopen);
		return;
	} 

	q = 0;
	if (data_format == BINARY_MODE) {
		SysLog(LOG_DEBUG, "receive %s in binary mode", fname);
		errno = 0;
		while (!q
		       && ((r = read(dfd, buf, FILEBUFSIZE)) > 0
			   || (r < 0 
			       && (errno == EINTR || errno == EAGAIN)))) {
			p = buf;
			while (r > 0) {
				int w = write(ffd, p, r);
				SysLog(LOG_DEBUG, "write returned: %i", w);
				if (w < 0) {
					if (errno != EINTR 
					    && errno != EAGAIN) {
						q++;
						break;
					}
				} else {
					r -= w;
					p += w;
				}
			}
		}
		if (q && errno == ENOSPC) {
			/* No space left on device.  */

			/* Since the whole file wasn't uploaded in its
			 * entirety, we could just as well truncate it
			 * in order to save space on the device.
			 */
			DO_RESP(fd, r_outofspace);
			if (ftruncate(ffd, 0) < 0) {
				SysLog(LOG_ERR, "failed to truncate "
				       "file %s (errno = %d). %m", fname, errno);
			}
		}
		close(dfd);
		close(ffd);
		free(buf);
	} else if (data_format == ASCII_MODE) {
		FILE *filestream;
		FILE *ofilestream;

		SysLog(LOG_DEBUG, "receiving %s in ascii mode", fname);

		filestream = fdopen(dfd, "r");
		ofilestream = fdopen(ffd, "w");

		/* FIXME if a line is FILEBUFSIZE long
		 * FIXME the last '\n' will not be in the buffer and the \r
		 * FIXME will not be removed. However it will work almost
		 * FIXME _all_ of the time ;-).
		 */

		while (!q && fgets(buf, FILEBUFSIZE - 1, filestream)) {
			p = buf;
			r = strlen (p);
			if ((r >= 2) 
			    && (p[r - 2] == '\r') && (p[r - 1] == '\n')) {
				p[r - 2] = '\n';
				p[r - 1] = '\0';
				r--;
			}
			if (fputs(p, ofilestream) == EOF) {
				q++;
			}
		}

		if (q) {
			/* No space left on device.  */
			DO_RESP(fd, r_outofspace);
			/* Since the whole file wasn't uploaded in its
			 * entirety, we could just as well truncate it
			 * in order to save space on the device.
			 */
			if (ftruncate(ffd, 0) < 0) {
				SysLog(LOG_ERR, "failed to truncate "
				       "file %s (errno = %d). %m", fname, errno);
			}
		}
		fclose(filestream);
		fclose(ofilestream);
		free(buf);
	} else {
		SysLog(LOG_ERR, "Unknown transfer mode.");
		return;
	}

	/* the virtual target is duplex, forward all messages */
	if (virtual_type == VIRT_CGI) {
		char buf[100];
		int n;

		DO_RESP(fd, r_virtexec);
		while ((n = read(statfd, buf, sizeof buf)) > 0) {
			if (write_to_fd(fd, buf, n) < 0) {
				send_error_resp(fd);
				return;
			}
		}
		DO_RESP(fd, r_virtexit);
	} else {
		DO_RESP(fd, r_trcomplete);
	}
}
