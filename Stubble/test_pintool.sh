#!/bin/bash

# User Input
./build.sh && ../pin/pin -t obj-ia32/MyPinTool.so -- ./userInputCrackme < file.txt
egrep "TAINT ZF" results/currentrun/taint.out | egrep "cmp eax, 0x58"

if [ $? -eq 0 ]; 
    then echo "[PASS] user input test"; 
else
    echo "[FAIL] user input test";
fi
    
# Function
../pin/pin -t obj-ia32/MyPinTool.so -- ./funcCrackme < file.txt
egrep "TAINT ZF" results/currentrun/taint.out | egrep "cmp ecx, 0x41"

if [ $? -eq 0 ]; 
    then echo "[PASS] function test"; 
else
    echo "[FAIL] function test";
fi



