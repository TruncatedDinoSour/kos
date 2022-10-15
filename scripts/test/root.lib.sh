. scripts/test/noroot.lib.sh

suid() {
    cmd="chown root:root kos && chmod 4111 kos; $1"

    compile "$cmd"
    optimising "$cmd"
}
