#include <windows.h>
#include <stdio.h>
#include <commctrl.h>

BOOL createworkfile(HWND hwndProgress)
{
	
	if (hwndProgress!=NULL)
	{
		SendMessage(hwndProgress,PBM_SETRANGE,0L,100L);

	}

	return TRUE;
}
