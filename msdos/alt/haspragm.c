/*
 * Check whether #pragma startup is supported.
 *
 * $Header: haspragm.c 1.0 95/07/26 $
 * $Log:	haspragm.c,v $
 * Revision 1.0  95/07/26  xx:xx:xx  BB
 * Initial revision.
 */

#include <stdio.h>
#include <stdlib.h>

int haspragma = 0;

void dotest() {
  haspragma = 1;
}
#pragma startup dotest

int main() {
  printf(
    "/*\n"
    " * haspragm.h\n"
    " */\n"
    "#ifndef __BB_HAS_PRAGMA_H__\n"
    "#define __BB_HAS_PRAGMA_H__\n");
  if (haspragma) printf("#define BB_HAS_PRAGMA_STARTUP\n");
  printf("#endif /* __BB_HAS_PRAGMA_H__ */\n");
  return 0;
}
