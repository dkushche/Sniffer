
# include "device_list.h"

void            download_devices( t_sniffer *this )
{
    int                descr;
    int                 buf;
    char                help;
    int                 stat;

    if ((descr = open(D_PATH, O_RDONLY)) >= 0)
    {
        buf = 0;
        
        while (1)
        {
            if ((stat = read(descr, &buf, sizeof(buf))) < 0)
            {
                err_log("Can't read from device file", this->status, 1);
                break;
            } else if (!stat)
                break;
            if (read(descr, &help, sizeof(char)) != sizeof(char) || help != '\n')
            {
                err_log("Someone changed devices file", this->status, 0);
                break;
            }
            if (buf != 0)
                push_in_list(&this->devices, buf, 0, 0);
            else
                err_log("Incorrect data in devices file", this->status, 1);
            buf = 0;
        }
        close(descr);
    }
    this->dev_file = open(D_PATH, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    if (this->dev_file < 0)
        err_log("Can't work with device files", this->status, 1);
}

static t_list   *create_node(int device, int dev_file, unsigned char write_in_dev)
{
    t_list      *res;

    res = (t_list *)malloc(sizeof(t_list));
    res->device = device;
    res->next = NULL;
    if (write_in_dev)
    {
        write(dev_file, &device, sizeof(device));
        write(dev_file, "\n", sizeof(char));
    }

    return res;
}

void            push_in_list(t_list **this, int device, int dev_file, char write_in_dev)
{
    if (!*this)
        *this = create_node(device, dev_file, write_in_dev);
    else
    {
        t_list  *buf = *this;

        while (buf->next)
        {
            if (buf->device == device)
                return ;
            buf = buf->next;
        }
        if (buf->device != device) {
            buf->next = create_node(device, dev_file, write_in_dev);
        }
    }
}

static void         stream_from_file(const char *path, t_sniffer *this)
{
    FILE                *descr;
    char                *s_buf = (char *)malloc(sizeof(char) * 17);
    unsigned int        package[3];
    struct in_addr      ip;
    unsigned long int   pack;

    descr = fopen(path,"r");
    if (!descr)
    {
        err_log("No statistic", this->status, 0);
        memset(package, 0, sizeof(unsigned char) * 12);
        write(this->out_chan, &package, sizeof(package));    
        return ;
    }
    pack = 0;
    while (fscanf(descr, "IP : %s ; packages : %lu \n", s_buf, &pack) != EOF)
    {
        if (!pack)
            err_log("Fatal error while sending", this->status, 1);
        inet_aton(s_buf, &ip);
        package[0] = *((unsigned long int *)&ip);
        *((unsigned long int *)(package + 1)) = pack;
        write(this->out_chan, &package, sizeof(package));
        pack = 0;
    }
    memset(package, 0, sizeof(unsigned int ) * 3);
    write(this->out_chan, &package, sizeof(package));
    fclose(descr);
    free(s_buf);
}

void                stream_in_chanel(t_sniffer *this, int device)
{
    unsigned char   stat = 0;
    char            all = 0;
    t_list          *buf = this->devices;
    unsigned char   first_time = 1;

    if (!device)
        all = 1;

    while (buf)
    {
        if (buf->device == device || all)
        {
            if (first_time)
            {
                stat = 1;
                write(this->out_chan, &stat, sizeof(unsigned char));
                first_time = 0;
            }
            write(this->out_chan, &buf->device, sizeof(buf->device));
            char    iface_name[IF_NAMESIZE];
            if_indextoname(buf->device, iface_name);
            char    file_name[sizeof(F_PATH)+strlen(iface_name)+2];

            snprintf(file_name, sizeof(file_name), ".%s%s", iface_name, F_PATH);
            file_name[sizeof(file_name) - 1] = '\0';
            stream_from_file(file_name, this);
        }
        buf = buf->next;
    }
    if (first_time)
        write(this->out_chan, &stat, sizeof(unsigned char));
    else
    {
        int         end = 0;
        write(this->out_chan, &end, sizeof(int));
    }
}

void                destroy_list(t_list **this)
{
    t_list          *head = *this;
    t_list          *buf;

    while (head)
    {
        buf = head;
        head = head->next;
        free(buf);
    }
    *this = NULL;
}