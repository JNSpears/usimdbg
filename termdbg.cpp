//
//
//	term.cpp
//
//	(C) R.P.Bellis 1994
//

#include <cstdlib>
#include <cassert>
#include "termdbg.h"

//------------------------------------------------------------------------
// Machine dependent Terminal implementations
//------------------------------------------------------------------------

//------------------------------------------------------------------------

TerminalDbg::TerminalDbg(mc6809dbg& _sys)
: Terminal(), sys(_sys)
{
}

TerminalDbg::~TerminalDbg()
{
}

void TerminalDbg::set_debug(void)
{
	tcsetattr(input_fd, TCSANOW, &oattr);
}

void TerminalDbg::set_exec(void)
{
	tcsetattr(input_fd, TCSANOW, &nattr);
}

//------------------------------------------------------------------------
// Machine independent part, handles ssh-like tilde-escapes
//------------------------------------------------------------------------

void TerminalDbg::tilde_escape_help_other()
{
	fprintf(stderr, " ~r - reboot emulator\r\n");
	fprintf(stderr, " ~b - break to debuger.\r\n");
}

void TerminalDbg::tilde_escape_do_other(char ch)
{
	switch (ch) {
	case 'b':
	case 'B':
		sys.halt();
		break;
	case 'r':
	case 'R':
		sys.reset();
		break;
	default:
		break;
	}
}


/*

./usimd ../usim/tests/test_main.hex
go
abc
XYZ
123
~.

*/
