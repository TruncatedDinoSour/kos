inline char *get_username(void);
inline bool is_passible_root(void);
inline void log_error(const std::string emsg);
int run_command(char *command[]);
int init(void);

#ifdef HAVE_VALIDATEPASS
int validate_password(amm_t __times);

#ifdef HAVE_NOECHO
std::string input_no_echo(std::string prompt, char end);
#endif
#endif

#ifdef HAVE_VALIDATEGRP
int get_group_count(void);
int validate_group(void);
#endif

#ifdef HAVE_ARG
constexpr unsigned int sc(const char *str, int h);
bool parse_arg(const char *arg);
#endif

#ifdef HAVE_MODIFYENV
int modify_env(const struct passwd *pw);
#endif
