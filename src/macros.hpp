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

#define EXIT_FLAG   2
#define EXIT_STDIN  3
#define EXIT_NOPIPE 4
#define EXIT_TERM   5
#define EXIT_AUTH   6
