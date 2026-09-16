savedcmd_the_message_exchange_service.mod := printf '%s\n'   message_exchange_service_with_intermediate_buffering.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_message_exchange_service.mod
