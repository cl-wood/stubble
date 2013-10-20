// DTA.h




// When user input read from memory, take account of this
VOID GetInput(ADDRINT addr, UINT32 numOps, UINT32 reg)
{
    //string reg = REG_StringShort(INS_OperandReg(ins, 0));
    /*
    list<UINT64>::iterator i;
    UINT64 addr = memOp;

    if (numOps != 2) { // TODO why?
        return;
    }

    for(i = addressTainted.begin(); i != addressTainted.end(); i++){
        if (addr == *i){
            std::cout << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
            taintReg(reg);
            return ;
        }
    }
    */

}

VOID Instruction(INS ins, VOID *v)
{
    ADDRINT address = INS_Address(ins);
    UINT32 numOps   = INS_OperandCount(ins);
    //UINT32 reg      = INS_OperandReg(ins, 0);

    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)GetInput,
                IARG_ADDRINT, address,
                IARG_UINT32, numOps,
                //IARG_UINT32, reg,
                IARG_END);
    }

}


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

} // End CatchReadSyscalls

VOID BeforeCFunction(int* args)
{
    //int i = 0;
    //while (args[i]) {
        //TaintFile << args[i] << endl;
    //    TaintFile << args[0] << endl;
    //    i++;
    //}
    TaintFile << "BEFORE" << endl;

}

VOID AfterCFunction(int* ret)
{
    TaintFile << ret << endl;
    TaintFile << "DONE" << endl;

}


// Instrument main function, this starts the other instrumentation objects.
VOID WatchCLibrary(IMG img, VOID *v)
{

    // TODO for all functions in clibrary read into DS, loop and register this
    RTN getcharRtn = RTN_FindByName(img, STRCPY); 
    if (RTN_Valid(getcharRtn))
    {
        RTN_Open(getcharRtn);

        // Instrument main() to print the input argument value and the return value.
        RTN_InsertCall(getcharRtn, IPOINT_BEFORE, (AFUNPTR)BeforeCFunction,
                        //IARG_FUNCARG_ENTRYPOINT_REFERENCE, 0,
                        IARG_FUNCARG_ENTRYPOINT_REFERENCE, 0,
                        IARG_END);
        RTN_InsertCall(getcharRtn, IPOINT_AFTER, (AFUNPTR)AfterCFunction,
                        //IARG_FUNCRET_EXITPOINT_REFERENCE, 
                        IARG_FUNCRET_EXITPOINT_VALUE, 
                        IARG_END);

        RTN_Close(getcharRtn);
    }

} // End WatchCLibrary

// Init and Fini functions
VOID InitDTA()
{
    TaintFile.open("taint.out");
    TaintFile << hex;
    TaintFile.setf(ios::showbase);

    IMG_AddInstrumentFunction(WatchCLibrary, 0);
    INS_AddInstrumentFunction(Instruction, 0);
}

VOID FiniDTA()
{
    TaintFile.close();
}

// End DTA.h

