#!/bin/bash -x

../pin/pin -t obj-ia32/MyPinTool.so -- ./userInputCrackme < file.txt
#../pin/pin -t obj-ia32/MyPinTool.so -- ./simpleCrackme fileLose.txt
#../pin/pin -t obj-ia32/MyPinTool.so -- ./a.out

