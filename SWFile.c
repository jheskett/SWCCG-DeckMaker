// SWFile.c

// File management functions for SWCCGDM

#include <stdio.h>

int	FileLength(FILE *fh)
{
	int iCurrentPos,iFileLength;

	iCurrentPos = ftell(fh);
	fseek(fh,0,SEEK_END);
	iFileLength = ftell(fh);

	fseek(fh,iCurrentPos,SEEK_SET);
	return iFileLength;
}
