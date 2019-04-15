
#include "interaction_with_daemon.c"

static void         client_loop(void);
static void         parse_input(char *input, t_client *client);


static void         client_loop( void )
{
    t_client        client;
    char            *input = NULL;
    size_t          size = 0;
    int             code;

    fifo_chanel((t_abstract *)&client);
    con_printf("");
    while ((code = getline(&input, &size, stdin)) > 0)
    {
        parse_input(input, &client);
        free(input);
        input = NULL;
        con_printf("");
    }
    if (code < 0)
        fatal_err_stdin("Read from stdin", 0);
    close(client.in_chan);
    close(client.out_chan);
}

static void         parse_input(char *input, t_client *client)
{
    const char      *keys[EXIT + 1] = { "start", "stop", "show",
                                        "select_iface", "stat",
                                        "die", "--help", "exit" };
    react_on_inp    answer[EXIT + 1] = {    &send_start, &send_stop, &send_show,
                                            &send_select, &send_stat, &send_die,
                                            &show_help, &make_exit };
    char            *token;
    char            *new_l;
    int             i = -1;

    new_l = strchr(input, '\n');
    *new_l = '\0';
    token = strtok(input, " ");
    while (++i < (EXIT + 1))
    {
        if (!strcmp(token, keys[i]))
        {
            answer[i](client, strtok(NULL, " "));
            return ;
        }
    }
    con_printf("Incorrect input\n");
}
