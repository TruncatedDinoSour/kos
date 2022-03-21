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

/* Should kos skip asking for password is the user is already root? */
const static short unsigned int SKIP_ROOT_PASS = 1;

/* Main kos group which all users have to be a part of when using kos */
const static char *MAIN_GROUP = "kos";
