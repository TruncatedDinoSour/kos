export TEST_CMD='id && whoami && echo "${USER:-$(id -un || printf %s "${HOME/*\//}")}@${HOSTNAME:-$(hostname)}"'
