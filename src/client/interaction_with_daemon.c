
static inline void         con_printf(const char *mess)
{
    printf("\033[36msniff_client:> \033[0m%s", mess);
}

static void         send_start( t_client *this, char *spec )
{
    unsigned char   act;

    TRASH_IN_LINE
    con_printf("Sending start request\n");
    act = START;
    write(this->out_chan, &act, sizeof(unsigned char));
    if (read(this->in_chan, &act, sizeof(unsigned char)) < 0)
        fatal_err_stdin("Fatal error can't read", 0);
    if (act)
        con_printf("Success!\n");
    else
        con_printf("Error! Check errors.txt\n");
}

static void         send_stop( t_client *this, char *spec )
{
    unsigned char act;

    TRASH_IN_LINE
    con_printf("Sending stop request\n");
    act = STOP;
    write(this->out_chan, &act, sizeof(unsigned char));
    if (read(this->in_chan, &act, sizeof(unsigned char)) < 0)
        fatal_err_stdin("Fatal error can't read", 0);
    if (act)
        con_printf("Success!\n");
    else
        con_printf("Error! Check errors.txt\n");
}

static void         send_show( t_client *this, char *spec )
{
    unsigned long int   packages = 0;
    struct in_addr      addres;
    unsigned char       act;

    con_printf("Sending show request\n");
    act = SHOW;
    write(this->out_chan, &act, sizeof(unsigned char));
    if (inet_aton(spec, &addres) < 0)
        { con_printf("Incorrect IP address\n"); return ; }
    write(this->out_chan, &addres, sizeof(struct in_addr));
    if (read(this->in_chan, &packages, sizeof(unsigned long int)) < 0)
        fatal_err_stdin("Fatal error can't read", 0);
    if (packages)
    {
        con_printf("");
        printf("IP : %s ; packages : %lu\n", spec, packages);
    }
    else
        con_printf("Error! Check errors.txt\n");
}

static void         send_select( t_client *this, char * spec )
{
    unsigned char   device[IFNAMSIZ];
    unsigned char   act;

    con_printf("Sending select request\n");
    act = SELECT;
    write(this->out_chan, &act, sizeof(unsigned char));
    if(strlen(spec) >= IFNAMSIZ)
        {con_printf("Incorrect iface name\n"); return ; }
    memcpy(device, spec, strlen(spec));
    device[strlen(spec)] = '\0';
    write(this->out_chan, device, IFNAMSIZ * sizeof(char));
    if (read(this->in_chan, &act, sizeof(unsigned char)) < 0)
        fatal_err_stdin("Fatal error can't read", 0);
    if (act)
        con_printf("Success!\n");
    else
        con_printf("Error! Check errors.txt\n");
}

static void         send_stat(t_client *this, char *spec)
{
    (void)this;
    (void)spec;
    con_printf("404 stat\n");
}

static void         send_die(t_client *this, char *spec )
{
    TRASH_IN_LINE
    (void)this;
    con_printf("404 die\n");
}

static void         show_help(t_client *this, char *spec)
{
    TRASH_IN_LINE
    (void)this;
    con_printf("404 help\n");
}

static void         make_exit(t_client *this, char *spec)
{
    TRASH_IN_LINE
    (void)this;
    con_printf("404 exit\n");
}