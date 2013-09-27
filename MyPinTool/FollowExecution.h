// FollowExecution.h
/* ===================================================================== */
/* Control Flow following functions, currently at bbl level */
/* ===================================================================== */
std::ofstream ControlFlowFile;

VOID InitFollowExecution()
{
    ControlFlowFile.open("controlpath.out");
    ControlFlowFile << hex;
    ControlFlowFile.setf(ios::showbase);
}

VOID FiniFollowExecution()
{
    ControlFlowFile.close();
}

// For Mutation 
typedef struct 
{
    vector<UINT32> addresses;
    vector<string> instructions;
} ControlFlowStruct;

ControlFlowStruct ControlFlow;


// Follow each instruction in each bbl of each trace.
// basic block is single entrace, single exit block of sequential instructions.
// trace is single entrace, multiple exits, and generated at entrace during runtime.
VOID FollowTraces(TRACE trace, VOID *v)
{
    if (!LOGGING) {
        return;
    }

    ControlFlowFile << "START: " << TRACE_Address(trace) << endl;
    // For each basic block, track each instruction
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins) ) {
		    ControlFlowFile << INS_Disassemble(ins) << "\t" << INS_Address(ins);

            // Get routine name if it's a call instruction
            if (INS_IsDirectCall(ins) && INS_IsDirectBranchOrCall(ins) ) {
                ADDRINT addrint = INS_DirectBranchOrCallTargetAddress(ins);
                ControlFlowFile << RTN_FindNameByAddress(addrint);
            }

            ControlFlowFile << endl;

        }
    }
} // End FollowTraces
