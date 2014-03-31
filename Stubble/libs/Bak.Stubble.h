#ifndef __STUBBLE_H__ 
#define __STUBBLE_H__ 

// Stubble.h
// http://en.wikibooks.org/wiki/X86_Assembly/Control_Flow and the book in general is very helpful

#include "syscalls.h"
#include "intercept_signals.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>

/* Globals for Stubble */

const string results_dir = "./results/currentrun/";
const int UNTAINTED = -1;
unsigned long INS_COUNT;

// For taint tracking EFLAGS register's carry/overflow/parity/sign/zero flags
enum {
    CF = 1,
    PF = 4,
    ZF = 64,
    SF = 128,
    OF = 2048
};

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
ofstream NopFile;
ofstream OneOpFile;

// Helper function, given the regs identified in the instruction, adds the partial registers associated
// Ex, EAX is tainted...AX, AH, and AL should also be tainted
// Use this whenever passing registers
// TODO Clear at start of callbacks, popping might take care of that?
VOID add_partial_registers()
{
    //REGSET_Clear(instruction_registers); // Use BEFORE this function

    // EAX register and its partial registers
    if (REGSET_Contains(instruction_registers, REG_EAX) ) {
        REGSET_Insert(instruction_registers, REG_AX); 
    }
    if (REGSET_Contains(instruction_registers, REG_AX) ) {
        REGSET_Insert(instruction_registers, REG_AH); 
        REGSET_Insert(instruction_registers, REG_AL); 
    }

    // EBX register and its partial registers
    if (REGSET_Contains(instruction_registers, REG_EBX) ) {
        REGSET_Insert(instruction_registers, REG_BX); 
    }
    if (REGSET_Contains(instruction_registers, REG_BX) ) {
        REGSET_Insert(instruction_registers, REG_BH); 
        REGSET_Insert(instruction_registers, REG_BL); 
    }

    // ECX register and its partial registers
    if (REGSET_Contains(instruction_registers, REG_ECX) ) {
        REGSET_Insert(instruction_registers, REG_CX); 
    }
    if (REGSET_Contains(instruction_registers, REG_CX) ) {
        REGSET_Insert(instruction_registers, REG_CH); 
        REGSET_Insert(instruction_registers, REG_CL); 
    }

    // EDX register and its partial registers
    if (REGSET_Contains(instruction_registers, REG_EDX) ) {
        REGSET_Insert(instruction_registers, REG_DX); 
    }
    if (REGSET_Contains(instruction_registers, REG_DX) ) {
        REGSET_Insert(instruction_registers, REG_DH); 
        REGSET_Insert(instruction_registers, REG_DL); 
    }

    // EDI/DI
    if (REGSET_Contains(instruction_registers, REG_EDI) ) {
        REGSET_Insert(instruction_registers, REG_DI); 
    }

    // ESI/SI
    if (REGSET_Contains(instruction_registers, REG_ESI) ) {
        REGSET_Insert(instruction_registers, REG_SI); 
    }


} // End add_partial_registers

VOID write_taint(string type, UINT32 userInput, string source, string sink, string insDis)
{

    TaintFile << "[" << type << "]";

    // Specify which byte of user input
    if (type == "TAINT") {
        TaintFile << "\t" << userInput;
    }

    TaintFile << "\t" << source << "\t->\t" << sink << "\tby\t" << insDis << endl;

}

// When tainted data is written to a register
//      1) Record in TaintFile
//      2) Update tainted_registers data structure
VOID taint_register(string source, string sink, string insDis, string source_type)
{
        cout << "T REG" << endl;


    // If EFLAGS tainted, figure out which flag
    // TODO does this have to be instruction independent?
    ADDRINT mem_addr = AddrintFromString(source);

    UINT32 originalUserInput = 0;

    // Check if taint source is register or memory
    if (source_type == "REGISTER") {
        originalUserInput = get<0>(tainted_registers[source]);
    } else if (source_type == "MEMORY") {
        originalUserInput = get<0>(tainted_memory[mem_addr]);
    }

        //tainted_registers[sink] = tuple<UINT32, string>(originalUserInput, insDis);
        //write_taint("TAINT", originalUserInput, source, sink, insDis);
    add_partial_registers();
    while (!REGSET_PopCountIsZero(instruction_registers) ) {
        REG reg = REGSET_PopNext(instruction_registers); 
        REGSET_Remove(instruction_registers, reg);
        string r = REG_StringShort(reg); 

        tainted_registers[r] = tuple<UINT32, string>(originalUserInput, insDis);
        write_taint("TAINT", originalUserInput, source, r, insDis);
    }


}


// Remove memory from  tainted_memory
VOID untaint_memory(string source, string sink, string insDis)
{
    ADDRINT mem_addr = AddrintFromString(source);

    tainted_memory.erase(mem_addr);

    // For UNTAINT, the 0 for originalUserInput isn't written to TaintFile
    write_taint("UNTAINT", 0, source, sink, insDis);

}

// Used when:
//  1) untainted data written to register
//  2) xor r1, r1
VOID untaint_register(string source, string sink, string insDis)
{
    add_partial_registers();

    while (!REGSET_PopCountIsZero(instruction_registers) ) {
        
        REG reg = REGSET_PopNext(instruction_registers); 
        REGSET_Remove(instruction_registers, reg);
        string r = REG_StringShort(reg); 

        tainted_registers.erase(r);
        write_taint("UNTAINT", 0, source, r, insDis);
    }

    //tainted_registers.erase(sink);

    // For UNTAINT, the 0 for originalUserInput isn't written to TaintFile
    //write_taint("UNTAINT", 0, source, sink, insDis);

}

// Memory to memory is not allowed in x86, so if memory is tainted it must be by a register
VOID taint_memory(string source, string sink, string insDis)
{
        cout << "T MEM" << endl;
    ADDRINT mem_addr = AddrintFromString(sink);

    UINT32 originalUserInput = get<0>(tainted_registers[source]);
    tainted_memory[mem_addr] = tuple<UINT32, string>(originalUserInput, insDis);

    write_taint("TAINT", originalUserInput, source, sink, insDis);

}

// Special case since cmp instructions and the like modify flags for branch instructions
// Handles taint tracking for EFLAGS register's carry/overflow/parity/sign/zero flags
VOID save_eflags_2regs(INS ins, string insDis, string reg1, string reg2, CONTEXT* context)
{

    if (tainted_registers.count(reg1) > 0 || tainted_registers.count(reg2) > 0) {
        ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
        EflagsFile << eflags << "\t" << insDis << endl;
    }

}

VOID handle_eflags_2regs(INS ins, string insDis, string reg1, string reg2, CONTEXT* context)
{

    if (tainted_registers.count(reg1) > 0 || tainted_registers.count(reg2) > 0) {
        ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
        EflagsFile << eflags << "\t" << insDis << endl;
    }

}

VOID save_eflags_reg_imm(INS ins, string insDis, string reg1, UINT32 imm, CONTEXT* context)
{

    if (tainted_registers.count(reg1) > 0) {
        ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
        EflagsFile << eflags << "\t" << insDis << "\t" << imm << endl;
    }

}

VOID handle_eflags_reg_imm(INS ins, string insDis, string reg1, UINT32 imm, CONTEXT* context)
{

    if (tainted_registers.count(reg1) > 0) {
        ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
        EflagsFile << eflags << "\t" << insDis << "\t" << imm << endl;
    }

}

// 5 cases

// If mem is tainted, reg is now tainted
// If mem is untainted and reg is tainted, reg is now untainted
VOID op_reg_mem(INS ins, string insDis, string regName, REG r, ADDRINT mem)
{
    stringstream stream;
    stream << std::hex << mem;
    string source(stream.str() );

    //REGSET_Insert(instruction_registers, r ); 

    if (tainted_memory.count(mem) > 0) {
    cout << "HERE" << endl;

        taint_register(source, regName, insDis, "MEMORY");

        //UINT32 originalUserInput = get<0>(tainted_memory[source]);
        //tainted_registers[regName] = tuple<UINT32, string>(originalUserInput, insDis);
        //write_taint("TAINT", originalUserInput, source, regName, insDis);
    }

    // Mem is untainted, try to erase
    else if (tainted_registers.erase(regName) != 0) {
        //write_taint("UNTAINT", 0, source, regName, insDis);
        untaint_register(source, regName, insDis);
    }

}

// TODO both regs could be tainted and r1 could be read AND WRITTEN, 
// in which case we really should give r1 multiple taint values
VOID op_reg_reg(INS ins, string insDis, string regName1, string regName2, REG r)
{
    REGSET_Insert(instruction_registers, r ); 

    if (tainted_registers.count(regName2) > 0) {
        taint_register(regName2, regName1, insDis, "REGISTER");
    }

}

// If immediate value written to register, remove any taint in register
VOID op_reg_imm(INS ins, string insDis, string regName, REG r)
{
    REGSET_Insert(instruction_registers, r ); 

    if (tainted_registers.count(regName) > 0) {
        untaint_register("IMMEDIATE", regName, insDis);
    }

}

// If register is tainted, memory is now tainted regardless
// If register is untainted and memory is tainted, remove taint
VOID op_mem_reg(INS ins, string insDis, string regName, UINT32 mem)
{
    stringstream stream;
    stream << std::hex << mem;
    string sink(stream.str() );

    if (tainted_registers.count(regName) > 0) {

        UINT32 originalUserInput = get<0>(tainted_registers[regName]);
        tainted_memory[mem] = tuple<UINT32, string>(originalUserInput, insDis);
        write_taint("TAINT", originalUserInput, regName, sink, insDis);
    }

    // Know register is not tainted here, if mem tainted, then remove taint
    else if (tainted_memory.erase(mem) ) {
        write_taint("UNTAINT", 0, regName, sink, insDis);
    }

}

// If memory is tainted, it is now untainted
VOID op_mem_imm(INS ins, string insDis, UINT32 mem)
{
    stringstream stream;
    stream << std::hex << mem;
    string sink(stream.str() );

    if (tainted_memory.erase(mem) ) {
        write_taint("UNTAINT", 0, "IMMEDIATE", sink, insDis);
    }

}

/* ===================================================================== */
/* Check conditional branch instructions                                 */
/* ===================================================================== */
VOID conditional_branch(ADDRINT ins, INT32 branchTaken, string insDis) 
{

    CondBranchFile << insDis << endl;

}

/* 
 * Instrument instructions to track taint.
 * If an instruction is not instrumented, record it in unhandled_instructions.out
 */
VOID Instructions(INS ins, VOID *v)
{


    // Operations with 0 operands
    if (INS_OperandCount(ins) == 0) {
        NopFile << INS_Disassemble(ins) << endl;
    }

    // Operations with 1 operand
    // Seems to include 'not's and 'data16's
    else if (INS_OperandCount(ins) == 1) {
        OneOpFile << INS_Disassemble(ins) << endl;

        // Conditional branches
        if (INS_Category(ins) == XED_CATEGORY_COND_BR) { 

            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(conditional_branch),
                    IARG_INST_PTR,
                    IARG_BRANCH_TAKEN,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_END);
        }
    }

    // Operations with 2 operands
    // Two times when memory is written and stack is not effected
    // 1) op mem, reg 
    // 2) op mem, imm
    else if (INS_IsMemoryWrite(ins) && !INS_IsStackWrite(ins) ) {

        // Case 1: op mem, reg
        if (INS_OperandIsReg(ins, 1) ) {

            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(op_mem_reg),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    IARG_MEMORYOP_EA, 0, // TODO this is broken, adding IsBranch "fixed" it, or maybe IsStackWrite?
                    IARG_END);
        }

        // Case 2: op mem, imm
        else if (INS_OperandIsImmediate(ins, 1) ) {

            // Remove taint, immediates can't be tainted
            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(op_mem_imm),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_MEMORYOP_EA, 0,
                    IARG_END);

        }

    }

    // For registers, 3 cases: 1) op reg, mem
    //                         2) op reg, reg
    //                         3) op reg, imm 
    else if (INS_OperandIsReg(ins, 0) && INS_OperandWritten(ins, 0) && !INS_IsStackWrite(ins) ) {

        // Case 1) reg, mem
        // TODO handle lea's separately
        if (!INS_OperandIsReg(ins, 1) && !INS_OperandIsImmediate(ins, 1) && !INS_IsLea(ins) ) {

            //if (!INS_OperandIsMemory(ins, 1) ) cout << "BAAAD" << endl;

//cout << INS_Disassemble(ins) << endl;
//PROTOTYPE op_reg_mem(INS ins, string insDis, string regName, ADDRINT mem, REG r)
            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(op_reg_mem),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    //IARG_ADDRINT, INS_OperandMemory(ins, 1),
                    // INS_OperandIsMemory is a thing
                    IARG_UINT32, INS_OperandReg(ins, 0),
                    
                    IARG_MEMORYOP_EA, 0, // TODO why is this 0 and not 1???
                    IARG_END);

        }

        // Case 2) reg, reg
        else if (INS_OperandIsReg(ins, 1) ) {

            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(op_reg_reg),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 1) ) ),
                    IARG_UINT32, INS_OperandReg(ins, 0),
                    //IARG_UINT32, INS_OperandReg(ins, 1),
                    IARG_END);

        }

        // Case 3) op reg, imm
        else if (INS_OperandIsImmediate(ins, 1) ) {

            // Remove taint, immediates can't be tainted
            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(op_reg_imm),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    IARG_UINT32, INS_OperandReg(ins, 0),
                    IARG_END);
        }

    }


    // Get instructions that modify eflags
    else if (INS_RegWContain(ins, REG_EFLAGS) && INS_HasFallThrough(ins) ) {

        // 2 Cases identified so far, op reg, reg and op reg, imm
        if (INS_OperandIsReg(ins, 0) && INS_OperandIsReg(ins, 1) ) {

            // So far this has only been tests and cmps
            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(save_eflags_2regs),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 1) ) ),
                    IARG_CONST_CONTEXT,
                    IARG_END);

            INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(handle_eflags_2regs),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 1) ) ),
                    IARG_CONST_CONTEXT,
                    IARG_END);

        }

        // Case 2) op reg, imm
        else if (INS_OperandIsReg(ins, 0) && INS_OperandIsImmediate(ins, 1) ) {

            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(save_eflags_reg_imm),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    IARG_UINT32, static_cast<UINT32>(INS_OperandImmediate(ins, 1)), // HACK, stacksmash if UINT64
                    IARG_CONST_CONTEXT,
                    IARG_END);

            INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(handle_eflags_reg_imm),
                    IARG_INST_PTR,
                    IARG_PTR, new string(INS_Disassemble(ins)),
                    IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                    IARG_UINT32, static_cast<UINT32>(INS_OperandImmediate(ins, 1)), // HACK, stacksmash if UINT64
                    IARG_CONST_CONTEXT,
                    IARG_END);
        }

    }

    // Put in "unhandle_instruction.out" for debugging
    else {
        UnhandledFile << INS_Disassemble(ins) << endl;
    }

    INS_COUNT++;

} // End Instructions


VOID init_stubble()
{
    //ControlFlowFile << hex;
    //ControlFlowFile.setf(ios::showbase);

    CondBranchFile.open(results_dir + "branches.out");
    TaintFile.open(results_dir + "taint.out");
    EflagsFile.open(results_dir + "eflags.out");
    UnhandledFile.open(results_dir + "unhandled_instructions.out");
    NopFile.open(results_dir + "nops.out");
    OneOpFile.open(results_dir + "one_ops.out");

    INS_AddInstrumentFunction(Instructions, 0);


    // Init libraries
    init_intercept_signals();
    init_syscalls();

/*
    REGSET_Clear(instruction_registers);
    REGSET_Insert(instruction_registers, REG_EAX);
    while (!REGSET_PopCountIsZero(instruction_registers) ) {
        REG reg = REGSET_PopNext(instruction_registers); 
        REGSET_Remove(instruction_registers, reg);
        string r = REG_StringShort(reg); 

        cout << r << endl;
    }
    */

    INS_COUNT = 0;

}

VOID fini_stubble()
{

    CondBranchFile.close();
    TaintFile.close();
    EflagsFile.close();
    UnhandledFile.close();
    NopFile.close();
    OneOpFile.close();

    fini_intercept_signals();
    fini_syscalls();
}

#endif /* __STUBBLE_H__ */

