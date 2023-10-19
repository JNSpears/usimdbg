//
//
//	term.h
//
//	(C) R.P.Bellis 1994
//

#pragma once

#include <functional>
#include <cstdio>
#include "term.h"
#include "mc6809dbg.h"

class TerminalDbg : public Terminal {

protected:
	virtual void		tilde_escape_help_other();
	virtual void 		tilde_escape_do_other(char ch);

public:
	virtual	void		set_debug(void);
	virtual	void		set_exec(void);

// Public constructor and destructor
public:
						 TerminalDbg(mc6809dbg& _sys);
	virtual				~TerminalDbg();

protected:
	mc6809dbg& sys;

};
