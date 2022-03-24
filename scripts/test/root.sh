. scripts/test/root.lib.sh

main() {
    log 'SUID tests'

    suid
    suid 'id && whoami && ls -a /root && echo "${USER:-$(id -un || printf %s "${HOME/*\//}")}@${HOSTNAME:-$(hostname)}"'
}

main "$@"
