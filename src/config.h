/* The type used to count times the password has been entered */
typedef const long int amm_t;

/* What is root user ID? */
const static short unsigned int ROOT_UID = 0;

/* What is root group ID? */
const static short unsigned int ROOT_GID = 0;

/* Should kos infinitely ask for a password? */
const static short unsigned int INFINITE_ASK = 0;

/* The valid ammount of times password should be entered */
static amm_t PASSWORD_AMMOUNT = 3;

/* Increment ammount for PASSWORD_AMMOUNT */
static amm_t PASSWORD_AMMOUNT_INC = 1;

/* Should kos skip checking authentication if the user is the user is already
 * root? */
const static short unsigned int SKIP_ROOT_AUTH = 1;

/* Main kos group which all users have to be a part of when using kos */
const static char *MAIN_GROUP = "kos";

/* How many bytes to pre-allocate for groups (get_group_count())?
 * https://docs.oracle.com/cd/E19082-01/820-0724/gfrbb/index.html
 * /usr/include/bits/typesizes.h:40 */
const static int PREALLOC_GROUP_BYTES = sizeof(unsigned int);

// NOTE: All build size increasions are with
// no optimisations, no stripping or anything
// Also:
//   Increases in this context means:
//      If you disable this option, output
//      binary will be X smaller
//
//   Descreases in this context means:
//      If you disable this option, output
//      binary will be X larger

/* Do you want to inherit groups to root user?
 * (Increases build size by ~1KB) */
#define HAVE_INITGROUP

/* Do you want to modify environment?
 * (Increases build size by ~39KB)*/
#define HAVE_MODIFYENV

/* Do you want argument support?
 * (Increases build size by ~2KB)*/
#define HAVE_ARG

/* Do you want to validate groups?
 * (Increases build size by ~5KB) */
#define HAVE_VALIDATEGRP

/* Do you want to validate the password?
 * (Increases build size by ~6KB) */
#define HAVE_VALIDATEPASS

/* Do you want to disable echoing?
 * (Increases build size by ~0.2KB) */
#define HAVE_NOECHO

/* Do you want to have logging?
 * (Increases build size by ~9KB) */
#define HAVE_LOGGING

/* Allow piping of password?
 * (Descreases the build size by ~0.04KB) */
#define HAVE_PIPE
