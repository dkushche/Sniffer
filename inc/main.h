
#ifndef MAIN_H
# define MAIN_H

# include <sys/types.h>
# include <linux/if_ether.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <netinet/tcp.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <net/if.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <signal.h>

# define DAE_PID ".daemon_pid\0"
# define FIFO_CL_DAE_CHAN ".conn_layer_cl_dae"
# define FIFO_DAE_CL_CHAN ".conn_layer_dae_cl"
# define F_PATH "reboot\0"

# define DEFAULT "eth0\0"
# define MY  "enp2s0f2\0" //"wlp3s0"
# define BUFF_SIZE 65536

enum MES
{
    START = 1,
    STOP = 2,
    SHOW = 3,
    SELECT = 4
};

void            fatal_err_stdin(const char *err, char is_errno);
void		    err_log( const char *err, int status_fd, char fatal );

void            demonize( void ); //?

#endif
