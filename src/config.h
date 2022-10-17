#pragma once

/* The type used to count times the password has been entered
 * NOTE: Change this if you go above the limit */
typedef const short int amm_t;

/* C Bool */
typedef unsigned char CBool;

/* Extremely small uint */
typedef CBool SmallUInt;

/* Some type helpers */

#define SC  static const
#define SV  static volatile
#define SVC SV const

/*
 * NOTE: All build size increasions are with
 * no optimisations (as in no flags passed),
 * no stripping or anything, also:
 *
 * Effect on the binary: <previous binary size (when disabled)> - <new binary
 * size (when enabled)>
 *
 * Meaning negative size when enabled adds onto the binary size,
 * where as positive -- removes, all of these values should be
 * treated like aprox. size differences, not solid and accurate,
 * it highly depends on other features, compiler flags, stripping
 * and many more variable factors
 */

/* Do you want to inherit groups to root user?
 * (Effect on the binary: -1.2879999999999985 KB) */
#define HAVE_INITGROUP

/* Do you want to modify environment?
 * (Effect on the binary: -0.272000000000002 KB)*/
#define HAVE_MODIFYENV

/* Do you want to validate groups?
 * (Effect on the binary: -0.7600000000000016 KB) */
#define HAVE_VALIDATEGRP

/* Do you want to validate the password?
 * (Effect on the binary: -10.783999999999999 KB) */
#define HAVE_VALIDATEPASS

/* Do you want to disable echoing?
 * (Effect on the binary: -0.20800000000000196 KB) */
#define HAVE_NOECHO

/* Do you want to have logging?
 * (Effect on the binary: -5.648 KB) */
#define HAVE_LOGGING

/* Allow piping of password?
 * (Effect on the binary: 0 KB) */
#define HAVE_PIPE

/* Remember authentication?
 * (Effect on the binary: -5.671999999999997 KB) */
#define HAVE_REMEMBERAUTH

/* Should kos infinitely ask for a password?
 * (Effect on the binary: 0 KB) */
#define HAVE_INFINITE_ASK

/* Should kos set the effective group and user IDs?
 * (Effect on the binary: -0.1015625 KB) */
#define HAVE_EFFECTIVE_ID

/* Shoud HAVE_REMEMBERAUTH have a safe implementation?
 * (Effect on the binary: -0.046875 KB) */
#ifdef HAVE_REMEMBERAUTH
#define HAVE_SAFE_REMEMBERAUTH
#endif

/* Make REMEMBER_AUTH_DIR automatically if it doesn't exist?
 * (Effect on the binary: -0.046875 KB) */
#ifdef HAVE_REMEMBERAUTH
#define HAVE_REMEMBERAUTH_AUTODIR
#endif

#ifdef HAVE_REMEMBERAUTH
/* The directory to store the remember files */
SC char *REMEMBER_AUTH_DIR = "/var/db/kos/";

/* Max grace period in seconds */
SVC amm_t GRACE_TIME = 300;
#endif

/* What is root user ID? */
SVC SmallUInt ROOT_UID = 0;

/* What is root group ID? */
SVC SmallUInt ROOT_GID = 0;

/* The valid ammount of times password should be entered
 * NOTE: Don't make it constant as it gets changed
 *       on ever password ammount slowly decrementing
 *       it by `PASSWORD_AMMOUNT_INC`
 */
SV amm_t PASSWORD_AMMOUNT = 3;

/* Increment ammount for PASSWORD_AMMOUNT */
SVC amm_t PASSWORD_AMMOUNT_INC = 1;

/* Should kos skip checking authentication if the user is the user is already
 * root? */
SVC CBool SKIP_ROOT_AUTH = 1;

#ifdef HAVE_VALIDATEGRP
/* Main kos group which all users have to be a part of when using kos */
SC char *MAIN_GROUP = "kos";
#endif
