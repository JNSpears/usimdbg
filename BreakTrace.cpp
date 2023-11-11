/*
 * BreakTrace.cpp
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */

#include "hexadump.h"
#include "string.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "BreakTrace.h"
#include "Trigger.h"
#include "Action.h"
#include "Symbol.h"


using namespace std;

// ==========================================================================================================

BreakTraceSubSystem::BreakTraceSubSystem()
{
    execHistoryIx = lastExecIx = 0;
    for (int ix=0; ix < MAX_BREAKPOINTS; ix++)
        TrigAct[ix] = { NULL, NULL };
}

BreakTraceSubSystem::~BreakTraceSubSystem()
{
}

void BreakTraceSubSystem::_dispTrace(long frame_number, int nFrom, int nTo)
{
    long ix; 

    if ((nTo - nFrom)  > MAX_EXEC_HISTORY)
        nFrom = nTo - MAX_EXEC_HISTORY + 1;

    cerr << endl;
    for(ix = nFrom; ix <= nTo; ix++, frame_number++)
    {
        TraceFrame& frame = execHistory[ix%MAX_EXEC_HISTORY];
        switch (frame.type) // exec, readbyte, writebyte, readword, writeword 
        {
            case exec:
                {
                    const char *pSymbol = getSymbol(frame.addr);
                    if (pSymbol)
                        cout << "                        " << pSymbol << ':' << endl;
                    fprintf(stderr, "(%03ld) X %04x           %s\r\n", frame_number, frame.addr, frame.text);
                }
                break;
            case readbyte:
                fprintf(stderr, "(%03ld) R   %02X @ %04x\r\n", frame_number, (Byte)frame.data, frame.addr);
                break;
            case writebyte:
                fprintf(stderr, "(%03ld) W   %02X @ %04x\r\n", frame_number, (Byte)frame.data, frame.addr);
                break;
            case readword:
                fprintf(stderr, "(%03ld) R %04X @ %04x\r\n", frame_number, frame.data, frame.addr);
                break;
            case writeword:
                fprintf(stderr, "(%03ld) W %04X @ %04x\r\n", frame_number, frame.data, frame.addr);
                break;
            default:
                fprintf(stderr, "(%03ld) ?\r\n", frame_number);
                break;
        }
    }
}

void BreakTraceSubSystem::dispTrace(int nStartFrame, int nEndFrame)
{

    long start_ix, end_ix, frame_number;

    if (nStartFrame > execHistoryIx)
        nStartFrame = execHistoryIx;
    start_ix = nStartFrame;
    frame_number = start_ix;
    end_ix = nEndFrame;
    if (end_ix > execHistoryIx)
        end_ix = execHistoryIx;

    _dispTrace(frame_number, start_ix, end_ix);    
}

void BreakTraceSubSystem::dispTrace(int nFrames)
{
    long start_ix, end_ix, frame_number;

    start_ix = execHistoryIx - nFrames;
    if (start_ix < 0)
        start_ix = 0L;
    frame_number = start_ix;
    end_ix = execHistoryIx-1;

    _dispTrace(frame_number, start_ix, end_ix);
}

// bool BreakTraceSubSystem::checkforbreak(void)
// {
//     if (pendingBreak)
//     {
//         pendingBreak = false;
//         return true;
//     }
//     return false;
// }

// breakpoint management.

void BreakTraceSubSystem::dispExecBreaks(void)
{
    for (int ix=0; ix < MAX_BREAKPOINTS; ix++)
    {
        if (TrigAct[ix].t)
        {
            cerr << "(" << ix << ")  ";        // index is used to unset breakpoints.
            TrigAct[ix].t->display();
            cerr << " -> " << TrigAct[ix].a->name << endl;
        }
    }
}

void BreakTraceSubSystem::clearExecBreak(int index)
{
    for (int ix=0; ix < MAX_BREAKPOINTS; ix++)
    {
        if (ix == index)
        {
            if (TrigAct[ix].t)
            {
                delete TrigAct[ix].t;
                TrigAct[ix].t = NULL;
            }
            if (TrigAct[ix].a)
            {
                delete TrigAct[ix].a;
                TrigAct[ix].a = NULL;
            }
        }
    }
}

// trigger/action breaks

int BreakTraceSubSystem::add(Trigger &t, const Action &a)
{
    for (int ix=0; ix < MAX_BREAKPOINTS; ix++)
    {
        if (TrigAct[ix].t == NULL)
        {
            TrigAct[ix].t = &t;
            TrigAct[ix].a = &a;
            return ix;
        }
    }
    return 99;
}


bool BreakTraceSubSystem::check_triggers_doActions(TraceFrame& frame, mc6809& cpu)
{
    bool ret_val = false;
    for (int ix=0; ix < MAX_BREAKPOINTS; ix++)
    {
        if (TrigAct[ix].t)
        {
            if (TrigAct[ix].t->isTriggered(frame, false))
            {
                if (TrigAct[ix].a->doAction(cpu))
                    ret_val = true;
            }
        }
    }
    return ret_val;
}


// trace hooks

void BreakTraceSubSystem::traceReadWord(mc6809& cpu, Word addr, Word val)
{
    TraceFrame frame;
    frame.type = readword;
    frame.addr = addr;
    frame.data = val;
    execHistory[execHistoryIx++ % MAX_EXEC_HISTORY] = frame;

    // check for r/w breaks;
    pendingBreak |= check_triggers_doActions(frame, cpu);
}

// Hooks for read read/write

void BreakTraceSubSystem::traceReadByte(mc6809& cpu, Word addr, Byte val)
{
    TraceFrame frame;
    frame.type = readbyte;
    frame.addr = addr;
    frame.data = val;
    execHistory[execHistoryIx++ % MAX_EXEC_HISTORY] = frame;

    // check for r/w breaks;
    pendingBreak = check_triggers_doActions(frame, cpu);
}

void BreakTraceSubSystem::traceWriteWord(mc6809& cpu, Word addr, Word val)
{
    TraceFrame frame;
    frame.type = writeword;
    frame.addr = addr;
    frame.data = val;
    execHistory[execHistoryIx++ % MAX_EXEC_HISTORY] = frame;

    // check for r/w breaks;
    pendingBreak = check_triggers_doActions(frame, cpu);
}

void BreakTraceSubSystem::traceWriteByte(mc6809& cpu, Word addr, Byte val)
{
    TraceFrame frame;
    frame.type = writebyte;
    frame.addr = addr;
    frame.data = val;
    execHistory[execHistoryIx++ % MAX_EXEC_HISTORY] = frame;

    // check for r/w breaks;
    pendingBreak = check_triggers_doActions(frame, cpu);
}


void BreakTraceSubSystem::traceExec(mc6809& cpu, Word addr)
{
    (void)cpu;
    // (void)text;
    TraceFrame frame;
    frame.type = exec;
    frame.addr = addr;
    frame.data = 0;
    // strncpy(&frame.text[0], text.c_str(), 50);
    lastExecIx = execHistoryIx % MAX_EXEC_HISTORY;
    execHistoryIx++;
    execHistory[lastExecIx] = frame;

    // // check for x breaks;
    // pendingBreak = check_triggers_doActions(frame, cpu);
}


void BreakTraceSubSystem::traceExecUPDATE(mc6809& cpu, std::string text)
{
    (void)cpu;
    (void)text;
    // (void)addr;
    TraceFrame frame = execHistory[lastExecIx];
    strncpy(&frame.text[0], text.c_str(), 50);
    execHistory[lastExecIx] = frame;

    // check for EXEC breaks;
    pendingBreak = check_triggers_doActions(frame, cpu);
}

void BreakTraceSubSystem::clearTraceBuffer(void)
{
    execHistoryIx = 0L;
}

bool BreakTraceSubSystem::isTraceBufferFull(void) const
{
    return (execHistoryIx >= MAX_EXEC_HISTORY);
}

// //////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////

TriggerNever::TriggerNever(void) : Trigger("TriggerNever")
{
}

bool TriggerNever::isTriggered(TraceFrame& frame, bool silent)
{
    (void)frame;
    (void)silent;
    return false;
}

// //////////////////////////////////////////////////////////////////////////////////////////////

TriggerOnPc::TriggerOnPc(Word expected) : Trigger("TriggerOnPc"), expected(expected)
{
}

TriggerOnPc::~TriggerOnPc()
{
}

void TriggerOnPc::display(void) const
{
    Trigger::display();
    cerr << ": pc=" << hex << uppercase << setw(4) << setfill('0')  << expected;
}

bool TriggerOnPc::isTriggered(TraceFrame& frame, bool silent)
{
    // fprintf(stderr, "TriggerOnPc::isTriggered frame.addr=%04x expected=%04x ---> %d\r\n", frame.addr, expected, (frame.addr == expected));
    bool ret_val = ((frame.addr == expected) && (frame.type == exec));
    if (ret_val && !silent)
        displayOccurred();
    return ret_val;
}

// //////////////////////////////////////////////////////////////////////////////////////////////
TriggerOnReadWrite::TriggerOnReadWrite(Word _addr, char _type)
: Trigger("TriggerOnReadWrite"), addr(_addr), type(_type)
{
}

TriggerOnReadWrite::~TriggerOnReadWrite()
{
}

void TriggerOnReadWrite::display(void) const
{
    Trigger::display();
    cerr << ": @=" << hex << uppercase << setw(4) << setfill('0') << addr << ' ' << type;
}

bool TriggerOnReadWrite::isTriggered(TraceFrame& frame, bool silent)
{
    bool accessmode = false;
    // cerr << "TriggerOnReadWrite::isTriggered( slient=" << silent << ") ";
    // cerr << ": @=" << hex << uppercase << setw(4) << setfill('0') << addr << ' ' << type << endl;
    switch(type)
    {
    case 'r':
        accessmode = (frame.type == readbyte) || (frame.type == readword);
        break;
    case 'w':
        accessmode = (frame.type == writebyte) || (frame.type == writeword);
        break;
    case 'a':
    default:
        accessmode = true;
        break;
    }
    const bool ret_val = (frame.addr == addr) && accessmode;
    if (ret_val && !silent)
        displayOccurred();
    return ret_val;
}

//////////////////////////////////////////////////////////////////////////////////////////////
TriggerOnReadWriteValue::TriggerOnReadWriteValue(const Word offset, const char _type, const Word value)
: TriggerOnReadWrite(offset, _type), _value(value)
{
}

TriggerOnReadWriteValue::~TriggerOnReadWriteValue()
{
}

void TriggerOnReadWriteValue::display(void) const
{
    TriggerOnReadWrite::display();
    cerr << " =" << hex << uppercase << setw(4) << setfill('0') << _value;
}

bool TriggerOnReadWriteValue::isTriggered(TraceFrame& frame, bool silent)
{
    const bool ret_val = (TriggerOnReadWrite::isTriggered(frame, true) && (frame.data == _value));
    if (ret_val && !silent)
        displayOccurred();
    return ret_val;
}

//////////////////////////////////////////////////////////////////////////////////////////////
TriggerTraceBufferFull::TriggerTraceBufferFull(const BreakTraceSubSystem &_BrkTrc)
: Trigger("TriggerTraceBufferFull"), BrkTrc(_BrkTrc)
{
}

TriggerTraceBufferFull::~TriggerTraceBufferFull()
{
}

bool TriggerTraceBufferFull::isTriggered(TraceFrame& frame, bool silent)
{
    (void)frame;
    const bool ret_val = (BrkTrc.isTraceBufferFull());
    if (ret_val && !silent)
        displayOccurred();
    return ret_val;
}


//////////////////////////////////////////////////////////////////////////////////////////////
TriggerEventCount::TriggerEventCount(Trigger *_event, int _count)
: Trigger("TriggerEventCount"), event(*_event), count(_count)
{
}

TriggerEventCount::~TriggerEventCount()
{
}

bool TriggerEventCount::isTriggered(TraceFrame& frame, bool silent)
{
    (void)frame;
    (void)silent;
    bool ret_val = (event.isTriggered(frame, true));
    if (ret_val && count)
    {
        count--;
        ret_val = !count;
    }  
    if (ret_val)
        displayOccurred();
    return ret_val;
}

/*

br 0E0D0 occurs 2
br
go
go

br 0E0D0 occurs 1
br


*/

void TriggerEventCount::display(void) const
{
    Trigger::display();
    cerr << ": #=" << dec << count << " [";
    event.display();
    cerr << ']';
}




//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

ActionNone::ActionNone()
: Action("ActionNone")
{
}

bool ActionNone::doAction(mc6809& cpu) const
{
    (void)cpu;
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
ActionBreak::ActionBreak()
: Action("ActionBreak")
{
}

ActionBreak::~ActionBreak()
{
}

bool ActionBreak::doAction(mc6809& cpu) const
{
    cpu.halt();
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
ActionAnotate::ActionAnotate(const char* const str)
: Action("ActionAnotate"), _str(str)
{
}

bool ActionAnotate::doAction(mc6809& cpu) const
{
    (void)cpu;
    cerr << "--->>" << _str << endl;
    return false;
}


// //////////////////////////////////////////////////////////////////////////////////////////////
// ActionTron::ActionTron()
// : Action("ActionTron")
// {
// }

// void ActionTron::doAction(mc6809& cpu) const
// {
//     cpu.tron();
// }

// ActionTroff::ActionTroff()
// : Action("ActionTroff")
// {
// }

// void ActionTroff::doAction(mc6809& cpu) const
// {
//     cpu.troff();
// }

// //////////////////////////////////////////////////////////////////////////////////////////////
// ActionHexDump::ActionHexDump(const char * const pAnotation, const Word offset, const Word len)
// : Action("ActionHexDump"), _pAnotation(pAnotation), _offset(offset), _len(len)
// {
// }

// void ActionHexDump::doAction(mc6809dbgDebugCore& cpu) const
// {
//     (void)cpu;

//     int i, width = 16;
//     int row;
//     char b[4096];
//     int l = std::min((unsigned)_len, (unsigned)sizeof(b));
// //    int end;

//     std::memset(b, '\0', sizeof(b));
// //    std::memcpy(b, s, min(_len, sizeof(b));
//     fprintf(stderr, "\r\n%s:\r\n", _pAnotation);
//     int end_offset = ((l%width)>0) ? (l/width)+1 : (l/width);
//     for (row = 0;row<end_offset;row++)
//     {
//       fprintf(stderr, "%04X: ", _offset+row*width);
//       for (i = 0;i<width;i++)
//       {
//          b[row*width+i] = cpu.read(_offset+row*width);
//          fprintf(stderr, "%02X ", b[row*width+i]);
//       }
//       fprintf(stderr, "|");
//       for (i = 0;i<width;i++) {
//         if ((b[row*width+i] >= 0x20) && (b[row*width+i] < 0x7f)) {
//           fprintf(stderr, "%c", b[row*width+i]);
//         } else {
//           fprintf(stderr, ".");
//         }
//       }
//       fprintf(stderr, "|\r\n");
//     }
//     fprintf(stderr, "\r\n");
// }


/*

br 1234
br 2222 write
br 2222 access
br
br tbf
br 3333 x occurs 12
br

*/