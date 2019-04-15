
#ifndef CLIENT_MAIN_H
# define CLIENT_MAIN_H

# include "main.h"

typedef struct
{
    int     in_chan;
    int     out_chan;
}           t_client;

# define USAGE "usage :\n\t\tstart​ (packets are being sniffed from now from default iface(eth0))\n\
                stop​ (packets are not sniffed)\n\
                show [ip] ​(print number of packets received from ip address)\n\
                select_iface [iface] ​(select interface for sniffing eth0, wlan0, ethN, wlanN...)\n\
                stat​ [iface]​ (show all collected statistics for particular interface), if iface omitted - for all interfaces.\n"

#endif
