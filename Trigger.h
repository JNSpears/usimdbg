/*
 * Trigger.h
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */

#ifndef TOOLS_USIM_PERCOM_TRIGGER_H_
#define TOOLS_USIM_PERCOM_TRIGGER_H_

#include <memory>
#include <vector>
#include <functional>
#include "typedefs.h"

#include "TraceFrame.h"

class mc6809;

class Trigger {
public:

	// typedef enum {
	// 	exec,
	// 	read,
	// 	write,
	// 	dontcare
	// } RW;

	Trigger(const char* name_);
	virtual ~Trigger();

	virtual void display(void) const;
	virtual void displayOccurred(void) const;

	virtual bool isTriggered(TraceFrame& frame, bool silent=false);

	const char* name;
};

#endif /* TOOLS_USIM_PERCOM_TRIGGER_H_ */
