#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <termios.h>
#include <sys/wait.h>
#include <unistd.h>
#include <shadow.h>
#include <pwd.h>
#include <grp.h>

#include "./config.h"

#ifdef HAVE_MODIFYENV
#include <unordered_map>
#endif

// VERSION //
#define VERSION "v0.0.13"

// FD //

inline char *get_username(void);
inline bool is_passible_root(void);
int validate_password(amm_t __times);
std::string input_no_echo(std::string prompt, char end);
inline void log_error(std::string emsg);
int run_command(char *command[]);
int get_group_count(void);
int validate_group(void);
int init(void);

#ifdef HAVE_MODIFYENV
int modify_env(const struct passwd *pw);
#endif

// MACROS //

#define RETIF_FAIL(ret)      \
    if (ret != EXIT_SUCCESS) \
        return EXIT_FAILURE;

#define ERRORIF_COND(emsg, cond) \
    if (cond) {                  \
        log_error(emsg);         \
        return EXIT_FAILURE;     \
    }

#define strerrno std::string(strerror(errno))

#define _errorif_cln_grp(msg, cond) \
    if (cond) {                     \
        free(groups);               \
        log_error(msg);             \
        return EXIT_FAILURE;        \
    }

// IMPL //

inline char *get_username(void) {
    struct passwd *pw = getpwuid(getuid());
    return pw ? pw->pw_name : NULL;
}

std::string input_no_echo(std::string prompt, char end = '\n') {
    /*
     * This function seems overcomplicated,
     * so as a TODO I say we leave it here
     * until we find another way to disable
     * echoing of STDIN in linux
     */

    std::string result;
    struct termios term;

    tcgetattr(fileno(stdin), &term);

    term.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), 0, &term);

    std::cout << '(' << prompt << ") ";
    std::getline(std::cin, result);

    std::cout << end;

    term.c_lflag |= ECHO;
    tcsetattr(fileno(stdin), 0, &term);

    return result;
}

inline void log_error(std::string emsg) {
    std::cerr << "ERROR: " << emsg << '\n';
}

inline bool is_passible_root(void) {
    // Skips checking of password if the user is already root
    return getuid() == ROOT_UID && getgid() == ROOT_GID &&
           geteuid() == ROOT_UID && SKIP_ROOT_AUTH;
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

    const char *username = get_username();

    ERRORIF_COND("Failed to get username: " + strerrno, username == NULL);

    struct spwd *pw;

    ERRORIF_COND("Failed to get shadow entry for `" + std::string(username) +
                     "`: " + strerror(errno),
                 (pw = getspnam(username)) == NULL);

    const std::string cleartext_password =
        input_no_echo("password for " + std::string(username));

    if (strcmp(crypt(cleartext_password.c_str(), pw->sp_pwdp), pw->sp_pwdp)) {
        log_error("Access denied: Wrong password");
        return validate_password(__times + PASSWORD_AMMOUNT_INC);
    }

    return EXIT_SUCCESS;
}

int run_command(char *command[]) {
    pid_t process = fork();

    ERRORIF_COND("Failed to fork(): " + strerrno,
                 process == -1) // Failed to fork child
    else if (process == 0) {    // Successfully forked child
        int ret_code = 0;

        ERRORIF_COND("Failed to execvp(): " + strerrno,
                     (ret_code = execvp(command[0], command)) < 0);

        return ret_code;
    }
    else { // In parent process
        int status;

        ERRORIF_COND("Failed to waitpid(): " + strerrno,
                     waitpid(process, &status, 0) < 0)
        else return WEXITSTATUS(status);
    }

    return EXIT_SUCCESS;
}

int get_group_count(void) {
    struct passwd *pw;
    int ngroups = 0;

    gid_t *groups        = (gid_t *)malloc(100);
    const char *username = get_username();

#define _errorif(msg, cond) \
    if (cond) {             \
        log_error(msg);     \
        return -1;          \
    }

    _errorif("malloc(): " + strerrno, groups == NULL);

    pw = getpwnam(username);
    _errorif_cln_grp("getpwnam() failed: " + strerrno, pw == NULL);

    getgrouplist(username, pw->pw_gid, groups, &ngroups);
    free(groups);

#undef _errorif

    return ngroups;
}

int validate_group(void) {
    if (is_passible_root())
        return EXIT_SUCCESS;

    const char *username = get_username();
    int group_count      = get_group_count();
    bool is_in_group     = false;

    struct passwd *pw;
    struct group *mg;

#define usr strerrno + ": for user " + username

    ERRORIF_COND("Failed to get the user group count", group_count == -1);

    gid_t *groups = (gid_t *)malloc(sizeof(*groups) * group_count);
    ERRORIF_COND("malloc() failed in validate_group(): " + usr, groups == NULL);

    pw = getpwnam(username);
    _errorif_cln_grp("Failed to getpwnam(): " + usr, pw == NULL);

    _errorif_cln_grp("Failed to get groups for user " + std::string(username),
                     getgrouplist(username, pw->pw_gid, groups, &group_count) ==
                         -1);

    mg = getgrnam(MAIN_GROUP);
    _errorif_cln_grp("Group `" + std::string(MAIN_GROUP) + "` does not exist",
                     mg == NULL);

    for (int grp = 0; grp < group_count; ++grp) {
        if (groups[grp] == mg->gr_gid) {
            is_in_group = true;
            break;
        }
    }

    free(groups);

#undef usr

    ERRORIF_COND("Permission denied: you are not a part of the `" +
                     std::string(MAIN_GROUP) + "` group.",
                 !is_in_group);
    return EXIT_SUCCESS;
}

#ifdef HAVE_MODIFYENV
int modify_env(const struct passwd *pw) {
    std::unordered_map<const char *, char *> env = {{"HOME", pw->pw_dir},
                                                    {"SHELL", pw->pw_shell},
                                                    {"USER", pw->pw_name},
                                                    {"LOGNAME", pw->pw_name}};

    for (const auto entry : env)
        ERRORIF_COND("Failed to modify environment: " + strerrno,
                     setenv(entry.first, entry.second, 1));

    return EXIT_SUCCESS;
}
#endif

int init(void) {
    struct passwd *pw = getpwuid(ROOT_UID);
    ERRORIF_COND("Failed to getpwuid() in init(): " + strerrno, pw == NULL);

#ifdef HAVE_INITGROUP
    ERRORIF_COND("Failed to initgroups(): " + strerrno,
                 initgroups(pw->pw_name, pw->pw_gid) == -1);
#endif

#ifdef HAVE_MODIFYENV
    RETIF_FAIL(modify_env(pw));
#endif

    return EXIT_SUCCESS;
}

// MAIN //

int main(int argc, char *argv[]) {

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--version") {
            std::cout << "kos version " << VERSION << std::endl;
            return EXIT_SUCCESS;
        }
    }

    if (validate_group() != EXIT_SUCCESS)
        return EXIT_FAILURE;

    ERRORIF_COND("Usage: <command> [args...]",
                 argc < 2 || !argv[1][0] || std::isspace(argv[1][0]));

    RETIF_FAIL((validate_password() != EXIT_SUCCESS));

    ERRORIF_COND("Set{g/u}id() failed: " + strerrno,
                 setuid(ROOT_UID) == -1 || setgid(ROOT_GID) == -1);

    RETIF_FAIL(init());

    argv++;
    return run_command(argv);
}
