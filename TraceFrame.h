/*
 * TraceFrame.h
 *
 *  Created on: June 10, 2023
 *      Author: james
 */
#ifndef TOOLS_USIM_PERCOM_TRACEFRAME_H_
#define TOOLS_USIM_PERCOM_TRACEFRAME_H_

#pragma once

// #include <stdlib.h>
// #include <string>
// #include <ctype.h>
#include "typedefs.h"

// #include "mc6809.h"
// #include "Action.h"
// #include "Trigger.h"
// #include "TriggerActionEntry.h"

// typedef std::vector<TriggerActionEntry> TriggerActionList;

// #define MAX_BREAKPOINTS 8
// #define BREAKPOINT_UNUSED 0xffff
// #define MAX_EXEC_HISTORY 64

enum FrameType { exec, readbyte, writebyte, readword, writeword };

struct TraceFrame {
	FrameType type;
	Word addr;
	Word data;
    char text[50];
};

#endif /* TOOLS_USIM_PERCOM_TRACEFRAME_H_ */
