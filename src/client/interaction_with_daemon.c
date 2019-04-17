
static inline void         con_printf(const char *mess)
{
    printf("\033[36msniff_client:> \033[0m%s", mess);
}

static inline void         request_no_params( t_client *this, char *spec, unsigned char act)
{
    TRASH_IN_LINE
    con_printf("Sending request\n");
    write(this->out_chan, &act, sizeof(unsigned char));
    if (read(this->in_chan, &act, sizeof(unsigned char)) < 0)
        fatal_err_stdin("Fatal error can't read", 0);
    if (act)
        con_printf("Success!\n");
    else
        con_printf("Error! Check errors.txt\n");
}

static void         send_start( t_client *this, char *spec )
{
    request_no_params(this, spec, START);
}

static void         send_stop( t_client *this, char *spec )
{
    request_no_params(this, spec, STOP);
}

static void         send_show( t_client *this, char *spec )
{
    unsigned long int   packages = 0;
    struct in_addr      addres;
    unsigned char       act;
    char                *buf = spec;

    spec = strtok(NULL, " ");
    TRASH_IN_LINE
    spec = buf;
    if (!spec || !strlen(spec)) {
        con_printf("Show need param [ip]\n"); return ; }
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
    char            *buf = spec;

    spec = strtok(NULL, " ");
    TRASH_IN_LINE
    spec = buf;
    if (!spec || !strlen(spec)) {
        con_printf("Select need param [iface]\n"); return ; }
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

static char         check_stat_arg(char *spec, int *req)
{
    if (!strcmp(spec, "-"))
        *req = 0;
    else
    {
        *req = if_nametoindex(spec);
        if (*req == 0)
            {con_printf("Incorrect iface name\n"); return 0; }
    }
    return 1;
}

static char         stat_handshake(t_client *this, int req)
{
    unsigned char   act;

    act = STAT;
    write(this->out_chan, &act, sizeof(unsigned char));
    write(this->out_chan, &req, sizeof(int));
    read(this->in_chan, &act, sizeof(unsigned char));
    if (!act)
    {
        con_printf("No statistic on this interface\n");
        return 0;
    }
    else
        con_printf("Success\n");    
    return 1;
}

static void         stat_stream( t_client *this )
{
    int                 get_ifc = 0;
    unsigned int        package[3];
    char                iface[IF_NAMESIZE];

    read(this->in_chan, &get_ifc, sizeof(int));
    while(get_ifc)
    {
        if_indextoname(get_ifc, iface);
        printf("*%s*****************************\n", iface);
        read(this->in_chan, &package, sizeof(package));
        while (*(unsigned long int *)(package))
        {
            struct in_addr      *buf = (struct in_addr *)package; 
            char *addr = inet_ntoa(*buf);
            printf("IP : %s ; packages %lu\n", addr, *((unsigned long int *)(package + 1)));
            read(this->in_chan, &package, sizeof(package));
        }
        read(this->in_chan, &get_ifc, sizeof(int));
    }
}

static void         send_stat(t_client *this, char *spec)
{
    char            *buf = spec;
    int             req;
    

    spec = strtok(NULL, " ");
    TRASH_IN_LINE
    spec = buf;
    if (!spec || !strlen(spec)) {
        con_printf("Stat need param [iface]\n"); return ; }
    if (!check_stat_arg(spec, &req))
        return ;
    con_printf("Sending stat request\n");
    if (!stat_handshake(this, req))
        return ;
    stat_stream(this);
    printf("-End of output-------------------------\n");
}

static void         send_die(t_client *this, char *spec )
{
    request_no_params(this, spec, DIE);
    con_printf("Good bye\n");
    exit(1);
}

static void         show_help(t_client *this, char *spec)
{
    TRASH_IN_LINE
    (void)this;
    con_printf(USAGE);
}

static void         make_exit(t_client *this, char *spec)
{
    TRASH_IN_LINE
    (void)this;
    con_printf("Good bye\n");
    exit(1);
}