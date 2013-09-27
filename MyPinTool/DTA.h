
// Catch all read syscalls since they are taint sources.
VOID CatchReadSyscalls(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
    if (PIN_GetSyscallNumber(ctxt, std) == __NR_read) {

        //ssize_t read(int fd, void *buf, size_t count);
        ADDRINT buf   = ((PIN_GetSyscallArgument(ctxt, std, 1)));
        ADDRINT count = ((PIN_GetSyscallArgument(ctxt, std, 2)));

        for (int i = 0; i < (int)count; i++) {
            // TODO use API instead of sizeof char to get bytes?
            pair<ADDRINT, UINT32> taintLocation(buf + i * sizeof(char), 0);
            // TODO use TaintMap
            //TaintMap.insert(taintLocation); 
            TaintFile << get<0>(taintLocation) << "\t" << count << endl;
        }
    }

}


