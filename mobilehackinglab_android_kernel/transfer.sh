#!/bin/bash

aarch64-linux-gnu-gcc -o poc poc.c -static

#sshpass -p "root" scp -P 10021 tryoutlab.ko root@127.0.0.1:/root/
sshpass -p "root" ssh -p 10021 root@127.0.0.1 'insmod /root/tryoutlab.ko'

sshpass -p "user" scp -P 10021 poc user@127.0.0.1:/tmp/

