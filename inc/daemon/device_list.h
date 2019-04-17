
#ifndef DEVICE_LIST_H
# define DEVICE_LIST_H

# include "daemon.h"

typedef struct      list_el
{
    int             device;
    struct list_el *next;
}                   t_list;

void            download_devices( t_sniffer *this );
void            push_in_list(t_list **this, int device, int dev_file, char write_in_dev);
void            stream_in_chanel(t_sniffer *this, int device);
void            destroy_list(t_list **this);

#endif
