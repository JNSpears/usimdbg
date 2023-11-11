/*
 * Action.h
 *
 *  Created on: Mar 25, 2022
 *      Author: james
 */

#ifndef TOOLS_USIM_PERCOM_ACTION_H_
#define TOOLS_USIM_PERCOM_ACTION_H_

#include <memory>
#include <vector>
#include <functional>
#include "typedefs.h"

class mc6809;

class Action {
public:
	Action(const char* name_);
	virtual ~Action();

public:
	// using shared_ptr = std::shared_ptr<Action>;

	virtual bool doAction(mc6809& cpu) const;
	const char* name;
};

#endif /* TOOLS_USIM_PERCOM_ACTION_H_ */
