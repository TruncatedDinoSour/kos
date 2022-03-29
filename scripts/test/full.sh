#!/usr/bin/env bash

set -e

main() {
    TESTS=(noroot valgrind)
    ROOT_TESTS=(root)

    for root_test in "${ROOT_TESTS[@]}"; do
        su -c "./scripts/test/$root_test.sh"
    done

    for test_sc in "${TESTS[@]}"; do
        "./scripts/test/$test_sc.sh"
    done
}

main "$@"
