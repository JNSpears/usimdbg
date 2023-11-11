/*
 * Trigger.cpp
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */

#include "Trigger.h"
#include <iostream>
#include <iomanip>

using namespace std;

Trigger::Trigger(const char* name_)
: name(name_)
{
}

Trigger::~Trigger()
{
}

void Trigger::display(void) const
{
    cerr << name;
}


void Trigger::displayOccurred(void) const
{
    cerr << "  ** ";
    display();
    cerr << " **\r\n";
}

bool Trigger::isTriggered(TraceFrame& frame, bool silent)
{
    (void)frame;
    (void)silent;
    return true;
}

