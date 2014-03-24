/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2013 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */


#ifndef __INTERCEPT_SIGNALS_H__ 
#define __INTERCEPT_SIGNALS_H__ 

#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */
ofstream SignalFile;


/* ===================================================================== */
/* User Libraries */
/* ===================================================================== */
#include "Stubble.h"

/* ===================================================================== */


// TODO print to 'signals.out', move signals.out to results/signals/signals.#
BOOL intercept_signals(THREADID tid, INT32 sig, CONTEXT *ctxt, BOOL hasHandler, const EXCEPTION_INFO *pExceptInfo, VOID *v)
{

    SignalFile << "[INTERCEPTED]\t" << sig << endl;
    SignalFile << PIN_ExceptionToString(pExceptInfo) << endl;
    return sig;
}

// Init function for this library
VOID init_intercept_signals()
{

    SignalFile.open(results_dir + "signals.out");

    // Two signals we can't intercept, sigkill and sigstop
    PIN_InterceptSignal(1, intercept_signals, 0);
    PIN_InterceptSignal(2, intercept_signals, 0);
    PIN_InterceptSignal(3, intercept_signals, 0);
    PIN_InterceptSignal(4, intercept_signals, 0);
    PIN_InterceptSignal(5, intercept_signals, 0);
    PIN_InterceptSignal(6, intercept_signals, 0);
    PIN_InterceptSignal(7, intercept_signals, 0);
    PIN_InterceptSignal(8, intercept_signals, 0);
    //PIN_InterceptSignal(9, intercept_signals, 0); // sigkill, can't handle
    PIN_InterceptSignal(10, intercept_signals, 0);
    PIN_InterceptSignal(11, intercept_signals, 0); // segfault
    PIN_InterceptSignal(12, intercept_signals, 0);
    PIN_InterceptSignal(13, intercept_signals, 0);
    PIN_InterceptSignal(14, intercept_signals, 0);
    PIN_InterceptSignal(15, intercept_signals, 0);
    PIN_InterceptSignal(16, intercept_signals, 0);
    PIN_InterceptSignal(17, intercept_signals, 0);
    PIN_InterceptSignal(18, intercept_signals, 0);
    PIN_InterceptSignal(19, intercept_signals, 0);
    PIN_InterceptSignal(20, intercept_signals, 0);
    PIN_InterceptSignal(21, intercept_signals, 0);
    PIN_InterceptSignal(22, intercept_signals, 0);
    //PIN_InterceptSignal(23, intercept_signals, 0); // sigstop, can't handle
    PIN_InterceptSignal(24, intercept_signals, 0);
    PIN_InterceptSignal(25, intercept_signals, 0);
    PIN_InterceptSignal(26, intercept_signals, 0);
    PIN_InterceptSignal(27, intercept_signals, 0);
    PIN_InterceptSignal(28, intercept_signals, 0);
    PIN_InterceptSignal(29, intercept_signals, 0);
    PIN_InterceptSignal(30, intercept_signals, 0);
    PIN_InterceptSignal(31, intercept_signals, 0);

}

// Cleanup function for this library
VOID fini_intercept_signals()
{

    SignalFile.close();

}
    
#endif /*__INTERCEPT_SIGNALS_H__*/

