savedcmd_baseline-char-dev.mod := printf '%s\n'   baseline-char-dev.o | awk '!x[$$0]++ { print("./"$$0) }' > baseline-char-dev.mod
