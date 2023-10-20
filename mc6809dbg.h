/*
 * mc6809dbg.cpp
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */

#pragma once

#include <stdlib.h>
#include <string>
#include <ctype.h>
#include "typedefs.h"

#include "mc6809.h"
#include "BreakTrace.h"
#include "range.h"
class mc6809dbg;
class TerminalDbg;
#include "termdbg.h"

class mc6809dbg : virtual public mc6809 {

public:
				mc6809dbg();		// public constructor
	virtual		~mc6809dbg();		// public destructor

public:
	//
	// Overrides for debugger functionality.
	//
	virtual void        invalid(const char*);
	virtual void 		tick();
	virtual void 		reset();

	//
	// New functions for debugger functionality.
	//

	virtual void debug(TerminalDbg &term);

	virtual void dispTrace(int nFrames);
	virtual void dispTrace(int nFrame, int lenTo);
	virtual void clearTrace();

	// void hexadump(int nBytes, int addr, const char *a=NULL, int lineWidth=16);
	virtual void annotations(void);

	void UnsetBreak(int ix);
	void dispBreak();
	void SetBreakOnExec(Word addr, int occurs);
	void SetBreakOnExec(Word addr, EBrkTyp brktyp, int occurs, DWord opt_value=0xffffffff);
	void SetBreakOnTraceBufferFull();
	virtual void help();

	virtual void cmdByteWrite(TRange& range, int *values, int actualValues);
	virtual void cmdWordWrite(TRange& range, int *values, int actualValues);

	virtual void cmdDumpBytes(TRange& range);
	virtual void cmdDumpWords(TRange& range);

	virtual void cmdDisassemble(TRange& range);

	virtual void cmdLoadFilename(char *pFilename, Word base, int append_flag);

	virtual void showSymbolsByTextMatch(char *pText);
	virtual void showSymbolsByAddressRange(TRange& range);

protected:
	//
	// Overrides for debugger functionality.
	//

	virtual Byte		read(Word offset);
	virtual Word		read_word(Word offset);
	virtual void		write(Word offset, Byte val);
	virtual void		write_word(Word offset, Word val);
	virtual Byte 		fetch();


	//
	// New functions for debugger functionality.
	//

	virtual void read_buffer(Word offset, uint length, Byte *pBuffer);
	virtual void write_buffer(Word offset, uint length, Byte *pBuffer);
	virtual void setBreak(const Action *act, Trigger *trig, int occurs);

	virtual void load_intelhex(const char *filename, Word base, bool fatal);
	virtual void load_srecord(const char *filename, Word base, bool fatal);
	virtual void load_raw(const char *filename, Word base, bool fatal);
	virtual void load_lwtools_sym(const char *filename, Word base, bool fatal);
	virtual void load_lwtools_map(const char *filename, Word base, bool fatal);

	// void cmdStepOverMPX9(char *s);

private:
	BreakTraceSubSystem BrkTrc;

};

