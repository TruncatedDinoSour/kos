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

#ifdef HAVE_REMEMBERAUTH
#include <sys/stat.h>
#include <fstream>
#endif

#include "./macros.hpp"
#include "./main.hpp"

#if defined HAVE_VALIDATEPASS || defined HAVE_VALIDATEGRP || \
    defined HAVE_INITGROUP
static const struct passwd *pw = getpwuid(getuid());
static const char *username    = pw ? pw->pw_name : NULL;
#endif

#ifdef HAVE_REMEMBERAUTH
// TODO: Better auth storing system
static volatile bool temp_validate_user     = false;
static volatile uid_t temp_validate_user_id = getuid();
#endif

bool is_passible_root(void) {
    // Returns true if user is already root or is authenticated

#ifdef HAVE_REMEMBERAUTH
    VER_CHECK_TIME_STAT(return false);

    if (temp_validate_user_id == ROOT_UID && getgid() == ROOT_GID &&
        geteuid() == ROOT_UID && SKIP_ROOT_AUTH)
        return true;

#ifdef HAVE_LOGGING
#define _remember_auth_dir +std::string(REMEMBER_AUTH_DIR) + "'\""
#else
#define _remember_auth_dir
#endif

    if (access(REMEMBER_AUTH_DIR, W_OK) == 0 ||
        access(REMEMBER_AUTH_DIR, R_OK) == 0) {
#ifdef HAVE_SAFE_REMEMBERAUTH
        if (access(REMEMBER_AUTH_DIR, F_OK) == 0) {
            EXIF_LOGGING(
                std::cerr
                << FATAL("Authentication failed, reason being that the "
                         "REMEMBER_AUTH_DIR is invalid\n"
                         "    Make sure the owner is root:\n"
                         "        $ su -c \"chown root:root -R "
                         "'" _remember_auth_dir "\n"
                         "    And/or the perms are 7411:\n"
                         "        $ su -c \"chmod 7411 -R '" _remember_auth_dir)
                << '\n');
            exit(EXIT_AUTH);
        }
#endif

        return false;
    }

#undef _remember_auth_dir

    return access(verpath.c_str(), F_OK) == 0;
#else
    return getuid() == ROOT_UID && getgid() == ROOT_GID &&
           geteuid() == ROOT_UID && SKIP_ROOT_AUTH;
#endif
}

#ifdef HAVE_LOGGING
inline void log_error(const std::string emsg) {
    std::cerr << "ERROR: " << emsg << '\n';
}
#endif

#ifdef HAVE_VALIDATEPASS
#ifdef HAVE_LOGGING
std::string input_no_echo(const std::string prompt)
#else
std::string internal_input_no_echo(void)
#endif
{
    /*
     * This function seems overcomplicated,
     * so as a TODO I say we leave it here
     * until we find another way to disable
     * echoing of STDIN in linux
     */

    if (std::cin.eof())
        exit(EXIT_STDIN);

    static const int stdin_num = STDIN_FILENO;
    static const bool piping   = !isatty(stdin_num);

#ifdef _STDIN_FILENO_FILENO_FN
    EXITIF_COND(WSTRERRNO("fileno(stdin) failed"), stdin_num == -1, EXIT_STDIN);
#endif

#ifndef HAVE_PIPE
    if (piping)
        exit(EXIT_NOPIPE);
#endif

    static std::string result;
    static unsigned char status = EXIT_SUCCESS;

#ifdef HAVE_NOECHO
    static struct termios term;

    if (!piping) {
        EXITIF_COND(WSTRERRNO("Failed to get tcgetattr()"),
                    tcgetattr(stdin_num, &term), EXIT_TERM);

        term.c_lflag &= (tcflag_t)~ECHO;
        tcsetattr(stdin_num, 0, &term);
    }
#endif

    EXIF_LOGGING(std::cerr << '(' << prompt << ") ");
    if (!std::getline(std::cin, result))
        status = EXIT_STDIN;

    EXIF_LOGGING(std::cout << '\n');

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

    ERRORIF_COND(FATAL("Detected invald PASSWORD_AMMOUNT_INC value (" +
                       std::to_string(PASSWORD_AMMOUNT_INC) +
                       "), will not proceed further"),
                 __times < 0);

#ifndef HAVE_INFINITE_ASK
    ERRORIF_COND("Failed to log in after " + std::to_string(__times) +
                     " atempts",
                 __times >= PASSWORD_AMMOUNT);
#endif

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
#ifdef HAVE_REMEMBERAUTH_AUTODIR
        if (access(REMEMBER_AUTH_DIR, F_OK) != 0)
            mkdir(REMEMBER_AUTH_DIR, 0711);
#endif

        std::ofstream(verpath).close();

#ifdef HAVE_SAFE_REMEMBERAUTH
        chmod(verpath.c_str(), 0600);
#endif

        temp_validate_user = false;
    }
#endif

    WERRORIF_COND("Failed to execvp()", (execvp(command[0], command)) < 0);

    return EXIT_SUCCESS;
}

#if defined HAVE_VALIDATEGRP || defined HAVE_INITGROUP
int get_group_count(const char *user = username) {
    int ngroups = 0;
    getgrouplist(user, pw->pw_gid, NULL, &ngroups);

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

#define usr std::string(strerror(errno)) + ": for user " + username

    ERRORIF_COND("Failed to get the user group count", group_count == -1);

    static gid_t *groups =
        (gid_t *)malloc(sizeof(*groups) * (unsigned long int)group_count);

    ERRORIF_COND("malloc() failed in validate_group(): " + usr, groups == NULL);

    _errorif_cln_grp(
        WSTRERRNO("Failed to get groups for user " + std::string(username)),
        getgrouplist(username, pw->pw_gid, groups, &group_count) == -1);

    mg = getgrnam(MAIN_GROUP);
    _errorif_cln_grp("Group `" + std::string(MAIN_GROUP) + "` does not exist",
                     mg == NULL);

    for (static volatile int grp = 0; grp < group_count; ++grp)
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

    WERRORIF_COND("Modifying environment failed in getpwuid()", rpw == NULL);

    // NOTE: Don't forget to change ENV_AMMOUNT in macros.h
    const char *env[ENV_AMMOUNT][2] = {{"HOME", rpw->pw_dir},
                                       {"SHELL", rpw->pw_shell},
                                       {"USER", rpw->pw_name},
                                       {"LOGNAME", rpw->pw_name}};

    for (static volatile unsigned int idx = 0; idx < ENV_AMMOUNT; ++idx)
        WERRORIF_COND("Failed to modify environment",
                      setenv(env[idx][0], env[idx][1], 1) == -1);

    return EXIT_SUCCESS;
}
#endif

#ifdef HAVE_INITGROUP
unsigned char init_groups(void) {
    static const struct passwd *rpw = getpwuid(ROOT_UID);
    static int gc                   = get_group_count(rpw->pw_name);

    ERRORIF_COND("Failed to get the root group count", gc == -1);

    gid_t *groups = (gid_t *)malloc(sizeof(*groups) * (unsigned long int)gc);
    WERRORIF_COND("malloc() failed in init_groups()", groups == NULL);

    _errorif_cln_grp(
        WSTRERRNO("Failed to get groups for user " + std::string(rpw->pw_name)),
        getgrouplist(username, pw->pw_gid, groups, &gc) == -1);

    for (static volatile int grp = 0; grp < gc; ++grp)
        if (initgroups(pw->pw_name, groups[grp]) == -1) {
            EXIF_LOGGING(std::cerr << WSTRERRNO("Failed to initgroups() for " +
                                                std::string(rpw->pw_name)));
            break;
        }

    free(groups);
    return EXIT_SUCCESS;
}
#endif

// MAIN //

#ifndef KOS_H
int main(int argc, char *argv[]) {
#ifdef HAVE_VALIDATEGRP
    if (validate_group() != EXIT_SUCCESS)
        return EXIT_FAILURE;
#endif

#if defined HAVE_VALIDATEPASS || defined HAVE_VALIDATEGRP || \
    defined HAVE_INITGROUP
    WERRORIF_COND("Failed to initialise /etc/passwd", pw == NULL);
    WERRORIF_COND("Failed to get username", username == NULL);
#endif

    ERRORIF_COND("Usage: <command> [args...]", argc < 2);

#ifdef HAVE_VALIDATEPASS
    RETIF_FAIL(validate_password() != EXIT_SUCCESS);
#endif

    /*
     * Embedding a directive within macro arguments has undefined behavior
     */

#ifdef HAVE_EFFECTIVE_ID
#define __setid_failed_bit "[e]"
#define __setid_effective_cond \
    || seteuid(ROOT_UID) == -1 || setegid(ROOT_GID) == -1
#else
#define __setid_failed_bit
#define __setid_effective_cond
#endif

    WERRORIF_COND("Setting user IDs (set" __setid_failed_bit
                  "{u,g}id()) failed",
                  setuid(ROOT_UID) == -1 ||
                      setgid(ROOT_GID) == -1 __setid_effective_cond);

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
