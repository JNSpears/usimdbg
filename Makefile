USIM_PATH	= ../usim/

SHELL=/bin/bash
CC 			= g++
CPPFLAGS 	= -O3  -I $(USIM_PATH)  --std=c++17
  
DLIB_SRCS	= parser.cpp lexer.cpp hexadump.cpp mc6809dbg.cpp BreakTrace.cpp \
			  Trigger.cpp Action.cpp termdbg.cpp range.cpp Symbol.cpp
DOBJS		= $(DLIB_SRCS:.cpp=.o) 6309dasm.o
DLIB		= libusimd.a

DBIN		= usimd

##########################################################################################

all: $(DBIN)

$(DLIB): $(DLIB)($(DOBJS))

$(DBIN): $(DLIB) main.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) main.o $(USIM_PATH)term.o -lreadline -L. -L$(USIM_PATH) -lusim -lusimd -o $(@)

lexer.cpp: lexer.l
	flex -i --header-file=lexer.hpp -o lexer.cpp lexer.l 

parser.cpp parser.hpp: parser.y
	bison -d -v -o parser.cpp parser.y

.SUFFIXES: .cpp .c

##########################################################################################

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output $(DBIN) *.o

##########################################################################################

depend:
	makedepend 	main.cpp $(DLIB_SRCS) 6309dasm.c

##########################################################################################

.ONESHELL:

test: test1 test2 test3 test4 test5 test6 test7 test8 test10 test11

test1:
	./$(DBIN) ../tests/test_main.hex >tests/$(@).log 2>&1 <<EOF
	help
	EOF
	diff -s -q tests/$(@).log tests/$(@).std

test2:
	./$(DBIN) ../tests/test_main.hex >tests/$(@).log 2>&1 <<EOF
	step 8 # 8 instructions
	trace 999 # all frames
	asm $$ len 1
	step
	EOF

ABINFILE=../../../percom/mpx9/my-utils/HelloWorld
test3:
	cp $(ABINFILE).cm $(ABINFILE).bin
	./$(DBIN) ../tests/test_main.hex >tests/$(@).log 2>&1 <<EOF
	load '../../../percom/psymon/psymon.ihex base 400
	load '../../../percom/mpx9/tForth/tforth09.s19
	load '$(ABINFILE).bin
	load '../../../percom/psymon/psymon.sym
	load '../../../percom/mpx9/mpx9+/mpx9+.map
	EOF
	rm $(ABINFILE).bin

test4:
	./$(DBIN) ../tests/test_main.hex >tests/$(@).log 2>&1 <<EOF
	br
	br 1234
	br 1111 write 
	br 2222 read occurs 33
	br 4444 access 77 
	br 6666 access 88 occurs 8
	br
	EOF

test5:
	./$(DBIN) ../tests/test_main.hex >tests/$(@).log 2>&1 <<EOF
	byte 30 = 11,22,33,44,55,66,77,88,99
	byte 44 len 7 = 12
	byte 0 len 80
	word 0 len 40
	byte 8f = 0aa
	byte 0 len 90
	byte 7f = 0bb
	byte 0 len 90
	EOF

test6:
	./$(DBIN) ../../../percom/psymon/psymon.ihex >tests/$(@).log 2>&1 <<EOF
	load '../../../percom/psymon/psymon.sym
	step
	asm $$ len 20 
	asm "GETCMD len 8
	eval "PROMPT
	byte "PROMPT
	EOF

test7:
	./$(DBIN) ../../../percom/psymon/psymon.ihex >tests/$(@).log 2>&1 <<EOF
	byte 123
	byte 123 len 12
	byte 123 len 6

	word 123
	word 123 len 12
	word 123 len 6

	asm 0
	asm 0 len 1
	asm 0 len 8
	asm 0 to 10


	byte 12 = 22,33,44,55,66,77,88,99,0aa,0bb,0cc
	byte 0 len 40

	byte 13 len 9 = 11
	byte 0 len 40

	byte 13 len 4 = 55,0aa
	byte 0 len 40

	byte 10 len 8 = 55,0aa
	byte 0 len 40

	byte 12 len 6 = 22,33,44,55,66,77,88,99,0aa,0bb,0cc
	byte 0 len 40

	byte 10 to 1f = 0ff
	byte 12 to 1c = 0ee
	byte 0 len 40

	byte 0 len 40 = 0
	byte 8 = 1111,2222,3333,4444,5555,6666,7777,8888,9999,0aaaa,0bbbb,0cccc
	byte 0 len 20
	byte 8 len 12 = 1111,2222,3333,4444,5555,6666,7777,8888,9999,0aaaa,0bbbb,0cccc
	byte 0 len 30

	byte 0 len 40 = 0
	byte 8 to 0c = 1111,2222,3333,4444,5555,6666,7777,8888,9999,0aaaa,0bbbb,0cccc
	byte 0 len 20
	byte 8 to 10 = 1111,2222,3333,4444,5555,6666,7777,8888,9999,0aaaa,0bbbb,0cccc
	byte 0 len 30

	byte 0 len 40 = 0
	word 8 = 1111,2222,3333,4444,5555,6666,7777,8888,9999,0aaaa,0bbbb,0cccc
	word 0 len 20

	byte 0 len 40 = 0
	word 6 to 11 = 1111,2222,3333,4444,5555,6666,7777,8888,9999,0aaaa,0bbbb,0cccc
	word 0 len 20
	EOF

test8:
	./$(DBIN) ../../../percom/psymon/psymon.ihex >tests/$(@).log 2>&1 <<EOF
	load '../../../percom/psymon/psymon.sym
	step 6
	regs
	rx
	ru
	rcc
	ra=0aa
	rb=0bb
	rx=1111
	$
	regs
	reset
	regs
	EOF

test9:
	./$(DBIN) ../tests/abc.ihex >tests/$(@).log 2>&1 <<EOF
	load '../tests/abc.sym
	br "handle_swi
	br
	asm "handle_swi len 1
	go
	trace 8
	asm $$ len 1
	EOF

test10:
	./$(DBIN) ../tests/test_main.hex >tests/$(@).log 2>&1 <<EOF
	br 0E0D0 occurs 2
	br
	go
	go
	trace 999

	reset
	trace clear

	br 0E0D0 occurs 1
	br
	trace 999

	EOF

test11:
	./$(DBIN) ../tests/test_main.hex >tests/$(@).log 2>&1 <<EOF
	load '../../../percom/psymon/psymon.sym
	sym         # all
	sym 'Com
	sym 'load
	sym 'In
	sym 1 len 10
	sym 0fc00 to 0fc80
	EOF


##########################################################################################

# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: /usr/include/unistd.h /usr/include/features.h
main.o: /usr/include/stdc-predef.h mc6809dbg.h /usr/include/stdlib.h
main.o: /usr/include/alloca.h /usr/include/ctype.h BreakTrace.h Action.h
main.o: Trigger.h TraceFrame.h range.h termdbg.h /usr/include/term.h
main.o: /usr/include/ncurses_dll.h /usr/include/termios.h
main.o: /usr/include/memory.h /usr/include/string.h /usr/include/strings.h
parser.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/alloca.h
parser.o: /usr/include/features.h /usr/include/stdc-predef.h lexer.hpp
parser.o: /usr/include/string.h /usr/include/strings.h /usr/include/errno.h
parser.o: /usr/include/unistd.h hexadump.h mc6809dbg.h /usr/include/ctype.h
parser.o: BreakTrace.h Action.h Trigger.h TraceFrame.h range.h termdbg.h
parser.o: /usr/include/term.h /usr/include/ncurses_dll.h
parser.o: /usr/include/termios.h /usr/include/memory.h Symbol.h
parser.o: /usr/include/limits.h
lexer.o: /usr/include/stdio.h /usr/include/string.h /usr/include/strings.h
lexer.o: /usr/include/features.h /usr/include/stdc-predef.h
lexer.o: /usr/include/errno.h /usr/include/stdlib.h /usr/include/alloca.h
lexer.o: parser.hpp BreakTrace.h /usr/include/ctype.h Action.h Trigger.h
lexer.o: TraceFrame.h mc6809dbg.h range.h termdbg.h /usr/include/term.h
lexer.o: /usr/include/ncurses_dll.h /usr/include/termios.h
lexer.o: /usr/include/readline/readline.h /usr/include/readline/rlstdc.h
lexer.o: /usr/include/readline/rltypedefs.h /usr/include/readline/keymaps.h
lexer.o: /usr/include/readline/chardefs.h /usr/include/readline/tilde.h
lexer.o: /usr/include/readline/history.h /usr/include/time.h
lexer.o: /usr/include/unistd.h
hexadump.o: hexadump.h /usr/include/stdlib.h /usr/include/alloca.h
hexadump.o: /usr/include/features.h /usr/include/stdc-predef.h
hexadump.o: /usr/include/stdio.h
mc6809dbg.o: /usr/include/string.h /usr/include/strings.h
mc6809dbg.o: /usr/include/features.h /usr/include/stdc-predef.h
mc6809dbg.o: /usr/include/libgen.h mc6809dbg.h /usr/include/stdlib.h
mc6809dbg.o: /usr/include/alloca.h /usr/include/ctype.h BreakTrace.h Action.h
mc6809dbg.o: Trigger.h TraceFrame.h range.h termdbg.h /usr/include/term.h
mc6809dbg.o: /usr/include/ncurses_dll.h /usr/include/termios.h hexadump.h
mc6809dbg.o: /usr/include/stdio.h Symbol.h parser.hpp
BreakTrace.o: hexadump.h /usr/include/stdlib.h /usr/include/alloca.h
BreakTrace.o: /usr/include/features.h /usr/include/stdc-predef.h
BreakTrace.o: /usr/include/stdio.h /usr/include/string.h
BreakTrace.o: /usr/include/strings.h BreakTrace.h /usr/include/ctype.h
BreakTrace.o: Action.h Trigger.h TraceFrame.h
Trigger.o: Trigger.h TraceFrame.h
Action.o: Action.h
termdbg.o: termdbg.h /usr/include/term.h /usr/include/ncurses_dll.h
termdbg.o: /usr/include/termios.h /usr/include/features.h
termdbg.o: /usr/include/stdc-predef.h mc6809dbg.h /usr/include/stdlib.h
termdbg.o: /usr/include/alloca.h /usr/include/ctype.h BreakTrace.h Action.h
termdbg.o: Trigger.h TraceFrame.h range.h
range.o: range.h
Symbol.o: /usr/include/string.h /usr/include/strings.h
Symbol.o: /usr/include/features.h /usr/include/stdc-predef.h Symbol.h
6309dasm.o: /usr/include/stdio.h /usr/include/string.h /usr/include/strings.h
6309dasm.o: /usr/include/features.h /usr/include/stdc-predef.h hd6309.h
6309dasm.o: Symbol.h
