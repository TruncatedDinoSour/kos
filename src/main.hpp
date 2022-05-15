inline bool is_passible_root(void);
inline void log_error(const std::string emsg);
int run_command(char *command[]);
unsigned char init(void);

#ifdef HAVE_VALIDATEPASS
int validate_password(amm_t __times);

#ifdef HAVE_NOECHO
std::string input_no_echo(std::string prompt, char end);
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
constexpr unsigned int sc(const char *str, int h);
bool parse_arg(const char *arg);
#endif

#ifdef HAVE_MODIFYENV
unsigned char modify_env(void);
#endif
