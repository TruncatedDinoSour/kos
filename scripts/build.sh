#!/usr/bin/env sh

[ "$DEBUG" ] && set -x
set -e

main() {
    ${CXX:-clang++} ${CXXFLAGS} -std=c++11 \
        -Wall -Wextra -Wpedantic -Wshadow -Werror -pedantic \
        -march=native -pipe \
        ./src/main.cpp -o "${DESTDIR}kos" $(pkg-config --cflags --libs libxcrypt)
}

main "$@"
