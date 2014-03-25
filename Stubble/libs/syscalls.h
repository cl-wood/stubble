#ifndef __SYSCALLS_H
#define __SYSCALLS_H

// FollowExecution.h

#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <list>

// Extern from Stubble.h
extern string results_dir;

list<UINT32> addressTainted;
unordered_map<UINT32, tuple<UINT32, string> > tainted_memory;

ofstream TaintIntroductionFile;

/*
 *  Instrument the following syscalls:
 *      -read
 *      -open
 */
VOID Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, VOID *v)
{
    unsigned int i;
    UINT32 start, size;
    //UINT32 read;
    if (PIN_GetSyscallNumber(ctx, std) == __NR_open) {
        //TaintIntroductionFile << "BLAH " << PIN_GetSyscallArgument(ctx, std, 1);
        TaintIntroductionFile << "OPEN " << PIN_GetSyscallArgument(ctx, std, 0) << " "
                             << PIN_GetSyscallArgument(ctx, std, 1) << endl;
    }

    if (PIN_GetSyscallNumber(ctx, std) == __NR_read) {

        //eipValue = PIN_GetContextReg(ctx, REG_EIP);
        TaintIntroductionFile << "EIP value\t" << std::hex << endl;

        start = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 1)));
        size  = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 2)));
        //UINT32 fd = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 0)));
        //read  = static_cast<UINT32>((PIN_GetSyscallReturn(ctx, std)));

        // Cheap trick for demo, will need to remove later?
        if (size != 500) {
            return; 
        }

        for (i = 0; i < size; i++) {
            addressTainted.push_back(start+i);
        }

        TaintIntroductionFile << "[TAINT]\t" << std::hex << start << ": " << start+size << " (read)" << endl;
    }
}


VOID Syscall_exit(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, VOID *v)
{
        /*
        UINT32 read;
        UINT32 start, size;
        start = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 1)));
        size  = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 2)));
        read = static_cast<UINT32>((PIN_GetSyscallReturn(ctx, std)));

        cout << hex << start << ":" << start+size << ":" << read << endl;
        */

    //if (PIN_GetSyscallNumber(ctx, std) == __NR_read) {
        //eipValue = PIN_GetContextReg(ctx, REG_EIP);
    //    TaintIntroductionFile << "EIP value now \t" << std::hex << endl;
    //}

}


VOID init_syscalls()
{
    TaintIntroductionFile.open(results_dir + "introduction.taint.out");

    PIN_AddSyscallEntryFunction(Syscall_entry, 0);
    //PIN_AddSyscallExitFunction(Syscall_exit, 0);

}

VOID fini_syscalls()
{

    TaintIntroductionFile.close();

}

#endif /* __SYSCALLS_H__ */

