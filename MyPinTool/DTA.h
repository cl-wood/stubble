//
//  Jonathan Salwan - Copyright (C) 2013-08
// 
//  http://shell-storm.org
//  http://twitter.com/JonathanSalwan
//
//  Note: Example 4 - http://shell-storm.org/blog/Taint-analysis-with-Pin/
//        Detect a use after free vulnerability via a old stack frame
//

#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>

std::ofstream TaintFile;
std::list<UINT64> addressTainted;
std::list<REG> regsTainted;


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

VOID removeMemTainted(UINT64 addr)
{
  addressTainted.remove(addr);
  TaintFile << std::hex << "\t\t\t" << addr << " is now freed" << std::endl;
}

VOID addMemTainted(UINT64 addr)
{
  addressTainted.push_back(addr);
  TaintFile << std::hex << "\t\t\t" << addr << " is now tainted" << std::endl;
}

bool taintReg(REG reg)
{
  if (checkAlreadyRegTainted(reg) == true){
    TaintFile << "\t\t\t" << REG_StringShort(reg) << " is already tainted" << std::endl;
    return false;
  }

  switch(reg){

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
  switch(reg){

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

VOID ReadMem(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT64 memOp, UINT64 sp)
{
  list<UINT64>::iterator i;
  UINT64 addr = memOp;
  
  if (opCount != 2)
    return;

  for(i = addressTainted.begin(); i != addressTainted.end(); i++){
      if (addr == *i){
        TaintFile << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        taintReg(reg_r);
      
        if (sp > addr && addr > 0x700000000000)
          TaintFile << std::hex << "[UAF in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;

        return ;
      }
  }
  /* if mem != tained and reg == taint => free the reg */
  if (checkAlreadyRegTainted(reg_r)){
    TaintFile << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
    removeRegTainted(reg_r);
  }
}

VOID WriteMem(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT64 memOp, UINT64 sp)
{
  list<UINT64>::iterator i;
  UINT64 addr = memOp;

  if (opCount != 2)
    return;

  for(i = addressTainted.begin(); i != addressTainted.end(); i++){
      if (addr == *i){
        TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        if (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r))
          removeMemTainted(addr);
        
        if (sp > addr && addr > 0x700000000000)
          TaintFile << std::hex << "[UAF in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;

        return ;
      }
  }
  if (checkAlreadyRegTainted(reg_r)){
    TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
    addMemTainted(addr);
  }
}

VOID spreadRegTaint(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, REG reg_w)
{
  if (opCount != 2)
    return;

  if (REG_valid(reg_w)){
    if (checkAlreadyRegTainted(reg_w) && (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r))){
      TaintFile << "[SPREAD]\t\t" << insAddr << ": " << insDis << std::endl;
      TaintFile << "\t\t\toutput: "<< REG_StringShort(reg_w) << " | input: " << (REG_valid(reg_r) ? REG_StringShort(reg_r) : "constant") << std::endl;
      removeRegTainted(reg_w);
    }
    else if (!checkAlreadyRegTainted(reg_w) && checkAlreadyRegTainted(reg_r)){
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

  if (checkAlreadyRegTainted(reg)){
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
        IARG_REG_VALUE, REG_STACK_PTR,
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
        IARG_REG_VALUE, REG_STACK_PTR,
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
      
      // TODO 2nd arg can be too large, might not be an issue if all the area alloced isn't re-used

      cout << start << ":" << size << endl;

      for (i = 0; i < size; i++)
        addressTainted.push_back(start+i);
      
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
}

VOID FiniDTA()
{
    TaintFile.close();
}

// End DTA.h

