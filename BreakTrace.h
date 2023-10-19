/*
 * BreakTrace.h
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */
#ifndef TOOLS_USIM_PERCOM_BREAKTRACE_H_
#define TOOLS_USIM_PERCOM_BREAKTRACE_H_

#pragma once

#include <stdlib.h>
#include <string>
#include <ctype.h>
#include "typedefs.h"

#include "mc6809.h"
#include "Action.h"
#include "Trigger.h"
// #include "TriggerActionEntry.h"

// typedef std::vector<TriggerActionEntry> TriggerActionList;

#define MAX_BREAKPOINTS 8
#define BREAKPOINT_UNUSED 0xffff
#define MAX_EXEC_HISTORY 256

struct TriggerAction {
	Trigger *t;
	const Action *a;
};


enum EBrkTyp { Exec, Read, Write, Access };

class BreakTraceSubSystem {

public:
				BreakTraceSubSystem();		// public constructor
	virtual		~BreakTraceSubSystem();		// public destructor


protected:
	void _dispTrace(long frame_number, int nFrom, int nTo);

public:
	void dispTrace(int nFrames);
	void dispTrace(int nStartFrame, int nEndFrame);

	void traceExec(mc6809& cpu, Word addr);
	void traceExecUPDATE(mc6809& cpu, std::string text);

	void traceReadWord(mc6809& cpu, Word addr, Word val);
	void traceReadByte(mc6809& cpu, Word addr, Byte val);

	void traceWriteWord(mc6809& cpu, Word addr, Word val);
	void traceWriteByte(mc6809& cpu, Word addr, Byte val);

	bool checkforbreak(void);

	void clearExecBreak(int index);
	void dispExecBreaks(void);

	int add(Trigger &t, const Action &a);

	bool check_triggers_do_actions(TraceFrame& frame, mc6809& cpu);
	bool pendingBreak;

	void clearTraceBuffer(void);
	bool isTraceBufferFull(void) const;

protected:
	struct TriggerAction	TrigAct[MAX_BREAKPOINTS];

private:
    long	execHistoryIx;
    int     lastExecIx; 
    TraceFrame	execHistory[MAX_EXEC_HISTORY];

};


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class TriggerNever: public Trigger {
public:
	TriggerNever();
	// virtual ~TriggerNever(void);

	virtual bool triggered(TraceFrame& frame, bool silent=false);
};

////////////////////////////////////////////////////////////
class TriggerOnPc: public Trigger {
public:
	TriggerOnPc(Word expected);
	virtual ~TriggerOnPc(void);

	virtual bool triggered(TraceFrame& frame, bool silent=false);

	virtual void display(void) const;

protected:
	Word expected;
};

////////////////////////////////////////////////////////////
class TriggerOnReadWrite: public Trigger {
public:
	TriggerOnReadWrite(Word _addr, char _type);
	virtual ~TriggerOnReadWrite(void);

	virtual bool triggered(TraceFrame& frame, bool silent=false);

	virtual void display(void) const;

protected:
	Word addr;
	char type;
};

//////////////////////////////////////////////////////////////
class TriggerOnReadWriteValue: public TriggerOnReadWrite {
public:
	TriggerOnReadWriteValue(const Word offset, char _type, const Word value);
	~TriggerOnReadWriteValue();

	virtual bool triggered(TraceFrame& frame, bool silent=false);

	virtual void display(void) const;

protected:
	const Word _value;
};

////////////////////////////////////////////////////////////
class TriggerTraceBufferFull: public Trigger {
public:
	TriggerTraceBufferFull(const BreakTraceSubSystem &_BrkTrc);
	virtual ~TriggerTraceBufferFull(void);

	virtual bool triggered(TraceFrame& frame, bool silent=false);

protected:
	const BreakTraceSubSystem &BrkTrc;
};

////////////////////////////////////////////////////////////
class TriggerEventCount: public Trigger {
public:
	TriggerEventCount(Trigger *_event, int _count);
	virtual ~TriggerEventCount(void);

	virtual bool triggered(TraceFrame& frame, bool silent=false);
	virtual void display(void) const;

protected:
	Trigger &event;
	int count;
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class ActionNone: public Action {
public:
	ActionNone();
	virtual bool do_action(mc6809& cpu) const;
};

////////////////////////////////////////////////////////////
class ActionAnotate: public Action {
public:
	ActionAnotate(const char* const str);
	virtual bool do_action(mc6809& cpu) const;
private:
	const char* const _str;
};

////////////////////////////////////////////////////////////
class ActionTron: public Action {
public:
	ActionTron();
	virtual bool do_action(mc6809& cpu) const;
};
class ActionTroff: public Action {
public:
	ActionTroff();
	virtual bool do_action(mc6809& cpu) const;
};

////////////////////////////////////////////////////////////
class ActionBreak: public Action {
public:
	ActionBreak();
	virtual ~ActionBreak();
	virtual bool do_action(mc6809& cpu) const; // mc6809DebugCore& cpu
};

// ////////////////////////////////////////////////////////////
// class ActionHexDump: public Action {
// public:
// 	ActionHexDump(const char * const pAnotation, const Word offset, const Word len);
// 	virtual void do_action(mc6809dbgDebugCore& cpu) const;
// private:
// 	const char * const _pAnotation;
// 	const Word _offset;
// 	const Word _len;
// };

// /*
//  * template to resolve smart point ambiguity issues
//  * see https://stackoverflow.com/questions/66032442/
//  */
// template<int n> struct rank : rank<n - 1> {};
// template<>      struct rank<0> {};

// ***** Triggers / Actions to implement
// TODO: ActionTrace(Bool on_off) -- turn trace on or off
// TODO: TriggerCycleCount(int n) -- trigger 
// TODO: cycle breakpoint? add cycle count to trace?
// TODO: symbols command, display symbols matchingh text prefix, or address range
// TODO: trigger then ie: br 1234 x then 8888 read
// TODO: trigger range of addresses
// TODO: trigger value other than equal
// TODO: trigger register with value

#endif /* TOOLS_USIM_PERCOM_BREAKTRACE_H_ */
