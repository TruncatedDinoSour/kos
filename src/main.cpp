// -D_KOS_VERSION_="..."
#ifndef _KOS_VERSION_
#define _KOS_VERSION_ "(unknown)"
#endif

#include "./config.h"

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#ifdef HAVE_LOGGING
#include <cerrno>
#endif

#ifdef HAVE_NOECHO
#include <termios.h>
#endif

#ifdef HAVE_VALIDATEPASS
#include <shadow.h>
#endif

#ifdef HAVE_MODIFYENV
#include <unordered_map>
#endif

#ifdef HAVE_REMEMBERAUTH
#include <sys/stat.h>
#include <fstream>
#endif

#include "./macros.hpp"
#include "./main.hpp"

static const struct passwd *pw = getpwuid(getuid());
static const char *username    = pw ? pw->pw_name : NULL;

#ifdef HAVE_REMEMBERAUTH
static bool temp_validate_user     = false;
static uid_t temp_validate_user_id = getuid();
#endif

inline bool is_passible_root(void) {
    // Returns true if user is already root or is authenticated

#ifdef HAVE_REMEMBERAUTH
    VER_CHECK_TIME_STAT(return false);

    if (temp_validate_user_id == ROOT_UID && getgid() == ROOT_GID &&
        geteuid() == ROOT_UID && SKIP_ROOT_AUTH)
        return true;

    if (access(REMEMBER_AUTH_DIR, W_OK) == 0 ||
        access(REMEMBER_AUTH_DIR, R_OK) != 0) {
        if (access(REMEMBER_AUTH_DIR, F_OK) == 0) {
            std::cerr << "FATAL: Authentication failed, reason being that the "
                         "REMEMBER_AUTH_DIR is invalid"
                      << '\n';
            exit(EXIT_AUTH);
        }

        return false;
    }

    return access(verpath.c_str(), F_OK) == 0;
#else
    return getuid() == ROOT_UID && getgid() == ROOT_GID &&
           geteuid() == ROOT_UID && SKIP_ROOT_AUTH;
#endif
}

inline void log_error(const std::string emsg) {
    std::cerr << "ERROR: " << emsg << '\n';
}

#ifdef HAVE_VALIDATEPASS
std::string input_no_echo(const std::string prompt) {
    /*
     * This function seems overcomplicated,
     * so as a TODO I say we leave it here
     * until we find another way to disable
     * echoing of STDIN in linux
     */

    if (std::cin.eof())
        exit(EXIT_FAILURE);

    static const int stdin_num = fileno(stdin);
    static const bool piping   = !isatty(stdin_num);

    EXITIF_COND("fileno(stdin) failed: " + strerrno, stdin_num == -1,
                EXIT_STDIN);

#ifndef HAVE_PIPE
    if (piping)
        exit(EXIT_NOPIPE);
#endif

    static std::string result;
    static unsigned char status = EXIT_SUCCESS;

#ifdef HAVE_NOECHO
    static struct termios term;

    if (!piping) {
        EXITIF_COND("Failed to get tcgetattr(): " + strerrno,
                    tcgetattr(stdin_num, &term), EXIT_TERM);

        term.c_lflag &= (tcflag_t)~ECHO;
        tcsetattr(stdin_num, 0, &term);
    }
#endif

    std::cerr << '(' << prompt << ") ";
    if (!std::getline(std::cin, result))
        status = EXIT_STDIN;

    std::cout << '\n';

#ifdef HAVE_NOECHO
    if (!piping) {
        term.c_lflag |= ECHO;
        tcsetattr(stdin_num, 0, &term);
    }
#endif

    if (status != EXIT_SUCCESS)
        exit(status);

    return result;
}

int validate_password(amm_t __times = 0) {
    if (is_passible_root())
        return EXIT_SUCCESS;

    ERRORIF_COND("Negative or zero value for PASSWORD_AMMOUNT",
                 PASSWORD_AMMOUNT <= 0);

    ERRORIF_COND("FATAL: Detected invald PASSWORD_AMMOUNT_INC value (" +
                     std::to_string(PASSWORD_AMMOUNT_INC) +
                     "), will not proceed further",
                 __times < 0);

    ERRORIF_COND("Failed to log in after " + std::to_string(__times) +
                     " atempts",
                 __times >= PASSWORD_AMMOUNT && !INFINITE_ASK);

    struct spwd *spw;

    ERRORIF_COND("Failed to get shadow entry for `" + std::string(username) +
                     "`: " + strerror(errno),
                 (spw = getspnam(username)) == NULL);

    const std::string cleartext_password =
        input_no_echo("password for " + std::string(username));

    if (strcmp(crypt(cleartext_password.c_str(), spw->sp_pwdp), spw->sp_pwdp)) {
        log_error("Access denied: Wrong password");
        return validate_password(__times + PASSWORD_AMMOUNT_INC);
    }

#ifdef HAVE_REMEMBERAUTH
    temp_validate_user    = true;
    temp_validate_user_id = getuid();
#endif

    return EXIT_SUCCESS;
}
#endif

int run_command(char *command[]) {
#ifdef HAVE_REMEMBERAUTH
    VER_CHECK_TIME_STAT(remove(verpath.c_str()));

    if (temp_validate_user) {
        if (access(REMEMBER_AUTH_DIR, F_OK) != 0)
            mkdir(REMEMBER_AUTH_DIR, 0755);

        std::ofstream(verpath).close();
        temp_validate_user = false;
    }
#endif

    ERRORIF_COND("Failed to execvp(): " + strerrno,
                 (execvp(command[0], command)) < 0);

    return EXIT_SUCCESS;
}

#if defined HAVE_VALIDATEGRP || defined HAVE_INITGROUP
int get_group_count(const char *user = username) {
    int ngroups = 0;

    gid_t *groups = (gid_t *)malloc(sizeof(gid_t *));

#define _errorif(msg, cond) \
    if (cond) {             \
        log_error(msg);     \
        return -1;          \
    }

    _errorif("malloc(): " + strerrno, groups == NULL);

    getgrouplist(user, pw->pw_gid, groups, &ngroups);
    free(groups);

#undef _errorif

    return ngroups;
}
#endif

#ifdef HAVE_VALIDATEGRP
unsigned char validate_group(void) {
    if (is_passible_root())
        return EXIT_SUCCESS;

    static int group_count  = get_group_count();
    static bool is_in_group = false;

    static struct group *mg;

#define usr strerrno + ": for user " + username

    ERRORIF_COND("Failed to get the user group count", group_count == -1);

    static gid_t *groups =
        (gid_t *)malloc(sizeof(*groups) * (unsigned long int)group_count);
    ERRORIF_COND("malloc() failed in validate_group(): " + usr, groups == NULL);

    _errorif_cln_grp("Failed to get groups for user " + std::string(username) +
                         ": " + strerrno,
                     getgrouplist(username, pw->pw_gid, groups, &group_count) ==
                         -1);

    mg = getgrnam(MAIN_GROUP);
    _errorif_cln_grp("Group `" + std::string(MAIN_GROUP) + "` does not exist",
                     mg == NULL);

    for (static int grp = 0; grp < group_count; ++grp)
        if (groups[grp] == mg->gr_gid) {
            is_in_group = true;
            break;
        }

    free(groups);

#undef usr

    ERRORIF_COND("Permission denied: you are not a part of the `" +
                     std::string(MAIN_GROUP) + "` group.",
                 !is_in_group);
    return EXIT_SUCCESS;
}
#endif

#ifdef HAVE_MODIFYENV
unsigned char modify_env(void) {
    static const struct passwd *rpw = getpwuid(ROOT_UID);

    ERRORIF_COND("Modifying environment failed in getpwuid(): " + strerrno,
                 rpw == NULL);

    std::unordered_map<const char *, char *> env = {{"HOME", rpw->pw_dir},
                                                    {"SHELL", rpw->pw_shell},
                                                    {"USER", rpw->pw_name},
                                                    {"LOGNAME", rpw->pw_name}};

    for (const auto entry : env)
        ERRORIF_COND("Failed to modify environment: " + strerrno,
                     setenv(entry.first, entry.second, 1) == -1);

    return EXIT_SUCCESS;
}
#endif

#ifdef HAVE_INITGROUP
unsigned char init_groups(void) {
    static const struct passwd *rpw = getpwuid(ROOT_UID);
    static int gc                   = get_group_count(rpw->pw_name);

    ERRORIF_COND("Failed to get the root group count", gc == -1);

    gid_t *groups = (gid_t *)malloc(sizeof(*groups) * (unsigned long int)gc);
    ERRORIF_COND("malloc() failed in init_groups(): " + strerrno,
                 groups == NULL);

    _errorif_cln_grp("Failed to get groups for user " +
                         std::string(rpw->pw_name) + ": " + strerrno,
                     getgrouplist(username, pw->pw_gid, groups, &gc) == -1);

    for (static int grp = 0; grp < gc; ++grp)
        if (initgroups(pw->pw_name, groups[grp]) == -1) {
            std::cerr << "Failed to initgroups() for " +
                             std::string(rpw->pw_name) + ": " + strerrno;
            break;
        }

    free(groups);
    return EXIT_SUCCESS;
}
#endif

#ifdef HAVE_ARG
constexpr inline unsigned int sc(const char *str, int h = 0) {
    return !str[h] ? 5381 : (sc(str, h + 1) * 33) ^ (unsigned int)str[h];
}

bool parse_arg(const char *arg) {
    if (!arg || arg[0] != '-')
        return false;

    switch (sc(arg)) {
        case sc("--version"):
            std::cout << "Kos version v" << _KOS_VERSION_ << '\n';
            break;
        default:
            log_error(std::string(arg) + ": flag not found");
            exit(EXIT_FLAG);
    }

    return true;
}
#endif

// MAIN //

#ifndef KOS_H
int main(int argc, char *argv[]) {
#ifndef HAVE_LOGGING
    std::cout.rdbuf(nullptr);
    std::cerr.rdbuf(nullptr);
#endif

#ifdef HAVE_VALIDATEGRP
    if (validate_group() != EXIT_SUCCESS)
        return EXIT_FAILURE;
#endif

#ifdef HAVE_ARG
#define _help_arg "command|flag"
    if (parse_arg(argv[1]))
        return EXIT_SUCCESS;
#else
#define _help_arg "command"
#endif

    ERRORIF_COND("Failed to initialise /etc/passwd: " + strerrno, pw == NULL);
    ERRORIF_COND("Failed to get username: " + strerrno, username == NULL);

    ERRORIF_COND("Usage: <" _help_arg "> [args...]", argc < 2);

#ifdef HAVE_VALIDATEPASS
    RETIF_FAIL((validate_password() != EXIT_SUCCESS));
#endif

    ERRORIF_COND("Set{g/u}id() failed: " + strerrno,
                 setuid(ROOT_UID) == -1 || setgid(ROOT_GID) == -1);

#ifdef HAVE_INITGROUP
    RETIF_FAIL(init_groups())
#endif

#ifdef HAVE_MODIFYENV
    RETIF_FAIL(modify_env());
#endif

    argv++;
    return run_command(argv);
}
#endif
