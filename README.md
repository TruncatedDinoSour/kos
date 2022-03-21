# Kos

<p align="center">
  <img src="https://img.shields.io/github/last-commit/TruncatedDinosour/kos?color=red&style=flat-square">
  <img src="https://img.shields.io/github/repo-size/TruncatedDinosour/kos?color=red&style=flat-square">
  <img src="https://img.shields.io/github/issues/TruncatedDinosour/kos?color=red&style=flat-square">
  <img src="https://img.shields.io/github/stars/TruncatedDinosour/kos?color=red&style=flat-square">
</p>

> A simple SUID tool written in C++

<p align="center">
    <img src="/logo/kos.png" alt="kos logo" width="50%"/>
</p>

# Requirements

- Pkg-config: https://www.freedesktop.org/wiki/Software/pkg-config/
- Libxcrypt: https://github.com/besser82/libxcrypt

# Third party software support

- [Bash completion](https://github.com/scop/bash-completion) (Install `completions/kos.bash` into `/usr/share/bash-completion/completions/kos`)

# Known issues

- Overcomplicated `input_no_echo()` function

```
I used that function because it's literally the only
way I know how to disable eching of STDIN in linux with
C++ without using some huge lib like GNU readline or something...
```

# People packaging kos for non-corporate use

```
I, the creator of kos, permit you to use any of these licenses:

- GPLv3
- BSD 3-clause
- ArAr2

If you do not want to bundle ArAr2 license together you are free
to use any of the other ones
```

# Building and installing

_If you are `root` you do not need to use `su`, just run commands directly_

## Manual

### Building

```sh
CXX=g++ ./scripts/build.sh  # Compiles with GCC instead of Clang (default)
```

### Installing

```sh
./scripts/strip.sh kos
su -c 'mkdir -p /usr/local/bin'
su -c 'chown root:root ./kos'
su -c 'install -Dm4711 ./kos /usr/local/bin'
```

### [Man page](https://man-db.nongnu.org/) installation

```sh
su -c 'mkdir -p /usr/share/man/man1'
su -c 'install -Dm0644 kos.1 /usr/share/man/man1/kos.1'
su -c 'mandb -qf /usr/share/man/man1/kos.1'
```

## Automated

Before running the script you can optionally:

- Set the `DO_STRIP` environment variable to strip the binary after compilation
- Set the `INSTALL_MAN` environment variable to also install man page

```sh
chmod a+rx ./scripts/setup.sh
su -c './scripts/setup.sh'
```

# Logic

```
Compile program linking the libxcrypt lib, then
give it suid privelages using 4711 permissions,
the program executable should also be owned by the root
user and group, program should first check
if a command is supplied as not to
trigger any bugs, then we validate the password using passwd
struct for getting the username of current logged in user,
getuid() for getting uid of current user and then getting
/etc/shadow entry using <shadow.h> for that specific user
after that we ask for the user to enter their password
and we do not echo STDIN, we wait for the user to hit enter and then
we hash the password using crypt() function of libxcrypt
and compare it to pw->sp_pwdp where pw is the spwd struct
which is the shadow entry of currently logged in user, if everything
is okay return true, else return false, if it passes we continue
and call setuid() and setgid(), we check if they fail,
if no, continue, else notify the user, then we
increment the argv pointer array to ignore argv[0] which
is the program name and call run_command() on it,
then in that function we fork the current process
and check if it succeeded, if no we notify the user,
else we contnue, if the currect process is in child we call
execvp() and run the command, if in parent we waitpid() the
child and get the exit code, which we return and then
finally we just exit with the code the child process
exited with
```

# Packages

- Gentoo linux:
  - [app-admin/kos::dinolay](https://ari-web.xyz/gentooatom/app-admin/kos)
