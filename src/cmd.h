#ifndef _CMD_H
#define _CMD_H
#include <sys/types.h>

typedef struct {
    const char *const name;
    char *value;
} Environment;

unsigned char switch_user(uid_t uid);
unsigned char inherit_environment(uid_t uid);
unsigned char inherit_groups(uid_t uid);
void run_command(char **command);

unsigned char set_env(const char *name, const char *value);
extern int kos_seteuid(uid_t uid);
extern int kos_setegid(uid_t gid);
#endif /* _CMD_H */
