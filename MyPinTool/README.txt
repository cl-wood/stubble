To modify, change MyPinTool.cpp and run: 

    make PIN_ROOT=../pin/ CXX="g++ -std=c++0x"
    ../pin/pin -t obj-intel64/MyPinTool.so -- [prog]

If it doesn't work, su to root and:
    echo 0 > /proc/sys/kernel/yama/ptrace_scope

Cleaning up, this modifying Pin's makefile is scary:

    rm input/input.* output/output.* taint/taint.* mutations/mutation.*
    echo > taint/explored.0


