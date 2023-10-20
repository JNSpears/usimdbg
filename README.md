# Usimdbg
**Usimdbg** is a debugger written to run with Ray Bellis' mc6809 emulator **usim** ([https://github.com/raybellis/usim/](URL)), it is written in C++.
## Functionality
Command keywords are displayed in mixed case, the upper case part is the short abbreavation.
### Help command
	HELP
Display Help text.
### RESET command
	RESET
Reset the processor state.
### Memory access commands
	(BYTE|WORD) addr [LEN expr | TO addr] [ '=' expr [, ...]]
Display, modify or fill memory.
### Step command
	S [expr]
Step expr instructions, (default=1) display registers before and disassembly of the executed instruction after.
### Go command
	G
Go until a breakpoint condition happens.
### Breakpoint command
	BReak [ [ addr ] [ read ] [ value ] [ OCCURS expr ] | TBF | CLEAR expr ]
Set, display or clear Breakpoint(s)

  * **BR**
&ndash; with no args displays breakpoints with their index.
  * **BR CLEAR**
&ndash; expr clears a speific breakpoint by the index.
  * **BR** *addr*
&ndash; sets an execution breakpoint.
  * **BR** *addr* (**read**|**write**|**access**)
&ndash; sets a memory Read, Write or Access break point at *addr*.
  * **BR** *addr* (**read**|**write**|**access**) *value*
&ndash; sets a memory Read, Write or Access break point at *addr* that will only break if the memory value is *value*
  * **BR** *addr* **OCCURS** *n*
&ndash; sets an execution breakpoint, break will happen on the *n*th occurance of the trigger.
  * **BR** *addr* (**read**|**write**|**access**) **OCCURS** *n*
&ndash; sets a memory Read, Write or Access break point at *addr*, break will happen on the *n*th occurance of the trigger.
  * **BR** *addr* (**read**|**write**|**access**) *value* **OCCURS** *n*
&ndash; sets a memory Read, Write or Access break point at *addr* that will only break if the memory value is *value*, , break will happen on the *n*th occurance of the trigger.
  * **BR TBF**
&ndash; Breaks when the Trace Buffer is Full.

***NOTE:*** when an **OCCURS** break point reaches zero count, it will break on every occurance of the trigger event.

### Register access commands

  * **Regs**
&ndash; Display all registers.
  * **R***id* [= *expr*]
&ndash; Display or change the value of a register. registers rA, rB ... rX, rY, rPC

***NOTE:*** Registers are prefixed with 'r' i.e. `RA`, `RBB`, `RCC`, `RDP`, `RX`, `RY`, `RU` `RS`, `RPC` (or $)
### Trace command
    Trace [*expr* | ( *addr* **LEN** *expr* | *addr* **TO** *addr*) | **CLEAR**]
Display Trace or clear trace.
#### Display last *expr* entries in the trace buffer
    Trace *expr*
#### Display frame *addr* for *expr* entries in the trace buffer
    Trace *addr* **LEN** *expr*
#### Display frame *addr1* to *addr2* entries in the trace buffer
    Trace *addr1* **TO** *addr2*
#### Clear the trace buffer
    Trace CLEAR
### Asm command
    ASM [LEN expr | TO addr]
Disassemble memory, if symbols are loaded they will be displayed also.
### EVAL command
    EVAL expr
Evaluate experssion and display as binary, octal, decimal hexadecimal and ASCII
### Load command
    LOAD '<filename> [BASE expr] [APPEND] 
Load a filename into memory, filetypes: iHex, s19, raw/bin extensions, optionally adding a base value to relocate into higher memory. Also load symbols, symbols may be appended to the previously loaded set of symbols. Currently supports 
loading symbols from lwtools symbol file and mapfile (.sym and .map extensions)
### Symbols command
	SYMbols [addr [LEN expr | TO addr] | 'prefix-text-to-match ]
Display all symbols currently loaded, a symbol matching a specific address or in a specified range of addresses or symbols matching a givent prefix
### Expressions
Expressions include the flowing:

  * **Numbers** (Hex digits must start with a decimal digit)
  * Math functions (**+**,**-**,**\***,**/**,**%**,**()**)
  * Logic functions OR,AND,XOR,Shift Right n, Shift Left n (**|**,**&**,**^**,**<<**,**>>**),
  * Registers `RA`, `RBB`, `RCC`, `RDP`, `RX`, `RY`, `RU` `RS`, `RPC` (or $)
  * **"***Symbol*
         ;
## Tilde Escape
The Tilde Escape feature has been extended to include:

  * R
&ndash; Reset the system.
  * B
&ndash; Break to **Usimd**

## Build
