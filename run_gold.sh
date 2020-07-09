#!/bin/sh

afl-fuzz -i testcases -o findings_gold -- binutils-gdb/build/ld.gold -o a.out -dynamic-linker /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o @@ /usr/lib/x86_64-linux-gnu/libc.so /usr/lib/x86_64-linux-gnu/crtn.o

