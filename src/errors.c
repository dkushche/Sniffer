
#include "main.h"

void            fatal_err_stdin(const char *err, char is_errno)
{
    if (!is_errno)
        printf("Fatal>: %s error\n", err);
    else
        printf("Fatal>:%s error : errno : %i\n", err, errno);
    exit(1);
}

void            err_log( const char *err, int status_fd, char fatal )
{
	time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    dprintf(status_fd, "%s daemon_error>%s\n", buffer, err);
    if (fatal)
	    exit(1);
}
