// FollowExecution.h

#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <list>

int insCount = 0;
std::list<UINT32> addressTainted;
std::list<REG> regsTainted;

/* ===================================================================== */
/* Control Flow following jumps                                          */
/* ===================================================================== */
std::ofstream ControlFlowFile;
std::ofstream TaintFile;
std::ofstream InstructionFile;

//unordered_map<int, string>addressToInstruction;
//unordered_map<int, int>addressToBranchTaken;

/*
 * Records whether branch taken or not 
 */
VOID TaintBranch(ADDRINT ins, INT32 branchTaken, string insDis) {

    ControlFlowFile << insDis << ":" << branchTaken << endl;

}


/*
    if (INS_Opcode(ins) == XED_ICLASS_MOV &&
        INS_IsMemoryRead(ins) && 
        INS_OperandIsReg(ins, 0) &&
        INS_OperandIsMemory(ins, 1))

 */

bool checkAlreadyRegTainted(REG reg)
{
    list<REG>::iterator i;

    for (i = regsTainted.begin(); i != regsTainted.end(); i++) {
        if (*i == reg){
            return true;
        }
    }
    return false;
}

VOID removeMemTainted(UINT32 addr)
{
    addressTainted.remove(addr);
    TaintFile << std::hex << "\t\t\t" << addr << " is now freed" << std::endl;
}

VOID addMemTainted(UINT32 addr)
{
    addressTainted.push_back(addr);
    TaintFile << std::hex << "\t\t\t" << addr << " is now tainted" << std::endl;
}

bool taintReg(REG reg)
{
    if (checkAlreadyRegTainted(reg) == true) {
        TaintFile << "\t\t\t" << REG_StringShort(reg) << " is already tainted" << std::endl;
        return false;
    }

    switch (reg) {

        case REG_EAX:  regsTainted.push_front(REG_EAX); 
        case REG_AX:   regsTainted.push_front(REG_AX); 
        case REG_AH:   regsTainted.push_front(REG_AH); 
        case REG_AL:   regsTainted.push_front(REG_AL); 
                       break;

        case REG_EBX:  regsTainted.push_front(REG_EBX);
        case REG_BX:   regsTainted.push_front(REG_BX);
        case REG_BH:   regsTainted.push_front(REG_BH);
        case REG_BL:   regsTainted.push_front(REG_BL);
                       break;

        case REG_ECX:  regsTainted.push_front(REG_ECX);
        case REG_CX:   regsTainted.push_front(REG_CX);
        case REG_CH:   regsTainted.push_front(REG_CH);
        case REG_CL:   regsTainted.push_front(REG_CL);
                       break;

        case REG_EDX:  regsTainted.push_front(REG_EDX); 
        case REG_DX:   regsTainted.push_front(REG_DX); 
        case REG_DH:   regsTainted.push_front(REG_DH); 
        case REG_DL:   regsTainted.push_front(REG_DL); 
                       break;

        case REG_EDI:  regsTainted.push_front(REG_EDI); 
        case REG_DI:   regsTainted.push_front(REG_DI); 
                       break;

        case REG_ESI:  regsTainted.push_front(REG_ESI); 
        case REG_SI:   regsTainted.push_front(REG_SI); 
                       break;

        default:
                       TaintFile << "\t\t\t" << REG_StringShort(reg) << " can't be tainted" << std::endl;
                       return false;
    }

    TaintFile << "\t\t\t" << REG_StringShort(reg) << " is now tainted" << std::endl;
    return true;
}

bool removeRegTainted(REG reg)
{
    switch(reg) {

        case REG_EAX:  regsTainted.remove(REG_EAX);
        case REG_AX:   regsTainted.remove(REG_AX);
        case REG_AH:   regsTainted.remove(REG_AH);
        case REG_AL:   regsTainted.remove(REG_AL);
                       break;

        case REG_EBX:  regsTainted.remove(REG_EBX);
        case REG_BX:   regsTainted.remove(REG_BX);
        case REG_BH:   regsTainted.remove(REG_BH);
        case REG_BL:   regsTainted.remove(REG_BL);
                       break;

        case REG_ECX:  regsTainted.remove(REG_ECX);
        case REG_CX:   regsTainted.remove(REG_CX);
        case REG_CH:   regsTainted.remove(REG_CH);
        case REG_CL:   regsTainted.remove(REG_CL);
                       break;

        case REG_EDX:  regsTainted.remove(REG_EDX); 
        case REG_DX:   regsTainted.remove(REG_DX); 
        case REG_DH:   regsTainted.remove(REG_DH); 
        case REG_DL:   regsTainted.remove(REG_DL); 
                       break;

        case REG_EDI:  regsTainted.remove(REG_EDI); 
        case REG_DI:   regsTainted.remove(REG_DI); 
                       break;

        case REG_ESI:  regsTainted.remove(REG_ESI); 
        case REG_SI:   regsTainted.remove(REG_SI); 
                       break;

        default:
                       return false;
    }
    TaintFile << "\t\t\t" << REG_StringShort(reg) << " is now freed" << std::endl;
    return true;
}

VOID ReadMem(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT32 memOp, UINT32 sp)
{
    list<UINT32>::iterator i;
    UINT32 addr = memOp;

    if (opCount != 2)
        return;

    //TaintFile << insDis << endl;

    for (i = addressTainted.begin(); i != addressTainted.end(); i++) {
        if (addr == *i) {
            TaintFile << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << endl;
            taintReg(reg_r);

            if (sp > addr && addr > 0x700000000000) {
                TaintFile << std::hex << "[UAF in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
            }

            return;
        }
    }
    /* if mem != tained and reg == taint => free the reg */
    if (checkAlreadyRegTainted(reg_r)) {
        TaintFile << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << endl;
        removeRegTainted(reg_r);
    }
}

VOID WriteMem(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT32 memOp, UINT32 sp)
{
    list<UINT32>::iterator i;
    UINT32 addr = memOp;

    if (opCount != 2)
        return;

    //TaintFile << insDis << endl;


    for (i = addressTainted.begin(); i != addressTainted.end(); i++) {
        if (addr == *i) {
            TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
            if (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r))
                removeMemTainted(addr);

            if (sp > addr && addr > 0x700000000000)
                TaintFile << std::hex << "[UAF in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;

            return;
        }
    }
    if (checkAlreadyRegTainted(reg_r)) {
        TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        addMemTainted(addr);
    }
}

//VOID handleLea(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_w, UINT32 memOp, UINT32 sp)
VOID handleLea(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_w, CONTEXT* context, UINT32 disp, REG base, REG index, UINT32 scale)
{
    list<UINT32>::iterator i;
    //UINT32 addr = ea - sp;

    UINT32 ea = disp + PIN_GetContextReg(context, base) + 
                PIN_GetContextReg(context, index) * scale; 


    // Taint register if effective address in tainted memory
    for (i = addressTainted.begin(); i != addressTainted.end(); i++) {
        if (ea == *i) {
            TaintFile << insDis << ":" << ea << endl;
        }
    }
    
}

VOID spreadRegTaint(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_r, REG reg_w)
{
    if (opCount != 2)
        return;

    if (REG_valid(reg_w)) {
        if (checkAlreadyRegTainted(reg_w) && (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r))) {
            TaintFile << "[SPREAD]\t\t" << insAddr << ": " << insDis << std::endl;
            TaintFile << "\t\t\toutput: "<< REG_StringShort(reg_w) << " | input: " << (REG_valid(reg_r) ? REG_StringShort(reg_r) : "constant") << std::endl;
            removeRegTainted(reg_w);
        }
        else if (!checkAlreadyRegTainted(reg_w) && checkAlreadyRegTainted(reg_r)) {
            TaintFile << "[SPREAD]\t\t" << insAddr << ": " << insDis << std::endl;
            TaintFile << "\t\t\toutput: " << REG_StringShort(reg_w) << " | input: "<< REG_StringShort(reg_r) << std::endl;
            taintReg(reg_w);
        }
    }
}

VOID followData(UINT32 insAddr, std::string insDis, REG reg)
{
    if (!REG_valid(reg)) {
        return;
    }

    if (checkAlreadyRegTainted(reg)) {
        TaintFile << "[FOLLOW]\t\t" << insAddr << ": " << insDis << std::endl;
    }

    // TODO catch lea eax, ptr[ebp - 0x1f8]


}


/*
 *  Handle taint propagation during branches. Record if branch was taken or not. 
 */ 
VOID Branches(INS ins, VOID *v)
{
    InstructionFile << insCount << ":" << INS_Disassemble(ins) << endl;
    insCount++;

/*
    if (INS_IsLea(ins)) {
        TaintFile << "LEA: " << INS_Disassemble(ins) << endl;
        TaintFile << "#Operands: " << INS_OperandCount(ins) << endl;
        TaintFile << "Invalid: " << REG_INVALID() << endl;
        TaintFile << "Reg: " << REG_StringShort( INS_OperandReg(ins, 0) ) << endl;
    }
*/

    // Print whether Conditional branch taken or not
    if (INS_Category(ins) == XED_CATEGORY_COND_BR) { 

        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(TaintBranch),
                       IARG_INST_PTR,
                       IARG_BRANCH_TAKEN,
                       IARG_PTR, new string(INS_Disassemble(ins)),
                       IARG_END);

    }

    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)ReadMem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_OperandReg(ins, 0),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);
    }
    else if ( (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsWritten(ins, 0) ) ) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)WriteMem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_OperandReg(ins, 1),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);
    }

    else if (INS_IsLea(ins) ) {

        //cout << INS_OperandMemoryDisplacement(ins, 1) << endl;
        // Effective address = Displacement + BaseReg + IndexReg * Scale 

        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)handleLea,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_RegW(ins, 0),

                IARG_CONST_CONTEXT,
                IARG_UINT32, INS_OperandMemoryDisplacement(ins, 1),
                IARG_UINT32, INS_OperandMemoryBaseReg(ins, 1),
                IARG_UINT32, INS_OperandMemoryIndexReg(ins, 1),
                IARG_UINT32, INS_OperandMemoryScale(ins, 1),

                //IARG_MEMORYOP_EA, 0,
                //IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);

    }
    /*
    else if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)) {

        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)spreadRegTaint,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_UINT32, INS_RegW(ins, 0),
                IARG_END);
    }
    */

    if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)followData,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_END);
    }

} // End Branches



/*
 *  Instrument read syscall
 */
VOID Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, VOID *v)
//VOID Syscall_exit(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, VOID *v)
{
    unsigned int i;
    UINT32 start, size;
    //UINT32 read;

    if (PIN_GetSyscallNumber(ctx, std) == __NR_read) {

        //UINT32 fd = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 0)));
        start = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 1)));
        size  = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 2)));
        //read  = static_cast<UINT32>((PIN_GetSyscallReturn(ctx, std)));

        // Cheap trick for demo, will need to remove later?
         if (size != 500) {
        //if (start > 0xbf000000) {
            return; // only look at the read we know we care about
        }

        for (i = 0; i < size; i++) {
            addressTainted.push_back(start+i);
        }

        TaintFile << "[TAINT]\t" << std::hex << start << 
                  ": " << start+size << " (read)" << endl;
    }
}


VOID Syscall_exit(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, VOID *v)
{
        UINT32 read;
        UINT32 start, size;
        start = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 1)));
        size  = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 2)));
        read = static_cast<UINT32>((PIN_GetSyscallReturn(ctx, std)));

        cout << hex << start << ":" << start+size << ":" << read << endl;

}


VOID InitFollowExecution()
{
    ControlFlowFile.open("controlpath.out");
    ControlFlowFile << hex;
    ControlFlowFile.setf(ios::showbase);

    InstructionFile.open("instructions.out");
    InstructionFile << hex;
    InstructionFile.setf(ios::showbase);

    INS_AddInstrumentFunction(Branches, 0);
    PIN_AddSyscallEntryFunction(Syscall_entry, 0);
    PIN_AddSyscallExitFunction(Syscall_exit, 0);

}

VOID FiniFollowExecution()
{
    // Find collisions between 1st elements in maps to instructions and branches taken
    // This is a round-about way of mapping each cond. jump->taken/~taken

    ControlFlowFile.close();
    TaintFile.close();
    InstructionFile.close();
}

// End FollowExecution.h

