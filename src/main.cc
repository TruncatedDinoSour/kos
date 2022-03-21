#include <iostream>
#include <shadow.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <string>
#include <termios.h>
#include <pwd.h>
#include <wait.h>

#include "./config.h"

// FD //

inline char *get_username(void);
bool validate_password(bool messages, amm_t __times);
std::string input_no_echo(std::string prompt, char end);
inline void log_error(std::string emsg);
int run_command(char *command[]);

// MACROS //

#define ERRORIF_COND(emsg, cond) \
    if (cond) {                  \
        log_error(emsg);         \
        return EXIT_FAILURE;     \
    }

// IMPL //

inline char *get_username(void) {
    /*
     * getlogin() is unreliable, but I wish there was a
     * better way to do this without needing <pwd.h>
     */

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

bool validate_password(bool messages = true, amm_t __times = 0) {
    /*
     * Returns `true` on success and `false` on failure
     * Argument `messages` decides if the function should print messages
     */

    if (getuid() == ROOT_UID && SKIP_ROOT_PASS)
        // Skips checking of password if the user is already root
        return true;

#define _error(message) \
    if (messages)       \
    log_error(message)
#define _errorif(message, cond) \
    if (cond) {                 \
        _error(message);        \
        return false;           \
    }

    _errorif("Negative or zero value for PASSWORD_AMMOUNT",
             PASSWORD_AMMOUNT <= 0);

    _errorif("FATAL: Detected invald PASSWORD_AMMOUNT_INC value (" +
                 std::to_string(PASSWORD_AMMOUNT_INC) +
                 "), will not proceed further",
             __times < 0);

    _errorif("Failed to log in after " + std::to_string(__times) + " atempts",
             __times >= PASSWORD_AMMOUNT && !INFINITE_ASK);

    const char *username = get_username();

    _errorif("Failed to get username: " + std::string(strerror(errno)),
             username == NULL);

    struct spwd *pw;

    _errorif("Failed to get shadow entry for `" + std::string(username) +
                 "`: " + strerror(errno),
             (pw = getspnam(username)) == NULL);

    const std::string cleartext_password =
        input_no_echo("password for " + std::string(username));

    if (strcmp(crypt(cleartext_password.c_str(), pw->sp_pwdp), pw->sp_pwdp)) {
        _error("Access denied: Wrong password");
        return validate_password(messages, __times + PASSWORD_AMMOUNT_INC);
    }

#undef _error
#undef _errorif

    return true;
}

int run_command(char *command[]) {
    pid_t process = fork();

    ERRORIF_COND("Failed to fork(): " + std::string(strerror(errno)),
                 process == -1) // Failed to fork child
    else if (process == 0) {    // Successfully forked child
        int ret_code = 0;

        ERRORIF_COND("Failed to execvp(): " + std::string(strerror(errno)),
                     (ret_code = execvp(command[0], command)) < 0)

        return ret_code;
    }
    else { // In parent process
        int status;

        ERRORIF_COND("Failed to waitpid(): " + std::string(strerror(errno)),
                     waitpid(process, &status, 0) < 0)
        else return WEXITSTATUS(status);
    }

    return EXIT_SUCCESS;
}

// MAIN //

int main(int argc, char *argv[]) {
    ERRORIF_COND("Usage: <command> [args..]",
                 argc < 2 || !argv[1][0] || std::isspace(argv[1][0]));

    if (!validate_password())
        return EXIT_FAILURE;

    ERRORIF_COND("Set{g/u}id() failed: " + std::string(strerror(errno)),
                 setuid(ROOT_UID) == -1 || setgid(ROOT_GID) == -1);

    argv++;
    return run_command(argv);
}
