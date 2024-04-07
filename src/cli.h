#ifndef _CLI_H
#define _CLI_H
#include <stddef.h>
#include <stdio.h>

#define cli_flags(flags)     flags, (sizeof(flags) / sizeof(flags[0]))
#define cli_argv(argv, argc) (char **)((argv) + 1), (size_t)((argc)-1)

typedef struct {
    const char *const flag;
    const char *const description;
    const unsigned char as_switch;
    const unsigned char required;
    char *value;
} CliFlag;

unsigned char cli_parse_flags(CliFlag flags[],
                              const size_t flags_size,
                              char *argv[],
                              const size_t argv_size);
void cli_print_flags(FILE *fp,
                     const CliFlag flags[],
                     const size_t flags_size,
                     const unsigned char only_required);
char *cli_get_line(void);
#endif /* _CLI_H */
