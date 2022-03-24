. scripts/test/noroot.lib.sh

main() {
    cmd='id && whoami && echo "${USER:-$(id -un || printf %s "${HOME/*\//}")}@${HOSTNAME:-$(hostname)}"'

    log 'COMPILATION'
    compile
    compile "$cmd"

    log 'OPTIMISATION FLAGS'
    optimising
    optimising "$cmd"
}

main "$@"

