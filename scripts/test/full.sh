#!/usr/bin/env bash

set -e

main() {
    TESTS=(noroot)
    ROOT_TESTS=(root)

    command -v valgrind >/dev/null && TESTS+=(valgrind)

    for root_test in "${ROOT_TESTS[@]}"; do
        ${__BASH_RUNAS:-su -c} "./scripts/test/$root_test.sh"
    done

    for test_sc in "${TESTS[@]}"; do
        "./scripts/test/$test_sc.sh"
    done
}

main "$@"
