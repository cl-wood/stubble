To modify, change MyPinTool.cpp and run: 

    make PIN_ROOT=../pin/ CXX="g++ -std=c++0x"
    ../pin/pin -t obj-ia32/MyPinTool.so -- [prog]

If it doesn't work, su to root and:
    echo 0 > /proc/sys/kernel/yama/ptrace_scope

Cleaning up, since modifying Pin's makefile is scary:

    ./cleanup.sh

Testing:
    python driver.py
    # Check output for win condition


Testing multiproc:
    # Works best with 4 processes
    /usr/bin/time python mpDriver.py 4

