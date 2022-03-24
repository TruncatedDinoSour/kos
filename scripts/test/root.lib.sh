. scripts/test/noroot.lib.sh

suid() {
    cmd="chown root:root kos && chmod 4711 kos; $1"

    compile "$cmd"
    optimising "$cmd"
}
