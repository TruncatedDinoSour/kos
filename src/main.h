#ifndef _MAIN_H
#define _MAIN_H
#include "config.h"
#include <stdio.h>

#ifdef ENABLE_FLAGS
#include "cli.h"
#endif /* ENABLE_FLAGS */

int main(const int, char *[]);

enum AppFlag {
    AppFlag_help = 0,
    AppFlag_uid,
    AppFlag_user,
    AppFlag_forget,
    AppFlag_noenv,
    AppFlag_quiet,
    AppFlag_verbose,
    AppFlag_echo,
    AppFlag_count
};

static void print_help(FILE *fp);
#endif /* _MAIN_H */
