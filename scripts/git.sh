#!/usr/bin/env sh

set -e

main() {
    if [ ! "$NO_TEST" ]; then
        ${__BASH_DOAS:-kos} ./scripts/test/root.sh
        ./scripts/test/noroot.sh
        ./scripts/test/valgrind.sh

        printf 'Press enter to continue'
        read -r
    fi

    git add -A
    git commit -sam "${m:-"update @ $(date)"}"
    git push -u origin "$(git rev-parse --abbrev-ref HEAD)"
}

main "$@"
