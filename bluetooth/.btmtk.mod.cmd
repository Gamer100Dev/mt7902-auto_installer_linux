savedcmd_btmtk.mod := printf '%s\n'   btmtk.o | awk '!x[$$0]++ { print("./"$$0) }' > btmtk.mod
