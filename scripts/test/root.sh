. scripts/test/root.lib.sh
. scripts/test/config.sh

main() {
    [ "$UID" != 0 ] && log 'Needs root access' && exit 1

    log 'SUID tests'

    suid
    suid "$TEST_CMD && ls -a /root"
}

main "$@"
