

static void             react_start( t_sniffer *this )
{
    unsigned char       stat;

    this->is_working = 1;
    bind_to_device(this, DEFAULT);
    stat = 1;
    write(this->out_chan, &stat, sizeof(unsigned char));
}

static void             react_stop( t_sniffer *this )
{
    unsigned char       stat;

    this->is_working = 0;
    stat = 1;
    write(this->out_chan, &stat, sizeof(unsigned char));
}

static void             react_show( t_sniffer *this )
{
    unsigned long int   pack;
    struct in_addr      buf;

    if (read(this->in_chan, &buf, sizeof(struct in_addr)) < 0)
        { err_log("Trouble with reading from chanel", this->status, 0); return ;}
    if (!(pack = searcher(&this->data, buf)))
        err_log("No packages from this address", this->status, 0);
    write(this->out_chan, &pack, sizeof(unsigned long int));
}

static void             react_select( t_sniffer *this )
{
    unsigned char       stat;
    char                device[IFNAMSIZ];

    if (read(this->in_chan, device, IFNAMSIZ * sizeof(char)) < 0)
    {
        err_log("Trouble with reading from chanel", this->status, 0);
        return ;
    }
    if (bind_to_device(this, device) < 0)
        stat = 0;
    else
        stat = 1;
    write(this->out_chan, &stat, sizeof(unsigned char));
}

static void             react_stat( t_sniffer *this )
{
    int                 device;

    read(this->in_chan, &device, sizeof(device));
    stream_in_chanel(this, device);
}

static void             react_die( t_sniffer *this )
{
    unsigned char       stat;

    stat = 1;
    write(this->out_chan, &stat, sizeof(unsigned char));
    err_log("Oh, time to die", this->status, 1);
}
