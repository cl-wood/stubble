// FollowExecution.h

#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>


/* ===================================================================== */
/* Control Flow following jumps                                          */
/* ===================================================================== */
std::ofstream ControlFlowFile;

unordered_map<int, string>addressToInstruction;
unordered_map<int, int>addressToBranchTaken;
// TODO both can have address of instruction, can put all the stuff in a DS and check after execting.

/*
 * Records whether branch taken or not 
 */
VOID TaintBranch(ADDRINT ins, INT32 branchTaken) {

    addressToBranchTaken.insert({ins, branchTaken});
    /*
    if (branchTaken) {
        ControlFlowFile << ins << " " << "BRANCH" << endl;
    }
    else {
        ControlFlowFile << ins << " " << "NO BRANCH" << endl;
    }
    */

}

/*
 *  Handle taint propagation during branches. Record if branch was taken or not. 
 */ 
VOID Branches(INS ins, VOID *v)
{

    // Print whether Conditional branch taken or not
    if (INS_Category(ins) == XED_CATEGORY_COND_BR) { 

        pair<int, string> mapping(INS_Address(ins), INS_Disassemble(ins));
        addressToInstruction.insert(mapping);

        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(TaintBranch),
                       IARG_INST_PTR,
                       IARG_BRANCH_TAKEN,
                       IARG_END);

    }

} // End Branches

/*
    if (INS_Opcode(ins) == XED_ICLASS_MOV &&
        INS_IsMemoryRead(ins) && 
        INS_OperandIsReg(ins, 0) &&
        INS_OperandIsMemory(ins, 1))

*/

VOID InitFollowExecution()
{
    ControlFlowFile.open("controlpath.out");
    ControlFlowFile << hex;
    ControlFlowFile.setf(ios::showbase);

    INS_AddInstrumentFunction(Branches, 0);

}

VOID FiniFollowExecution()
{
    // Find collisions between 1st elements in maps to instructions and branches taken
    // This is a round-about way of mapping each cond. jump->taken/~taken

    for (auto it = addressToBranchTaken.begin(); it != addressToBranchTaken.end(); ++it ) {

        std::unordered_map<int, string>::const_iterator got = addressToInstruction.find(it->first);

        if (got != addressToInstruction.end() ) {
            ControlFlowFile << got->second << ":" << it->second << endl;
        }

    }


    ControlFlowFile.close();
}

// End FollowExecution.h

