#ifndef __SYSCALLS_H
#define __SYSCALLS_H

#include <vector>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

// Extern from Stubble.h
extern const string results_dir;
extern unsigned long INS_COUNT;

// Transitioning from Salwan's
//extern std::list<UINT32> addressTainted;

unordered_map<ADDRINT, tuple<UINT32, string> > tainted_memory;

ofstream SyscallsFile;
extern ofstream TaintFile;

/*
 *  Instrument the following Linux syscalls:
 *      -open
 */
VOID Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, VOID *v)
{
    
    ADDRINT start, size;
    //if (PIN_GetSyscallNumber(ctx, std) == __NR_open) {
    //    SyscallsFile << "[OPEN]\t" << PIN_GetSyscallArgument(ctx, std, 0) << endl; 
    //}

    // If syscall arg 0 is 0, then STDIN
    if (PIN_GetSyscallNumber(ctx, std) == __NR_read && PIN_GetSyscallArgument(ctx, std, 0) == 0 ) {

        start = static_cast<ADDRINT>((PIN_GetSyscallArgument(ctx, std, 1)));
        size  = static_cast<ADDRINT>((PIN_GetSyscallArgument(ctx, std, 2)));

        for (unsigned int i = 0; i < size; i++) {
            //string mem_addr = StringFromAddrint(start + i);
            //cout << mem_addr << "\t\t\t" << AddrintFromString(mem_addr) << endl;
            //cout << mem_addr << "\t\t\t" << Uint32FromString(mem_addr) << endl;

            tainted_memory[start + i] = tuple<UINT32, string>(i, "");
            //addressTainted.push_back(start+i);

        }

        SyscallsFile << INS_COUNT << ":\t" << "READ fd " << PIN_GetSyscallArgument(ctx, std, 0) << ":[" 
                     << StringFromAddrint(start) << ", " << StringFromAddrint(start + size) << "]" <<endl;

        TaintFile << INS_COUNT << ":\t" << "READ fd " << PIN_GetSyscallArgument(ctx, std, 0) << ":[" 
                     << StringFromAddrint(start) << ", " << StringFromAddrint(start + size) << "]" << endl;

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

