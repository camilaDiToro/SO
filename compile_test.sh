#!/bin/bash
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name dockerSO agodio/itba-so:1.0

docker exec -u root -it dockerSO wget -q -O - https://files.pvs-studio.com/etc/pubkey.txt | apt-key add - &&
                                 wget -O /etc/apt/sources.list.d/viva64.list https://files.pvs-studio.com/etc/viva64.list &&
                                 apt-get -y install apt-transport-https &&
                                 apt-get -y update &&
                                 apt-get -y install pvs-studio &&
                                 
docker exec -u root -it dockerSO make clean    -C /root/Toolchain
docker exec -u root -it dockerSO make all      -C /root/Toolchain
docker exec -u root -it dockerSO make clean    -C /root/
docker exec -u root -it dockerSO  pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"
docker exec -u root -it dockerSO make check    -C /root/
docker stop dockerSO
docker rm dockerSO