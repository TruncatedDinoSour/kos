#include <termios.h>
#include <unistd.h>
#include <crypt.h>
#include <shadow.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include <sys/stat.h>

#include "cli.h"
#include "auth.h"
#include "group.h"

AuthResult authenticate(const Config config, const uid_t uid) {
    char *session;
    char *password;

    struct spwd *spw;
    struct stat attr;
    struct passwd *cpw, *tpw;

    struct termios old_term, new_term;

    /* Skip root authentication */

    if ((getuid() + geteuid() + getgid() + getegid()) == 0)
        return AUTH_OK;

    /* Verify session directory permissions */

    mkdir(config.session_directory, 07411);

    if (access(config.session_directory, W_OK) == 0 ||
        access(config.session_directory, R_OK) == 0)
        return AUTH_SESSION_PERMISSIONS;

    /* Verify group */

    if ((cpw = getpwuid(getuid())) == NULL)
        return AUTH_FAILURE;

    if (config.user_group && !user_in_group(config.groups, config.groups_size,
                                            cpw->pw_name, config.user_group))
        return AUTH_GROUP;

    if (config.target_group) {
        if ((tpw = getpwuid(uid)) == NULL)
            return AUTH_FAILURE;

        if (!user_in_group(config.groups, config.groups_size, tpw->pw_name,
                           config.target_group))
            return AUTH_GROUP;
    }

    /* Get shadow entries */

    spw = getspnam(cpw->pw_name);

    if (spw == NULL)
        return AUTH_SHADOW;

    /* Skip session authentication */

    session = malloc(strlen(config.session_directory) + 32);
    sprintf(session, "%s/%u", config.session_directory, getuid());

    stat(session, &attr);

    if (difftime(time(NULL), attr.st_mtime) <= config.session_grace)
        return AUTH_OK;

    /* Verify password */

    printf(config.prompt_format, cpw->pw_name);
    fflush(stdout);

    if (!isatty(STDIN_FILENO))
        return AUTH_PIPE;

    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~((tcflag_t)(ECHO | ECHOE | ECHOK | ECHONL | ICANON));
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    password = cli_get_line();

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

    if (strcmp(crypt(password, spw->sp_pwdp), spw->sp_pwdp) == 0) {
        free(password);

        /* Creates and/or updates the session */
        close(open(session, O_CREAT | O_TRUNC | O_RDWR | O_NDELAY));
        chmod(session, 0600);

        return AUTH_OK;
    }

    free(password);
    return AUTH_FAILURE;
}

const char *authstatus_to_string(const AuthResult ar) {
    switch (ar) {
        case AUTH_OK: return "authentication was successful";
        case AUTH_SESSION_PERMISSIONS:
            return "the session or the session store directory is invalid, "
                   "make sure it is owned by root and has permissions of 7411";
        case AUTH_SHADOW:
            return "failed to get shadow entries, is your /etc/shadow file OK? "
                   "Is the Kos binary SUID?";
        case AUTH_GROUP:
            return "group authentication error - either you or the target user "
                   "are not in proper Kos groups";
        case AUTH_PIPE: return "blocked pipe";
        case AUTH_FAILURE: return "generic authentication failure";
    }

    return "unkown authentication error";
}
