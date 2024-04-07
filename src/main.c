#include "config.h"

#include <stdlib.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>

#include "auth.h"
#include "main.h"
#include "group.h"
#include "cli.h"
#include "cmd.h"

#define __kos_to_string(v) #v

static CliFlag flags[AppFlag_count] = {
    /* AppFlag_help */ {"-help", "Print the help page", 1, 0, NULL},
    /* AppFlag_uid */
    {"-uid", "The UID (User ID) of the user you want to execute the command as",
     0, 1, __kos_to_string(DEFAULT_UID)},
    /* AppFlag_user */
    {"-user", "The username of the user (sets `uid` implicitly)", 0, 0, NULL},
    /* AppFlag_forget */
    {"-forget", "Forget the session save, so it'd ask the password next time",
     1, 0, NULL},
    /* AppFlag_noenv */
    {"-noenv", "Don't save the environment variables", 1, 0, NULL},
    /* AppFlag_quiet */ {"-quiet", "Disable logging", 1, 0, NULL},
    /* AppFlag_verbose */ {"-verbose", "Enable verbose logging", 1, 0, NULL},
    /* AppFlag_echo */ {"-echo", "Enable password echoing", 1, 0, NULL},
};

static void print_help(FILE *fp) {
    fprintf(fp,
            "Kos version %s - a simple SUID tool written in C.\n"
            "Usage: kos [flags] <command...>\n\n",
            version);
    cli_print_flags(fp, cli_flags(flags), 0);
}

int main(const int argc, char *argv[]) {
    static long int flag = DEFAULT_UID;
    static struct passwd *pw;
    static AuthResult ar;

    static Config config = {
        DEFAULT_UID,
        SESSION_DIRECTORY,
        SESSION_GRACE,
        KOS_USER_GROUP,
        KOS_TARGET_GROUP,
        PROMPT_FORMAT_BE_CAREFUL,
        NULL,
        0,
    };

    if (argc < 2) {
        print_help(stderr);
        return 1;
    }

    if (!cli_parse_flags(cli_flags(flags), cli_argv(argv, argc))) {
        fputs("Flag parsing error. Are you sur4e you specified the "
              "(required) flags?\n\n",
              stderr);
        print_help(stderr);
        return 1;
    }

    if (flags[AppFlag_help].value != NULL) {
        print_help(stdout);
        return 0;
    }

    if (flags[AppFlag_user].value != NULL) {
        pw = getpwnam(flags[AppFlag_user].value);

        if (pw == NULL) {
            fprintf(stderr, "%s is not a valid user.\n",
                    flags[AppFlag_user].value);
            return 1;
        }

        flag = pw->pw_uid;
    } else {
        flag = atol(flags[AppFlag_uid].value);

        if (flag < 0) {
            fprintf(stderr, "%ld is not a valid user id.\n", flag);
            return 1;
        }
    }

    config.groups = load_groups(&config.groups_size);

    if ((ar = authenticate(config, (uid_t)flag)) != AUTH_OK) {
        fprintf(stderr, "Authentication failure (%d): %s (%s)\n", ar,
                authstatus_to_string(ar), strerror(errno));
        free(config.groups);
        return 1;
    }

    free(config.groups);

    if (!switch_user((uid_t)flag)) {
        perror("Failed to switch_user");
        return 1;
    }

    if (!inherit_environment((uid_t)flag)) {
        perror("Failed to inherit_environment");
        return 1;
    }

    return 0;
}
