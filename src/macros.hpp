#pragma once

#define RETIF_FAIL(ret)      \
    if (ret != EXIT_SUCCESS) \
        return EXIT_FAILURE;

#ifdef HAVE_LOGGING
#define ERRORIF_COND(emsg, cond) \
    if (cond) {                  \
        log_error(emsg);         \
        return EXIT_FAILURE;     \
    }
#else
#define ERRORIF_COND(emsg, cond) \
    if (cond) {                  \
        return EXIT_FAILURE;     \
    }
#endif

#ifdef HAVE_LOGGING
#define strerrno std::string(strerror(errno))
#else
#define strerrno std::string()
#endif

#ifdef HAVE_LOGGING
#define _errorif_cln_grp(msg, cond) \
    if (cond) {                     \
        free(groups);               \
        log_error(msg);             \
        return EXIT_FAILURE;        \
    }
#else
#define _errorif_cln_grp(msg, cond) \
    if (cond) {                     \
        free(groups);               \
        return EXIT_FAILURE;        \
    }
#endif

#ifdef HAVE_LOGGING
#define EXITIF_COND(emsg, cond, ex) \
    if (cond) {                     \
        log_error(emsg);            \
        exit(ex);                   \
    }
#else
#define EXITIF_COND(emsg, cond, ex) \
    if (cond) {                     \
        exit(ex);                   \
    }
#endif

#ifdef HAVE_REMEMBERAUTH
#define VER_CHECK_TIME_STAT(code)                                    \
    static char uid_str[UID_MAX] = {0};                              \
    snprintf(uid_str, UID_MAX, "%d", temp_validate_user_id);         \
    std::string verpath =                                            \
        std::string(REMEMBER_AUTH_DIR) + "/" + std::string(uid_str); \
    struct stat t_stat;                                              \
    if (stat(verpath.c_str(), &t_stat) != -1) {                      \
        if ((time(NULL) - t_stat.st_mtime) > GRACE_TIME)             \
            code;                                                    \
    }
#endif

#define EXIT_FLAG   2
#define EXIT_STDIN  3
#define EXIT_NOPIPE 4
#define EXIT_TERM   5
#define EXIT_AUTH   6
