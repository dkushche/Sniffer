
#include "ip_vector.h"

static void         reallocate(t_ip_vector *this, int status_fd);
static void         init_elem(t_elem *obj, struct in_addr new_ip);
static void         without_shifts(t_ip_vector *this, int status_fd,
                            unsigned int pos, struct in_addr new_ip);
static void         with_shifts(t_ip_vector *this, int status_fd,
                            unsigned int pos, struct in_addr new_ip);
static void         push_new_in_pos(t_ip_vector *this, int status_fd,
                            unsigned int pos, struct in_addr new_ip);
static void         binary_search(t_ip_vector *this,
                            int status_fd, struct in_addr new_ip);
static void         vector_err(const char *err, int status_fd);

t_ip_vector         *create_vector(t_ip_vector *in_stack)
{
    t_ip_vector     *res;

    res = (in_stack) ? in_stack : (t_ip_vector *)malloc(sizeof(t_ip_vector));
    res->size = 0;
    res->fiz_size = 256;
    res->array = (t_elem *)malloc(sizeof(t_elem) * res->fiz_size);
    memset(res->array, 0, res->fiz_size * sizeof(t_elem));
    return res;
}

void                push(t_ip_vector *this, struct in_addr new_ip, int status_fd)
{
    if (!this->size)
        push_new_in_pos(this, status_fd, 0, new_ip);
    else
        binary_search(this, status_fd, new_ip);
}

void                dump(t_ip_vector *this, int fd, int status_fd)
{
    unsigned int    buf = 0;

    for (unsigned int i = 0; i < this->size; i++) {
        dprintf(fd, "IP : %s ; packages : %lu \n",
                inet_ntoa(this->array[i].ip), this->array[i].n_of_pack);
        if (buf > this->array[i].ip.s_addr)
            vector_err("Sort Error", status_fd);
        buf = this->array[i].ip.s_addr;
    }
}

void                push_back(t_ip_vector *vector, int status_fd,
                                struct in_addr ip, unsigned long int n_of_pack)
{
    without_shifts(vector, status_fd, vector->size, ip);
    vector->array[vector->size - 1].n_of_pack = n_of_pack;
}

void                destroy_vector(t_ip_vector *vector, char in_stack)
{
    free(vector->array);
    if (!in_stack)
        free(vector);
}

static void         binary_search(t_ip_vector *this,
                                int status_fd, struct in_addr new_ip)
{
    unsigned int    start = 0;
    unsigned int    end = this->size - 1;
    unsigned int    now;

    while (1)
    {
        now = (start + end) >> 1;
        if (new_ip.s_addr == this->array[now].ip.s_addr)
        {
            this->array[now].n_of_pack++;
            break;
        }
        if (start == end || start > end)
        {
            if (new_ip.s_addr > this->array[now].ip.s_addr)
                push_new_in_pos(this, status_fd, now + 1, new_ip);
            else
                push_new_in_pos(this, status_fd, now, new_ip);
            break;
        }
        if (new_ip.s_addr > this->array[now].ip.s_addr)
            start = now + 1;
        else
            end = (now) ? (now - 1) : 0;
    }
}

unsigned long int    searcher(t_ip_vector *this,
                                        struct in_addr who)
{
    unsigned int    start = 0;
    unsigned int    end = this->size - 1;
    unsigned int    now;

    while (1) {
        now = (start + end) >> 1;
        if (who.s_addr == this->array[now].ip.s_addr)
            return this->array[now].n_of_pack;
        if (start == end || start > end)
            return 0;
        if (who.s_addr > this->array[now].ip.s_addr)
            start = now + 1;
        else
            end = (now) ? (now - 1) : 0;
    }
}

static void         reallocate(t_ip_vector *this, int status_fd)
{
    t_elem          *buf;

    buf = realloc(this->array,
                           this->fiz_size * sizeof(t_elem) << 1);

    if (buf == NULL)
        vector_err("Can't realloc enough memory", status_fd);

    this->array = buf;
    this->fiz_size <<= 1;
}

static void         init_elem(t_elem *obj, struct in_addr new_ip)
{
    obj->ip.s_addr = new_ip.s_addr;
    obj->n_of_pack = 1;
}

static void         without_shifts(t_ip_vector *this, int status_fd,
                            unsigned int pos, struct in_addr new_ip)
{
    if (pos >= this->fiz_size)
        reallocate(this, status_fd);

    init_elem(this->array + pos, new_ip);
    this->size++;
}

static void         with_shifts(t_ip_vector *this, int status_fd,
                            unsigned int pos, struct in_addr new_ip)
{
    if( this->size + 1 >= this->fiz_size)
        reallocate(this, status_fd);

    memcpy(this->array + pos + 1, this->array + pos,
                (this->size - pos) * sizeof(t_elem));
    init_elem(&this->array[pos], new_ip);
    this->size++;
}

static void         push_new_in_pos(t_ip_vector *this, int status_fd,
                            unsigned int pos, struct in_addr new_ip)
{
    if (pos != this->size)
        with_shifts(this, status_fd, pos, new_ip);
    else
        without_shifts(this, status_fd, pos, new_ip);
}


static void         vector_err(const char *err, int status_fd)
{

    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    dprintf(status_fd, "%s daemon_error>%s\n", buffer, err);
	exit(1);
}

void                restart(t_ip_vector *this)
{
    free(this->array);
    create_vector(this);
}