// FollowExecution.h

/* ===================================================================== */
/* Control Flow following jumps                                          */
/* ===================================================================== */
std::ofstream ControlFlowFile;


// For Mutation 
typedef struct 
{
    vector<UINT32> addresses;
    vector<string> instructions;
} ControlFlowStruct;

ControlFlowStruct ControlFlow;


// Record every taken jump
// TODO find a better way to ONLY look at instructions inside main
VOID FollowBranches(INS ins, VOID *v)
{
    if (!IN_MAIN) {
        return;
    }

    if (INS_IsBranch(ins)) {
        //ControlFlowFile << INS_Disassemble(ins) << endl;
        // TODO instead of addresses, which will change, get the instruction and (jmpAddr - currentAddr), maybe that'll work better
        //ControlFlowFile << StringFromAddrint(INS_NextAddress(ins)) << endl;
        ControlFlowFile << CATEGORY_StringShort(INS_Category(ins)) << ":" 
                        << StringFromAddrint(INS_NextAddress(ins) - INS_Address(ins)) << endl;
    }

} // End FollowBranches

VOID InitFollowExecution()
{
    ControlFlowFile.open("controlpath.out");
    ControlFlowFile << hex;
    ControlFlowFile.setf(ios::showbase);

    INS_AddInstrumentFunction(FollowBranches, 0);
}

VOID FiniFollowExecution()
{
    ControlFlowFile.close();
}

// End FollowExecution.h

