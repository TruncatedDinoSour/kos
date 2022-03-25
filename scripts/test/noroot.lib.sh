#!/usr/bin/env bash

COMPILERS=(g++ clang++)
OPTIMISE_FLAGS=(-Os -O2 -O3 -Ofast -flto)
CXXFLAGS="${CXXFLAGS:-}"

log() {
    echo -e " * $1" >&2
}

run() {
    log "$1"
    eval "$2"

    printf ' >>> '
    ls --color=auto -lahF kos

    echo
}

compile() {
    flags="$CXXFLAGS $_CXXFLAGS"

    for compiler in "${COMPILERS[@]}"; do
        run "Compiling with $compiler (CXXFLAGS = ${flags})" \
            "CXX='$compiler' CXXFLAGS='$flags' ./scripts/build.sh"

        [ "$1" ] && eval "$1"

        ./kos
        if [ "$?" -ne 1 ]; then
            log 'Failed to execute -- aborting'
            exit 1
        fi
    done
}

optimising() {
    for flag in "${OPTIMISE_FLAGS[@]}"; do
        export _CXXFLAGS="$flag"
        compile "$1"
    done
}
