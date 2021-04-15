# Makefile
# This file is a part of cnu-auto-network-auth-client project.
# Author: mmdjiji (JiJi)
# GitHub: https://github.com/mmdjiji/cnu-auto-network-auth-client
# License: GPL v3.0
# Author's Blog: https://mmdjiji.com

TAR = autonet
OBJ = cJSON.o main.o
CC = g++
CFLAGS = -lwsock32

$(TAR): $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

%.o: %.cpp
	$(CC) -c $^ -o $@

%.o: %.c
	$(CC) -c $^ -o $@
