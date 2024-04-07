#ifndef _CONFIG_H
#define _CONFIG_H

#include <time.h>
#include <sys/types.h>

/*
 * Build-time configuration for Kos
 * ! NOTE: Be careful! This configuration is very sensitive. !
 * Use #undef and/or #define when configuring.
 */

static const char *const version =
    "44.0.0"; /* The version of Kos. Don't touch this
      (unless you're the person updating it). */

#define DEFAULT_UID 0 /* The default UID that Kos uses */

#define SESSION_DIRECTORY "/var/db/kos-dev" /* The session store directory */
#define SESSION_GRACE     600               /* The session grace period */

#define KOS_USER_GROUP                                                    \
    "kos" /* The group users need to be a part of to use Kos. Set NULL to \
             disable user group check */
#define KOS_TARGET_GROUP                                                  \
    NULL /* The group users need to be a part of to be usable by Kos. Set \
            NULL to disable target user check */

#define PROMPT_FORMAT_BE_CAREFUL                                             \
    "(password for %s) " /* The 'ask password' format. NOTE: Be VERY careful \
                           with formats!!! */

/* Feel free to use scripts and replace the comment with your own config (such
 * as through `sed`). This will never change: */

/* user_config */

typedef struct {
    const uid_t default_uid;
    const char *const session_directory;
    const time_t session_grace;
    const char *const user_group;
    const char *const target_group;
    const char *const prompt_format;

    char *groups;
    size_t groups_size;
} Config;
#endif /* _CONFIG_H */
