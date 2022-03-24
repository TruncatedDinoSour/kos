#!/usr/bin/env sh

set -e

main() {
    ${__BASH_DOAS:-kos} ./scripts/test/root.sh
    ./scripts/test/noroot.sh

    git add -A
    git commit -sam "${m:-"update @ $(date)"}"
    git push -u origin "$(git rev-parse --abbrev-ref HEAD)"
}

main "$@"
