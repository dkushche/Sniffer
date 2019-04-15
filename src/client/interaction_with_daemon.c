
static void         start_send( t_client *this );
static void         stop_send( t_client *this );
static void         show_send( t_client *this, char *spec );
static void         select_send( t_client *this, char *spec );


static void         start_send( t_client *this )
{
    unsigned char   act;

    CL;printf("Sending start request\n");
    act = START;
    write(this->out_chan, &act, sizeof(unsigned char));
    if (read(this->in_chan, &act, sizeof(unsigned char)) < 0)
        {CL;fatal_err_stdin("Fatal error can't read", 0);}
    if (act)
        {CL;printf("Success!\n");}
    else
        {CL;printf("Error! Check errors.txt\n");}
}

static void         stop_send( t_client *this )
{
    unsigned char act;


    CL;printf("Sending stop request\n");
    act = STOP;
    write(this->out_chan, &act, sizeof(unsigned char));
    if (read(this->in_chan, &act, sizeof(unsigned char)) < 0)
        {CL;fatal_err_stdin("Fatal error can't read", 0);}
    if (act)
        {CL;printf("Success!\n");}
    else
        {CL;printf("Error! Check errors.txt\n");}
}

static void         show_send( t_client *this, char *spec )
{
    unsigned long int   packages = 0;
    struct in_addr      addres;
    unsigned char       act;

    CL;printf("Sending show request\n");
    act = SHOW;
    write(this->out_chan, &act, sizeof(unsigned char));
    if (inet_aton(spec, &addres) < 0)
        {CL;printf("Incorrect IP address\n"); return;}
    write(this->out_chan, &addres, sizeof(struct in_addr));
    if (read(this->in_chan, &packages, sizeof(unsigned long int)) < 0)
        {CL;fatal_err_stdin("Fatal error can't read", 0);}
    if (packages)
        {CL;printf("IP : %s ; packages : %lu\n", spec, packages);}
    else
        {CL;printf("Error! Check errors.txt\n");}
}

static void         select_send( t_client *this, char * spec )
{
    unsigned char   device[IFNAMSIZ];
    unsigned char   act;

    CL;printf("Sending select request\n");
    act = SELECT;
    write(this->out_chan, &act, sizeof(unsigned char));
    if(strlen(spec) >= IFNAMSIZ)
        {CL;printf("Incorrect iface name\n"); return ; }
    memcpy(device, spec, strlen(spec));
    device[strlen(spec)] = '\0';
    write(this->out_chan, device, IFNAMSIZ * sizeof(char));
    if (read(this->in_chan, &act, sizeof(unsigned char)) < 0)
        {CL;fatal_err_stdin("Fatal error can't read", 0);}
    if (act)
        {CL;printf("Success!\n");}
    else
        {CL;printf("Error! Check errors.txt\n");}
}
