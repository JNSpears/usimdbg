/*
 * Symbol.h
 *
 *  Created on: Oct 12, 2023
 *      Author: james
 */

#ifndef TOOLS_USIM_USIMDBG_SYMBOL_H_
#define TOOLS_USIM_USIMDBG_SYMBOL_H_

#include "typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* C functions */
const char* getSymbol(Word addr);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "string.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <bit>
#include <iomanip>
#include <libgen.h>

#include "mc6809dbg.h"
#include "termdbg.h"

/* C++ Functions */
void addSymbol(Word addr, std::string symbol);
void dispSymbols(void);
void clearSymbols(void);
Word getSymbolAddr(mc6809dbg cpu, TerminalDbg term, std::string symbol);
void showSymbolsByTextMatch(char *pText);
void showSymbolsByAddressRange(TRange& range);

#endif



#endif /* TOOLS_USIM_USIMDBG_SYMBOL_H_ */
