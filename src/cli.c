#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"

unsigned char cli_parse_flags(CliFlag flags[],
                              const size_t flags_size,
                              char *argv[],
                              const size_t argv_size) {
    char *arg;
    CliFlag *flag;
    size_t idx, jdx;

    for (idx = 0; idx < flags_size; ++idx) {
        flag = &flags[idx];

        if (*flag->flag != '-')
            continue;

        for (jdx = 0; jdx < argv_size; ++jdx) {
            arg = argv[jdx];

            if (*arg != '-' || strcmp(flag->flag, arg) != 0)
                continue;

            if (flag->as_switch)
                flag->value = argv[jdx];
            else if (++jdx == argv_size)
                break;

            flag->value = argv[jdx];
        }

        if (flag->required && flag->value == NULL)
            return 0;
    }

    return 1;
}

void cli_print_flags(FILE *fp,
                     const CliFlag flags[],
                     const size_t flags_size,
                     const unsigned char only_required) {
    size_t idx;

    for (idx = 0; idx < flags_size; ++idx) {
        if (only_required &&
            (!flags[idx].required || flags[idx].value != NULL)) {
            continue;
        }

        fprintf(fp, "%s%s%s - %s\n", flags[idx].flag,
                flags[idx].as_switch ? "" : " <value>",
                flags[idx].required ? " (required)" : "",
                flags[idx].description);
    }
}

char *cli_get_line(void) {
    char c;
    size_t size = 0, cap = 64;
    char *buf = malloc(cap + 1), *t;

    if (buf == NULL)
        return NULL;

    while (read(STDIN_FILENO, &c, 1) == 1 && c != '\n') {
        if (size >= cap) {
            cap += 64;
            t = realloc(buf, cap + 1);

            if (t == NULL) {
                free(buf);
                return NULL;
            }

            buf = t;
        }

        buf[size++] = c;
    }

    buf[size] = '\0';

    return buf;
}
