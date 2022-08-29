#!/usr/bin/env sh

set -e

main() {
    if [ ! "$NO_TEST" ]; then
        ./scripts/test/full.sh

        printf 'Press enter to continue'
        read -r
    fi

    git add -A
    git commit -sa
    git push -u origin "$(git rev-parse --abbrev-ref HEAD)"
}

main "$@"
