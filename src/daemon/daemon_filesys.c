
static void             create_log_file( t_sniffer *this );
static void             rewrite_file( t_sniffer *this );
static void             load_in_sniff( t_sniffer *this );
static int              open_file( int status_fd, char *now_device );
static int              open_status( void );
static void             check_prev( void );

static int          open_status( void )
{
    int             res = open (LOGS, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    unsigned int    buf;

    if (res < 0)
        fatal_err_stdin("Logs creation error", 1);
    check_prev();
    int     daemon = open(DAE_PID, O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);

    if (daemon < 0)
        fatal_err_stdin("Can't open PID file for writing", 1);
    buf = getpid();
    write(daemon, &buf, sizeof(unsigned int));
    close(daemon);
    return res;
}

static void     check_prev( void )
{
    int         daemon = open(DAE_PID, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    int         pid;
    int         count;

    if (daemon < 0 && errno == ENOENT)
        return ;
    if (daemon < 0)
        fatal_err_stdin("Can't open PID file for reading", 1);
    count = read(daemon, &pid, sizeof(unsigned int));
    if (count < 0)
    {
        printf("Read error\n");
        close(daemon);
        return;
    }
    kill(pid, SIGKILL);    
}

static int          open_file( int status_fd, char *now_device)
{
    char            real_name[strlen(now_device) + sizeof(F_PATH) + 2];
    int             res;

    snprintf(real_name, sizeof(real_name), ".%s%s", now_device, F_PATH);
    real_name[strlen(now_device) + sizeof(F_PATH) + 1] = '\0';
    res = open(real_name, O_RDWR, S_IRUSR | S_IRGRP | S_IROTH);
    if (res < 0)
        err_log("Open reboot file on rewrite. Can't update statistic", status_fd, 1);
    return res;
}

static void         create_log_file( t_sniffer *this )
{
    int             res;
    char            real_name[strlen(this->now_device) + sizeof(F_PATH) + 2];

    snprintf(real_name, sizeof(real_name), ".%s%s", this->now_device, F_PATH);
    real_name[strlen(this->now_device) + sizeof(F_PATH) + 1] = '\0';
    res = open(real_name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IRGRP | S_IROTH);
    if (res < 0 && errno == EEXIST)
        load_in_sniff( this );
    else if (res < 0)
        err_log("Open file creating reboot", this->status, 1);

    this->file = res;
}

static void             load_in_sniff( t_sniffer *this )
{
    char                real_name[strlen(this->now_device) + sizeof(F_PATH) + 2];
    FILE                *descr;
    char                *s_buf = (char *)malloc(sizeof(char) * 256);
    unsigned long int   n_buf = 0;
    struct in_addr      ip_buf;

    snprintf(real_name, sizeof(real_name), ".%s%s", this->now_device, F_PATH);
    real_name[strlen(this->now_device) + sizeof(F_PATH) + 1] = '\0';
    descr = fopen(real_name, "r");

    ip_buf.s_addr = 0;

    while (fscanf(descr, "IP : %s ; packages : %lu \n", s_buf, &n_buf) != EOF)
    {     
        if (!n_buf)
            err_log("Isn't read", this->status, 1);
        if (inet_aton(s_buf, &ip_buf) < 0)
            err_log("aton", this->status, 1);
        push_back(&this->data, this->status, ip_buf, n_buf);
        n_buf = 0;
        ip_buf.s_addr = 0;
    }
    fclose(descr);
    free(s_buf);
}

static void         rewrite_file( t_sniffer *this )
{
    if (this->file >= 0)
        close(this->file);
    this->file = open_file( this->status, this->now_device );
}
