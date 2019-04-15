
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
static char         algorithm(t_ip_vector *this, struct in_addr ip, unsigned int *now);

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
            err_log("Sort Error", status_fd, 1);
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

static char         algorithm(t_ip_vector *this, struct in_addr ip, unsigned int *now)
{
    unsigned int    start = 0;
    unsigned int    end = this->size - 1;

    while (1)
    {
        *now = (start + end) >> 1;
        if (ip.s_addr == this->array[*now].ip.s_addr)
            return 1;
        if (start == end || start > end)
        {
            if (ip.s_addr > this->array[*now].ip.s_addr)
            {
                (*now)++;
                return 0;
            }
            else
                return 0;
        }
        if (ip.s_addr > this->array[*now].ip.s_addr)
            start = *now + 1;
        else
            end = (*now) ? (*now - 1) : 0;
    }
}

static void         binary_search(t_ip_vector *this,
                                int status_fd, struct in_addr new_ip)
{
    char            status;
    unsigned int    now;

    status = algorithm(this, new_ip, &now);
    if (status)
        this->array[now].n_of_pack++;
    else
        push_new_in_pos(this, status_fd, now, new_ip);
}

unsigned long int    searcher(t_ip_vector *this,
                                        struct in_addr who)
{
    char            status;
    unsigned int    now;

    status = algorithm(this, who, &now);
    if (status)
        return this->array[now].n_of_pack;
    else
        return 0;
}

static void         reallocate(t_ip_vector *this, int status_fd)
{
    t_elem          *buf;

    buf = realloc(this->array,
                           this->fiz_size * sizeof(t_elem) << 1);

    if (buf == NULL)
        err_log("Can't realloc enough memory", status_fd, 1);

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

void                restart(t_ip_vector *this)
{
    free(this->array);
    create_vector(this);
}
