#!/bin/bash

# Test ls
cat ./tests/command.txt | ./a.out config_files/myconf.conf > ./tests/ls.mine
cat ./tests/command.txt | ./sample/proj1_linprog.x config_files/myconf.conf > ./tests/ls.test

diff ./tests/ls.mine ./tests/ls.test

