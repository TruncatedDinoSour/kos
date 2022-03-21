#!/usr/bin/env bash

set -e

main() {
    if [ ! "$1" ]; then
        echo "Strip binaries"
        echo "Usage: $0 [binaries...]"
        return 1
    fi

    for file in "${@:1}"; do
        echo "Stripping $file..."
        ${STRIP:-strip} ${STRIPFLAGS} -N __gentoo_check_ldflags__ -R .comment -R .GCC.command.line \
            --remove-section=.note.gnu.gold-version --remove-section=.note.gnu.build-id \
            --remove-section=.note.ABI-tag --remove-section=.note --remove-section=.gnu.version \
            --remove-section=.comment --strip-debug --strip-unneeded "$file"
    done
}

main "$@"
