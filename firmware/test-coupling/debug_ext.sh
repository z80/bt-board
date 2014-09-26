#!/bin/sh
PATH=$PATH:~/programs/code-sourcery/2011.03-42/bin
ddd --eval-command "target remote 127.0.0.1:4242" --debugger arm-none-eabi-gdb ./build/ch.elf
