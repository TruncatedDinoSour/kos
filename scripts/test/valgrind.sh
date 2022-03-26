#!/usr/bin/env bash

. scripts/test/noroot.lib.sh

main() {
    export __CXXFLAGS="${CXXFLAGS:-} -Og -g"

    log_file="${LOGFILE:-valgrind.log}"
    tools=(memcheck cachegrind callgrind helgrind drd massif dhat lackey none exp-bbv)

    base_cmd='valgrind --trace-children=yes --log-file=valgrind.log -v'
    end_cmd="cat -- '$log_file' && head -n 1 -- '$log_file' && exit 127"

    for tool in "${tools[@]}"; do
        log "Trying tool $tool"

        cmd_1="$base_cmd --tool='$tool' ./kos && $end_cmd"
        cmd_2="$base_cmd --tool='$tool' -s ./kos && $end_cmd"

        for cmd in "$cmd_1" "$cmd_2"; do
            log "Trying with command '$cmd'"

            export CXXFLAGS="$__CXXFLAGS"
            rm -rf ./*.out.*

            log 'Compilation'
            compile "$cmd"

            log 'Optimisation flags'
            optimising "$cmd"

            log 'Command line arguments'
            flags "$cmd"
        done

        rm -rf ./*.out.*
    done
}

main "$@"
