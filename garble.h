#ifndef GARBLE_H
#define GARBLE_H
#include "spark.h"

void set_password(char *p);
void init_garble(void);
Byte ungarble(Byte byte);
Byte garble(Byte byte);

#endif
