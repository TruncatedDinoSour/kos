# kos(1) completion                                    -*- shell-script -*-

_kos() {
    local cword cmd
    _init_completion -s || return

    for ((cmd = 1; cmd <= cword; cmd++)); do
        # if [[ "${words[cmd]}" != -* ]]; then

        local PATH="$PATH:/sbin:/usr/sbin:/usr/local/sbin"
        _command_offset $cmd
        return

        # else
        #     COMPREPLY=('--version')
        # fi
    done
} && complete -F _kos -o bashdefault -o default kos

# ex: filetype=sh
