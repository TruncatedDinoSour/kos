#ifndef _GROUP_H
#define _GROUP_H
#include <sys/types.h>

char *load_groups(size_t *size);
unsigned char user_in_group(const char *groups,
                            const size_t size,
                            const char *user,
                            const char *group);
#endif /* _GROUP_H */
