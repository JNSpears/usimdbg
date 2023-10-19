/*
 * hexadump.h
 *
 *  Created on: Apr 11, 2022
 *      Author: james
 *      (C) J. Spears 2017, 2022
 */

#ifndef PERCOM_HEXADUMP_H_
#define PERCOM_HEXADUMP_H_

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <cstdio>

extern void hexdump(char *s, int nBytes, int addr, const char *annotation=NULL, int lineWidth=16);

#endif /* PERCOM_HEXADUMP_H_ */
