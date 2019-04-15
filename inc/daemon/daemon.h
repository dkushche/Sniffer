
#ifndef DAEMON_H
# define DAEMON_H

#include "main.h"
#include "ip_vector.h"

# define LOGS   "errors.txt"

typedef struct      sniffer
{
    int             out_chan;
    int             in_chan;
    int             sock_raw;
    t_ip_vector     data;
    int             file;
    int             status;
    unsigned char   is_working;
    char            now_device[IFNAMSIZ];    
}                   t_sniffer;

void                init_daemon(void);

#endif
