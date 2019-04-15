
static void         send_mess(int id, char *spec, t_client *client);
static void         client_proccesing(char *spec);
static void         client_loop(void);
static void         parse_input(char *input, t_client *client);
static void         start_send( t_client *this );
static void         stop_send( t_client *this );
static void         show_send( t_client *this, char *spec );
static void         select_send( t_client *this, char *spec );

static void         client_loop( void )
{
    t_client        client;
    char            *input = NULL;
    size_t          size = 0;
    int             code;

    CL;
    client.out_chan = open(FIFO_CL_DAE_CHAN, O_RDWR);
    if (client.out_chan < 0)
        fatal_err_stdin("Opening FIFO CL->DAE use sudo", 0);
    client.in_chan = open(FIFO_DAE_CL_CHAN, O_RDWR);
    if (client.in_chan < 0)
        fatal_err_stdin("Opening FIFO DAE<-CL use sudo", 0);
    while ((code = getline(&input, &size, stdin)) > 0)
    {
        parse_input(input, &client);
        free(input);
        input = NULL;
        CL;
    }
    if (code < 0)
        fatal_err_stdin("Read from stdin", 0);
    close(client.in_chan);
    close(client.out_chan);
}

static void         parse_input(char *input, t_client *client)
{
    char            *token;
    char            *new_l;

    new_l = strchr(input, '\n');
    *new_l = '\0';
    token = strtok(input, " ");
    if (!strcmp(token, "start") && !strtok(NULL, " "))
    { send_mess(START, NULL, client); }
    else if (!strcmp(token, "stop") && !strtok(NULL, " "))
    { send_mess(STOP, NULL, client); }
    else if (!strcmp(token, "--help") && !strtok(NULL, " "))
    { CL; printf(USAGE);}
    else if (!strcmp(token, "show"))
    {
        token = strtok(NULL, " ");
        if (token)
            send_mess(SHOW, token, client);
        else
            {CL;printf("Need ip to show\n");}
    }
    else if (!strcmp(token, "select_iface"))
    {
        token = strtok(NULL, " ");
        if (token)
            send_mess(SELECT, token, client);
        else
            {CL;printf("Need iface in seect_iface\n");}
    }
    else if (!strcmp(token, "stat"))
    {
        token = strtok(NULL, " ");
        if (token)
            {client_proccesing(token);}
        else
            {CL;printf("write iface after stat\n");}
    }
    else if (!strcmp(token, "exit"))
    {
        CL;printf("Good bye\n");
        exit(1);
    }
    else
    { CL;printf("Incorrect input\n"); }
}

static void         send_mess( int id, char *spec, t_client *client )
{
    switch(id)
    {
        case START :    start_send(client);
                        break;
        case STOP :     stop_send(client);
                        break;
        case SHOW :     show_send(client, spec);
                        break;
        case SELECT :   select_send(client, spec);
                        break;
        default :       break;
    }
}

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


    CL;printf("Sending end request\n");
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

static void         client_proccesing( char *spec )
{
    char            real_name[strlen(spec) + sizeof(F_PATH) + 2];
    int             res;

    snprintf(real_name, sizeof(real_name), ".%s%s", spec, F_PATH);
    real_name[strlen(spec) + sizeof(F_PATH) + 1] = '\0';
    res = open(real_name, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    if (res < 0)
        {CL;printf("No data on this device"); return ;}
    else
    {
        struct stat d_file;

        if (stat(real_name, &d_file) < 0)
            {CL;printf("I can't find your file size\n"); return ;}
        char    *buf = (char *)malloc(sizeof(char) * (d_file.st_size + 1));
        buf[d_file.st_size] = '\0';
        printf("\n* * * * * * %s * * * * * * *\n", spec);
        if (read(res, buf, d_file.st_size) < 0)
            printf("Reading error\n");
        printf("%s", buf);
        free(buf);
    }
}
