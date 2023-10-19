/*
 * Z5023.cpp
 *
 *  Created on: Apr 11, 2022
 *      Author: james
 */

#include "hexadump.h"
#include <cstring>
#include "typedefs.h"



void hexdump(char *s, int nBytes, int addr, const char *annotation, int lineWidth)
{
    int i;
    int line;
    int last_line;
    unsigned char ch, b[4096*2];

    fprintf(stderr, "\r\n");
    if (annotation)
    	fprintf(stderr, "%s\r\n", annotation);
    memset(b, '\0', 4096);
    memcpy(b, s, nBytes);
    last_line = ((nBytes%lineWidth)>0)?(nBytes/lineWidth)+1:(nBytes/lineWidth);
    for (line = 0;line<last_line;line++) {
      Word CurrLineAddr = addr+line*lineWidth;
      fprintf(stderr, "%04X: ", CurrLineAddr);
      for (i = 0;i<lineWidth;i++) {
                  if (CurrLineAddr+i < addr+nBytes)
                	  fprintf(stderr, "%02x ", b[line*lineWidth+i]);
                  else
                	  fprintf(stderr, "   ");
				  if (i==7)
					  fprintf(stderr, " ");
      }
      fprintf(stderr, "|");
      for (i = 0;i<lineWidth;i++) {
        if ((b[line*lineWidth+i] >= 0x20) && (b[line*lineWidth+i] < 0x7f)) {
          ch = b[line*lineWidth+i];
        } else {
          ch = '.';
        }
        if (CurrLineAddr+i < addr+nBytes)
          fprintf(stderr, "%c", ch);
        // else
        //   fprintf(stderr, " ");
        if (i==7)
            fprintf(stderr, "  ");


      }
      fprintf(stderr, "|\r\n");
    }
}
