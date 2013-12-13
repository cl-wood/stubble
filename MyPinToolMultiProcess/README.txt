Clark Wood
COP5570 Project
10 December 2013

This project requires Intel Pin, available for free download at: 

    http://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool
    After downloading and unzipping pin, the pin/ directory should be placed in the same parent directory as MyPinToolMultiProcess/, so that it can be reached with "../pin/" from inside the MyPinToolMultiProcess/ directory.

To run, touch MyPinTool.cpp and run the following commands: 

    tcc simpleCrackme.c -o simpleCrackme
    gcc mp.c -o multiProc
    make PIN_ROOT=../pin/ CXX="g++ -std=c++0x"
    python mpDriver.py 4 # Or a different number, argv1 is the number of processes to create


The sequential version can be run with:

    make PIN_ROOT=../pin/ CXX="g++ -std=c++0x"
    python driver.py

If it doesn't work, su to root and:

    echo 0 > /proc/sys/kernel/yama/ptrace_scope

What happens:

    The pintool finds tainted branches and explores them. To test if the interesting branches
    have been found, grep for the "win" string in the output directory after mpDriver is done.

Cleaning up, since modifying Pin's makefile is scary:

    ./cleanup.sh




