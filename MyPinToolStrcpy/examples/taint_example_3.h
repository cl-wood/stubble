unordered_map<ADDRINT, BOOL> AddressTaintMap;
unordered_map<ADDRINT, BOOL> RegisterTaintMap;
std::ofstream TaintFile;

bool checkAlreadyRegTainted(REG reg)
{
    if (RegisterTaintMap.count(reg) > 0) {
      return true;
    }
  return false;
}

VOID removeMemTainted(UINT32 addr)
{
    AddressTaintMap.erase(addr);
  TaintFile << std::hex << "\t\t\t" << addr << " is now freed" << std::endl;
}

VOID addMemTainted(UINT32 addr)
{
    AddressTaintMap[addr] = false;
  TaintFile << std::hex << "\t\t\t" << addr << " is now tainted" << std::endl;
}

bool taintReg(REG reg)
{
  if (checkAlreadyRegTainted(reg) == true){
    TaintFile << "\t\t\t" << REG_StringShort(reg) << " is already tainted" << std::endl;
    return false;
  }

  switch(reg){

    case REG_EAX:  RegisterTaintMap.insert(REG_EAX); 
    case REG_AX:   RegisterTaintMap.insert(REG_AX); 
    case REG_AH:   RegisterTaintMap.insert(REG_AH); 
    case REG_AL:   RegisterTaintMap.insert(REG_AL); 
         break;

    case REG_EBX:  RegisterTaintMap.insert(REG_EBX);
    case REG_BX:   RegisterTaintMap.insert(REG_BX);
    case REG_BH:   RegisterTaintMap.insert(REG_BH);
    case REG_BL:   RegisterTaintMap.insert(REG_BL);
         break;

    case REG_ECX:  RegisterTaintMap.insert(REG_ECX);
    case REG_CX:   RegisterTaintMap.insert(REG_CX);
    case REG_CH:   RegisterTaintMap.insert(REG_CH);
    case REG_CL:   RegisterTaintMap.insert(REG_CL);
         break;

    case REG_EDX:  RegisterTaintMap.insert(REG_EDX); 
    case REG_DX:   RegisterTaintMap.insert(REG_DX); 
    case REG_DH:   RegisterTaintMap.insert(REG_DH); 
    case REG_DL:   RegisterTaintMap.insert(REG_DL); 
         break;

    case REG_EDI:  RegisterTaintMap.insert(REG_EDI); 
    case REG_DI:   RegisterTaintMap.insert(REG_DI); 
         break;

    case REG_ESI:  RegisterTaintMap.insert(REG_ESI); 
    case REG_SI:   RegisterTaintMap.insert(REG_SI); 
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

    case REG_EAX:  RegisterTaintMap.erase(REG_EAX);
    case REG_AX:   RegisterTaintMap.erase(REG_AX);
    case REG_AH:   RegisterTaintMap.erase(REG_AH);
    case REG_AL:   RegisterTaintMap.erase(REG_AL);
         break;

    case REG_EBX:  RegisterTaintMap.erase(REG_EBX);
    case REG_BX:   RegisterTaintMap.erase(REG_BX);
    case REG_BH:   RegisterTaintMap.erase(REG_BH);
    case REG_BL:   RegisterTaintMap.erase(REG_BL);
         break;

    case REG_ECX:  RegisterTaintMap.erase(REG_ECX);
    case REG_CX:   RegisterTaintMap.erase(REG_CX);
    case REG_CH:   RegisterTaintMap.erase(REG_CH);
    case REG_CL:   RegisterTaintMap.erase(REG_CL);
         break;

    case REG_EDX:  RegisterTaintMap.erase(REG_EDX); 
    case REG_DX:   RegisterTaintMap.erase(REG_DX); 
    case REG_DH:   RegisterTaintMap.erase(REG_DH); 
    case REG_DL:   RegisterTaintMap.erase(REG_DL); 
         break;

    case REG_EDI:  RegisterTaintMap.erase(REG_EDI); 
    case REG_DI:   RegisterTaintMap.erase(REG_DI); 
         break;

    case REG_ESI:  RegisterTaintMap.erase(REG_ESI); 
    case REG_SI:   RegisterTaintMap.erase(REG_SI); 
         break;

    default:
      return false;
  }
  TaintFile << "\t\t\t" << REG_StringShort(reg) << " is now freed" << std::endl;
  return true;
}

VOID ReadMem(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT32 memOp)
{
  list<UINT32>::iterator i;
  UINT32 addr = memOp;
  
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
  if (checkAlreadyRegTainted(reg_r)){
    TaintFile << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
    removeRegTainted(reg_r);
  }
}

VOID WriteMem(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT32 memOp)
{
  list<UINT32>::iterator i;
  UINT32 addr = memOp;

  if (opCount != 2)
    return;

    for (auto it = AddressTaintMap.begin(); it != AddressTaintMap.end(); ++it) {
        if (addr == it->first) {
        TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        if (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r))
          removeMemTainted(addr);
        return ;
      }
  }
  if (checkAlreadyRegTainted(reg_r)){
    TaintFile << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
    addMemTainted(addr);
  }
}

VOID spreadRegTaint(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_r, REG reg_w)
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

VOID followData(UINT32 insAddr, std::string insDis, REG reg)
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
  UINT32 start, size;

  if (PIN_GetSyscallNumber(ctx, std) == __NR_read){

      TRICKS(); /* tricks to ignore the first open */

      start = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 1)));
      size  = static_cast<UINT32>((PIN_GetSyscallArgument(ctx, std, 2)));

      for (i = 0; i < size; i++)
        addressTainted.push_back(start+i);
      
      TaintFile << "[TAINT]\t\t\tbytes tainted from " << std::hex << "0x" << start << " to 0x" << start+size << " (via read)"<< std::endl;
  }
}


