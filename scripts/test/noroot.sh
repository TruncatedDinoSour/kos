#!/usr/bin/env bash

. scripts/test/noroot.lib.sh
. scripts/test/config.sh

main() {
    log 'COMPILATION'
    compile
    compile "$TEST_CMD"

    log 'OPTIMISATION FLAGS'
    optimising
    optimising "$TEST_CMD"
}

main "$@"

