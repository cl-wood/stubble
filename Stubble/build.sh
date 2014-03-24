#!/bin/bash -x

touch MyPinTool.cpp
make PIN_ROOT=../pin/ CXX="g++ -std=c++0x"

