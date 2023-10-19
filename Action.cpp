/*
 * Action.cpp
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */

#include "Action.h"

Action::Action(const char* name_)
: name(name_)
{
}

Action::~Action()
{
}

bool Action::do_action(mc6809& cpu) const
{
	(void)cpu;
	fprintf(stderr, "Action::do_action()\r\n");
    return false;
}
