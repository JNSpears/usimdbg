/*
 * BreakTrace.cpp
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */


#include "string.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <bit>
#include <iomanip>
#include <libgen.h>

#include "mc6809dbg.h"
#include "BreakTrace.h"
#include "hexadump.h"
#include "Symbol.h"
#include "parser.hpp"

using namespace std;


// ==========================================================================================================

mc6809dbg::mc6809dbg()
{
}

mc6809dbg::~mc6809dbg()
{
}

// ==========================================================================================================
void mc6809dbg::invalid(const char *msg)
{
	fprintf(stderr, "invalid: %s [PC:$%04X IR:$%04X]\n", msg, pc, ir);
	halt(); 	// force break to debugger
}

void mc6809dbg::tick()
{
	// fprintf(stderr, "mc6809dbg::tick()\n\r");

	BrkTrc.traceExec(*this, pc);

	mc6809::tick();

	char text[50];
	snprintf(text, 50, "%-8s%s", insn, disasm_operand().c_str());
	BrkTrc.traceExecUPDATE(*this, text);

	// if (BrkTrc.checkforbreak())
	// 	halt();
}


// ==========================================================================================================
void mc6809dbg::annotations(void)
{
}


// ==========================================================================================================
void mc6809dbg::debug(TerminalDbg &term)
{
	term.set_debug();
    yyparse(*this, term); // Calls yylex() for tokens.
	term.set_exec();
}

// ==========================================================================================================

// display the last nFrames
void mc6809dbg::dispTrace(int nFrames)
{
	BrkTrc.dispTrace(nFrames);
}

// display a specific range of frames
void mc6809dbg::dispTrace(int nFrame, int lenTo)
{
	BrkTrc.dispTrace(nFrame, lenTo);
}

void mc6809dbg::clearTrace()
{
	BrkTrc.clearTraceBuffer();
}


// ==========================================================================================================
void mc6809dbg::UnsetBreak(int ix)
{
	BrkTrc.clearExecBreak(ix);
}

void mc6809dbg::dispBreak()
{
	BrkTrc.dispExecBreaks();
}

void mc6809dbg::setBreak(const Action *act, Trigger *trig, int occurs)
{
	if (occurs)
	{
		auto occurs_trig = new TriggerEventCount(trig, occurs);
		BrkTrc.add(*occurs_trig, *act);
	}
	else
		BrkTrc.add(*trig, *act);
}

Trigger *triggerFactory(Word addr, char mode, DWord opt_value)
{
	Trigger *trig;
	if (opt_value != 0xffffffff) 
		// break value specified.
		trig = new TriggerOnReadWriteValue(addr, mode, opt_value);
	else
		// no value specified
		trig = new TriggerOnReadWrite(addr, mode);
	return trig;
}

void mc6809dbg::SetBreakOnExec(Word addr, int occurs)
{
	(void)occurs;
	auto act = new const ActionBreak();
	auto trig = new TriggerOnPc(addr);
	setBreak(act, trig, occurs);
}



void mc6809dbg::SetBreakOnExec(Word addr, EBrkTyp brktyp, int occurs, DWord opt_value)
{
	(void)occurs;
	auto act = new const ActionBreak();
	switch (brktyp) {
	case Exec:
		{
			auto trig = new TriggerOnPc(addr);
			setBreak(act, trig, occurs);
		}
		break;
	case Read:
		{
			auto trig = triggerFactory(addr, 'r', opt_value);
			setBreak(act, trig, occurs);
		}
		break;
	case Write:
		{
			auto trig = triggerFactory(addr, 'w', opt_value);
			setBreak(act, trig, occurs);
		}
		break;
	case Access:
		{
			auto trig = triggerFactory(addr, 'a', opt_value);
			setBreak(act, trig, occurs);
		}
		break;
	}
}

/*

br
br 1234
br 1111 write 
br 2222 read occurs 33
br 4444 access 77 
br 6666 access 88 occurs 8

br
(0)  TriggerOnPc: pc=1234 -> ActionBreak
(1)  TriggerOnReadWrite: @=1111 w -> ActionBreak
(2)  TriggerEventCount: #=51 [TriggerOnReadWrite: @=2222 r] -> ActionBreak
(3)  TriggerOnReadWrite: @=4444 a =0077 -> ActionBreak
(4)  TriggerEventCount: #=8 [TriggerOnReadWrite: @=6666 a =0088] -> ActionBreak
 
*/


void mc6809dbg::SetBreakOnTraceBufferFull()
{
	auto act = new const ActionBreak();
	auto trig = new TriggerTraceBufferFull(BrkTrc);
	BrkTrc.add(*trig, *act);
}


// ==========================================================================================================

// void mc6809dbg::cmdStepOverMPX9(char *s) // addr [xrw] [data]
// {
// 	(void)s;
// 	auto act = new const ActionBreak();
// 	auto trig = new const TriggerOnPc(pc+3);
// 	int ix = BrkTrc.add(*trig, *act);
// 	Mode oldDebugMode = DebugMode;
// 	DebugMode = _go;
	
// 	bool old_m_trace = m_trace;
// 	troff();

// 	do {
// 		BrkTrc.traceExec(*this, pc);

// 		mc6809::tick();

// 		// fprintf(stderr, "/ %04X: [%2d] %-8s%s\r\n", insn_pc, cycles, insn, disasm_operand().c_str());
// 		// std::string s = fmt::sprintf( "%s%d", foo, bar );
// 		char text[50];
// 		// int snprintf( char* buffer, std::size_t buf_size, const char* format, ... );
// 		snprintf(text, 50, "%-8s%s", insn, disasm_operand().c_str());
// 		BrkTrc.traceExecUPDATE(*this, text);
// 	} while (DebugMode == _go);

// 	DebugMode = oldDebugMode;
// 	m_trace = old_m_trace;

// 	BrkTrc.clearExecBreak(ix);
// }


// ==========================================================================================================

//----------------------------------------------------------------------------
// Byte/Word memory access routines for big-endian (Motorola type)
//----------------------------------------------------------------------------

// Single byte read
Byte mc6809dbg::read(Word offset)
{
	// fprintf(stderr, "mc6809dbg::read(%04X)\r\n", offset);
	Byte val = USim::read(offset);
	BrkTrc.traceReadByte(*this, offset, val);
	return val;
}

// Single byte write
void mc6809dbg::write(Word offset, Byte val)
{
	// fprintf(stderr, "mc6809dbg::write(%04X, %02X)\r\n", offset, val);
	USim::write(offset, val);
	BrkTrc.traceWriteByte(*this, offset, val);
}

Word mc6809dbg::read_word(Word offset)
{
	Word		tmp;
	tmp  = USim::read(offset) << 8;
	tmp |= USim::read(offset+1);
	// fprintf(stderr, "rw M[%04x] -> %04x\r\n", offset, tmp);
	BrkTrc.traceReadWord(*this, offset, tmp);
	return tmp;
}

void mc6809dbg::write_word(Word offset, Word val)
{
	USim::write(offset, (Byte)(val >> 8));
	USim::write(offset+1, (Byte)val);
	BrkTrc.traceWriteWord(*this, offset, val);
}

Byte mc6809dbg::fetch()
{
	return mc6809::read(pc++);
}

void mc6809dbg::reset()
{
	mc6809::reset();
	s = 0x0000;
	u = 0x0000;
}

// ==========================================================================================================
void mc6809dbg::help()
{
const char * foobar = R"FOOBAR(
	HELP			-- Help
	RESET			-- Reset processor
	(BYTE|WORD) addr [LEN expr | TO addr] [ '=' expr [, ...]]
				-- display or modify/fill memory
	S [expr]		-- Step n instructions (default=1)
	G			-- Go
	BReak [ [ addr ] [ arwx ] [ value ] [ OCCURS expr ] | TBF | CLEAR expr ]
				-- set/display/clear Breakpoint(s)
	Regs			-- display registers
	R<ID> [= expr]		-- display or change registers rA, rB ... rX, rY, rPC
	Trace [n [LEN expr | TO n] | CLEAR]
				-- display Trace or clear trace
	ASM [LEN expr | TO addr]
				-- disassemble memory
	LOAD '<filename> [BASE expr] [APPEND] 
				-- load file into memory or symbol sym_table
				-- filetypes: iHex, s19, raw/bin lwtools .sym and .map
	SYMbols [addr [LEN expr | TO addr] | 'text-to-match ]
)FOOBAR";

    cerr << endl << foobar << endl;
}

// ==========================================================================================================
void mc6809dbg::cmdByteWrite(TRange& range, int *values, int actualValues)
{
	int dstOff = 0;
	int srcOff = 0;
	for(Word addr=getStartAddr(range); !isDone(range); Next(range,1), dstOff++, srcOff++) 
		mc6809::write(addr+dstOff, values[srcOff%actualValues]);

}

// ==========================================================================================================
void mc6809dbg::cmdWordWrite(TRange& range, int *values, int actualValues)
{
	int dstOff = 0;
	int srcOff = 0;
	for(Word addr=getStartAddr(range); !isDone(range); Next(range,2), dstOff+=2, srcOff++) 
		mc6809::write_word(addr+dstOff, values[srcOff%actualValues]);

}

// ==========================================================================================================
void mc6809dbg::read_buffer(Word offset, uint length, Byte *pBuffer)
{
	for(uint ix=0; ix<length; ix++)
		pBuffer[ix] = mc6809::read(offset+ix);
}

// ==========================================================================================================
void mc6809dbg::write_buffer(Word offset, uint length, Byte *pBuffer)
{
	for(uint ix=0; ix<length; ix++)
		mc6809::write(offset+ix, pBuffer[ix]);
}

// ==========================================================================================================
void fmt_ascii(Byte *buffer, int length)
{
	unsigned char ch;
	fprintf(stderr, "|");
	for (int i = 0; i < length ; i++)
	{
		if ((buffer[i] >= 0x20) && (buffer[i] < 0x7f))
			ch = buffer[i];
		else
			ch = '.';

		fprintf(stderr, "%c", ch);
		if (i == 7 && length != 8)
			fprintf(stderr, "  ");
	}
	fprintf(stderr, "|\r\n");
}

// ==========================================================================================================
const int bufferlen = 16;

// ==========================================================================================================
void mc6809dbg::cmdDumpBytes(TRange& range)
{
	char buffer[bufferlen];
	// const int l = std::min(bufferlen,length);
	int l = bufferlen;
	Word offset = 0;

	fprintf(stderr, "\r\n");
	for(offset=getStartAddr(range); !isDone(range); ) 
	{
		// dumpRange(range);
		offset=getStartAddr(range);
		read_buffer(offset, sizeof(buffer), (Byte*)&buffer);
		fprintf(stderr, "%04X: ", offset);

		for (int i = 0; i < bufferlen ; i++)
		{
			if (!isDone(range))
			{
				l = i;
				fprintf(stderr, "%02x ", (unsigned char)buffer[i]);
			}
			else
				fprintf(stderr, "   ");
			if (i == 7)
				fprintf(stderr, " ");
			Next(range,sizeof(Byte));
		}
		fmt_ascii((Byte*)buffer, l+sizeof(Byte));
	}
}

// ==========================================================================================================
void mc6809dbg::cmdDumpWords(TRange& range)
{
	char buffer[bufferlen];
	// const int l = std::min(bufferlen,length);
	int l = bufferlen;
	Word offset = 0;

	fprintf(stderr, "\r\n");
	for(offset=getStartAddr(range); !isDone(range); ) 
	{
		offset=getStartAddr(range);
		read_buffer(offset, sizeof(buffer), (Byte*)&buffer);
		fprintf(stderr, "%04X: ", offset);

		for (int i = 0; i < bufferlen ; i += 2)
		{
			if (!isDone(range))
			{
				l = i;
				fprintf(stderr, "%02x%02x ", (unsigned char)buffer[i], (unsigned char)buffer[i + 1]);
			}
			else
				fprintf(stderr, "     ");
			if (i == 6)
				fprintf(stderr, " ");
			Next(range, sizeof(Word));
		}
		fmt_ascii((Byte*)buffer, l+sizeof(Word));
	}
}


/*

byte 30 = 11,22,33,44,55,66,77,88,99
byte 44 len 7 = 12
byte 0 len 80
word 0 len 40
byte 8f = 0aa
byte 0 len 90
byte 7f = 0bb
byte 0 len 90

*/


extern "C" unsigned Dasm6309 (char *buffer, int pc, unsigned char *memory, size_t memory_size, int memory_base);

// ==========================================================================================================

void mc6809dbg::cmdDisassemble(TRange& range)
{
	Byte memory_buffer[bufferlen];
	char disasm_buffer[200];
	char OpBytes[8];

	fprintf(stderr, "\r\n");
	unsigned n = 0;

	for (Word offset=getStartAddr(range); !isDone(range); Next(range,n))
	{
		read_buffer(offset, sizeof(memory_buffer), (Byte*)&memory_buffer);
	    n = Dasm6309(disasm_buffer, offset, memory_buffer, bufferlen, offset);
		const char *pSymbol = getSymbol(offset);
		if (pSymbol)
			cout << "                   " << pSymbol << ':' << endl;
	    for (unsigned j = 0; j < n; j++) {
	      sprintf(OpBytes+j+j, "%02X", (unsigned char)memory_buffer[j]);
	    }
	    fprintf(stderr, "%04X:  %-12s\t%s\n", offset, OpBytes, disasm_buffer);
	    offset += n;
	}
}


/*

./usimd ../../../percom/psymon/psymon.ihex 
load '../../../percom/psymon/psymon.sym
step
asm $ len 20 
asm "GETCMD len 8
eval "PROMPT
byte "PROMPT

*/


// ==========================================================================================================

void mc6809dbg::cmdLoadFilename(char *pFilename, Word base, int append_flag)
{
	char *basec, *bname;

	// fprintf(stderr, "cmdLoadFilename(%s, %04X)\n", pFilename, base);

	basec = strdup(pFilename);
	bname = basename(basec);
	char *ext = strrchr(bname, '.');
	// fprintf(stderr, "basename=%s ext=%s\n", bname, ext);

	if (!append_flag)
	{
		clearSymbols();
	}

	if (ext)
	{
		if (!strcmp(ext, ".raw") || !strcmp(ext, ".bin"))
		{
			load_raw(pFilename, base, false);
		}
		else if (!strcmp(ext, ".s19"))
		{
			load_srecord(pFilename, base, false);
		}
		else if (!strcmp(ext, ".ihex"))
		{
			load_intelhex(pFilename, base, false);
		}
		else if (!strcmp(ext, ".sym"))
		{
			load_lwtools_sym(pFilename, base, false);
		}
		else if (!strcmp(ext, ".map"))
		{
			load_lwtools_map(pFilename, base, false);
		}
		else 
		{
			perror("Unknowen file Extension.");
		}
	}
}

extern Byte fread_hex_byte(FILE *fp);
extern Word fread_hex_word(FILE *fp);

/**
 * Load file in Intel Hex format into memory. The memory address
 * to load into is stored in the file.
 * There are several Intel hex formats available. The most common format used
 * is the 'Intel MCS-86 Hex Object'. This format uses the following
 * structure.
 *
 * First char.     Start character
 * Next two char.  Byte count
 * next four char. Address
 * next two char.  Record type
 * last two char.  checksum
 */



void mc6809dbg::load_intelhex(const char *filename, Word base, bool fatal)
{
	(void)fatal;
	FILE *fp;
	int done = 0;
    unsigned int lowAddr = 0xffff, highAddr = 0;

	fp = fopen(filename, "r");
	if (!fp) {
		perror("filename");
		exit(EXIT_FAILURE);
	}

	while (!done) {
		Byte		n, t;
		Word		addr;
		Byte		b;

		(void)fgetc(fp);
		n = fread_hex_byte(fp);
		addr = fread_hex_word(fp);
       	lowAddr = addr+base < lowAddr ? addr+base : lowAddr;
		t = fread_hex_byte(fp);
		if (t == 0x00) {
			while (n--) {
				b = fread_hex_byte(fp);
				write(addr + base, b);
        		highAddr = addr+base > highAddr ? addr+base : highAddr;
				++addr;
			}
		} else if (t == 0x01) {
			done = 1;
		}
		// Read and discard checksum byte
		(void)fread_hex_byte(fp);
		if (fgetc(fp) == '\r') (void)fgetc(fp);
	}
    fclose(fp);

    fprintf(stderr, "\t%04x - %04x\r\n", lowAddr, highAddr);
}

//----------------------------------------------------------------------------
// Processor loading routines
//----------------------------------------------------------------------------

/**
 * Load file in Motorola S-record format into memory. The address
 * to load into is stored in the file.
 * Motorola's S-record format for output modules was devised for the
 * purpose of encoding programs or data files in a printable (ASCII)
 * format.  This allows viewing of the object file with standard
 * tools and easy file transfer from one computer to another, or
 * between a host and target.  An individual S-record is a single
 * line in a file composed of many S-records.
 *
 * S-Records are character strings made of several fields which
 * specify the record type, record length, memory address, data, and
 * checksum.  Each byte of binary data is encoded as a 2-character
 * hexadecimal number: the first ASCII character representing the
 * high-order 4 bits, and the second the low-order 4 bits of the
 * byte.
 */
void mc6809dbg::load_srecord(const char *filename, Word base, bool fatal)
{
    FILE *fp;
    int done = 0;
    unsigned int lowAddr = 0xffff, highAddr = 0;

    fp = fopen(filename, "rt");
    if (!fp) {
        perror("filename");
        if (fatal)
            exit(EXIT_FAILURE);
        return;
    }

    while (!done) {
        Byte        n, t;
        Word        addr;
        Byte        b;

        while(fgetc(fp) != 'S' && !feof(fp)) // Look for the S
              ;
        if(feof(fp))
            break;
        t = fgetc(fp);            // Type
        n = fread_hex_byte(fp);        // Length
        addr = fread_hex_word(fp);        // Address

        n -= 2;
        if (t == '0') {
            while (--n) {
                b = fread_hex_byte(fp);
                if (b=='\n') putchar('\r');
                putchar(b);
            }
            putchar('\r');
            putchar('\n');
        } else if (t == '1') {
           	lowAddr = addr+base < lowAddr ? addr+base : lowAddr;
            while (--n) {
                b = fread_hex_byte(fp);
				write(addr + base, b);
        		highAddr = addr+base > highAddr ? addr+base : highAddr;
				addr++;
            }

        } else if (t == '9') {
            pc = addr + base;
            done = 1;
        }
        // Read and discard checksum byte
        (void)fread_hex_byte(fp);

    }
    fclose(fp);

    fprintf(stderr, "\t%04x - %04x\r\n", lowAddr, highAddr);
}

// void mc6809dbg::save_srecord(const char *filename, Word beginAddr, Word endAddr)
// {
//     FILE        *fp;

//     fp = fopen(filename, "wt");
//     if (!fp) {
//         perror("filename");
// //        if (fatal)
// //            exit(EXIT_FAILURE);
//         return;
//     }

//     char foo[33];
//     for (Word addr = beginAddr; addr < endAddr; addr += 16)
//     {
//         Byte length = 16;
//         if (endAddr-addr < 16)
//             length = endAddr-addr;
//         Byte checksum = 0;

//         for(int i=0; i<length; i++)
//         {
//             sprintf(&foo[i*2], "%02x", memory[addr+i]);
//         }
//         foo[32] = 0;

//         fprintf(fp, "S1%02x%04x%s%02x\n", length+3, addr, foo, checksum);
//     }
//     fclose(fp);
// }

void mc6809dbg::load_raw(const char *filename, Word base, bool fatal)
{
    FILE *fp;
    Byte buffer[100];
    unsigned int lowAddr = 0xffff, highAddr = 0;
    Word addr = 0;

    fp = fopen(filename, "rb");
    if (!fp) {
        perror("filename");
        if (fatal)
            exit(EXIT_FAILURE);
        return;
    }

    lowAddr = addr+base;
    int cnt = fread(buffer, 1, sizeof(buffer), fp);
    while(cnt)
    {
    	write_buffer(addr+base, cnt, buffer);
    	addr += cnt;
    	highAddr = addr+base > highAddr ? addr+base : highAddr;
    	cnt = fread(buffer, 1, sizeof(buffer), fp);
    }
    fclose(fp);

    fprintf(stderr, "\t%04x - %04x\r\n", lowAddr, highAddr);
}

void mc6809dbg::load_lwtools_sym(const char *filename, Word base, bool fatal)
{
// BrkPnt EQU $FE83

	(void)fatal;

	ifstream file;
	file.open(filename, ifstream::in);
	string sym;
	string equ;
	string addrs;
	char ch; // to get file >> .... to throw away the $ infront of the address.
	unsigned int addr;
    unsigned int lowAddr = 0xffff, highAddr = 0;

	if(file.is_open()) 
	{
	    while(file  >>sym >> equ >> ch >> hex >> addr)  
	    {
           	lowAddr = addr+base < lowAddr ? addr+base : lowAddr;
       		highAddr = addr+base > highAddr ? addr+base : highAddr;
	        // cout << sym << "..."  << addr << endl;
	        // addr = strtol(addrs.c_str(), NULL, 16);
			addSymbol(addr + base, sym);
	    }
	}
	else
	{
	    cout<< "file open fail" <<endl;
	}

	// sym = "BrkPnt";
	// int addr2 = 0xFE83;
	// sym_table[addr2] = sym;

    fprintf(stderr, "\t%04x - %04x\r\n", lowAddr, highAddr);
}

void mc6809dbg::load_lwtools_map(const char *filename, Word base, bool fatal)
{
// Section: .dp (mpx9+/RptErr.obj) load at 0013, length 0006
// Section: .dp (mpx9+/Mod.obj) load at 0019, length 0006
// Symbol: \02.bss (mpx9+/CmdLine.obj) = 0DD2
// Symbol: \02.data (mpx9+/CmdLine.obj) = 0DD2
// Symbol: \02.data (mpx9+/start.obj) = 0DD2

	(void)fatal;

	ifstream file;
	file.open(filename, ifstream::in);
	string sym, tag;
	string equ;
	string addrs;
	char ch; // to get file >> .... to throw away the $ infront of the address.
	unsigned int addr;
    unsigned int lowAddr = 0xffff, highAddr = 0;

	if(file.is_open()) 
	{
	    while(file >> tag)  
	    {
		   	// cout << "tag:" << tag << endl;
			if (tag == "Section:")
			{
				file.ignore(99, '\n');
				// cout << tag << endl;
			}
			else // tag == "Symbol:"
			{
				file >> sym >> equ >> ch >> hex >> addr;
	           	lowAddr = addr+base < lowAddr ? addr+base : lowAddr;
	       		highAddr = addr+base > highAddr ? addr+base : highAddr;
				// cout << sym << "..." << equ << "..." << ch << "..." << hex << addr << endl;
				addSymbol(addr + base, sym);
			}
		}
	}
	else
	{
	    cout<< "file open fail" <<endl;
	}

    fprintf(stderr, "\t%04x - %04x\r\n", lowAddr, highAddr);
}

/*

load '../../../percom/psymon/psymon.ihex base 400
load '../../../percom/mpx9/tForth/tforth09.s19
load '../../../tools/usim/usimdbg/helloworld.bin
load '../../../percom/psymon/psymon.sym
load '../../../percom/mpx9/mpx9+/mpx9+.map

*/


void mc6809dbg::showSymbolsByTextMatch(char *pText)
{
    ::showSymbolsByTextMatch(pText);
}

void mc6809dbg::showSymbolsByAddressRange(TRange& range)
{
	::showSymbolsByAddressRange(range);
}


// const char* GetSymForAddr(Word addr)
// {
// 	static string symbol = sym_table.at(addr);
// 	return symbol.c_str();
// }