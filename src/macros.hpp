#pragma once

#ifdef HAVE_LOGGING
#define EXIF_LOGGING(what) what;
#else
#define EXIF_LOGGING(what)
#endif

#ifdef HAVE_LOGGING
#define FATAL(msg) "FATAL: " msg
#else
#define FATAL(msg) ""
#endif

#define RETIF_FAIL(ret)      \
    if (ret != EXIT_SUCCESS) \
        return EXIT_FAILURE;

#define ERRORIF_COND(emsg, cond)       \
    if (cond) {                        \
        EXIF_LOGGING(log_error(emsg)); \
        return EXIT_FAILURE;           \
    }

#define WERRORIF_COND(emsg, cond) ERRORIF_COND(WSTRERRNO(emsg), cond)

#ifdef HAVE_LOGGING
#define WSTRERRNO(msg) std::string(msg) + ": " + strerror(errno)
#else
#define WSTRERRNO(msg)
#endif

#define _errorif_cln_grp(msg, cond)   \
    if (cond) {                       \
        free(groups);                 \
        EXIF_LOGGING(log_error(msg)); \
        return EXIT_FAILURE;          \
    }

#define EXITIF_COND(emsg, cond, ex)    \
    if (cond) {                        \
        EXIF_LOGGING(log_error(emsg)); \
        exit(ex);                      \
    }

#ifdef HAVE_REMEMBERAUTH
#define VER_CHECK_TIME_STAT(code)                                \
    std::string verpath = std::string(REMEMBER_AUTH_DIR) + "/" + \
                          std::to_string(temp_validate_user_id); \
    struct stat t_stat;                                          \
    if (stat(verpath.c_str(), &t_stat) != -1)                    \
        if ((time(NULL) - t_stat.st_mtime) > GRACE_TIME)         \
            code;
#endif

#ifndef STDIN_FILENO
#ifdef _POSIX_C_SOURCE
#define STDIN_FILENO 0
#else
#define _STDIN_FILENO_FILENO_FN
#define STDIN_FILENO fileno(stdin)
#endif
#endif

#define ENV_AMMOUNT 4

#define EXIT_STDIN  2
#define EXIT_NOPIPE 3
#define EXIT_TERM   4
#define EXIT_AUTH   5
