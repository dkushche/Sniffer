
#include "main.h"
#include "daemon.h"

int					main()
{
    int     pid;

    pid = fork();
    switch(pid)
    {
        case 0 :    init_daemon();
                    break;
        case -1 :   printf("Fork Error\n");
                    break;
        default :   printf("Ok\n");
                    break;
    }
}
