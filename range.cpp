/*
 * Range.cpp
 *
 *  Created on: Sep 6, 2023
 *      Author: james
 */

#include "range.h"
#include <iostream>


void dumpRange(TRange& range)
{
	fprintf(stderr, ".addr  :%04x\r\n", range.addr);
	fprintf(stderr, ".typeTo:%d\r\n", range.typeTo);
	fprintf(stderr, ".to    :%04x\r\n", range.to);
	fprintf(stderr, ".len   :%d\r\n", range.len);
	fprintf(stderr, ".def   :%d\r\n", range.def);
}

Word getStartAddr(TRange& range)
{
	return range.addr;
}

bool isDone(TRange& range)
{
	if (!range.typeTo)
		return range.len <= 0;
	return range.addr > range.to;
}

void Next(TRange& range, unsigned int cnt)
{
	range.addr += cnt;
	range.len--;
}


/*

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

*/
