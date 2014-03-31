#ifndef __STUBBLE_H__ 
#define __STUBBLE_H__ 

/* Debugging: 1 to write debugging info, 0 otherwise */
#define DEBUG 1

// Stubble.h
// http://en.wikibooks.org/wiki/X86_Assembly/Control_Flow and the book in general is very helpful

#include "taint.h"
#include "syscalls.h"
#include "intercept_signals.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <list>
#include <tuple>
#include <unordered_map>
#include <vector>


/* Globals for Stubble */
const string results_dir = "./results/currentrun/";

VOID init_stubble()
{

    // Init libraries
    init_taint();
    init_intercept_signals();
    init_syscalls();

}

VOID fini_stubble()
{

    fini_taint();
    fini_intercept_signals();
    fini_syscalls();

}

#endif /* __STUBBLE_H__ */

