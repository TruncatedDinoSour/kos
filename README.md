# This repository has been migrated to the self-hosted ari-web Forgejo instance: <https://git.ari.lt/ari/kos>
# Kos

<p align="center">
  <img src="https://img.shields.io/github/last-commit/TruncatedDinosour/kos?color=red&style=flat-square">
  <img src="https://img.shields.io/github/repo-size/TruncatedDinosour/kos?color=red&style=flat-square">
  <img src="https://img.shields.io/github/issues/TruncatedDinosour/kos?color=red&style=flat-square">
  <img src="https://img.shields.io/github/stars/TruncatedDinosour/kos?color=red&style=flat-square">
</p>

> A simple SUID tool written in C++

# Requirements

-   Pkg-config: https://www.freedesktop.org/wiki/Software/pkg-config/
-   Libxcrypt: https://github.com/besser82/libxcrypt

# Why kos?

-   Very fast compilation times
-   Small amount of dependencies
-   GPLv3 License
-   Quite simple
-   Does not use PAM

# Third party software support

-   [Bash completion](https://github.com/scop/bash-completion) (Install `completions/kos.bash` into `/usr/share/bash-completion/completions/kos`)

# Known issues

-   Only supported on Linux

```
shadow.h is one of the requirements of kos which is a "linux thing",
so automatically won't work on something like OpenBSD, MacOS, SerenityOS or
any other OSes.

Kos will not work on any non-unix OSes as kos uses a lot of
unix stuff, for example pwd.h, meaning will not work on stuff
like Windows and other non-unix OSes.

Though this is not really a big issue, this app is meant to
be ran on linux and was made with the intention to be used on
and with linux...
```

-   Overcomplicated `input_no_echo()` function

```
I used that function because it's literally the only
way I know how to disable eching of STDIN in linux with
C++ without using some huge lib like GNU readline or something...
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
su -c 'install -Dm4111 ./kos /usr/local/bin'
```

### [Man page](https://man-db.nongnu.org/) installation

```sh
su -c 'mkdir -p /usr/share/man/man1'
su -c 'install -Dm0644 kos.1 /usr/share/man/man1/kos.1'
su -c 'mandb -qf /usr/share/man/man1/kos.1'
```

### Completions

#### Bash

```sh
su -c 'cp completions/kos.bash /usr/share/bash-completion/completions/kos'
```

## Automated

Before running the script you can optionally:

-   Set the `DO_STRIP` environment variable to strip the binary after compilation
-   Set the `INSTALL_MAN` environment variable to also install man page
-   Set the `INSTALL_BCOMP` environment variable to also install bash completion

```sh
chmod a+rx ./scripts/setup.sh
su -c './scripts/setup.sh'
```

# Note for packagers

-   Arch Linux

```
Permission issues (ERROR: Failed getting groups for user ...)
    The issue can be solved with one install command:
    $ install -Dm4755 -o root "$srcdir/$pkgname-$pkgver/kos" "$pkgdir/usr/bin/kos"
Although this reduces security
```

# Packages

-   Gentoo Linux:
    -   [app-admin/kos::dinolay](https://ari-web.xyz/gentooatom/app-admin/kos)
-   Arch Linux
    -   [AUR/kos@moonaru](https://aur.archlinux.org/packages/kos)

# Tips

-   If you're building for size make sure to build with `-Os` or `-Oz` in `CXXFLAGS`
    as it barely touches start times but it decreases the size largely, stripping
    can help too
-   If you every want to debug kos use `-g -O0` CXXFLAGS

# Testing

There are two scripts in the [testing scripts](/scripts/test) directory,
one is `noroot.sh` and other `root.sh`, `*.lib.sh` are just
libs.

If you want to test it you just run the scripts, though which ones?

-   If you have access to root run: `root.sh`
-   If you have access to a non-privileged user run: `noroot.sh`
-   If you have access to both run.. Well both

## Highly recommended

-   Run [valgrind](https://valgrind.org/info/about.html) on kos

```sh
CXXFLAGS='-Og -g' ./scripts/build.sh
valgrind ./kos
valgrind -s ./kos
```

Or run [valgrind.sh](/scripts/test/valgrind.sh) testing script,
will test all compilers, tools and stuff, in general a much more
in-depth test

Returns code `127` on failure (detection of a memory leak) and
you can see the log file in `valgrind.log`

## Requirements

-   Clang
-   GCC
-   Coreutils
-   Bash
-   [Net-tools](http://net-tools.sourceforge.net/) (or a `hostname` command)

## Net-tools is not a thing for me!

You can easily make your own `hostname` command which is the
only thing testing depends on:

```sh
#!/usr/bin/env sh
cat /etc/hostname
```

Add this to /usr/bin/hostname and make it executable:

```sh
su -c 'chmod 755 /usr/bin/hostname'
```

# Using as a header

Just define `KOS_H` before including the `main.cpp` file

# Inputting password from external sources

**This section only applies if `HAVE_PIPE` is set**

`Kos` supports piping to STDIN so you can easily just
pipe (`|`) the password to STDIN :)

For example:

```sh
echo 'Top-secret-passw0rd' | kos id
```

Or even

```sh
printf '' | dmenu -l 0 -p 'Password: ' | kos id
```
