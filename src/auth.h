#ifndef _AUTH_H
#define _AUTH_H
#include "config.h"

#include <sys/types.h>

typedef enum {
    AUTH_OK = 0,
    AUTH_SESSION_PERMISSIONS,
    AUTH_SHADOW,
    AUTH_GROUP,
    AUTH_PIPE,
    AUTH_FAILURE
} AuthResult;

AuthResult authenticate(const Config config, const uid_t uid);

const char *authstatus_to_string(const AuthResult ar);
#endif /* _AUTH_H */
