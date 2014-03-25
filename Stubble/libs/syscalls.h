#ifndef __SYSCALLS_H
#define __SYSCALLS_H

#include <vector>
#include <tuple>
#include <unordered_map>

// Extern from Stubble.h
extern string results_dir;

list<UINT32> addressTainted;
unordered_map<UINT32, tuple<UINT32, string> > tainted_memory;

ofstream SyscallsFile;

/*
 *  Instrument the following Linux syscalls:
 *      -open
 */
VOID Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, VOID *v)
{
    
    UINT32 start, size;
    //if (PIN_GetSyscallNumber(ctx, std) == __NR_open) {
    //    SyscallsFile << "[OPEN]\t" << PIN_GetSyscallArgument(ctx, std, 0) << endl; 
    //}

    // If syscall arg 0 is 0, then STDIN
    if (PIN_GetSyscallNumber(ctx, std) == __NR_read && PIN_GetSyscallArgument(ctx, std, 0) == 0 ) {

        start = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 1)));
        size  = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 2)));

        for (unsigned int i = 0; i < size; i++) {
            //addressTainted.push_back(start+i);
            tainted_memory[start + i] = tuple<UINT32, string>(i, "");

        }

        SyscallsFile << "READ\t" << PIN_GetSyscallArgument(ctx, std, 0) << " " 
                        << std::hex << start << " " << start+size << endl;
    }
}

VOID init_syscalls()
{
    SyscallsFile.open(results_dir + "syscalls.out");

    PIN_AddSyscallEntryFunction(Syscall_entry, 0);

}

VOID fini_syscalls()
{

    SyscallsFile.close();

}

#endif /* __SYSCALLS_H__ */

