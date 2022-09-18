#pragma once

inline bool is_passible_root(void);

#ifdef HAVE_LOGGING
inline void log_error(const std::string emsg);
#else
#define log_error(msg)
#endif

int run_command(char *command[]);

#ifdef HAVE_VALIDATEPASS
int validate_password(amm_t __times);

#ifdef HAVE_NOECHO
#ifdef HAVE_LOGGING
std::string input_no_echo(const std::string prompt);
#else
std::string internal_input_no_echo(void);
#define input_no_echo(prompt) internal_input_no_echo();
#endif
#endif
#endif

#if defined HAVE_VALIDATEGRP || defined HAVE_INITGROUP
int get_group_count(const char *user);
unsigned char validate_group(void);
#endif

#ifdef HAVE_INITGROUP
unsigned char init_groups(void);
#endif

#ifdef HAVE_ARG
constexpr inline unsigned int sc(const char *str, int h);
bool parse_arg(const char *arg);
#endif

#ifdef HAVE_MODIFYENV
unsigned char modify_env(void);
#endif
