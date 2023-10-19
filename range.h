/*
 * Range.h
 *
 *  Created on: Sep 6, 2023
 *      Author: james
 */

#ifndef RANGE_H_
#define RANGE_H_

#include "typedefs.h"

// class Range {
// public:
// 	Range();
// 	Range(Word addr);
// 	Range(Word addr, unsigned int len);
// 	Range(Word addr, Word to);
// 	virtual ~Range();

// public:
// 	Word getStartAddr(void);
// 	bool isDone();
// 	void Next(unsigned int cnt = 0);

// private:
// 	bool _default_range;
// 	bool _is_to;
// 	Word _addr;
// 	Word _to_addr;
// 	unsigned int _len;
// };

typedef struct TRange {
	DWord addr; 
	int len; 
	DWord to;
	bool typeTo; 
	bool def;
} SRange;

extern void dumpRange(TRange& range);
extern Word getStartAddr(TRange& range);
extern bool isDone(TRange& range);
extern void Next(TRange& range, unsigned int cnt = 1);

#endif /* RANGE_H_ */
