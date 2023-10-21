**Usimdbg** is a debugger written to run with Ray Bellis' mc6809 emulator **Usim** ([https://github.com/raybellis/usim/](URL)), it is written in C++.
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

  * **byte** 1234 = 88
&mdash; Set a byte in memory.
  * **byte** 0f000 **len** 20 = 0
&mdash; Zero a region of memory.
  * **byte** 0 **to** 400 = 0ff
&mdash; Set a region of memory to 0xff.
  * **byte** 0 **to** 100
&mdash; Display memory as bytes.
  * **word** c002 = *expr*
&mdash; Set a word of memory to a value.
  * **word** 200 = 1111,2222,3333,4444,5555,6666
&mdash; Set a sequence of words to values.
  * **byte** 400 
&mdash; Display the value of memory location 400 (display defaults to 16 bytes.)
  * **byte** 1000 **len** 1000 = 00,11,22,33,44,55,66,77
&mdash; Fill a 4K of memory at 4K to 8K with a repeating pattern.
### Step command
	STep [expr]
Step expr instructions, (default=1) display registers before and disassembly of the executed instruction after.
### Go command
	Go
Go until a breakpoint condition happens.
### Breakpoint command
	BReak [ [ addr ] [ read ] [ value ] [ OCCURS expr ] | TBF | CLEAR expr ]
Set, display or clear Breakpoint(s)

  * **BR**
&mdash; with no args displays breakpoints with their index.
  * **BR CLEAR**
&mdash; expr clears a speific breakpoint by the index.
  * **BR** *addr*
&mdash; sets an execution breakpoint.
  * **BR** *addr* (**read**|**write**|**access**)
&mdash; sets a memory Read, Write or Access break point at *addr*.
  * **BR** *addr* (**read**|**write**|**access**) *value*
&mdash; sets a memory Read, Write or Access break point at *addr* that will only break if the memory value is *value*
  * **BR** *addr* **OCCURS** *n*
&mdash; sets an execution breakpoint, break will happen on the *n*th occurance of the trigger.
  * **BR** *addr* (**read**|**write**|**access**) **OCCURS** *n*
&mdash; sets a memory Read, Write or Access break point at *addr*, break will happen on the *n*th occurance of the trigger.
  * **BR** *addr* (**read**|**write**|**access**) *value* **OCCURS** *n*
&mdash; sets a memory Read, Write or Access break point at *addr* that will only break if the memory value is *value*, , break will happen on the *n*th occurance of the trigger.
  * **BR TBF**
&mdash; Breaks when the Trace Buffer is Full.

***NOTE:*** when an **OCCURS** break point reaches zero count, it will break on every occurance of the trigger event.

### Register access commands

  * **Regs**
&mdash; Display all registers.
  * *id* [= *expr*]
&mdash; Display or change the value of a register. Registers A, B, CC, D, DP X, Y, U, S, PC (or $)
### Trace command
    Trace [expr | ( addr LEN expr | addr TO addr) | CLEAR]
Display Trace or clear trace.
#### Display last *expr* entries in the trace buffer
    Trace expr
#### Display frame *addr* for *expr* entries in the trace buffer
    Trace addr LEN expr
#### Display frame *addr1* to *addr2* entries in the trace buffer
    Trace addr1 TO addr2
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
  * Registers `A`, `B`, `CC`, `DP`, `X`, `Y`, `U` `S`, `PC` (or $)
  * **"***Symbol*
## Tilde Escape
The Tilde Escape feature has been extended to include:

  * R
&mdash; Reset the system.
  * B
&mdash; Break to **Usimdbg**

## Build
This has only been built and tested on Linux, windows should work with cgywin or some other Linux tool set.

### Requirements
Build the **Usim** library (currently in my fork of **Usim** ([https://github.com/JNSpears/usim.git](URL)) first. *If the **Usim** and **Usimdbg** directorys are not in the same parent directory
you will need to edit USIM_PATH in Makefile.*
## ToDo
Features to consider implementing in the future:

  * Features:
    * Add cycle count to trace.
    * load AS9 created symbol files.
  * Triggers:
    * Trigger range of addresses &mdash; `BR 400 to 480 write`.
    * Trigger if cycle counter matches N &mdash; `BR TSC 100000`.
    * Trigger THEN &mdash; `BR 1234 write then 8888 read`.
        &mdash; or maybe an **ARM**/**DISARM** **Trigger Action**
    * Trigger value other than equal &mdash; `BR "state write > 12"`, (need *relops*, and bit test).
    * Trigger register with value.
  * Trigger Actions:
    * Count number of times a trigger has occured. (need display and clear functions).
    * Turn trace **on** or **off**.
    * Annotation at when trigger happens
    *  &mdash; `ANNO "WrAcia write [Entering Acia port Write function]`
    *  &mdash; `ANNO "DevCtrl x [dev control block] : "Dcb len 10`
  
