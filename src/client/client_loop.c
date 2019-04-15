
#include "interaction_with_daemon.c"

static void         send_mess(int id, char *spec, t_client *client);
static void         client_loop(void);
static void         parse_input(char *input, t_client *client);


static void         client_loop( void )
{
    t_client        client;
    char            *input = NULL;
    size_t          size = 0;
    int             code;

    fifo_chanel((t_abstract *)&client);
    CL;
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
            {CL;printf("Stat\n");}
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

