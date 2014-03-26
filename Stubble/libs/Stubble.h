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

// Tainted registers and memory
unordered_map<string, tuple<UINT32, string> > tainted_registers; // TODO make registers an enumerated array of tuple(byteLocation, fileName), if untainted byteLocation = -1
extern unordered_map<UINT32, tuple<UINT32, string> > tainted_memory;

// Array of registers to check for tainted writes
REG registers[] = { 
                    REG_EFLAGS,
                    REG_EDI, 
                    REG_ESI, 
                    REG_EBP, 
                    REG_ESP, 
                    REG_EDX, 
                    REG_ECX, 
                    REG_EBX, 
                    REG_EAX,  
                    REG_EIP,
                    REG_AL, 
                    REG_AH, 
                    REG_AX, 
                    REG_CL, 
                    REG_CH, 
                    REG_CX, 
                    REG_DL, 
                    REG_DH, 
                    REG_DX, 
                    REG_BL, 
                    REG_BH, 
                    REG_BX, 
                    REG_BP, 
                    REG_SI, 
                    REG_DI, 
                    REG_SP 
                   };

// Files for recording various information
ofstream CondBranchFile;
ofstream TaintFile;
ofstream EflagsFile;
ofstream UnhandledFile;
ofstream NopFile;
ofstream OneOpFile;

// Handles taint tracking for EFLAGS register's carry/overflow/parity/sign/zero flags
VOID taint_eflags()
{

}

VOID write_taint(string type, string source, string sink, string insDis)
{

    TaintFile << "[" << type << "] from\t" << sink << "\tto\t" << source << "\tby\t" << insDis << endl;

}

// When tainted data is written to a register
//      1) Record in TaintFile
//      2) Update tainted_registers data structure
VOID taint_register(string source, string sink, string insDis, string source_type)
{
    

    // If EFLAGS tainted, figure out which flag
    // TODO does this have to be instruction independent?
    //if (regName == "eflags") {
    //    EflagsFile << insDis << "\t[CATEGORY]\t" << category << endl;
    //}

    // tuple should be (memLoc, fileName)?

    // Check if taint source is register or memory
    if (source_type == "REGISTER") {
        tainted_registers[sink] = tuple<UINT32, string>(get<0>(tainted_registers[source]), insDis);
    }
    else if (source_type == "MEMORY") {

    }
    write_taint("TAINT", source, sink, insDis);

}


// Remove memory from  tainted_memory
VOID untaint_memory(string source, string sink, string insDis)
{

    //tainted_memory[Uint32FromString(sink)] = tuple<UINT32, string>(UNTAINTED, "UNTAINTED");
    write_taint("UNTAINT", source, sink, insDis);

}

// Used when:
//  1) untainted data written to register
//  2) xor r1, r1
VOID untaint_register(string source, string sink, string insDis)
{

    tainted_registers.erase(sink);
    write_taint("UNTAINT", source, sink, insDis);

}

// Tainted memory location written to register
VOID taint_mem2reg(string source, string sink, string insDis)
{

    tainted_registers[sink] = tuple<UINT32, string>(0, ""); // TODO should the 0 be memory-loc or loc in user input?
    write_taint("TAINT", source, sink, insDis);

}

// Check to see if reg tainted/untainted by memory
VOID check_mem2reg(INS ins, string insDis, string regName, UINT32 mem, CONTEXT* context) 
{
    stringstream stream;
    stream << std::hex << mem;
    string source(stream.str() );

    // Memory tainted, so register now tainted
    if (tainted_memory.count(mem) > 0) {
        taint_mem2reg(source, regName, insDis);
    }

    // Register tainted but memory not tainted, so register now untainted 
    else if (tainted_registers.count(regName) > 0) {// && get<0>(tainted_registers[regName]) != UNTAINTED) {
        untaint_register(source, regName, insDis);
    }

}

VOID taint_reg2mem(string source, string sink, string insDis)
{

    tainted_registers[sink] = tuple<UINT32, string>(0, ""); // TODO should the 0 be memory-loc or loc in user input?
    write_taint("TAINT", source, sink, insDis);

}

// Check to see if register value written to memory results in T/NT
VOID check_reg2mem(INS ins, string insDis, string regName, UINT32 mem, CONTEXT* context) 
{

    stringstream stream;
    stream << std::hex << mem;
    string source(stream.str() );

    // Memory tainted, so register now tainted
    if (tainted_registers.count(regName) > 0) {
        taint_reg2mem(source, regName, insDis);
    }

    // Register tainted but memory not tainted, so register now untainted 
    else if (tainted_registers.count(regName) > 0) {// && get<0>(tainted_registers[regName]) != UNTAINTED) {
        untaint_register(source, regName, insDis);
    }

}

// TODO both regs could be tainted and r1 could be read AND WRITTEN, 
// in which case we really should give r1 multiple taint values
VOID op_reg_reg(INS ins, string insDis, string regName1, string regName2)
{
    if (tainted_registers.count(regName2) > 1) {
        taint_register(regName2, regName1, insDis, "REGISTER");
        cout << insDis << endl;
    }

}

// If immediate value written to register, remove any taint in register
VOID op_reg_imm(INS ins, string insDis, string regName)
{
    if (tainted_registers.count(regName) > 0) {
        untaint_register("IMMEDIATE", regName, insDis);
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
    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
    else if ( (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsWritten(ins, 0) ) ) {
    else if (INS_IsLea(ins) ) {
    else if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)) {
    if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0) 
        && INS_OperandIsImmediate(ins, 1) ) { 
*/

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

    // TODO ex., add r1, r2 gets r1 AND r2s taint if both are tainted...make DS ->list of tuples or something

    // Taint status only changes when data is written
    else if (INS_OperandWritten(ins, 0) || INS_OperandWritten(ins, 1) ) { 
        
        // 5 cases: 1) op reg, mem
        //          2) op reg, reg
        //          3) op reg, imm 

        //          4) op mem, reg 
        //          5) op mem, imm

        // Case 2) reg, reg
        if (INS_OperandIsReg(ins, 0) && INS_OperandIsReg(ins, 1) ) {

            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(op_reg_reg),
                IARG_INST_PTR,
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 1) ) ),
                IARG_END);

        }

        // Case 3) op reg, imm
        if (INS_OperandIsReg(ins, 0) && INS_OperandIsImmediate(ins, 1) ) {

            // Remove taint, immediates can't be tainted
            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(op_reg_imm),
                IARG_INST_PTR,
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                IARG_END);
        }
        //if (INS_OperandReg(ins, 0) && INS_OperandReg(ins, 1) ) {
        //    cout << INS_Disassemble(ins) << endl;
        //}


    }


        // 
        // Operations with 2 operands
        //
        // TODO taint_memory can use INS_IsMemoryWrite(ins)
        // INS_IsStackWrite also could be useful

        // TODO check for taint here instead of in function??
        // TODO figure out why this doesn't work without these 3
        // Handled tainting and untainting of registers
        //else if (INS_OperandIsReg(ins, 0) && INS_MemoryOperandIsRead(ins, 0) ) {

        // Check all reads
        /*
           else if (INS_MemoryOperandIsRead(ins, 0) || INS_MemoryOperandIsRead(ins, 1) ) {


        // if either in tainted memory

        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(check_mem2reg),
        IARG_INST_PTR,
        IARG_PTR, new string(INS_Disassemble(ins)),
        IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
        IARG_MEMORYOP_EA, 0,
        IARG_CONST_CONTEXT,
        IARG_END);
        }

        // Memory written
        else if (INS_MemoryOperandIsWritten(ins, 0) && INS_OperandIsReg(ins, 1) ) {

        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(check_reg2mem),
        IARG_INST_PTR,
        IARG_PTR, new string(INS_Disassemble(ins)),
        IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
        IARG_MEMORYOP_EA, 0,
        IARG_CONST_CONTEXT,
        IARG_END);

        }
         */

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
    TaintFile.open(results_dir + "taint_regs.out");
    EflagsFile.open(results_dir + "eflags.out");
    UnhandledFile.open(results_dir + "unhandled_instructions.out");
    NopFile.open(results_dir + "nops.out");
    OneOpFile.open(results_dir + "one_ops.out");

    INS_AddInstrumentFunction(Instructions, 0);

    // Init libraries
    init_intercept_signals();
    init_syscalls();

    INS_COUNT = 0;

}

VOID fini_stubble()
{
    // iterate over DS to check
    //for (string s : tainted_registers) {
    //    cout << s << "\t" << get<0>(tainted_registers[s]) << endl;
    //}

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

