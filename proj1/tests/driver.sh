#!/bin/bash

# Test ls
cat ./tests/list.txt | ./a.out config_files/myconf.conf > ./tests/ls.mine
cat ./tests/list.txt | ./sample/proj1_linprog.x config_files/myconf.conf > ./tests/ls.test

diff ./tests/ls.mine ./tests/ls.test

