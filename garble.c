#include "garble.h"

static char *password;
static char *passp;

void
set_password(char *p)
{
	password = p;
}

void
init_garble()
{
	passp = password;
}

Byte
ungarble(Byte byte)
{
	if (password) {
		byte ^= *passp++;
		if (!*passp) {
			passp = password;
		}
	}

	return byte;
}
