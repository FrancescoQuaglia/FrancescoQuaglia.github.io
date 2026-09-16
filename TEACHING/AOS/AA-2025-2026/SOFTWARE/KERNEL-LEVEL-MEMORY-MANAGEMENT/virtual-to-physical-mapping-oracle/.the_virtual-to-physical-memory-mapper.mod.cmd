savedcmd_the_virtual-to-physical-memory-mapper.mod := printf '%s\n'   virtual-to-physical-memory-mapper.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_virtual-to-physical-memory-mapper.mod
