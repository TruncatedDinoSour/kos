#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#include <syscall.h>
#include <sys/types.h>

#include "cmd.h"

extern char **environ;

unsigned char switch_user(uid_t uid) {
    struct passwd *pw = getpwuid(uid);

    return setuid(pw->pw_uid) == 0 && setgid(pw->pw_gid) == 0 &&
           kos_seteuid(pw->pw_uid) == 0 && kos_setegid(pw->pw_gid) == 0;
}

unsigned char inherit_environment(uid_t uid) {
    unsigned char idx;
    struct passwd *pw;
    const char *keys[4] = {"HOME", "SHELL", "USER", "LOGNAME"};
    char *values[sizeof(keys) / sizeof(keys[0])];

    pw = getpwuid(uid);

    values[0] = pw->pw_dir;
    values[1] = pw->pw_shell;
    values[2] = pw->pw_name;
    values[3] = pw->pw_name;

    for (idx = 0; idx < sizeof(keys) / sizeof(keys[0]); ++idx)
        if (!set_env(keys[idx], values[idx]))
            return 0;

    return 1;
}

unsigned char inherit_groups(uid_t uid) {
    (void)uid;
    return 0;
}

void run_command(char **command) { (void)command; }

unsigned char set_env(const char *name, const char *value) {
    size_t n = 0, idx;
    char **new_environ;
    char *eqpos;

    while (environ[n])
        n++;

    new_environ = malloc(sizeof(char *) * (n + 2));
    if (new_environ == NULL)
        return 0;

    eqpos = malloc(strlen(name) + strlen(value) + 2);
    if (eqpos == NULL) {
        free(new_environ);
        return 0;
    }

    strcpy(eqpos, name);
    strcat(eqpos, "=");
    strcat(eqpos, value);

    new_environ[0] = eqpos;

    for (idx = 0; idx < n; ++idx)
        new_environ[idx + 1] = environ[idx];

    new_environ[n + 1] = NULL;

    environ = new_environ;

    return 1;
}
