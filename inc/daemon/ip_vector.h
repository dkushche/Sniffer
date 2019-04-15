
# ifndef IP_VECTOR_H
# define IP_VECTOR_H

# include "main.h"

typedef struct              elem
{
    struct in_addr          ip;
    unsigned long int       n_of_pack;
}                           t_elem;

typedef struct      ip_vector
{
    t_elem          *array;
    size_t          size;
    size_t          fiz_size;
}                   t_ip_vector;

t_ip_vector         *create_vector(t_ip_vector *in_stack);
void                push(t_ip_vector *this, struct in_addr new_ip, int status_fd);
void                dump(t_ip_vector *this, int fd, int status_fd);
void                destroy_vector(t_ip_vector *vector, char in_stack);
unsigned long int   searcher(t_ip_vector *this, struct in_addr who);
void                push_back(t_ip_vector *vector, int status_fd,
                                struct in_addr ip, unsigned long int n_of_pack);
void                restart(t_ip_vector *vector);

#endif
