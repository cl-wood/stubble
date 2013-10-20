// DTA.h

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */
std::ofstream TaintFile;
unordered_map<ADDRINT, BOOL> AddressTaintMap;
unordered_map<ADDRINT, BOOL> RegisterTaintMap;

// Instrument main function, this starts the other instrumentation objects.
/*
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
 */

// check unordered_map instead
/*
   bool checkAlreadyRegTainted(REG reg)
   {
   list<REG>::iterator i;

   for(i = regsTainted.begin(); i != regsTainted.end(); i++){
   if (*i == reg){
   return true;
   }
   }
   return false;
   }
 */

VOID removeMemTainted(UINT64 addr)
{
    AddressTaintMap.erase(addr);
    TaintFile << std::hex << "\t\t\t" << addr << " is now freed" << std::endl;
}

VOID addMemTainted(UINT64 addr)
{
    AddressTaintMap[addr] = false;
    TaintFile << std::hex << "\t\t\t" << addr << " is now tainted" << std::endl;
}

bool taintReg(REG reg)
{
    //if (checkAlreadyRegTainted(reg) == true){
    if (RegisterTaintMap.count(reg) > 0) {
        TaintFile << "\t\t\t" << REG_StringShort(reg) << " is already tainted" << std::endl;
        return false;
    }

    RegisterTaintMap[reg] = false;

    TaintFile << "\t\t\t" << REG_StringShort(reg) << " is now tainted" << std::endl;
    return true;
}

bool removeRegTainted(REG reg)
{
    RegisterTaintMap.erase(reg);
    TaintFile << "\t\t\t" << REG_StringShort(reg) << " is now freed" << std::endl;
    return true;
}

VOID ReadMem(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT64 memOp)
{
    UINT64 addr = memOp;

    if (opCount != 2)
        return;
    for (auto it = AddressTaintMap.begin(); it != AddressTaintMap.end(); ++it) {
        if (addr == it->first){
            TaintFile << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
            taintReg(reg_r);
            return ;
        }
    }
    /* if mem != tained and reg == taint => free the reg */
    if (RegisterTaintMap.count(reg_r) > 0) {
        TaintFile << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        removeRegTainted(reg_r);
    }
}

VOID WriteMem(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT64 memOp)
{
    UINT64 addr = memOp;

    if (opCount != 2)
        return;

    for (auto it = AddressTaintMap.begin(); it != AddressTaintMap.end(); ++it) {
        if (addr == it->first) {
            TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
            //if (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r)) //TODO is change correct?
            if (!REG_valid(reg_r) || !(RegisterTaintMap.count(reg_r) > 0) ) {
                removeMemTainted(addr);
            }
            return;
        }
    }
    if (RegisterTaintMap.count(reg_r) > 0) {
        TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        addMemTainted(addr);
    }
}

VOID spreadRegTaint(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, REG reg_w)
{
    if (opCount != 2)
        return;

    if (REG_valid(reg_w)){
        if (RegisterTaintMap.count(reg_w) > 0 && (!REG_valid(reg_r) || !(RegisterTaintMap.count(reg_r) > 0))) {
            TaintFile << "[SPREAD]\t\t" << insAddr << ": " << insDis << std::endl;
            TaintFile << "\t\t\toutput: "<< REG_StringShort(reg_w) << " | input: " << (REG_valid(reg_r) ? REG_StringShort(reg_r) : "constant") << std::endl;
            removeRegTainted(reg_w);
        }
        else if (!(RegisterTaintMap.count(reg_w) > 0) && RegisterTaintMap.count(reg_r) > 0) {
            TaintFile << "[SPREAD]\t\t" << insAddr << ": " << insDis << std::endl;
            TaintFile << "\t\t\toutput: " << REG_StringShort(reg_w) << " | input: "<< REG_StringShort(reg_r) << std::endl;
            taintReg(reg_w);
        }
    }
}

VOID followData(UINT64 insAddr, std::string insDis, REG reg)
{
    if (!REG_valid(reg))
        return;

    if (RegisterTaintMap.count(reg) > 0) {
        TaintFile << "[FOLLOW]\t\t" << insAddr << ": " << insDis << std::endl;
    }
}

VOID Instruction(INS ins, VOID *v)
{
    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)ReadMem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_OperandReg(ins, 0),
                IARG_MEMORYOP_EA, 0,
                IARG_END);
    }
    else if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsWritten(ins, 0)){
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)WriteMem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_OperandReg(ins, 1),
                IARG_MEMORYOP_EA, 0,
                IARG_END);
    }
    else if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)spreadRegTaint,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_UINT32, INS_RegW(ins, 0),
                IARG_END);
    }

    if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)followData,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_END);
    }
}

static unsigned int tryksOpen;

#define TRICKS(){if (tryksOpen++ == 0)return;}

VOID Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, void *v)
{
    unsigned int i;
    UINT64 start, size;

    if (PIN_GetSyscallNumber(ctx, std) == __NR_read){

        TRICKS(); /* tricks to ignore the first open */

        start = static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 1)));
        size  = static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 2)));

        for (i = 0; i < size; i++)
            AddressTaintMap[start+i] = false; 

        TaintFile << "[TAINT]\t\t\tbytes tainted from " << std::hex << "0x" << start << " to 0x" << start+size << " (via read)"<< std::endl;
    }
}

// Init and Fini functions
VOID InitDTA()
{
    TaintFile.open("taint.out");
    TaintFile << hex;
    TaintFile.setf(ios::showbase);

    //IMG_AddInstrumentFunction(WatchCLibrary, 0);
    PIN_AddSyscallEntryFunction(Syscall_entry, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    // INS_AddInstrumentFunction(Instruction, 0);
}

VOID FiniDTA()
{
    TaintFile.close();
}

// End DTA.h

