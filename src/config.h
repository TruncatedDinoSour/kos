/* The type used to count times the password has been entered */
typedef unsigned int amm_t;

/* What is root user ID? */
short unsigned int ROOT_UID = 0;

/* What is root group ID? */
short unsigned int ROOT_GID = 0;

/* Should kos infinitely ask for a password? */
short unsigned int INFINITE_ASK = 0;

/* The valid ammount of times password should be entered */
amm_t PASSWORD_AMMOUNT = 3;

/* Increment ammount for PASSWORD_AMMOUNT */
amm_t PASSWORD_AMMOUNT_INC = 1;

/* Should kos skip asking for password is the user is already root? */
short unsigned int SKIP_ROOT_PASS = 1;
