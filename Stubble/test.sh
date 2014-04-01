#!/bin/bash

./build.sh && ../pin/pin -t obj-ia32/MyPinTool.so -- ./userInputCrackme < file.txt
egrep "TAINT ZF" results/currentrun/taint.out | egrep "cmp eax, 0x58"

if [ $? -eq 0 ]; 
    then echo "[PASS] user input test"; 
else
    echo "[FAIL] user input test";
fi
    
