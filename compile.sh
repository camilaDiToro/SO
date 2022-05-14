#!/bin/bash
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name dockerSO agodio/itba-so:1.0
docker exec -it dockerSO make clean    -C /root/Toolchain
docker exec -it dockerSO make all      -C /root/Toolchain
docker exec -it dockerSO make clean    -C /root/
docker exec -it dockerSO make all      -C /root/
docker stop dockerSO
docker rm dockerSO