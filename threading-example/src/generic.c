/*
	Generic utility functions that may be used anywhere
*/

#include "common.h"

void kbPause()
{
	int c;
	printf("Press ENTER to exit...\n");
	do
	{
		c = getchar();
	}while((c != '\n') && (c != EOF));
}
