
#include "client.h"
#include "client_loop.c"

static int          check_sniffer( void );
static void         animate_daemon( void );

int                 main( void )
{
    int             out_chan;
    int             in_chan;

    out_chan = mkfifo(FIFO_CL_DAE_CHAN, 777);
    if (out_chan < 0 && errno != EEXIST)
        fatal_err_stdin("Creating FIFO", 1);
    in_chan = mkfifo(FIFO_DAE_CL_CHAN, 777);
    if (in_chan < 0 && errno != EEXIST)
        fatal_err_stdin("Creating FIFO", 1);
    if (check_sniffer()) {
        printf("Connecting with daemon\n");
        client_loop();
    }
    else
        animate_daemon();
    return 0;
}

static void         animate_daemon( void )
{
    int     pid;

    printf("Daemon isn't alive\nAnimate it\n");
    pid = fork();
    switch(pid)
    {
        case 0 :    demonize();
                    break;
        case -1 :   printf("Fork Error\n");
                    break;
        default :   client_loop();
                    break;
    }
}

static int          check_sniffer( void )
{
    int             fpid = open(DAE_PID, O_RDONLY);
    unsigned int    pid;

    if (fpid < 0 && errno == ENOENT)
        return 0;
    else if (fpid < 0)
        fatal_err_stdin("Can't check pid_file(", 0);

    int count;
    
    count = read(fpid, &pid, sizeof(unsigned int));
    if (count != sizeof(unsigned int))
        fatal_err_stdin("Read PID", 0);
    close(fpid);
    if (kill(pid, 0) < 0)
        return 0;
    else
        return 1;
}

