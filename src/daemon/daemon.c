
#include "daemon.h"
#include "device_list.h"
#include "daemon_filesys.c"
#include "daemon_sock.c"
#include "interaction_with_client.c"

static void             chan_interact(t_sniffer *this, char make_block);
static void             sniff_time( t_sniffer *daemon, unsigned char *buf );
static void             free_res( void );
static void             read_chanel( t_sniffer *daemon );
static void             daemon_loop( t_sniffer *daemon );

void                    init_daemon( void )
{
    t_sniffer       daemon;

    daemon.status = open_status();
    free_res();
    daemon.devices = NULL;
    daemon.sock_raw = -1;
	daemon.data.array = NULL;
    download_devices(&daemon);
	if (bind_to_device(&daemon, DEFAULT) < 0)
		bind_to_device(&daemon, MY);
    fifo_chanel((t_abstract *)&daemon);
    chan_interact(&daemon, 0);
    daemon_loop(&daemon);
    destroy_vector(&daemon.data, 1);
    sock_close(&daemon);
    close(daemon.out_chan);
    close(daemon.in_chan);
}

static void             daemon_loop( t_sniffer *daemon )
{
    unsigned char   *buf;

    daemon->is_working = 1;
    buf = (unsigned char *)malloc(sizeof(unsigned char) * BUFF_SIZE);
    while (1)
    {
        read_chanel(daemon);
        while (daemon->is_working) {
            sniff_time(daemon, buf);
            read_chanel(daemon);
        }
    }
    free(buf);
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
        chan_interact(this, 1);
        interact_func       functions[DIE + 1] = {&react_start, &react_stop, &react_show,
                                                &react_select, &react_stat, &react_die};

        functions[cmd](this);
        chan_interact(this, 0);
    }
}

static void             sniff_time( t_sniffer *daemon, unsigned char *buf )
{
    struct sockaddr     saddr;
    socklen_t           saddr_len = sizeof (saddr);
    struct sockaddr_in  source;

    if (recvfrom(daemon->sock_raw, buf, BUFF_SIZE, 0, &saddr, &saddr_len) < 0)
    {
        if (errno == EAGAIN)
            return;
        else
            err_log("Reciving error", daemon->status, 1);
    }
    memset(&source, 0, sizeof(source));
    struct iphdr *ip = (struct iphdr*)(buf + sizeof(struct ethhdr));

    source.sin_addr.s_addr = ip->saddr;

    push(&daemon->data, source.sin_addr, daemon->status);
    rewrite_file(daemon);
    dump(&daemon->data, daemon->file, daemon->status);
}

static void             chan_interact( t_sniffer *this, char make_block )
{
    int flags = fcntl(this->in_chan, F_GETFL, 0);

    if (make_block)
        fcntl(this->in_chan, F_SETFL, flags & ~O_NONBLOCK);
    else
        fcntl(this->in_chan, F_SETFL, flags | O_NONBLOCK);
}

static void             free_res( void )
{
    setsid();
    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
}
