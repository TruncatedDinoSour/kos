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
 * Effect on the binary: <previous binary size> - <new binary size>
 *
 * Meaning negative size adds onto the binary size, where
 * as positive -- removes, all of these values should be
 * treated like aprox. size differences, not solid
 */

/* Do you want to inherit groups to root user?
 * (Effect on the binary: -1.2879999999999985 KB) */
#define HAVE_INITGROUP

/* Do you want to modify environment?
 * (Effect on the binary: -0.272000000000002 KB)*/
#define HAVE_MODIFYENV

/* Do you want argument support?
 * (Effect on the binary: -0.11199999999999832 KB)*/
#define HAVE_ARG

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

#ifdef HAVE_REMEMBERAUTH
/* The directory to store the remember files */
SC char *REMEMBER_AUTH_DIR = "/var/kos/";

/* Max grace period in seconds */
SVC amm_t GRACE_TIME = 300;
#endif

/* What is root user ID? */
SVC SmallUInt ROOT_UID = 0;

/* What is root group ID? */
SVC SmallUInt ROOT_GID = 0;

/* The valid ammount of times password should be entered */
SV amm_t PASSWORD_AMMOUNT = 3;

/* Increment ammount for PASSWORD_AMMOUNT */
SV amm_t PASSWORD_AMMOUNT_INC = 1;

/* Should kos skip checking authentication if the user is the user is already
 * root? */
SVC CBool SKIP_ROOT_AUTH = 1;

#ifdef HAVE_VALIDATEGRP
/* Main kos group which all users have to be a part of when using kos */
SC char *MAIN_GROUP = "kos";
#endif
