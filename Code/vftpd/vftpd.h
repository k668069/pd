#ifndef __VFTPD_H__
#define __VFTPD_H__

/* used in system logging */
#define PROGRAM_NAME "vftpd"

/* presented to the user at each new session */
#define BANNER_FILE "/etc/vftpd.banner"
#define BANNER_LEN 80

/* default access permissions of regular files, rw-r-r */
#define DEFAULT_MODE_REGULAR_FILES 0644

/* 900 seconds timeout is common */
#define FTPD_TIMEOUT 900

/* length of the username/password buffers */
#define U_P_BUFLEN 16

/* longest string accepted from the client */
#define MAX_CMD_SIZE 256

/* largest possible file to buffer */
#define FILEBUFSIZE 8180

/* root restricted login */
#define ROOT_USER "root"

/* flash user name and group used in special
   case SNMP handling of login */
#define FLASH_USER  "flash"
#define FLASH_GROUP "flash"

/* change password in special case SNMP handling of login */
#define PROGRAM_FLASH_PASSWD "/bin/change_pwd"

/* binary and ascii modes */
#define BINARY_MODE 'I'
#define ASCII_MODE  'A'

/* send responses to the client */
#define DO_RESP(x,y) write(x, y, strlen(y))

/* interpret commands from the client */
#define IS_CMD(cl_cmd,cmd) (strncasecmp(cl_cmd, cmd, strlen(cmd)) == 0)

/* are there any parameters following the command */
#define HAS_PARAM(cl_cmd,cmd) (strlen(cl_cmd) > (strlen(cmd) + 1))

/* states in the FTP state machine */
enum {
	FST_AUTHENTICATE1,
	FST_AUTHENTICATE2,
	FST_RUNNING,
	FST_STOR,
	FST_RETR,
	FST_QUIT
};

/* represents an established connection */
struct s_client {
	struct in_addr ip;
	unsigned short port;
	int fd;
};

#endif /* __VFTPD_H__ */
