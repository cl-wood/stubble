#ifndef __TAINT_H__ 
#define __TAINT_H__ 

// Taint.h
// Based on Salwan's work, implements DTA

#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <list>
#include <tuple>
#include <unordered_map>
#include <vector>

/* Globals for Stubble */
extern const string results_dir; 
extern unsigned long INS_COUNT;

// Taint globals
const int UNTAINTED = -1;
unsigned long INS_COUNT;

// For taint tracking EFLAGS register's carry/overflow/parity/sign/zero flags
enum {
    CF = 1,     // 0th bit in eflags
    PF = 4,     // 2nd bit
    ZF = 64,    // 6th bit
    SF = 128,   // 7th bit
    OF = 2048   // 11th bit
};

struct eflags_struct {
    ADDRINT value;

    bool CF_tainted;
    string CF_instruction;
    UINT32 CF_user_input_byte;

    bool PF_tainted;
    string PF_instruction;
    UINT32 PF_user_input_byte;

    bool ZF_tainted;
    string ZF_instruction;
    UINT32 ZF_user_input_byte;

    bool SF_tainted;
    string SF_instruction;
    UINT32 SF_user_input_byte;

    bool OF_tainted;
    string OF_instruction;
    UINT32 OF_user_input_byte;

    eflags_struct() {
        value = 0;
        CF_tainted = false;
        PF_tainted = false;
        ZF_tainted = false;
        SF_tainted = false;
        OF_tainted = false;
    }
};

eflags_struct eflags;

// Tainted registers and memory
unordered_map<string, tuple<UINT32, string> > tainted_registers; // TODO make registers an enumerated array of tuple(byteLocation, fileName), if untainted byteLocation = -1
extern unordered_map<ADDRINT, tuple<UINT32, string> > tainted_memory;

// Set of all registers and one for each funct that needs to manipulate regs
REGSET instruction_registers; 

// Files for recording various information
ofstream CondBranchFile;
ofstream TaintFile;
ofstream EflagsFile;
ofstream UnhandledFile;
ofstream NotUsedOpsFile;

/* 
 *  Creates a list of the register and any partial registers, in order to correctly track tainting.
 *      Example: EAX is tainted...AX, AH, and AL should also be tainted.
 *  Use this whenver tainting or untainting registers
 *
 *  ARGS
 *      REG:        register to find partial registers of
 */
list<REG> add_partial_registers(REG reg)
{
    list<REG> regs;
    
    switch(reg) {

        // TODO can we mov to just AH, thus making AH-> AH and AL not true?
        case REG_EAX:  regs.push_front(REG_EAX); 
        case REG_AX:   regs.push_front(REG_AX); 
        case REG_AH:   regs.push_front(REG_AH); 
        case REG_AL:   regs.push_front(REG_AL); 
                       break;

        case REG_EBX:  regs.push_front(REG_EBX);
        case REG_BX:   regs.push_front(REG_BX);
        case REG_BH:   regs.push_front(REG_BH);
        case REG_BL:   regs.push_front(REG_BL);
                       break;

        case REG_ECX:  regs.push_front(REG_ECX);
        case REG_CX:   regs.push_front(REG_CX);
        case REG_CH:   regs.push_front(REG_CH);
        case REG_CL:   regs.push_front(REG_CL);
                       break;

        case REG_EDX:  regs.push_front(REG_EDX); 
        case REG_DX:   regs.push_front(REG_DX); 
        case REG_DH:   regs.push_front(REG_DH); 
        case REG_DL:   regs.push_front(REG_DL); 
                       break;

        case REG_EDI:  regs.push_front(REG_EDI); 
        case REG_DI:   regs.push_front(REG_DI); 
                       break;

        case REG_ESI:  regs.push_front(REG_ESI); 
        case REG_SI:   regs.push_front(REG_SI); 
                       break;

        default:
                       break;
    }

    return regs;

} // End add_partial_registers

/*
 *  Removes taint from a register and all of its partial registers.
 *  Logs the instruction responsible.
 *
 *  ARGS    
 *      REG:        register to remove taint from
 *      string:     the disassembled instruction which resulted in untainting
 */
VOID untaint_register(REG reg, string insDis)
{
    // Create a list containing the register and any of its partial registers
    list<REG> regs = add_partial_registers(reg);

    // For the register and its partial registers, remove taint
    for (list<REG>::iterator it=regs.begin(); it != regs.end(); ++it) {

        string regString = REG_StringShort(*it);
        UINT32 userInputByte = get<0>(tainted_registers[regString]);

        tainted_registers.erase(regString);

        if (DEBUG) {
            TaintFile << INS_COUNT << ":" << userInputByte << "\t" 
                  << "UNTAINT REG\t" << regString << "\t" << insDis << endl;
        }
    }

} // end untaint_register

/* 
 *  Taints a register and all of its partial registers. 
 *      TODO should AL tainted -> EAX tainted? Do we taint parent regs?
 *
 *  ARGS
 *      REG:       register to be tainted
 *      string:    source of taint
 *      string:    whether source of taint is a register or memory location
 */
VOID taint_register(REG reg, string source, string source_type, string insDis)
{
    // Create a set containing the register and any of its partial registers
    list<REG> regs = add_partial_registers(reg);
    UINT32 userInputByte;

    // Decide where to get taint from: need to get cast source to ADDRINT for tainted_memory
    if (source_type == "REGISTER") {
        userInputByte = get<0>(tainted_registers[source]);
    } else if (source_type == "MEMORY") {
        ADDRINT mem_addr = AddrintFromString(source);
        userInputByte = get<0>(tainted_memory[mem_addr]);
    }

    // For the register and its partial registers, propagate taint
    for (list<REG>::iterator it=regs.begin(); it != regs.end(); ++it) {
        string regString = REG_StringShort(*it);

        tainted_registers[regString] = tuple<UINT32, string>(userInputByte, "");
        if (DEBUG) {
            TaintFile << INS_COUNT << ":" << userInputByte << "\t"
                  << "TAINT REG\t" << regString << "\t" << insDis << endl;
        }
    }

} // end taint_register

/* 
 *  Removes taint from memory and logs the transaction.
 *
 * ARGS
 *      string:     the source of the untainting
 *      ADDRINT:    the memory location to remove taint from
 *      string:     the instruction that resulting in taint removal
 */
VOID untaint_memory(string source, ADDRINT sink, string insDis)
{

    if (tainted_registers.count(source) > 0) {
        UINT32 userInputByte = get<0>(tainted_registers[source]);
        if (DEBUG) {
            TaintFile << INS_COUNT << ":" << userInputByte << "\t"
              << "UNTAINT MEM\t" << StringFromAddrint(sink) << "\t" << insDis << endl;
        }
    }
    else {
        if (DEBUG) {
            TaintFile << INS_COUNT << ":IMM" << "\t"
              << "UNTAINT MEM\t" << StringFromAddrint(sink) << "\t" << insDis << endl;
        }
        
    }

    tainted_memory.erase(sink);

}

/* 
 *  Adds taint to memory.
 *  NB: Memory cannot be tainted by other memory or immediate values, so source must be a register.
 *
 *  ARGS
 *      string:     the source of the tainting
 *      ADDRINT:    the memory location to add taint to
 *      string:     the instruction that resulting in taint addition
 */
VOID taint_memory(string source, ADDRINT sink, string insDis)
{
    // Get memory address from string and
    //ADDRINT mem_addr = AddrintFromString(sink);
    UINT32 userInputByte = get<0>(tainted_registers[source]);

    tainted_memory[sink] = tuple<UINT32, string>(userInputByte, insDis);
    if (DEBUG) {
        TaintFile << INS_COUNT << ":" << userInputByte << "\t"
        << "TAINT MEM\t" << StringFromAddrint(sink) << "\t" << insDis << endl;
    }

}

/*
 *  
 *  For register read and register written, create set with those registers and all of their partial registers
 *      Case 1) If reg_r is tainted -> reg_w is now tainted.
 *      Case 2) If reg_r is not tainted and reg_w is tainted -> reg_w is now NOT tainted.
 *
 *  ARGS
 *      UINT32:     not used. TODO see if it's needed--if not then remove 
 *      string:     the op_reg_reg instruction
 *      REG:        the register being read
 *      REG:        the register being written
 */
VOID op_reg_reg(UINT32 insAddr, string insDis, REG reg_r, REG reg_w)
{
    // For read and write registers. Add register and its partial registers to regsets
    list<REG> regs_r = add_partial_registers(reg_r);

    bool reg_r_tainted = false;

    // Any tainted registers in regset_r -> taint propagates to all equivalent registers in regset_w
    for (list<REG>::iterator it=regs_r.begin(); it != regs_r.end(); ++it) {
        string regString = REG_StringShort(*it);
        if (tainted_registers.count(regString) > 0) {
            reg_r_tainted = true;

            // TODO between registers is complicated
            taint_register(reg_w, regString, "REGISTER", insDis);

            break;
        }
    }

    // If reg_r is NOT tainted and reg_w IS tainted, remove taint from reg_w
    if (!reg_r_tainted && tainted_registers.count(REG_StringShort(reg_w) ) > 0) {
        untaint_register(reg_w, insDis);
    }

} // end op_reg_reg

/*
 *      if register not tainted and memory location tainted, remove memory taint.
 *      else 
 *          TODO: Does REG_valid take care of immediate values untainting?
 */
VOID op_mem_reg(UINT32 insAddr, std::string insDis, REG reg_r, UINT32 memOp, UINT32 sp)
{

    string regString = REG_StringShort(reg_r);

    if (tainted_memory.count(memOp) > 0 && 
            (!REG_valid(reg_r) || tainted_registers.count(regString) == 0) ) {
        untaint_memory(regString, memOp, insDis);
        return;    
    }

    // memory not tainted, register is tainted
    if (tainted_memory.count(memOp) == 0 && tainted_registers.count(regString) > 0) { 
        taint_memory(regString, memOp, insDis);
    } 

}

VOID ReadMem(UINT32 insAddr, string insDis, REG reg_r, UINT32 memOp, UINT32 sp)
{
    string regString = REG_StringShort(reg_r);
    // Mem tainted, so taint propagates to register
    if (tainted_memory.count(memOp) > 0) {
        taint_register(reg_r, StringFromAddrint(memOp), "MEMORY", insDis);
    }

    // Mem ~tainted, but register is, so remove taint
    else if (tainted_registers.count(regString) > 0) {
        untaint_register(reg_r, insDis);   
    }

}

VOID followData(UINT32 insAddr, string insDis, REG reg)
{
    if (!REG_valid(reg)) {
        return;
    }

    string regString = REG_StringShort(reg);
    if (tainted_registers.count(regString) > 0) {
        if (DEBUG) {
            TaintFile << INS_COUNT << ":" << get<0>(tainted_registers[regString]) << "\t"
            << "[FOLLOW]\t\t" << insDis << endl;
        }
    }
}


// TODO these two might need to be expanded to handle when it isn't "cmp reg, imm"
VOID save_eflags(INS ins, string insDis, REG reg, CONTEXT* context)
{

    eflags.value = PIN_GetContextReg(context, REG_EFLAGS);

    if (DEBUG) {
        EflagsFile << eflags.value << "\t" << insDis << endl;
    }

}

/*
 *  Checks to see if a tainted instruction modified eflags
 *      If eflags has changed, one of the 5 flags which determine conditional branches
 *      might have become tainted or untainted
 *
 *  ARGS
 *      INS:
 *      string:
 *      REG:
 *      CONTEXT*:
 */
VOID diff_eflags(INS ins, string insDis, REG reg, CONTEXT* context)
{
    string regString = REG_StringShort(reg);

    if (tainted_registers.count(regString) > 0) 
    {
        ADDRINT new_eflags_value = PIN_GetContextReg(context, REG_EFLAGS);

        // Check ZF
        if ( (new_eflags_value & ZF) != (eflags.value & ZF) ) 
        {
            UINT32 userInputByte = get<0>(tainted_registers[regString]);

            eflags.ZF_tainted = true;
            eflags.ZF_instruction = insDis;
            eflags.ZF_user_input_byte = userInputByte;
            if (DEBUG) {
                EflagsFile << new_eflags_value << "\t" << insDis << endl;
                TaintFile << INS_COUNT << ":" << userInputByte << "\t"
                      << "TAINT ZF\t" << insDis << endl;
            }
        }
    }
    else {
        eflags.ZF_tainted = false;
    }

}


/* 
 * Check conditional branch instructions
 *
 *  ARGS
 *      ADDRINT:
 *      string:
 */ 
VOID conditional_branch(ADDRINT ins, string insDis) 
{
    if (eflags.ZF_tainted) {

        if (DEBUG) {
            CondBranchFile << insDis << "\tby\t" << eflags.ZF_instruction 
                       << " from user input " << eflags.ZF_user_input_byte << endl;
        }
    }
}

/* 
 * Instrument instructions to track taint.
 * If an instruction is not instrumented, record it in unhandled_instructions.out
 */
VOID Instructions(INS ins, VOID *v)
{
    // Handle instructions which write EFLAGS register
    //if (INS_RegWContain(ins, REG_EFLAGS) && INS_HasFallThrough(ins) && INS_OperandIsReg(ins, 0) ) {
    if (INS_RegWContain(ins, REG_EFLAGS) && INS_HasFallThrough(ins) ) {

        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)save_eflags,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_CONST_CONTEXT,
                IARG_END);

        INS_InsertCall(
                ins, IPOINT_AFTER, (AFUNPTR)diff_eflags,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_CONST_CONTEXT,
                IARG_END);
    }

    // Handle tainted conditional branches
    if (INS_Category(ins) == XED_CATEGORY_COND_BR) { 

        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)conditional_branch,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                //IARG_UINT32, INS_OperandReg(ins, 0),
                //IARG_MEMORYOP_EA, 0,
                //IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);

    }

    // Next few if/else ifs handle taint introduction, removal and propagation
    // Operations with 0 or 1 operands
    if (INS_OperandCount(ins) < 2) {
        if (DEBUG) {
            NotUsedOpsFile << INS_Disassemble(ins) << endl;
        }
    }

    // TODO also need to handle immediates being op1 and op0 being written by it to untaint?
    

    // Taint can spread in 3 ways:
    //  Case 1) op reg, mem 
    else if (INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)ReadMem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandReg(ins, 0),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);

    }
    //  Case 2) op mem, reg 
    else if (INS_MemoryOperandIsWritten(ins, 0) ) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)op_mem_reg,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandReg(ins, 1),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);

    }

    //  Case 3) op reg, reg 
    else if (INS_OperandIsReg(ins, 0) ) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)op_reg_reg,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_UINT32, INS_RegW(ins, 0),
                IARG_END);
    }

    // Put in "unhandle_instruction.out" for debugging
    else {
        if (DEBUG) {
            UnhandledFile << INS_Disassemble(ins) << endl;
        }
    }

    if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)followData,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_END);
    }

    INS_COUNT++;

} // End Instructions


VOID init_taint()
{

    if (DEBUG) {
        CondBranchFile.open(results_dir + "branches.out");
        TaintFile.open(results_dir + "taint.out");
        EflagsFile.open(results_dir + "eflags.out");
        UnhandledFile.open(results_dir + "unhandled_instructions.out");
        NotUsedOpsFile.open(results_dir + "not_used_ops.out");
    }

    INS_AddInstrumentFunction(Instructions, 0);

    INS_COUNT = 0;

}

VOID fini_taint()
{
    if (DEBUG) {
        CondBranchFile.close();
        TaintFile.close();
        EflagsFile.close();
        UnhandledFile.close();
        NotUsedOpsFile.close();
    }

}

#endif /* __TAINT_H__ */

