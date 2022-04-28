
#include <windows.h>

BOOL FindStats(HWND,char *); // fill stats window with find results
BOOL updatestats(HWND hDlg);
void testdraw(HWND);
BOOL printstats(HWND,UINT);
BOOL printdeck(HWND);
void stripcr(char *);

struct printeroptions
{
	UINT	uDestination;
	BOOL	bCardQty;
	BOOL	bNotes;
	BOOL	bSummary;
	BOOL	bDate;
	BOOL	bFSet;
	BOOL	bFSide;
	BOOL	bFRarity;
	BOOL	bFType;
	BOOL	bSSet;
	BOOL	bSSide;
	BOOL	bSRarity;
	BOOL	bSType;
} printopt;
