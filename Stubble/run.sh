#!/bin/bash -x

#../pin/pin -t obj-ia32/MyPinTool.so -- ./userInputCrackme < file.txt
#../pin/pin -t obj-ia32/MyPinTool.so -- ./funcCrackme < file.txt
../pin/pin -t obj-ia32/MyPinTool.so -- ./loopsCrackme < file.txt

