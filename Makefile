USIM_PATH	= ../usim/

SHELL 		= /bin/bash
CC 			= g++
CPPFLAGS 	= -O3  -I $(USIM_PATH)  --std=c++17
DIFFFLAGS   = --ignore-all-space -s -q

DLIB_SRCS	= parser.cpp lexer.cpp hexadump.cpp mc6809dbg.cpp BreakTrace.cpp \
			  Trigger.cpp Action.cpp termdbg.cpp range.cpp Symbol.cpp
DOBJS		= $(DLIB_SRCS:.cpp=.o) 6309dasm.o
DLIB		= libusimdbg.a

DBIN		= usimdbg

##########################################################################################

all: $(DBIN) tests

$(DLIB): $(DOBJS)
	ar crs $(@) $^
	ranlib $(@)

$(DBIN): $(DLIB) main.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) main.o $(USIM_PATH)term.o -lreadline -L. -L$(USIM_PATH) -lusim -lusimdbg -o $(@)

lexer.cpp lexer.hpp: lexer.l
	flex -i --header-file=lexer.hpp -o lexer.cpp lexer.l 

parser.cpp parser.hpp: parser.y
	bison -d -v -o parser.cpp parser.y

.SUFFIXES: .cpp .c

##########################################################################################

clean:
	$(RM) lexer.[hc]pp parser.[ch]pp parser.output $(DBIN) *.o $(DLIB)

##########################################################################################

depend:
	makedepend 	main.cpp $(DLIB_SRCS) 6309dasm.c

##########################################################################################

.ONESHELL:

run: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex

tests: testsetup test-all report

.PHONY:
testsetup: tests/test_main.ihex tests/test_main.s19 tests/test_main.bin
	rm -f tests/summary

test-all: test1 test2 test3 test4 test5 test6 test7 test8 test10 test11

.PHONY:
report:
	@echo '--------------------------------------------------------------------'
	@cat tests/summary | awk '{ gsub("differ", "\033[1m\033[1;31m&\033[0m"); gsub("identical", "\033[1;36m&\033[0m"); print }'
	@echo '--------------------------------------------------------------------'
	@echo  Resuts: `grep identical tests/summary | wc -l` of `cat tests/summary  | wc -l` passed
	@echo '--------------------------------------------------------------------'

tests/test_main.s: $(USIM_PATH)tests/test_main.s
	cp $^ $@

tests/test.s: $(USIM_PATH)tests/test.s
	cp $^ $@

tests/test_main.ihex tests/test_main.sym tests/test_main.map: tests/test_main.s tests/test.s
	lwasm $< -fihex -o$(basename $@).ihex --symbol-dump=$(basename $@).sym --map=$(basename $@).map  -l$(basename $@).lst
tests/test_main.s19: tests/test_main.s tests/test.s
	lwasm $< -fsrec -o$(basename $@).s19
tests/test_main.bin: tests/test_main.s tests/test.s
	lwasm $< -fraw -o$(basename $@).bin

test1: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	help
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test2: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	step 8 # 8 instructions
	trace 999 # all frames
	asm $$ len 1
	step
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

ABINFILE=../../../percom/mpx9/my-utils/HelloWorld
test3: tests/test_main.ihex tests/test_main.s19 tests/test_main.bin tests/test_main.sym tests/test_main.map
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	load 'tests/test_main.ihex  base 400
	load 'tests/test_main.s19
	load 'tests/test_main.bin
	load 'tests/test_main.sym
	load 'tests/test_main.map
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test4: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	br
	br 1234
	br 1111 write 
	br 2222 read occurs 33
	br 4444 access 77 
	br 6666 access 88 occurs 8
	br
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test5: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	byte 30 = 11,22,33,44,55,66,77,88,99
	byte 44 len 7 = 12
	byte 0 len 80
	word 0 len 40
	byte 8f = 0aa
	byte 0 len 90
	byte 7f = 0bb
	byte 0 len 90
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test6:tests/test_main.ihex tests/test_main.sym
	./$(DBIN) tests/test_main.ihex  >tests/$(@).log 2>&1 <<EOF
	load 'tests/test_main.sym
	step
	asm $$ len 20 
	asm "status len 8
	eval "str_cc
	byte "str_cc len 20
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test7: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
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
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test8: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	load 'tests/test_main.sym
	step 6
	regs
	.a
	a=0aa
	b=0bb
	x=1111
	.a
	.b
	.x
	$
	regs
	reset
	regs
	x = 100
	a = 4
	x = x + a*2
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std

test9: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	load '../tests/abc.sym
	br "handle_swi
	br
	asm "handle_swi len 1
	go
	trace 8
	asm $$ len 1
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test10: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
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
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

test11: tests/test_main.ihex
	./$(DBIN) tests/test_main.ihex >tests/$(@).log 2>&1 <<EOF
	load 'tests/test_main.sym
	sym         # all
	sym 'put
	sym 'testabc
	sym 't
	sym 0 len 400+1
	sym 0e000 to 0e080
	EOF
	diff $(DIFFFLAGS) tests/$(@).log tests/$(@).std | tee -a tests/summary

##########################################################################################

# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: /usr/include/unistd.h /usr/include/features.h
main.o: /usr/include/stdc-predef.h mc6809dbg.h /usr/include/stdlib.h
main.o: /usr/include/alloca.h /usr/include/ctype.h BreakTrace.h Action.h
main.o: Trigger.h TraceFrame.h range.h termdbg.h /usr/include/term.h
main.o: /usr/include/ncurses_dll.h /usr/include/termios.h
main.o: /usr/include/memory.h /usr/include/string.h /usr/include/strings.h
hexadump.o: hexadump.h /usr/include/stdlib.h /usr/include/alloca.h
hexadump.o: /usr/include/features.h /usr/include/stdc-predef.h
hexadump.o: /usr/include/stdio.h
mc6809dbg.o: /usr/include/string.h /usr/include/strings.h
mc6809dbg.o: /usr/include/features.h /usr/include/stdc-predef.h
mc6809dbg.o: /usr/include/libgen.h mc6809dbg.h /usr/include/stdlib.h
mc6809dbg.o: /usr/include/alloca.h /usr/include/ctype.h BreakTrace.h Action.h
mc6809dbg.o: Trigger.h TraceFrame.h range.h termdbg.h /usr/include/term.h
mc6809dbg.o: /usr/include/ncurses_dll.h /usr/include/termios.h hexadump.h
mc6809dbg.o: /usr/include/stdio.h Symbol.h
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
