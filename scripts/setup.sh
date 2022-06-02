#!/usr/bin/env sh

set -e

main() {
    if [ "$USER" != 'root' ]; then
        echo 'Needs root privalages'
        exit 1
    fi

    echo 'Using setup script... please be careful'
    sleep 1

    PREFIX="${DESTDIR}${PREFIX:-/usr/local}/"
    BINDIR="${PREFIX}bin"
    MANPREFIX="${MANPREFIX:-${PREFIX}/share/man/man1}"

    sh ./scripts/build.sh
    [ "$DO_STRIP" ] && sh ./scripts/strip.sh kos

    mkdir -p "$BINDIR"
    chown root:root ./kos
    install -o root -Dm4711 ./kos "$BINDIR"

    if [ "$INSTALL_MAN" ]; then
        mkdir -p "$MANPREFIX"
        install -Dm0644 kos.1 "$MANPREFIX"
        mandb -qf "${MANPREFIX}/kos.1"
    fi

    [ "$INSTALL_BCOMP" ] && cp completions/kos.bash /usr/share/bash-completion/completions/kos
}

main "$@"
