#include <grp.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "group.h"

char *load_groups(size_t *size) {
    int fd;
    char *content;

    fd = open("/etc/group", O_RDONLY);

    if (fd < 0)
        return 0;

    *size = (size_t)lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    content = malloc((*size) + 1);

    if (content == NULL)
        return NULL;

    content[read(fd, content, *size)] = '\0';
    close(fd);

    return content;
}

/* Checks if a user is in a supplimentary group */
unsigned char user_in_group(const char *groups,
                            const size_t size,
                            const char *user,
                            const char *group) {
    size_t idx = 0;
    size_t us, gs;
    size_t gidx;

    const char *users_start;
    char *users_end;
    char *found_user;

    us = strlen(user);
    gs = strlen(group);

    if (size == 0 || us == 0 || gs == 0)
        return 0;

    for (gidx = 0; gidx < size; ++gidx) {
        /* Find line */

        while (groups[gidx] != '\n' && gidx < size)
            ++gidx;

        /* Find group */

        if (strncmp(&groups[idx], group, gs) == 0 && groups[idx + gs] == ':') {
            idx = idx + gs + 3; /* Skip `group:x:`, the 3 is `:x:` */

            /* Skip the group ID */

            while (groups[idx] != ':' && idx < size)
                ++idx;

            idx++; /* Skip ":" */

            /* Find the EXACT user in users list (if exists) */

            users_start = &groups[idx];
            users_end   = strchr(users_start, '\n');
            found_user  = strstr(users_start, user);

            /* User string should not be NULL, user string should be before end
               of line, user string should match exactly (hence checking next
               character) */

            if (found_user != NULL && found_user < users_end &&
                (found_user[us] == ',' || found_user[us] == '\n'))
                return 1;
        }

        /* Set the last line position */
        idx = gidx + 1;
    }

    return 0;
}

/*
 * A C89 compatible initgroups() implementation using setgroups(),
 * takes the groups of one user and initializes them on the current running
 * process although keeping the current available groups to the current user
 */
unsigned char
init_user_groups(const char *groups, const size_t size, const char *whose) {
    gid_t *group;
    int ngroups;
    long ngroups_max;

    ngroups_max = sysconf(_SC_NGROUPS_MAX) + 1;
    group       = (gid_t *)malloc(ngroups_max * sizeof(gid_t));

    ngroups = getgroups(ngroups_max, group);

    if (ngroups < 0) {
        free(group);
        return 0;
    }

    /* Find the groups that `whose` user is in, update the `group` list, and then call `setgroups()` */

    free(group);
    return 1;
}
