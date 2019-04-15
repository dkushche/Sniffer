
#include "daemon.h"
#include "daemon_filesys.c"
#include "daemon_sock.c"

static void             sniff_time( t_sniffer *daemon, unsigned char *buf );
static void             free_res( void );
static void             gen_fifo( t_sniffer *this );
static void             read_chanel( t_sniffer *daemon );
static void             processing_request(t_sniffer *this, unsigned char cmd);

void                    init_daemon( void )
{
    t_sniffer       daemon;
    unsigned char   *buf;

    daemon.status = open_status();
    free_res();
	daemon.sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL) );
	if (daemon.sock_raw < 0)
		err_log("Trouble with creating socket", daemon.status, 1);
	daemon.data.array = NULL;
	if (bind_to_device(&daemon, DEFAULT) < 0)
		bind_to_device(&daemon, MY);
    make_promiscious(daemon.sock_raw, daemon.status);
    gen_fifo(&daemon);
    daemon.is_working = 1;

    buf = (unsigned char *)malloc(sizeof(unsigned char) * BUFF_SIZE);
    while (1)
    {
        read_chanel(&daemon);
        while (daemon.is_working) {
            sniff_time(&daemon, buf);
            read_chanel(&daemon);
        }
    }
    free(buf);
    destroy_vector(&daemon.data, 1);
    promiscious_off(daemon.sock_raw, daemon.status);
    close(daemon.sock_raw);
    close(daemon.out_chan);
    close(daemon.in_chan);
}

static void             read_chanel(t_sniffer *this)
{
    unsigned char       cmd;
    int                 status;

    status = read(this->in_chan, &cmd, sizeof(unsigned char));
    if ( status < 0 && errno == EAGAIN)
        return ;
    else if (status < 0)
        err_log("Reading from chanel", this->status, 1);
    else if (status > 0)
    {
        int saved_flags = fcntl(this->in_chan, F_GETFL);

        fcntl(this->in_chan, F_SETFL, saved_flags & ~O_NONBLOCK);
        processing_request(this, cmd);
        saved_flags = fcntl(this->in_chan, F_GETFL, 0);
        fcntl(this->in_chan, F_SETFL, saved_flags | O_NONBLOCK);
    }
}

static void             processing_request(t_sniffer *this, unsigned char cmd)
{
    unsigned char       stat;
    struct in_addr      buf;
    unsigned long int   pack;
    char                device[IFNAMSIZ];

    if (cmd == START)
    {
        this->is_working = 1;
        stat = 1;
        write(this->out_chan, &stat, sizeof(unsigned char));
    } else if (cmd == STOP) {
        this->is_working = 0;
        stat = 1;
        write(this->out_chan, &stat, sizeof(unsigned char));
    } else if (cmd == SHOW) {
        if (read(this->in_chan, &buf, sizeof(struct in_addr)) < 0)
        { err_log("Trouble with reading from chanel", this->status, 0); return ;}
        if (!(pack = searcher(&this->data, buf)))
            err_log("No packages from this address", this->status, 0);
        write(this->out_chan, &pack, sizeof(unsigned long int));
    } else if (cmd == SELECT) {
        if (read(this->in_chan, device, IFNAMSIZ * sizeof(char)) < 0)
        { err_log("Trouble with reading from chanel", this->status, 0); return ;}
        if (bind_to_device(this, device) < 0)
            stat = 0;
        else
            stat = 1;
        write(this->out_chan, &stat, sizeof(unsigned char));
    } else {
        err_log("Incorect client request", this->status, 0);
        stat = 0;
        write(this->out_chan, &stat, sizeof(unsigned char));
    }
}

static void             gen_fifo(t_sniffer *this)
{
    int         fifo;

    fifo = mkfifo(FIFO_CL_DAE_CHAN, 777);
    if (fifo < 0 && errno != EEXIST)
        err_log("Creating FIFO", this->status, 1);
    fifo = mkfifo(FIFO_DAE_CL_CHAN, 777);
    if (fifo < 0 && errno != EEXIST)
        err_log("Creating FIFO", this->status, 1);
    this->out_chan = open(FIFO_DAE_CL_CHAN, O_RDWR);
    this->in_chan = open(FIFO_CL_DAE_CHAN, O_RDWR);
    if (this->in_chan < 0 || this->out_chan < 0)
        err_log("Opening chanels", this->status, 1);
    int flags = fcntl(this->in_chan, F_GETFL, 0);
    fcntl(this->in_chan, F_SETFL, flags | O_NONBLOCK);
}

static void             sniff_time( t_sniffer *daemon, unsigned char *buf )
{
    struct sockaddr     saddr;
    socklen_t           saddr_len = sizeof (saddr);
    struct sockaddr_in  source;

    if (recvfrom(daemon->sock_raw, buf, BUFF_SIZE, 0, &saddr, &saddr_len) < 0)
        err_log("Reciving error", daemon->status, 1);
    
    memset(&source, 0, sizeof(source));
    struct iphdr *ip = (struct iphdr*)(buf + sizeof(struct ethhdr));

    source.sin_addr.s_addr = ip->saddr;

    push(&daemon->data, source.sin_addr, daemon->status);
    rewrite_file(daemon);
    dump(&daemon->data, daemon->file, daemon->status);
}

static void             free_res( void )
{
    setsid();
    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void                demonize(void)
{
    int             pid;

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    pid = fork();

    switch(pid)
    {
        case 0 :    init_daemon();
                    break;
        case -1 :   printf("Fork Error\n");
                    break;
        default :   exit(1);
                    break;
    }
}

