// Star Wars CCG DeckMaker 3.0
// for Windows 95

// started July 13, 1996

#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <commdlg.h>
#include <direct.h>
#include <math.h>

#include "resource.h"
#include "SWDeck.h"
#include "attributes.h"
#include "help.h"
#include "ezfont.h"
#include "SWStats.h"
#include "SWFile.h"
#include "SWPrint.h"

#define rWidth(r) (r.right-r.left)
#define rHeight(r) (r.bottom-r.top)

#define WMU_CHANGE_FONT 1L
#define WMU_FOCUS_NOTES 2L

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK InfoWndProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK ExcludeDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK IncludeDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK HelpDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK EditDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK SplashDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK SortDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK PriceDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK StatsDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK JumpDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK FindDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK QtyDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK MassEntryDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK WantDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK PrintDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK OrderDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK BrainiacDlgProc(HWND,UINT,WPARAM,LPARAM);

HWND	hwndInfo;

HANDLE	hInst;
char	*szAppName= "DeckMaker 3.2";

char	pHelpText[10000];	// buffer for storing context-sensitive help

int		iBuf;		// global buffer for passing values back and forth

int		cyChar;		// height of a standard character
int		cxClient,cyClient; // dimensions of the main client window
int		iNdent;
#define NUMSPACE 46  // iNdent-NUMSPACE=right edge of icons
int		iVPos,iVMax;
int		iSel;		// cardlist[] index to currently selected card
int		iSelPos;
int		iUndoSel;
BOOL	bViewBinder;

HFONT	hCardFont;

BOOL	bNeedSave;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   PSTR szCmdLine,      int iCmdShow)
{
	HWND		hwnd;
	HWND		hwndSplash;	// handle of "splash" window
	MSG			msg;
	WNDCLASSEX	wndclass;
	HACCEL		hAccel;
	int			i;
	HDC			hdc;
	int			cxWindow,cyWindow,xWindow,yWindow;
	char		szTmp[_MAX_PATH];

	DefineICO(hInstance);	// load all the images first off

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SWCCGDM));
	wndclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU);
	wndclass.lpszClassName	= szAppName;
	wndclass.hIconSm		= LoadImage(hInstance,MAKEINTRESOURCE(IDI_SWCCGDM),
							  IMAGE_ICON,16,16,0);

	RegisterClassEx(&wndclass);

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= InfoWndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SWCCGDM));
	wndclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDR_MENU);
	wndclass.lpszClassName	= "Card Information";
	wndclass.hIconSm		= LoadImage(hInstance,MAKEINTRESOURCE(IDI_SWCCGDM),
							  IMAGE_ICON,16,16,0);

	RegisterClassEx(&wndclass);

	hInst = hInstance;

	InitCommonControls();

	// before we do anything, get the current directory and save it for future use
	_getcwd(szTmp,_MAX_PATH);
	sprintf(szIni,"%s\\SWCCG DeckMaker.ini",szTmp);

	hwndSplash = CreateDialog(hInst,MAKEINTRESOURCE(IDD_SPLASHDLG),NULL,(DLGPROC)SplashDlgProc);

	i = OpenSWCCGData(hwndSplash);

	if (i>ALL_OK) DestroyWindow(hwndSplash);

    hdc = CreateIC("DISPLAY",NULL,NULL,NULL);
	cxWindow = GetDeviceCaps(hdc,HORZRES);
	cyWindow = GetDeviceCaps(hdc,VERTRES);
	DeleteDC(hdc);
	xWindow = cxWindow/20;					// main window's left is 10% in
	yWindow = cyWindow/10;					// main window's top is 6.7% down

	rMainWin.left = cxWindow/10;
	rMainWin.right = cxWindow/2+cxWindow/10;
	rMainWin.top = cyWindow/10;
	rMainWin.bottom = cyWindow-cyWindow/10;

	rMainWin.left=GetPrivateProfileInt("WindowPositions","Main_Left",rMainWin.left,szIni);
	rMainWin.right=GetPrivateProfileInt("WindowPositions","Main_Right",rMainWin.right,szIni);
	rMainWin.top=GetPrivateProfileInt("WindowPositions","Main_Top",rMainWin.top,szIni);
	rMainWin.bottom=GetPrivateProfileInt("WindowPositions","Main_Bottom",rMainWin.bottom,szIni);

	rStatsWin.left = cxWindow/2+cxWindow/10;
	rStatsWin.right = cxWindow-(cxWindow/10);
	rStatsWin.top = rMainWin.top;
	rStatsWin.bottom = cyWindow/2;

	rStatsWin.left=GetPrivateProfileInt("WindowPositions","Stats_Left",rStatsWin.left,szIni);
	rStatsWin.right=GetPrivateProfileInt("WindowPositions","Stats_Right",rStatsWin.right,szIni);
	rStatsWin.top=GetPrivateProfileInt("WindowPositions","Stats_Top",rStatsWin.top,szIni);
	rStatsWin.bottom=GetPrivateProfileInt("WindowPositions","Stats_Bottom",rStatsWin.bottom,szIni);

	rInfoWin.left = cxWindow/2+cxWindow/10;
	rInfoWin.right = cxWindow-(cxWindow/10);
	rInfoWin.top = cyWindow/2;
	rInfoWin.bottom = rMainWin.bottom;

	rInfoWin.left=GetPrivateProfileInt("WindowPositions","Info_Left",rInfoWin.left,szIni);
	rInfoWin.right=GetPrivateProfileInt("WindowPositions","Info_Right",rInfoWin.right,szIni);
	rInfoWin.top=GetPrivateProfileInt("WindowPositions","Info_Top",rInfoWin.top,szIni);
	rInfoWin.bottom=GetPrivateProfileInt("WindowPositions","Info_Bottom",rInfoWin.bottom,szIni);

	hwnd = CreateWindow(szAppName,				// class name
						szAppName,				// caption
						WS_OVERLAPPEDWINDOW|WS_VSCROLL,	// style
						rMainWin.left,			// x pos
						rMainWin.top,			// y pos
						rWidth(rMainWin),			// cx width
						rHeight(rMainWin),			// cy height
						NULL,					// parent window
						NULL,					// menu handle
						hInstance,				// instance
						NULL);					// creation parameters

	hwndInfo = CreateWindow("Card Information","Card Information",
							WS_OVERLAPPED|WS_THICKFRAME,
							rInfoWin.left,rInfoWin.top,
							rWidth(rInfoWin),rHeight(rInfoWin),
							hwnd,NULL,hInstance,NULL);

	if (i>ALL_OK)
	{
		MsgError(hwnd,i,"SWCCG Data Error");
	}
	else
	{

		hDlgStats = CreateDialog(hInst,MAKEINTRESOURCE(IDD_STATSDLG),
								hwnd,(DLGPROC)StatsDlgProc);

		DestroyWindow(hwndSplash);

		ShowWindow(hwndInfo,iCmdShow);
		UpdateWindow(hwndInfo);

		ShowWindow(hwnd,iCmdShow);
		UpdateWindow(hwnd);

		hAccel = LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));

		while (GetMessage(&msg,NULL,0,0))
		{
			if (!TranslateAccelerator(hwnd,hAccel,&msg))
			{
				if (hDlgStats==0||!IsDialogMessage(hDlgStats,&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		CloseSWCCGData();

	}

	DeleteICO();			// delete images loaded at start of program
	
	return msg.wParam;

}

void OkMessage(HWND hwnd,char *szMessage,char *szTitleName)
{
	char szBuffer[64+_MAX_FNAME+8];
	sprintf(szBuffer,szMessage,szTitleName[0]?szTitleName:UNTITLED);
	MessageBox(hwnd,szBuffer,szAppName,MB_OK|MB_ICONEXCLAMATION);
}

void updateinfo(HWND hwnd)
{
	UpdateWindow(hwnd);
	InvalidateRect(hwndInfo,NULL,TRUE);
	//UpdateWindow(hwndInfo);
}

// this adjusts scroll and position values/ranges
void UpdateRange(HWND hwnd)
{
	iVMax = max(0,iCListSize+2-cyClient/cyChar);
	iVPos = min(iVPos,iVMax);
	SetScrollRange(hwnd,SB_VERT,0,iVMax,FALSE);
	SetScrollPos(hwnd,SB_VERT,iVPos,TRUE);
	iSel=max(min(iSel,iCListSize-1),0);
	iSelPos=min(iSelPos,iVMax);
}

// this updates a selection to turn highlight off/on
void UpdateSel(HWND hwnd)
{
	RECT	rect;

	rect.top = (1-iVPos+iSel)*cyChar;
	if  ((rect.top>(-cyChar))&&(rect.top<cyClient))
	{
		rect.bottom = rect.top+cyChar;
		rect.left = iNdent;
		rect.right = cxClient;
		InvalidateRect(hwnd,&rect,TRUE);
	}
}

// this just updates the .deck displayed with the card
void UpdateSelQty(HWND hwnd)
{
	RECT	rect;

	bNeedSave = TRUE;

	if (menu.bDeckQty==TRUE)
	{
		rect.top = (1-iVPos+iSel)*cyChar;
		rect.bottom = rect.top+cyChar;
		rect.left = iNdent-NUMSPACE;
		rect.right = iNdent;
		InvalidateRect(hwnd,&rect,TRUE);
		UpdateWindow(hwnd);
	}
}

void viewnotes(HWND hDlg,BOOL bViewNotes)
{
	RECT	rect;
	int		cxDlg,cyDlg;

	GetClientRect(hDlg,&rect);
	cxDlg=rect.right-rect.left;
	cyDlg=rect.bottom-rect.top;
	MoveWindow(GetDlgItem(hDlg,IDC_STATSLABEL),0,0,cxDlg,cyChar,TRUE);
	if (!bViewNotes)
	{
		MoveWindow(GetDlgItem(hDlg,IDC_STATSLIST),0,cyChar,cxDlg,cyDlg-cyChar,TRUE);
		MoveWindow(GetDlgItem(hDlg,IDC_STATSNOTES),0,0,0,0,TRUE);
	}
	else
	{
		MoveWindow(GetDlgItem(hDlg,IDC_STATSNOTES),0,cyChar,cxDlg,cyDlg-cyChar,TRUE);
		MoveWindow(GetDlgItem(hDlg,IDC_STATSLIST),0,0,0,0,TRUE);
	}
	rect.bottom=cyChar;
	InvalidateRect(hDlg,&rect,TRUE);
	GetWindowRect(hDlg,&rStatsWin);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static HINSTANCE	hInstance;
	char				szBuf[100];
	int					i,j,k;
	static int			cxVScroll;
	int					cx,cy;
	RECT				rect;
	static int			aWidths[4];
	HDC					hdc;
	PAINTSTRUCT			ps;
	TEXTMETRIC			tm;
	int					iPaintBeg,iPaintEnd;
	static HMENU		hMenu,hCardMenu;
	int					iVInc;
	HCURSOR				hCursor;
	HBRUSH				hBrush;
	POINT				point;
	static BOOL			bPopUp;
	static BOOL			bNeedUpdate;
	UINT				ui;
	int					cxWindow,cyWindow,xWindow,yWindow;

	switch (iMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID__ENTERQTY:
					j = cardlist[iSel].deck;
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_QTYDLG),hwnd,(DLGPROC)QtyDlgProc);
					cardlist[iSel].wb += (cardlist[iSel].deck-j);
					cardlist[iSel].wb = max(0,min(cardlist[iSel].wb,99999));
					return 0;

				case ID__ZEROQTY:
					cardlist[iSel].deck = 0;
					cardlist[iSel].wb = 0;
					UpdateSelQty(hwnd);
					updatestats(hDlgStats);
					return 0;

				case ID__EDITPRICE:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_PRICEDLG),hwnd,(DLGPROC)PriceDlgProc);
					updatestats(hDlgStats);
					updateinfo(hwnd);
					return 0;

				case ID__EDITCARD:
					iBuf=cardlist[iSel].idx;
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_EDITDLG),hwnd,(DLGPROC)EditDlgProc);
					if (iBuf!=-1) SendMessage(hwnd,WM_COMMAND,(WPARAM)ID_FILE_NEW_ALL,0);
					return 0;

				case ID_OPTIONS_IGNOREUNLIMITED:
					ToggleMenu(hMenu,ID_OPTIONS_IGNOREUNLIMITED,&menu.bUnlimited);

					hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
					ShowCursor(TRUE);								// make the pointer the "hourglass"

					if (menu.bUnlimited==TRUE)
						ExcludeCList(ATT_SET,'U',FALSE);
					else
						IncludeCList(ATT_SET,'U');

					UpdateRange(hwnd);
					newsort(szSort);
					updatestats(hDlgStats);

					ShowCursor(FALSE); 	// restore cursor to original shape
					SetCursor(hCursor);

					InvalidateRect(hwnd,NULL,TRUE);

					return 0;

				case ID_SORT_BYSIDEFIRST:
					ToggleMenu(hMenu,ID_SORT_BYSIDEFIRST,&menu.bSortSides);
					SendMessage(hwnd,WM_COMMAND,uSortOrder,0);
					return 0;

				case ID_SORT_BYSETSFIRST:
					ToggleMenu(hMenu,ID_SORT_BYSETSFIRST,&menu.bSortSets);
					SendMessage(hwnd,WM_COMMAND,uSortOrder,0);
					return 0;

				case ID_SORT_BYNAME:
				case ID_SORT_BYTYPE:
				case ID_SORT_BYRARITY:
				case ID_SORT_BYDESTINY:
				case ID_SORT_BYCUSTOM:
					CheckMenuItem(hMenu,uSortOrder,MF_UNCHECKED);
					uSortOrder = wParam;
					CheckMenuItem(hMenu,uSortOrder,MF_CHECKED);

					hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
					ShowCursor(TRUE);								// make the pointer the "hourglass"

					if (menu.bSortSets&&menu.bSortSides) sprintf(szBuf,"ST");
					else if (menu.bSortSets&&!menu.bSortSides) sprintf(szBuf,"T");
					else if (!menu.bSortSets&&menu.bSortSides) sprintf(szBuf,"S");
					else sprintf(szBuf,"");

					switch (wParam)
					{
						case ID_SORT_BYTYPE: sprintf(szSort,"%sY",szBuf); break;
						case ID_SORT_BYRARITY: sprintf(szSort,"%sR",szBuf); break;
						case ID_SORT_BYDESTINY: sprintf(szSort,"%sD",szBuf); break;
						case ID_SORT_BYCUSTOM: sprintf(szSort,"%s%s",szBuf,szCustomSort); break;
						default: sprintf(szSort,"%s",szBuf); break;
					}
						
					newsort(szSort);
					updatestats(hDlgStats);

					ShowCursor(FALSE); 	// restore cursor to original shape
					SetCursor(hCursor);

					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_SORT_DEFINECUSTOM:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_SORTDLG),hwnd,(DLGPROC)SortDlgProc);
					if (iBuf) SendMessage(hwnd,WM_COMMAND,ID_SORT_BYCUSTOM,0);
					return 0;

				case ID_VIEW_DECKQUANTITY:
					ToggleMenu(hMenu,ID_VIEW_DECKQUANTITY,&menu.bDeckQty);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_BINDER:
					ToggleMenu(hMenu,ID_VIEW_BINDER,&menu.bBinderQty);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_NOUNLIMITED:
					if (!menu.bNoUnlimited)
						if (IDYES!=MessageBox(hwnd,"Are you sure you want to disable unlimited sets?\nNote that decks that use unlimited cards without combining may no longer work properly...",szAppName,MB_YESNO|MB_ICONQUESTION))
							return 0;
					ToggleMenu(hMenu,ID_VIEW_NOUNLIMITED,&menu.bNoUnlimited);
					DeleteFile(DATAFILEN);
					MessageBox(hwnd,"The data will update the next time this program is run.\r\n\
To update now, exit and run the DeckMaker again.",szAppName,MB_OK);
					return 0;

				case ID_VIEW_COMBINE:
					ToggleMenu(hMenu,ID_VIEW_COMBINE,&menu.bCombineWB);
					hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
					ShowCursor(TRUE);								// make the pointer the "hourglass"
					CombineWB();
					UpdateRange(hwnd);
//					newsort(szSort);
					updatestats(hDlgStats);
					ShowCursor(FALSE); 	// restore cursor to original shape
					SetCursor(hCursor);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_LSONLY:
					//if (menu.bExcludeDark==TRUE) return 0;
					menu.bExcludeLight=FALSE;
					menu.bExcludeDark=TRUE;
					UpdateOnlyMenu(hwnd);

					hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
					ShowCursor(TRUE);								// make the pointer the "hourglass"

					IncludeCList(ATT_SIDE,'*');
					ExcludeCList(ATT_SIDE,'d',FALSE);
					if (menu.bCombineWB) LoadCombineWB();
					UpdateRange(hwnd);
					updatestats(hDlgStats);

					ShowCursor(FALSE); 	// restore cursor to original shape
					SetCursor(hCursor);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_DSONLY:
					//if (menu.bExcludeLight==TRUE) return 0;
					menu.bExcludeLight=TRUE;
					menu.bExcludeDark=FALSE;
					UpdateOnlyMenu(hwnd);

					hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
					ShowCursor(TRUE);								// make the pointer the "hourglass"

					IncludeCList(ATT_SIDE,'*');
					ExcludeCList(ATT_SIDE,'L',FALSE);
					if (menu.bCombineWB) LoadCombineWB();
					UpdateRange(hwnd);
					updatestats(hDlgStats);

					ShowCursor(FALSE); 	// restore cursor to original shape
					SetCursor(hCursor);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;
					
				case ID_VIEW_BOTHONLY:
					//if ((menu.bExcludeDark==FALSE)&&(menu.bExcludeLight==FALSE)) return 0;
					menu.bExcludeLight=FALSE;
					menu.bExcludeDark=FALSE;
					UpdateOnlyMenu(hwnd);

					hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
					ShowCursor(TRUE);								// make the pointer the "hourglass"

					IncludeCList(ATT_SIDE,'*');
					if (menu.bCombineWB) LoadCombineWB();
					UpdateRange(hwnd);
					updatestats(hDlgStats);

					ShowCursor(FALSE); 	// restore cursor to original shape
					SetCursor(hCursor);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_FONT_COMPRESSED:
					ToggleMenu(hMenu,ID_VIEW_FONT_COMPRESSED,&menu.bCompressed);
					InvalidateRect(hwnd,NULL,TRUE);
					InvalidateRect(hwndInfo,NULL,TRUE);
					SendMessage(hwndInfo,WM_USER,WMU_CHANGE_FONT,0);
					SendMessage(hDlgStats,WM_USER,WMU_CHANGE_FONT,0);
					return 0;

				case ID_VIEW_FONT_BOLD:
					ToggleMenu(hMenu,ID_VIEW_FONT_BOLD,&menu.bBold);
					InvalidateRect(hwnd,NULL,TRUE);
					InvalidateRect(hwndInfo,NULL,TRUE);
					SendMessage(hwndInfo,WM_USER,WMU_CHANGE_FONT,0);
					SendMessage(hDlgStats,WM_USER,WMU_CHANGE_FONT,0);
					return 0;

				case ID_VIEW_ICONS_TYPE:
					ToggleMenu(hMenu,ID_VIEW_ICONS_TYPE,&menu.bTypeIcons);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_ICONS_DESTINY:
					ToggleMenu(hMenu,ID_VIEW_ICONS_DESTINY,&menu.bDestIcons);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_ICONS_RARITY:
					ToggleMenu(hMenu,ID_VIEW_ICONS_RARITY,&menu.bRareIcons);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_ICONS_SET:
					ToggleMenu(hMenu,ID_VIEW_ICONS_SET,&menu.bSetIcons);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_ICONS_EXTRA:
					ToggleMenu(hMenu,ID_VIEW_ICONS_EXTRA,&menu.bExtraIcons);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_CARDNAMES_COLOR:
					ToggleMenu(hMenu,ID_VIEW_CARDNAMES_COLOR,&menu.bColor);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_CARDNAMES_DARKSHADED:
					ToggleMenu(hMenu,ID_VIEW_CARDNAMES_DARKSHADED,&menu.bDarkShaded);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_FIND:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_FINDDLG),hwnd,(DLGPROC)FindDlgProc);
					return 0;

				case ID_NAV_JUMP:
					iUndoSel=iSel;
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_JUMPDLG),hwnd,(DLGPROC)JumpDlgProc);
					return 0;

				case ID_RANGES_LMH:
				case ID_RANGES_LH:
				case ID_RANGES_M:
					if (menu.iRangeMode==LOWORD(wParam)) return 0;
					CheckMenuItem(hMenu,menu.iRangeMode,MF_UNCHECKED);
					menu.iRangeMode=LOWORD(wParam);
					CheckMenuItem(hMenu,menu.iRangeMode,MF_CHECKED);
					updatestats(hDlgStats);
					return 0;

				case ID_STATS_DETAIL:
				case ID_STATS_SUMMARY:
				case ID_STATS_DESTINY:
				case ID_STATS_SET:
				case ID_STATS_PROBABILITY:
				case ID_STATS_CHARACTERS:
				case ID_STATS_LOCATIONS:
				case ID_STATS_SHIPS:
				case ID_STATS_INTERRUPTS:
				case ID_STATS_WEAPONS:
				case ID_STATS_FIND:
				case ID_STATS_NOTES:
//					if (menu.iStatsMode==LOWORD(wParam)) return 0;
					for (ui=ID_STATS_DETAIL;ui<=ID_STATS_FIND;ui++)
						CheckMenuItem(hMenu,ui,MF_UNCHECKED);
					CheckMenuItem(hMenu,ID_STATS_NOTES,MF_UNCHECKED);
					if ((menu.iStatsMode==ID_STATS_NOTES)&&(LOWORD(wParam)==ID_STATS_NOTES))
					{
						wParam = uPrevStats;
						SetForegroundWindow(hwnd);
					}
					menu.iStatsMode = LOWORD(wParam);
					if (menu.iStatsMode!=ID_STATS_NOTES) uPrevStats = menu.iStatsMode;
					CheckMenuItem(hMenu,menu.iStatsMode,MF_CHECKED);
					if (menu.iStatsMode!=ID_STATS_FIND) EnableMenuItem(hMenu,ID_STATS_FIND,MF_GRAYED);
					if (menu.iStatsMode==ID_STATS_NOTES)
					{
						viewnotes(hDlgStats,TRUE);
						SetForegroundWindow(hDlgStats);
						SendMessage(hDlgStats,WM_USER,WMU_FOCUS_NOTES,0L);
					}
					else viewnotes(hDlgStats,FALSE);
					updatestats(hDlgStats);
					return 0;

				case ID_STATS_KEEPFIND:
					ToggleMenu(hMenu,ID_STATS_KEEPFIND,&menu.bKeepFind);
					if (!menu.bKeepFind)
					{
						menu.iStatsMode=uPrevStats;
						updatestats(hDlgStats);
					}
					return 0;

				case ID_STATS_SEND_CLIPBOARD:
				case ID_STATS_SEND_FILE:
				case ID_STATS_SEND_PRINTER:
					if (menu.iStatsMode!=ID_STATS_NOTES)
						printstats(hDlgStats,LOWORD(wParam));
					else
					{
						SendMessage(hwnd,WM_COMMAND,uPrevStats,0L);
						updatestats(hDlgStats);
						printstats(hDlgStats,LOWORD(wParam));
					}
					return 0;

				case ID_TOOLS_TESTDRAW:
					testdraw(hDlgStats);
					return 0;

				case ID_TOOLS_SAVESTATES:
					ToggleMenu(hMenu,ID_TOOLS_SAVESTATES,&menu.bSaveStates);
					return 0;

				case ID_TOOLS_RESTOREWIN:
					hdc = CreateIC("DISPLAY",NULL,NULL,NULL);
					cxWindow = GetDeviceCaps(hdc,HORZRES);
					cyWindow = GetDeviceCaps(hdc,VERTRES);
					DeleteDC(hdc);
					xWindow = cxWindow/20;					// main window's left is 10% in
					yWindow = cyWindow/10;					// main window's top is 6.7% down

					rMainWin.left = cxWindow/10;
					rMainWin.right = cxWindow/2+cxWindow/10;
					rMainWin.top = cyWindow/10;
					rMainWin.bottom = cyWindow-cyWindow/10;

					rStatsWin.left = cxWindow/2+cxWindow/10;
					rStatsWin.right = cxWindow-(cxWindow/10);
					rStatsWin.top = rMainWin.top;
					rStatsWin.bottom = cyWindow/2;

					rInfoWin.left = cxWindow/2+cxWindow/10;
					rInfoWin.right = cxWindow-(cxWindow/10);
					rInfoWin.top = cyWindow/2;
					rInfoWin.bottom = rMainWin.bottom;

					MoveWindow(hwnd,rMainWin.left,rMainWin.top,rMainWin.right-rMainWin.left,rMainWin.bottom-rMainWin.top,TRUE);
					MoveWindow(hDlgStats,rStatsWin.left,rStatsWin.top,rStatsWin.right-rStatsWin.left,rStatsWin.bottom-rStatsWin.top,TRUE);
					MoveWindow(hwndInfo,rInfoWin.left,rInfoWin.top,rInfoWin.right-rInfoWin.left,rInfoWin.bottom-rInfoWin.top,TRUE);

					return 0;

				case ID_TOOLS_UPDATE:
					MessageBox(hwnd,"The data will update the next time this program is run.\r\n\
To update now, exit and run the DeckMaker again.",szAppName,MB_OK|MB_ICONEXCLAMATION);
					return 0;

				case ID_TOOLS_CHANGESET:
					iOrderMode=ORDERSETS;
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_ORDERDLG),hwnd,(DLGPROC)OrderDlgProc);
					if (iOrderMode==ORDERSETS) MessageBox(hwnd,"The set order will update the next time this program is run.\r\n\
To change it now, exit and run the DeckMaker again.",szAppName,MB_OK|MB_ICONEXCLAMATION);
					return 0;

				case ID_TOOLS_CHANGETYPE:
					iOrderMode=ORDERTYPE;
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_ORDERDLG),hwnd,(DLGPROC)OrderDlgProc);
					if (iOrderMode==ORDERTYPE) MessageBox(hwnd,"The type order will update the next time this program is run.\r\n\
To change it now, exit and run the DeckMaker again.",szAppName,MB_OK|MB_ICONEXCLAMATION);
					return 0;

				case ID_TOOLS_BRAINIAC:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_BRAINIACDLG),hwnd,(DLGPROC)BrainiacDlgProc);
					return 0;

				case ID_TOOLS_FILLRARITIES:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_MASSENTRYDLG),hwnd,(DLGPROC)MassEntryDlgProc);
					return 0;

				case ID_TOOLS_CREATENEED:
					if (bNeedSave&&IDCANCEL==
							AskAboutSave(hwnd,szTitleName))
						return 0;
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_WANTDLG),hwnd,(DLGPROC)WantDlgProc);
					if (bNeedSave)
					{
						szFileName[0]='\0';
						szTitleName[0]='\0';
						DoCaption(hwnd,szTitleName);
					}
					return 0;

				case ID_FILE_NEW_ALL:
				case ID_FILE_NEW_LIGHT:
				case ID_FILE_NEW_DARK:
					if (bNeedSave&&IDCANCEL==
							AskAboutSave(hwnd,szTitleName))
						return 0;
					szFileName[0]='\0';
					szTitleName[0]='\0';
					DoCaption(hwnd,szTitleName);
					bNeedSave=FALSE;
					ResetSWCCGData();
					SetWindowText(GetDlgItem(hDlgStats,IDC_STATSNOTES),"");
					menu.bExcludeLight=FALSE;
					menu.bExcludeDark=FALSE;
					switch (wParam)
					{
						case ID_FILE_NEW_LIGHT:
							ExcludeCList(ATT_SIDE,'d',FALSE);
							menu.bExcludeDark=TRUE;
							break;
						case ID_FILE_NEW_DARK:
							ExcludeCList(ATT_SIDE,'l',FALSE);
							menu.bExcludeLight=TRUE;
							break;
					}
					if (menu.bCombineWB) CombineWB();
					UpdateOnlyMenu(hwnd);
					UpdateRange(hwnd);
					newsort(szSort);
					updatestats(hDlgStats);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_FILE_OPENDECK:
					if (bNeedSave&&IDCANCEL==
							AskAboutSave(hwnd,szTitleName))
						return 0;
					if (SWFileOpenDlg(hwnd,szFileName,szTitleName))
					{
						hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
						ShowCursor(TRUE);								// make the pointer the "hourglass"
						UpdateWindow(hwnd);
						UpdateWindow(hwndInfo);
						UpdateWindow(hDlgStats);
						if (!SWFileRead(hwnd,szFileName))
						{
							OkMessage(hwnd,"Could not read file %s!",
								szTitleName);
							szFileName[0]='\0';
							szTitleName[0]='\0';
						}
						else
						{
							UpdateRange(hwnd);
							updatestats(hDlgStats);
							InvalidateRect(hwnd,NULL,TRUE);
						}

						ShowCursor(FALSE); 	// restore cursor to original shape
						SetCursor(hCursor);

					}
					DoCaption(hwnd,szTitleName);
					bNeedSave=FALSE;
					return 0;

				case ID_FILE_OPENBINDER:
					if (SWFileOpenDlg(hwnd,szFileName,szTitleName))
					{
						hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
						ShowCursor(TRUE);								// make the pointer the "hourglass"

						UpdateWindow(hwnd);
						UpdateWindow(hwndInfo);
						UpdateWindow(hDlgStats);
						if (!SWBinderRead(hwnd,szFileName))
						{
							OkMessage(hwnd,"Could not read file %s!",
								szTitleName);
						}
						else
						{
							menu.bBinderQty=TRUE;
							CheckMenuItem(GetMenu(hwnd),ID_VIEW_BINDER,MF_CHECKED);
							InvalidateRect(hwnd,NULL,TRUE);
						}

						ShowCursor(FALSE); 	// restore cursor to original shape
						SetCursor(hCursor);

					}
					return 0;


				case ID_FILE_SAVEDECK:
					if (szFileName[0])
					{
						if (SWFileWrite(hwnd,szFileName))
						{
							bNeedSave=FALSE;
							return 1;
						}
						else
							OkMessage(hwnd,"Could not write file %s!",
								szTitleName);
							return 0;
					}
				// fall through
				case ID_FILE_SAVEDECKAS:
					if (SWFileSaveDlg(hwnd,szFileName,szTitleName))
					{
						DoCaption(hwnd,szTitleName);
						if (SWFileWrite(hwnd,szFileName))
						{
							bNeedSave=FALSE;
							return 1;
						}
						else
							OkMessage(hwnd,"Could not write file %s!",
								szTitleName);
					}
					return 0;

				case ID_FILE_PRNT:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_PRINTDLG),hwnd,(DLGPROC)PrintDlgProc);
					return 0;

				case ID_FILE_EXIT:
					SendMessage(hwnd,WM_CLOSE,0,0);
					return 0;

				case ID_DATAFILES_EXPORT:
					ExportData();
					return 0;

				case ID_DATAFILES_IMPORT:
					ImportData();
					return 0;

				case ID_DATAFILES_CREATE:
					CreateOldData();
					return 0;

				case ID_VIEW_INCLUDE:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_INCLUDEDLG),hwnd,(DLGPROC)IncludeDlgProc);
					UpdateRange(hwnd);
					updatestats(hDlgStats);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;
					
				case ID_VIEW_EXCLUDE:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_EXCLUDEDLG),hwnd,(DLGPROC)ExcludeDlgProc);
					UpdateRange(hwnd);
					updatestats(hDlgStats);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_HELP_ABOUT:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_ABOUTDLG),hwnd,(DLGPROC)AboutDlgProc);
					return 0;
				case ID_HELP_ANHRULES:
					WinHelp(hwnd,"SWCCG DECKMAKER.hlp",HELP_CONTENTS,0);
					return 0;
				case ID_HELP_RULES:
					WinHelp(hwnd,"SWCCG RULES.hlp",HELP_CONTENTS,0);
					return 0;
			}
			break;

		case WM_CREATE:
			if (getsets(hwnd)==FALSE)
			{
				PostQuitMessage(0);
				return 0;
			}

//			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

			hdc = GetDC(hwnd);
			GetTextMetrics(hdc,&tm);
			cyChar = tm.tmHeight+tm.tmExternalLeading;
			if (cyChar<17) cyChar=17;
			if (GetDeviceCaps(hdc,NUMCOLORS)==-1) bHighColorMode=TRUE;
			else bHighColorMode=FALSE;
			if (bHighColorMode)
				hDarkBrush = CreateSolidBrush(RGB(232,232,232));
			else hDarkBrush = CreateSolidBrush(RGB(192,192,192));
			ReleaseDC(hwnd,hdc);

			cxVScroll = GetSystemMetrics(SM_CXVSCROLL);

			hMenu = GetMenu(hwnd);
			menu.bCompressed=TRUE;
			menu.bBold=TRUE;
			menu.bTypeIcons=TRUE;
			menu.bDestIcons=TRUE;
			menu.bRareIcons=TRUE;
			menu.bSetIcons=TRUE;
			menu.bExtraIcons=TRUE;
			menu.bColor=TRUE;
			menu.bDarkShaded=TRUE;
			menu.bSortSets=FALSE;
			menu.bSortSides=FALSE;
			menu.bUnlimited=FALSE;
			menu.bDeckQty=TRUE;
			menu.bDeckWindow=TRUE;
			menu.bBinderQty=FALSE;

			menu.bFindLore=TRUE;
			menu.bFindDesc=TRUE;
			menu.bFindName=FALSE;
			menu.bFindLight=TRUE;
			menu.bFindDark=TRUE;

			menu.bKeepFind=TRUE;
			menu.iStatsMode=ID_STATS_DETAIL;
			menu.iRangeMode=ID_RANGES_LMH;

			menu.bCombineWB=FALSE;

			menu.bExcludeLight=FALSE;
			menu.bExcludeDark=FALSE;

			uPrevStats=menu.iStatsMode;
			bPriceGuideChanged=FALSE;

			hCardMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDR_CARDMENU));
			hCardMenu = GetSubMenu(hCardMenu,0);

			sprintf(pHelpText,"");
			sprintf(szCustomSort,"YZR2V");
			sprintf(szSort,"");

			uSortOrder = ID_SORT_BYNAME;

			iBuf = 0;
			iSel = 0;
			iSelPos = 0;

			bPopUp = FALSE;

			printopt.uDestination = IDC_RADIO_DESTCLIP;
			printopt.bCardQty = TRUE;
			printopt.bNotes = FALSE;
			printopt.bSummary = FALSE;
			printopt.bDate = TRUE;
			printopt.bFSet = FALSE;
			printopt.bFSide = FALSE;
			printopt.bFRarity = FALSE;
			printopt.bFType = FALSE;
			printopt.bSSet = FALSE;
			printopt.bSSide = FALSE;
			printopt.bSRarity = FALSE;
			printopt.bSType = FALSE;

			menu.bSaveStates = TRUE;

			GetMenuProfile(hMenu); // overwrite any of the above
								   // if profile is different

			GetPrivateProfileString("Sort","TypeOrder","LCRVSIEWDNJ",szTypeOrder,31,szIni);

			SWPrintFileInitialize(hwnd);
			SWFileInitialize(hwnd);
			sprintf(szFileName,"\0");

//			strcpy(szFileName,(PSTR)
//				(((LPCREATESTRUCT)lParam)->lpCreateParams));
//			if (strlen(szFileName)>0)
//			{
//				GetFileTitle(szFileName,szTitleName,
//					sizeof(szTitlename));
			DoCaption(hwnd,szTitleName);

			SendMessage(hwnd,WM_COMMAND,uSortOrder,0);
			if (menu.bCombineWB) CombineWB();
			updateinfo(hwnd);
//			updatestats(hDlgStats);
			return 0;

		case WM_MOUSEMOVE:	// if user clicks and holds the LBUTTON during a move,
			if (!(wParam & MK_LBUTTON))	// probably going to new iSelected.
				return 0;				// ignore(return) if LBUTTON not down
			if (iSel==(iVPos+HIWORD(lParam)/cyChar-1))
				return 0;				// also ignore if no change in iSel

		// ** FALL THROUGH TO NEXT CASE ** if LBUTTON is down during move

		case WM_LBUTTONDOWN:
			if (iSel==(iVPos+HIWORD(lParam)/cyChar-1))
			{
				if (LOWORD(lParam)>iNdent)
				{
					cardlist[iSel].deck=min(cardlist[iSel].deck+1,99999);
					if (menu.bCombineWB) cardlist[iSel].wb=min(cardlist[iSel].wb+1,99999);
					UpdateSelQty(hwnd);
					updatestats(hDlgStats);
				}
				else
				{
					cardlist[iSel].deck=max(0,cardlist[iSel].deck-1);
					if (menu.bCombineWB) cardlist[iSel].wb=max(0,cardlist[iSel].wb-1);
					UpdateSelQty(hwnd);
					updatestats(hDlgStats);
				}
			}
			else
			{
				UpdateSel(hwnd);
				iSelPos=iVPos;
				iSel = min(iCListSize-1,max(0,iVPos+HIWORD(lParam)/cyChar-1));
				UpdateSel(hwnd);
				updateinfo(hwnd);
			}
			return 0;


		case WM_RBUTTONDOWN:

			if (bPopUp) return 0;

			if (iSel!=(iVPos+HIWORD(lParam)/cyChar-1))
			{
				UpdateSel(hwnd);
				iSelPos=iVPos;
				iSel = min(iCListSize-1,max(0,iVPos+HIWORD(lParam)/cyChar-1));
				UpdateSel(hwnd);
				updateinfo(hwnd);
			}

			if ((iSel<0)||(iSel>iCListSize)) return 0;
	
			point.x=LOWORD(lParam);
			point.y=HIWORD(lParam);
			ClientToScreen(hwnd,&point);
			bPopUp=TRUE;
			TrackPopupMenu(hCardMenu,0,point.x,point.y,0,hwnd,NULL);
			bPopUp=FALSE;
			return 0;


		case WM_SIZE:

			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			GetWindowRect(hwnd,&rMainWin);
			UpdateRange(hwnd);

			return 0;

		case WM_MOVE:
			GetWindowRect(hwnd,&rMainWin);
			return 0;

		case WM_VSCROLL:

			switch (LOWORD(wParam))
			{
				case SB_TOP: iVInc = -iVPos; break;
				case SB_BOTTOM:	iVInc = iVMax-iVPos; break;
				case SB_LINEUP: iVInc = -1;	break;
				case SB_LINEDOWN: iVInc = 1; break;
				case SB_PAGEUP: iVInc = min(-1,-cyClient/cyChar); break;
				case SB_PAGEDOWN: iVInc = max(1,cyClient/cyChar); break;
				case SB_THUMBTRACK: iVInc=HIWORD(wParam)-iVPos; break;
				default: iVInc = 0;
			}

			iVInc = max(-iVPos,min(iVInc,iVMax-iVPos));

			if (iVInc!=0)
			{
				iVPos += iVInc;
				ScrollWindow(hwnd,0,-cyChar*iVInc,NULL,NULL);
				SetScrollPos(hwnd,SB_VERT,iVPos,TRUE);
				UpdateWindow(hwnd);
			}

			//sprintf(szBuf,"iVPos:%i iVMax:%i iVInc:%i",iVPos,iVMax,iVInc);
			//SetWindowText(hwnd,szBuf);

			return 0;

		case WM_KEYDOWN:
			bNeedUpdate=FALSE;
			if (iSelPos!=iVPos)
			{
				iVPos=iSelPos;
				InvalidateRect(hwnd,NULL,TRUE);
				UpdateWindow(hwnd);
			}

			switch (wParam)
			{
				case VK_HOME:
					UpdateSel(hwnd);
					iSel = 0;
					iSelPos = 0;
					SendMessage(hwnd,WM_VSCROLL,SB_TOP,0L);
					UpdateSel(hwnd);
					bNeedUpdate=TRUE;
					break;

				case VK_END:
					UpdateSel(hwnd);
					iSel=max(0,iCListSize-1);
					iSelPos=iVMax;
					SendMessage(hwnd,WM_VSCROLL,SB_BOTTOM,0L);
					UpdateSel(hwnd);
					bNeedUpdate=TRUE;
					break;

				case VK_DOWN:
					UpdateSel(hwnd);
					iSel = min(iCListSize-1,iSel+1);
					if (iSel>(iVPos+cyClient/cyChar-2))
					{
						SendMessage(hwnd,WM_VSCROLL,SB_LINEDOWN,0L);
						iSelPos=min(iSelPos+1,iVMax);
					}
					UpdateSel(hwnd);
					bNeedUpdate=TRUE;
					break;

				case VK_UP:
					UpdateSel(hwnd);
					iSel=max(0,iSel-1);
					if (iSel<iVPos)
					{
						SendMessage(hwnd,WM_VSCROLL,SB_LINEUP,0L);
						iSelPos=max(iSelPos-1,0);
					}
					UpdateSel(hwnd);
					bNeedUpdate=TRUE;
					break;

				case VK_NEXT:
					UpdateSel(hwnd);
					iSel=min(iSel+cyClient/cyChar,iCListSize-1);
					SendMessage(hwnd,WM_VSCROLL,SB_PAGEDOWN,0L);
					iSelPos=min(iSelPos+cyClient/cyChar,iVMax);
					UpdateSel(hwnd);
					bNeedUpdate=TRUE;
					break;

				case VK_PRIOR:
					UpdateSel(hwnd);
					iSel=max(0,iSel-cyClient/cyChar);
					SendMessage(hwnd,WM_VSCROLL,SB_PAGEUP,0L);
					iSelPos=max(0,iSelPos-cyClient/cyChar);
					UpdateSel(hwnd);
					bNeedUpdate=TRUE;
					break;

				case VK_INSERT:
				case VK_RIGHT:
					cardlist[iSel].deck=min(cardlist[iSel].deck+1,99999);
					if (menu.bCombineWB) cardlist[iSel].wb = min(cardlist[iSel].wb+1,99999);
					UpdateSelQty(hwnd);
					updatestats(hDlgStats);
					bNeedUpdate=TRUE;
					break;

				case VK_DELETE:
				case VK_LEFT:
					cardlist[iSel].deck=max(0,cardlist[iSel].deck-1);
					if (menu.bCombineWB) cardlist[iSel].wb = max(0,cardlist[iSel].wb-1);
					UpdateSelQty(hwnd);
					updatestats(hDlgStats);
					bNeedUpdate=TRUE;
					break;

			}
			if (bNeedUpdate) updateinfo(hwnd);
			return 0;

		case WM_CHAR:
			bNeedUpdate = FALSE;
			if (iCListSize<=0) return 0;
			for (i=0;i<(int)LOWORD(lParam);i++)
			{
				switch (wParam)
				{
					case '\r':
						cardlist[iSel].deck = min(cardlist[iSel].deck+1,99999);
						if (menu.bCombineWB) cardlist[iSel].wb = min(cardlist[iSel].wb+1,99999);
						UpdateSelQty(hwnd);
						updatestats(hDlgStats);
						break;

					case '\b':
						cardlist[iSel].deck = max(0,cardlist[iSel].deck-1);
						if (menu.bCombineWB) cardlist[iSel].wb = max(0,cardlist[iSel].wb-1);
						UpdateSelQty(hwnd);
						updatestats(hDlgStats);
						break;

					case '/':
						SendMessage(hwnd,WM_COMMAND,ID_NAV_JUMP,0L);
						return 0;

					case '0':
						if ((iSel<0)||(iSel>iCListSize)) return 0;
	
						point.x=iNdent+20;
						point.y=(iSel-iVPos+2)*cyChar;
						ClientToScreen(hwnd,&point);
						bPopUp=TRUE;
						TrackPopupMenu(hCardMenu,0,point.x,point.y,0,hwnd,NULL);
						bPopUp=FALSE;
						break;

					default:
						for (k=1,j=(iSel+1)%iCListSize;k&&(j!=iSel);j=(j+1)%iCListSize)
						{
							if (((char)wParam>='a')&&((char)wParam<='z')) wParam -= 'a'-'A';
							if (masterlist[cardlist[j].idx].name[0]==(char)wParam)
							{
								UpdateSel(hwnd);
								iSel = j;
								UpdateSel(hwnd);
								k=0;
								iSelPos = max(0,min(iSel-2,iVMax));
								iVPos = iSelPos;
								SendMessage(hwnd,WM_VSCROLL,MAKEWORD(SB_THUMBTRACK,
													iSelPos),0);
								InvalidateRect(hwnd,NULL,TRUE);
								bNeedUpdate=TRUE;
							//	UpdateWindow(hwnd);
							}
						}
						break;
						
					
				}
			}
			if (bNeedUpdate) updateinfo(hwnd);
			return 0;		

		case WM_USER:
			j = wParam;
			if (j!=iSel)
			{
				UpdateSel(hwnd);
				iSel = j;
				UpdateSel(hwnd);
				k=0;
				iSelPos = max(0,min(iSel-2,iVMax));
				iVPos = iSelPos;
				SendMessage(hwnd,WM_VSCROLL,MAKEWORD(SB_THUMBTRACK,
									iSelPos),0);
				InvalidateRect(hwnd,NULL,TRUE);
				updateinfo(hwnd);
			}
			return 0;
			
		case WM_PAINT:
			hdc = BeginPaint(hwnd,&ps);

			if (!menu.bCompressed&&!menu.bBold)
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,100,0,TRUE));
			else if (menu.bCompressed&&!menu.bBold)
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,90,0,TRUE));
			else if (!menu.bCompressed&&menu.bBold)
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,100,EZ_ATTR_BOLD,TRUE));
			else
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,90,EZ_ATTR_BOLD,TRUE));

			SetBkMode(hdc,TRANSPARENT);

			iPaintBeg=max(0,iVPos+ps.rcPaint.top/cyChar-1);
			iPaintEnd=min(iCListSize,iVPos+ps.rcPaint.bottom/cyChar);

			for (i=iPaintBeg;i<iPaintEnd;i++)
			{

				cy = max(0,(1-iVPos+i))*cyChar;
				cx = 0;

				if (menu.bExtraIcons)
				{
					if (masterlist[cardlist[i].idx].icons[CEXTRA3])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CEXTRA3]],cx,cy);
					cx += 17;
					if (masterlist[cardlist[i].idx].icons[CEXTRA2])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CEXTRA2]],cx,cy);
					cx += 17;
					if (masterlist[cardlist[i].idx].icons[CEXTRA1])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CEXTRA1]],cx,cy);
					cx += 17;
				}
				if (menu.bSetIcons)
				{
					if (masterlist[cardlist[i].idx].icons[CSET])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CSET]],cx,cy);
					cx += 17;
				}
				if (menu.bRareIcons)
				{
					if (masterlist[cardlist[i].idx].icons[CRARITY])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CRARITY]],cx,cy);
					cx += 17;
				}
				if (menu.bDestIcons)
				{
					if (masterlist[cardlist[i].idx].icons[CDEST])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CDEST]],cx,cy);
					cx += 17;
					if (masterlist[cardlist[i].idx].icons[CDEST2])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CDEST2]],cx,cy);
					cx += 17;
				}
				if (menu.bTypeIcons)
				{
					if (masterlist[cardlist[i].idx].icons[CTYPE])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[CTYPE]],cx,cy);
					cx += 17;
				}

				iNdent = cx;

				if (menu.bBinderQty==TRUE)
					if (cardlist[i].binder>0)
					{
						SetTextColor(hdc,RGB_DKGREY);
						sprintf(szBuf,"%i",cardlist[i].binder);
						SetTextAlign(hdc,TA_RIGHT);
						iNdent+=NUMSPACE-4;
						TextOut(hdc,iNdent,cy,szBuf,strlen(szBuf));
						SetTextAlign(hdc,TA_LEFT);
						iNdent+=4;
					} else iNdent+=NUMSPACE;


				if (menu.bDeckQty==TRUE)
					if (cardlist[i].deck>0)
					{
						SetTextColor(hdc,RGB_BLACK);
						sprintf(szBuf,"%i",cardlist[i].deck);
						SetTextAlign(hdc,TA_RIGHT);
						iNdent+=NUMSPACE-4;
						TextOut(hdc,iNdent,cy,szBuf,strlen(szBuf));
						SetTextAlign(hdc,TA_LEFT);
						iNdent+=4;
					} else iNdent+=NUMSPACE;

				if ((masterlist[cardlist[i].idx].attr[ATT_SIDE]=='d')&&menu.bDarkShaded&&(iSel!=i))
				{
					rect.left=iNdent; rect.right=cxClient;
					rect.top=cy; rect.bottom=rect.top+cyChar-1;
					FillRect(hdc,&rect,hDarkBrush);
				}

				if (iSel!=i)
				{
					if (menu.bColor)
					{
						switch (masterlist[cardlist[i].idx].attr[ATT_TYPE])
						{
							case 'E': case 'I':	SetTextColor(hdc,RGB_RED); break;
							case 'C': SetTextColor(hdc,RGB_BLACK); break;
							case 'L': SetTextColor(hdc,RGB_GREY); break;
							case 'S': case 'V':	SetTextColor(hdc,RGB_BLUE);	break;
							case 'D': case 'W':	SetTextColor(hdc,RGB_GREEN); break;
							case 'N': case 'O': SetTextColor(hdc,RGB_PURPLE);	break;
							case 'J': SetTextColor(hdc,RGB_DKGREEN); break;
							default: SetTextColor(hdc,RGB_DKGREY); break;
						}
					} else SetTextColor(hdc,RGB_BLACK);
				}
				else
				{
					if (menu.bColor)
					{
						switch (masterlist[cardlist[i].idx].attr[ATT_TYPE])
						{
							case 'E': case 'I': hBrush=hBr[BR_RED]; break;
							case 'C': hBrush=hBr[BR_BLACK]; break;
							case 'L': hBrush=hBr[BR_GREY]; break;
							case 'S': case 'V': hBrush=hBr[BR_BLUE]; break;
							case 'D': case 'W': hBrush=hBr[BR_GREEN]; break;
							case 'N': case 'O': hBrush=hBr[BR_PURPLE]; break;
							case 'J': hBrush=hBr[BR_DKGREEN]; break;
							default:  hBrush=hBr[BR_DKGREY]; break;
						}
					} else hBrush=hBr[BR_BLACK];
					SetTextColor(hdc,RGB_WHITE);
					rect.left=iNdent; rect.right=cxClient;
					rect.top=cy; rect.bottom=rect.top+cyChar-1;
					FillRect(hdc,&rect,hBrush);

				}

				TextOut(hdc,iNdent+4,cy,masterlist[cardlist[i].idx].name,strlen(masterlist[cardlist[i].idx].name));
				SetTextColor(hdc,RGB_BLACK);

			}

			DeleteObject(SelectObject(hdc,GetStockObject(SYSTEM_FONT)));
			EndPaint(hwnd,&ps);
			return 0;

		case WM_CLOSE:
			if (!bNeedSave||IDCANCEL!=AskAboutSave(hwnd,szTitleName))
				DestroyWindow(hwnd);
		    		GetWindowRect(hwnd,&rect);
    		sprintf(szBuf,"%i",rMainWin.left);
			if (menu.bSaveStates)
			{
	    		sprintf(szBuf,"%i",rMainWin.left);
	    		WritePrivateProfileString("WindowPositions","Main_Left",szBuf,szIni);
				sprintf(szBuf,"%i",rMainWin.right);
    			WritePrivateProfileString("WindowPositions","Main_Right",szBuf,szIni);
    			sprintf(szBuf,"%i",rMainWin.top);
    			WritePrivateProfileString("WindowPositions","Main_Top",szBuf,szIni);
    			sprintf(szBuf,"%i",rMainWin.bottom);
    			WritePrivateProfileString("WindowPositions","Main_Bottom",szBuf,szIni);
				sprintf(szBuf,"%i",rInfoWin.left);
    			WritePrivateProfileString("WindowPositions","Info_Left",szBuf,szIni);
				sprintf(szBuf,"%i",rInfoWin.right);
    			WritePrivateProfileString("WindowPositions","Info_Right",szBuf,szIni);
				sprintf(szBuf,"%i",rInfoWin.top);
    			WritePrivateProfileString("WindowPositions","Info_Top",szBuf,szIni);
				sprintf(szBuf,"%i",rInfoWin.bottom);
    			WritePrivateProfileString("WindowPositions","Info_Bottom",szBuf,szIni);
				sprintf(szBuf,"%i",rStatsWin.left);
    			WritePrivateProfileString("WindowPositions","Stats_Left",szBuf,szIni);
				sprintf(szBuf,"%i",rStatsWin.right);
    			WritePrivateProfileString("WindowPositions","Stats_Right",szBuf,szIni);
				sprintf(szBuf,"%i",rStatsWin.top);
    			WritePrivateProfileString("WindowPositions","Stats_Top",szBuf,szIni);
				sprintf(szBuf,"%i",rStatsWin.bottom);
    			WritePrivateProfileString("WindowPositions","Stats_Bottom",szBuf,szIni);
				WriteMenuProfile();
			}
			else WritePrivateProfileString("Tools","SaveStates","0",szIni);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;


	}

	return DefWindowProc(hwnd,iMsg,wParam,lParam);

}

BOOL CALLBACK AboutDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDOK)
			{
				EndDialog(hDlg,0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK ExcludeDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static HWND	hList;
	int		i;
	int		nSel,aSel[(6+MAXSETS)*sizeof(int)];
	BOOL	bLeave;
	
	switch (iMsg)
	{
		case WM_INITDIALOG:
			hList = GetDlgItem(hDlg,IDC_EXCLUDELIST);
			//SendMessage(GetDlgItem(hDlg,IDC_LEAVE_USED),BM_SETCHECK,BST_CHECKED,0);
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)0,(LPARAM)"Dark Side cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)1,(LPARAM)"Light Side cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)2,(LPARAM)"Rare cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)3,(LPARAM)"Uncommon cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)4,(LPARAM)"Common cards");
			for (i=0;i<MAXSETS;i++)
				if (sets[i][0]!='\0')
					SendMessage(hList,LB_INSERTSTRING,5+i,(LPARAM)(sets[i]+1));
			return TRUE;

		case WM_COMMAND:
			if (HIWORD(wParam)==LBN_DBLCLK) wParam = IDOK;
			switch (LOWORD(wParam))
			{
				case IDOK:
					iBuf = 0;
					nSel = SendMessage(hList,LB_GETSELCOUNT,0,0);
					if (nSel>=1)
					{
						if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_LEAVE_USED),BM_GETCHECK,0,0))
							bLeave=TRUE; else bLeave=FALSE;
						SendMessage(hList,LB_GETSELITEMS,(WPARAM)(4+MAXSETS),(LPARAM)(LPINT)aSel);
						for (i=0;i<nSel;i++)
						{
							switch (aSel[i])
							{
								case 0: iBuf+=ExcludeCList(ATT_SIDE,'d',bLeave); break;
								case 1:	iBuf+=ExcludeCList(ATT_SIDE,'l',bLeave); break;
								case 2: iBuf+=ExcludeCList(ATT_RARITY,'R',bLeave); break;
								case 3: iBuf+=ExcludeCList(ATT_RARITY,'U',bLeave); break;
								case 4: iBuf+=ExcludeCList(ATT_RARITY,'C',bLeave); break;
								default:
									if ((aSel[i]>4)&&(aSel[i]<(5+MAXSETS)))
										iBuf+=ExcludeCList(ATT_SET,sets[aSel[i]-5][0],bLeave);
									break;
							}
						}
					}
					EndDialog(hDlg,0);
					return TRUE;

				case IDCANCEL:
					iBuf = 0;
					EndDialog(hDlg,0);
					return TRUE;

				case IDHELP:
					sprintf(pHelpText,HELP_EXCLUDE);
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,(DLGPROC)HelpDlgProc);
					return TRUE;
			}
			break;
	}
	return FALSE;
}

// this dialog is so similar to ExcludeDlgProc that the template was
// copied, so it refers to EXCLUDE id's
BOOL CALLBACK IncludeDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static HWND	hList;
	int		i;
	int		nSel,aSel[(6+MAXSETS)*sizeof(int)];
	
	switch (iMsg)
	{
		case WM_INITDIALOG:
			hList = GetDlgItem(hDlg,IDC_EXCLUDELIST);
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)0,(LPARAM)"Dark Side cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)1,(LPARAM)"Light Side cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)2,(LPARAM)"Rare cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)3,(LPARAM)"Uncommon cards");
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)4,(LPARAM)"Common cards");
			for (i=0;i<MAXSETS;i++)
				if (sets[i][0]!='\0')
					SendMessage(hList,LB_INSERTSTRING,5+i,(LPARAM)(sets[i]+1));
			return TRUE;

		case WM_COMMAND:
			if (HIWORD(wParam)==LBN_DBLCLK) wParam = IDOK;
			switch (LOWORD(wParam))
			{
				case IDOK:
					iBuf = 0;
					nSel = SendMessage(hList,LB_GETSELCOUNT,0,0);
					if (nSel>=1)
					{
						SendMessage(hList,LB_GETSELITEMS,(WPARAM)(4+MAXSETS),(LPARAM)(LPINT)aSel);
						for (i=0;i<nSel;i++)
						{
							switch (aSel[i])
							{
								case 0: iBuf+=IncludeCList(ATT_SIDE,'d'); break;
								case 1:	iBuf+=IncludeCList(ATT_SIDE,'l'); break;
								case 2: iBuf+=IncludeCList(ATT_RARITY,'R'); break;
								case 3: iBuf+=IncludeCList(ATT_RARITY,'U'); break;
								case 4: iBuf+=IncludeCList(ATT_RARITY,'C'); break;
								default:
									if ((aSel[i]>4)&&(aSel[i]<(5+MAXSETS)))
										iBuf+=IncludeCList(ATT_SET,sets[aSel[i]-5][0]);
									break;
							}
						}
					}
					EndDialog(hDlg,0);
					return TRUE;

				case IDCANCEL:
					iBuf = 0;
					EndDialog(hDlg,0);
					return TRUE;

				case IDHELP:
					sprintf(pHelpText,HELP_INCLUDE);
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,(DLGPROC)HelpDlgProc);
					return TRUE;
			}
			break;
	}
	return FALSE;
}


BOOL CALLBACK HelpDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
			if (pHelpText[0]) SetDlgItemText(hDlg,IDC_HELPEDIT,pHelpText);
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDOK)
			{
				EndDialog(hDlg,0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK EditDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	FILE	*fh;
	int		i;
	char	szBuf[MAXDESC];

	switch (iMsg)
	{
		case WM_INITDIALOG:
			SetDlgItemText(hDlg,IDC_EDITATTRIBUTES,masterlist[iBuf].attr);
			SetDlgItemText(hDlg,IDC_EDITNAME,masterlist[iBuf].name);
			SetDlgItemText(hDlg,IDC_EDITLORE,masterlist[iBuf].lore);
			SetDlgItemText(hDlg,IDC_EDITGAMETEXT,masterlist[iBuf].desc);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDCANCEL:
					iBuf=-1;
					EndDialog(hDlg,0);
					return TRUE;

				case IDHELP:
					sprintf(pHelpText,HELP_EDITCARD);
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,(DLGPROC)HelpDlgProc);
					return TRUE;

				case IDOK:
					fh=fopen(DATAFILEN,"wb");
					for (i=0;i<iMListSize;i++)
					{
						if (i!=iBuf)
						{
							fprintf(fh,"%s",masterlist[i].attr);
							fputc(0,fh);
							fprintf(fh,"%s",masterlist[i].name);
							fputc(0,fh);
							fprintf(fh,"%s",masterlist[i].desc);
							fputc(0,fh);
						}
						else
						{
							GetDlgItemText(hDlg,IDC_EDITATTRIBUTES,szBuf,MAXATTR);
							fprintf(fh,"%s",szBuf);
							fputc(0,fh);
							GetDlgItemText(hDlg,IDC_EDITNAME,szBuf,MAXNAME);
							fprintf(fh,"%s",szBuf);
							fputc(0,fh);
							GetDlgItemText(hDlg,IDC_EDITGAMETEXT,szBuf,MAXDESC);
							fprintf(fh,"%s",szBuf);
							fputc(0,fh);
						}
					}
					fclose(fh);
					
					EndDialog(hDlg,0);
					return TRUE;
					
			}
			break;

	}
	return FALSE;
}

BOOL CALLBACK SplashDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
			return TRUE;
	}
	return FALSE;
}

void convertsort(int ch,UINT *u,char *label)
{
	switch (ch)
	{
		case 'V': *u=IDC_SORTSIDE; sprintf(label,"Value"); break;
		case '2': *u=IDC_SORTSET; sprintf(label,"Subrarity"); break;
		case 'R': *u=IDC_SORTRARITY; sprintf(label,"Rarity"); break;
		case 'D': *u=IDC_SORTDESTINY; sprintf(label,"Destiny"); break;
		case 'Y': *u=IDC_SORTTYPE; sprintf(label,"Type"); break;
		case 'Z': *u=IDC_SORTSUBTYPE; sprintf(label,"Subtype"); break;
		case 'P': *u=IDC_SORTPOWER; sprintf(label,"Power"); break;
		case 'A': *u=IDC_SORTABILITY; sprintf(label,"Ability"); break;
		case 'L': *u=IDC_SORTDEPLOY; sprintf(label,"Deploy"); break;
		case 'F': *u=IDC_SORTFORFEIT; sprintf(label,"Forfeit"); break;
		default: *u=0; sprintf(label,""); break;
	}
}

int convertsortcode(UINT u)
{
	int ch;

	switch (u)
	{
		case IDC_SORTSIDE:		ch='V'; break;
		case IDC_SORTSET:		ch='2'; break;
		case IDC_SORTRARITY:	ch='R'; break;
		case IDC_SORTDESTINY:	ch='D'; break;
		case IDC_SORTTYPE:		ch='Y'; break;
		case IDC_SORTSUBTYPE:	ch='Z'; break;
		case IDC_SORTPOWER:		ch='P'; break;
		case IDC_SORTABILITY:	ch='A'; break;
		case IDC_SORTDEPLOY:	ch='L'; break;
		case IDC_SORTFORFEIT:	ch='F'; break;
		default: ch='0'; break;
	}
	return ch;
}

BOOL CALLBACK SortDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static HWND		hList;
	static char		szTmpSort[MAXSORT];
	int				i;
	static int		iLevels;
	char			szBuf[16];
	UINT			uint;
	
	switch (iMsg)
	{
		case WM_INITDIALOG:
			hList = GetDlgItem(hDlg,IDC_SORTLIST);
			sprintf(szTmpSort,"%s",szCustomSort);
			for (i=0;i<(int)strlen(szCustomSort);i++)
			{
				convertsort(szTmpSort[i],&uint,szBuf);
				if (uint)
				{
					SendMessage(hList,LB_INSERTSTRING,i,(LPARAM)szBuf);
					EnableWindow(GetDlgItem(hDlg,uint),FALSE);
				}
				else szTmpSort[i]='\0';
			}
			iLevels=i;
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{

				case IDC_SORTCLEAR:
					SendMessage(hList,LB_RESETCONTENT,0,0);
					for (i=0;i<iLevels;i++)
					{
						convertsort(szTmpSort[i],&uint,szBuf);
						EnableWindow(GetDlgItem(hDlg,uint),TRUE);
					}
					iLevels=0;
					sprintf(szTmpSort,"");
					return TRUE;

				case IDC_SORTBACK:
					if (iLevels>0)
					{
						SendMessage(hList,LB_DELETESTRING,iLevels-1,0);
						iLevels--;
						convertsort(szTmpSort[iLevels],&uint,szBuf);
						EnableWindow(GetDlgItem(hDlg,uint),TRUE);
						szTmpSort[iLevels]='\0';
					}
					return TRUE;

				case IDC_SORTSIDE:				case IDC_SORTSET:
				case IDC_SORTRARITY:			case IDC_SORTDESTINY:
				case IDC_SORTTYPE:				case IDC_SORTSUBTYPE:
				case IDC_SORTPOWER:				case IDC_SORTABILITY:
				case IDC_SORTDEPLOY:			case IDC_SORTFORFEIT:
					i=convertsortcode(LOWORD(wParam));
					convertsort(i,&uint,szBuf);
					if (uint)
					{
						szTmpSort[iLevels]=i;
						SendMessage(hList,LB_INSERTSTRING,iLevels,(LPARAM)szBuf);
						iLevels++;
						EnableWindow(GetDlgItem(hDlg,uint),FALSE);
					}
					return TRUE;

				case IDCANCEL:
					iBuf=0;
					EndDialog(hDlg,0);
					return TRUE;

				case IDOK:
					szTmpSort[iLevels]='\0';
					sprintf(szCustomSort,"%s",szTmpSort);
					iBuf=1;
					EndDialog(hDlg,0);
					return TRUE;

				case IDHELP:
					sprintf(pHelpText,HELP_SORT);
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,(DLGPROC)HelpDlgProc);
					return TRUE;

			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK DeckDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	RECT	rect;
	int		cxDlg,cyDlg;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			MoveWindow(hDlg,rDeckWin.left,rDeckWin.top,
						rDeckWin.right-rDeckWin.left,
						rDeckWin.bottom-rDeckWin.top,TRUE);
			return TRUE;

		case WM_SIZE:
			GetClientRect(hDlg,&rect);
			cxDlg=rect.right-rect.left;
			cyDlg=rect.bottom-rect.top;
			MoveWindow(GetDlgItem(hDlg,IDC_DECKCARDS),0,0,cxDlg,cyChar,TRUE);
			MoveWindow(GetDlgItem(hDlg,IDC_DECKLIST),0,cyChar,cxDlg,cyDlg-cyChar,TRUE);
			rect.bottom=cyChar;
			InvalidateRect(hDlg,&rect,TRUE);
			GetWindowRect(hDlg,&rDeckWin);
			break;

		case WM_MOVE:
			GetWindowRect(hDlg,&rDeckWin);
			break;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			menu.bDeckWindow=FALSE;
			break;

	}
	return FALSE;
}

// converts '0' to '9' as 0-9, everything else as 10
int digiticon(int ch)
{
	if ((ch>='0')&&(ch<='9')) ch-='0';
	else ch=10;
	return ch;
}


LRESULT CALLBACK InfoWndProc(HWND hwnd, UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static HWND		hLore,hGT;
	static int		cxIClient,cyIClient;
	static int		cx;
	static RECT		rLore,rGT;
	HDC				hdc;
	char			szBuf[80];
	static char		szName[80];
	PAINTSTRUCT		ps;
	int				i,j,k;
	SIZE			size;
	static HFONT	hInfoFont,hBackFont;
	
	switch (iMsg)
	{
		case WM_CREATE:
			hdc=GetDC(hwnd);
			if (!menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,0,TRUE);
			else if (menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,90,0,TRUE);
			else if (!menu.bCompressed&&menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,EZ_ATTR_BOLD,TRUE);
			else
				hCardFont=EzCreateFont(hdc,"Arial",100,90,EZ_ATTR_BOLD,TRUE);

			//hCardFont=EzCreateFont(hdc,"Arial",90,90,EZ_ATTR_BOLD,TRUE);
			ReleaseDC(hwnd,hdc);
			hLore=CreateWindow("edit",NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|
								WS_EX_CLIENTEDGE|WS_BORDER|ES_LEFT|ES_MULTILINE, //|ES_READONLY,
								0,0,0,0,hwnd,(HMENU)1,hInst,NULL);
			hGT=CreateWindow("edit",NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|
								WS_EX_CLIENTEDGE|WS_BORDER|ES_LEFT|ES_MULTILINE, //|ES_READONLY,
								0,0,0,0,hwnd,(HMENU)1,hInst,NULL);
			SendMessage(hLore,WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			SendMessage(hGT,WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			cx=cyChar+4;
			sprintf(szName,"I Thought They Smelled Bad On The Outside");
			return 0;

		case WM_USER:
			hdc=GetDC(hwnd);
			if (!menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,0,TRUE);
			else if (menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,90,0,TRUE);
			else if (!menu.bCompressed&&menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,EZ_ATTR_BOLD,TRUE);
			else
				hCardFont=EzCreateFont(hdc,"Arial",100,90,EZ_ATTR_BOLD,TRUE);

			ReleaseDC(hwnd,hdc);
			SendMessage(hLore,WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			SendMessage(hGT,WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			return 0;

		case WM_SIZE:
			cxIClient=LOWORD(lParam);
			cyIClient=HIWORD(lParam);

			rLore.left=cx;
			rLore.right=cxIClient-cx;
			rLore.top=cyChar+2;
			rLore.bottom=cyIClient/2-cyChar*2;

			rGT.left=rLore.left;
			rGT.right=rLore.right;
			rGT.top=cyIClient/2;
			rGT.bottom=cyIClient-cyChar;

			MoveWindow(hLore,rLore.left,rLore.top,rWidth(rLore),rHeight(rLore),TRUE);
			MoveWindow(hGT,rGT.left,rGT.top,rWidth(rGT),rHeight(rGT),TRUE);

			GetWindowRect(hwnd,&rInfoWin);
			return 0;

		case WM_MOVE:
			GetWindowRect(hwnd,&rInfoWin);
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hwnd,&ps);
			//SelectObject(hdc,hBackFont);

			if (!menu.bCompressed&&!menu.bBold)
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,100,0,TRUE));
			else if (menu.bCompressed&&!menu.bBold)
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,90,0,TRUE));
			else if (!menu.bCompressed&&menu.bBold)
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,100,EZ_ATTR_BOLD,TRUE));
			else
				SelectObject(hdc,EzCreateFont(hdc,"Arial",100,90,EZ_ATTR_BOLD,TRUE));

			parsename(szBuf,masterlist[cardlist[iSel].idx].name);

			if (masterlist[cardlist[iSel].idx].icons[CDEST2] != ICO_NONE) {
				j=rLore.right-rLore.left-16;
			} else {
				j=rLore.right-rLore.left;
			}
			if (strlen(szBuf)>3)
				do {
					GetTextExtentPoint32(hdc,szBuf,strlen(szBuf),&size);
					if (size.cx>j)
					{
						k=0;
						for (i=strlen(szBuf)-1;i&&!k;i--)
						{	
							if (szBuf[i]==' ')
							{
								szBuf[i]='.';
								szBuf[i+1]='.';
								szBuf[i+2]='.';
								szBuf[i+3]='\0';
								k=1;
							}
						}
						if (!k) j=size.cx+1;
					}
				} while (size.cx>j);


			TextOut(hdc,rLore.left,0,szBuf,strlen(szBuf));
			//(hdc,szBuf,strlen(szBuf),&rect,DT_VCENTER|DT_SINGLELINE);

			if (masterlist[cardlist[iSel].idx].attr[ATT_SIDE]=='L')
				DrawBackBitmap(hdc,hIcons[ICO_LSBACK],2,46);
			else
				DrawBackBitmap(hdc,hIcons[ICO_DSBACK],2,46);

			i=masterlist[cardlist[iSel].idx].icons[CTYPE];
			if (i) DrawBitmap(hdc,hIcons[i],2,2);
			if (masterlist[cardlist[iSel].idx].icons[CDEST2] != ICO_NONE) {
				i=masterlist[cardlist[iSel].idx].icons[CDEST2];
				if (i) DrawBitmap(hdc,hIcons[i],cxIClient-18,2);
				i=masterlist[cardlist[iSel].idx].icons[CDEST];
				if (i) DrawBitmap(hdc,hIcons[i],cxIClient-36,2);
			} else {
				i=masterlist[cardlist[iSel].idx].icons[CDEST];
				if (i) DrawBitmap(hdc,hIcons[i],cxIClient-18,2);
			}
			i=masterlist[cardlist[iSel].idx].icons[CRARITY];
			if (i) DrawBitmap(hdc,hIcons[i],2,24);
			i=masterlist[cardlist[iSel].idx].icons[CSET];
			if (i) DrawBitmap(hdc,hIcons[i],cxIClient-18,24);
			i=masterlist[cardlist[iSel].idx].icons[CEXTRA1];
			if (i) DrawBitmap(hdc,hIcons[i],cxIClient-18,cyIClient-18);
			i=masterlist[cardlist[iSel].idx].icons[CEXTRA2];
			if (i) DrawBitmap(hdc,hIcons[i],cxIClient-18,cyIClient-18-20);
			i=masterlist[cardlist[iSel].idx].icons[CEXTRA3];
			if (i) DrawBitmap(hdc,hIcons[i],cxIClient-18,cyIClient-18-20-20);

			sprintf(szBuf,"New Set?");
			for (i=0;i<MAXSETS;i++)
			{
				if (masterlist[cardlist[iSel].idx].attr[ATT_SET]==sets[i][0])
				{
					sprintf(szBuf,"%s",sets[i]+1);
					i=MAXSETS;
				}
			}
			TextOut(hdc,rLore.left+cyChar/2,cyIClient-cyChar,szBuf,strlen(szBuf));
			SetTextAlign(hdc,TA_RIGHT);
			TextOut(hdc,rLore.right-cyChar/2,cyIClient-cyChar,masterlist[cardlist[iSel].idx].szPrice,
								strlen(masterlist[cardlist[iSel].idx].szPrice));
			SetTextAlign(hdc,TA_LEFT);

			SetWindowText(hLore,masterlist[cardlist[iSel].idx].lore);
			SetWindowText(hGT,masterlist[cardlist[iSel].idx].desc);


			switch (masterlist[cardlist[iSel].idx].attr[ATT_TYPE])
			{
				case 'C':
					sprintf(szBuf,"Power %c",masterlist[cardlist[iSel].idx].attr[ATT_POWER]);
					TextOut(hdc,rLore.left,rGT.top-cyChar,szBuf,strlen(szBuf));
					if (masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]!='0')
					{
						if (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE]!='D')
							sprintf(szBuf,"Ability %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
						else sprintf(szBuf,"Armor %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
						SetTextAlign(hdc,TA_CENTER);
						TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
					}
					DrawBitmap(hdc,hIcons[ICO_DEPLOY0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_DEPLOY])],
						2,cyIClient-18-20);
					DrawBitmap(hdc,hIcons[ICO_FORFEIT0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_FORFEIT])],
						2,cyIClient-18);
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'R': sprintf(szBuf,"Rebel Character"); break;
						case 'I': sprintf(szBuf,"Imperial Character"); break;
						case 'D': sprintf(szBuf,"Droid Character"); break;
						case 'A': sprintf(szBuf,"Alien Character"); break;
						case 'J': sprintf(szBuf,"Jedi Master"); break;
						default:  sprintf(szBuf,"Character"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'S':
					sprintf(szBuf,"Power %i",masterlist[cardlist[iSel].idx].attr[ATT_POWER]-'0');
					TextOut(hdc,rLore.left,rGT.top-cyChar,szBuf,strlen(szBuf));
					if (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE]!='C')
						sprintf(szBuf,"Manv %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
					else
						sprintf(szBuf,"Armor %i",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]-'0');
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
					sprintf(szBuf,"Hyper %c",masterlist[cardlist[iSel].idx].attr[ATT_SPEED]);
					SetTextAlign(hdc,TA_RIGHT);
					TextOut(hdc,rLore.right,rGT.top-cyChar,szBuf,strlen(szBuf));
					if (masterlist[cardlist[iSel].idx].attr[ATT_DEPLOY]=='?')
					{
						DrawBitmap(hdc,hIcons[ICO_DEPLOY15],2,cyIClient-18-20);
						DrawBitmap(hdc,hIcons[ICO_FORFEIT15],2,cyIClient-18);
					} else
					{
						DrawBitmap(hdc,hIcons[ICO_DEPLOY0+
							digiticon(masterlist[cardlist[iSel].idx].attr[ATT_DEPLOY])],
							2,cyIClient-18-20);
						DrawBitmap(hdc,hIcons[ICO_FORFEIT0+
							digiticon(masterlist[cardlist[iSel].idx].attr[ATT_FORFEIT])],
							2,cyIClient-18);
					}
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'C': sprintf(szBuf,"Capital Starship"); break;
						case 'F': sprintf(szBuf,"Starfighter"); break;
						case 'S': sprintf(szBuf,"Starfighter Squadron"); break;
						default:  sprintf(szBuf,"Starship"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'R':
					sprintf(szBuf,"Ferocity %c",masterlist[cardlist[iSel].idx].attr[ATT_POWER]);
					TextOut(hdc,rLore.left,rGT.top-cyChar,szBuf,strlen(szBuf));
					sprintf(szBuf,"Defense %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
					DrawBitmap(hdc,hIcons[ICO_DEPLOY0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_DEPLOY])],
						2,cyIClient-18-20);
					DrawBitmap(hdc,hIcons[ICO_FORFEIT0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_FORFEIT])],
						2,cyIClient-18);
					break;

				case 'L':
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'P': case 'M':
							sprintf(szBuf,"Parsec %c",masterlist[cardlist[iSel].idx].attr[ATT_PARSEC]);
							SetTextAlign(hdc,TA_CENTER);
							TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
							sprintf(szBuf,"System");
							if (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE]=='M')
								sprintf(szBuf,"Mobile System");
							break;
						case 'T':
							switch (masterlist[cardlist[iSel].idx].attr[ATT_ICON])
							{
								case 'X':
									if (masterlist[cardlist[iSel].idx].attr[ATT_PARSEC]!='0')
									{
										sprintf(szBuf,"Marker # %c",masterlist[cardlist[iSel].idx].attr[ATT_PARSEC]);
										SetTextAlign(hdc,TA_CENTER);
										TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
									}
									sprintf(szBuf,"Exterior Site");
									break;
								case 'N': sprintf(szBuf,"Interior Site"); break;
								case 'D': sprintf(szBuf,"Dejarik Holosite"); break;
								default: sprintf(szBuf,"Site"); break;
							}
							break;
						case 'V':
							sprintf(szBuf,"Mobile Site");
							break;
						case 'S':
							sprintf(szBuf,"Sector");
							break;
						default:
							sprintf(szBuf,"Location");
							break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;
				
				case 'N':
					sprintf(szBuf,"Epic Event");
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'J':
					sprintf(szBuf,"Jedi Test # %c",masterlist[cardlist[iSel].idx].attr[ATT_DESTINY]);
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'I':
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'L': sprintf(szBuf,"Lost Interrupt"); break;
						case 'U': sprintf(szBuf,"Used Interrupt"); break;
						case 'B': sprintf(szBuf,"Used or Lost Interrupt"); break;
						case 'S': sprintf(szBuf,"Used or Starting Interrupt"); break;
						default: sprintf(szBuf,"Interrupt"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'E':
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'U': sprintf(szBuf,"Utinni Effect"); break;
						case 'I': sprintf(szBuf,"Immediate Effect"); break;
						case 'M': sprintf(szBuf,"Mobile Effect"); break;
						default: sprintf(szBuf,"Effect"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'D':
					sprintf(szBuf,"Device");
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;


				case 'W':
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'C': sprintf(szBuf,"Character Weapon"); break;
						case 'D': sprintf(szBuf,"Death Star Weapon"); break;
						case 'A': sprintf(szBuf,"Automated Weapon"); break;
						case 'S': sprintf(szBuf,"Starship Weapon"); break;
						case 'R':
							sprintf(szBuf,"Artillery Weapon");
							DrawBitmap(hdc,hIcons[ICO_DEPLOY0+
								digiticon(masterlist[cardlist[iSel].idx].attr[ATT_DEPLOY])],
								2,cyIClient-18-20);
							DrawBitmap(hdc,hIcons[ICO_FORFEIT0+
								digiticon(masterlist[cardlist[iSel].idx].attr[ATT_FORFEIT])],
								2,cyIClient-18);
							break;
						case 'V': sprintf(szBuf,"Vehicle Weapon"); break;
						default:  sprintf(szBuf,"Weapon"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'V':
					sprintf(szBuf,"Power %c",masterlist[cardlist[iSel].idx].attr[ATT_POWER]);
					TextOut(hdc,rLore.left,rGT.top-cyChar,szBuf,strlen(szBuf));
					sprintf(szBuf,"Armor %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
					sprintf(szBuf,"Land %c",masterlist[cardlist[iSel].idx].attr[ATT_SPEED]);
					SetTextAlign(hdc,TA_RIGHT);
					TextOut(hdc,rLore.right,rGT.top-cyChar,szBuf,strlen(szBuf));
					DrawBitmap(hdc,hIcons[ICO_DEPLOY0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_DEPLOY])],
						2,cyIClient-18-20);
					DrawBitmap(hdc,hIcons[ICO_FORFEIT0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_FORFEIT])],
						2,cyIClient-18);
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'T': sprintf(szBuf,"Transport Vehicle"); break;
						case 'C': sprintf(szBuf,"Creature Vehicle"); break;
						case 'S': sprintf(szBuf,"Shuttle Vehicle"); break;
						case 'B': sprintf(szBuf,"Combat Vehicle"); break;
						default:  sprintf(szBuf,"Vehicle"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;



			}

			DeleteObject(SelectObject(hdc,GetStockObject(SYSTEM_FONT)));
			//SelectObject(hdc,GetStockObject(SYSTEM_FONT));
			EndPaint(hwnd,&ps);
			return 0;
					
		case WM_DESTROY:
			DeleteObject(hInfoFont);
			DeleteObject(hBackFont);
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd,iMsg,wParam,lParam);
}

BOOL CALLBACK PriceDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static BOOL bReadOnly;
	char   szBuf[10];

	switch (iMsg)
	{
		case WM_INITDIALOG:
			SetWindowText(GetDlgItem(hDlg,IDC_PRICE_SOURCE),szPriceGuideSource);
			SetWindowText(GetDlgItem(hDlg,IDC_PRICE_ASOF),szPriceGuideDate);
			SetWindowText(GetDlgItem(hDlg,IDC_PRICE_CARDNAME),masterlist[cardlist[iSel].idx].name);
			SetWindowText(GetDlgItem(hDlg,IDC_PRICE_OLD),masterlist[cardlist[iSel].idx].szPrice);
			bReadOnly=TRUE;
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					if (bReadOnly==FALSE)
					{
						GetDlgItemText(hDlg,IDC_PRICE_SOURCE,szPriceGuideSource,250);
						GetDlgItemText(hDlg,IDC_PRICE_ASOF,szPriceGuideDate,19);
					}
					GetDlgItemText(hDlg,IDC_PRICE_NEW,szBuf,9);
					if (strlen(szBuf))
					{
						if ((atof(szBuf)<(float)0.01)&&(szBuf[0]!='0'))
						{
							MessageBox(hDlg,"Enter a numeric value only.","Invalid Value",MB_ICONSTOP);
							return TRUE;
						}
						if (atof(szBuf)>(float)0.0)
						{
							sprintf(masterlist[cardlist[iSel].idx].szPrice,"%s",szBuf);
							masterlist[cardlist[iSel].idx].fPrice=(float)atof(szBuf);
						}
						else
						{
							sprintf(masterlist[cardlist[iSel].idx].szPrice,"---");
							masterlist[cardlist[iSel].idx].fPrice=(float)0.0;
						}
						bPriceGuideChanged=TRUE;
					}
					EndDialog(hDlg,0);
					return TRUE;

				case IDCANCEL:
					EndDialog(hDlg,0);
					return TRUE;

				case IDHELP:
					sprintf(pHelpText,HELP_EDITVALUE);
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,(DLGPROC)HelpDlgProc);
					return TRUE;

				case IDC_PRICE_EDITSOURCE:
					if (bReadOnly==TRUE) bReadOnly=FALSE;
					else bReadOnly=TRUE;
					SendMessage(GetDlgItem(hDlg,IDC_PRICE_SOURCE),EM_SETREADONLY,
								(WPARAM)bReadOnly,0);
					SendMessage(GetDlgItem(hDlg,IDC_PRICE_ASOF),EM_SETREADONLY,
								(WPARAM)bReadOnly,0);
					return TRUE;
			}
			break;
	}
	return FALSE;
}


BOOL CALLBACK StatsDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
//	RECT	rect;
//	int		cxDlg,cyDlg;
	HDC		hdc;
	int		i,j,k;
	char	szBuf[4096],szBuf2[4096],szBuf3[4096];

	switch (iMsg)
	{
		case WM_INITDIALOG:
			MoveWindow(hDlg,rStatsWin.left,rStatsWin.top,
						rStatsWin.right-rStatsWin.left,
						rStatsWin.bottom-rStatsWin.top,TRUE);
			hdc=GetDC(hDlg);
			if (!menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,0,TRUE);
			else if (menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,90,0,TRUE);
			else if (!menu.bCompressed&&menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,EZ_ATTR_BOLD,TRUE);
			else
				hCardFont=EzCreateFont(hdc,"Arial",100,90,EZ_ATTR_BOLD,TRUE);
			//hCardFont=EzCreateFont(hdc,"Arial",90,90,EZ_ATTR_BOLD,TRUE);
			ReleaseDC(hDlg,hdc);
			SendMessage(GetDlgItem(hDlg,IDC_STATSLABEL),WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			SendMessage(GetDlgItem(hDlg,IDC_STATSLIST),WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));			
			SendMessage(GetDlgItem(hDlg,IDC_STATSNOTES),WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			MoveWindow(GetDlgItem(hDlg,IDC_STATSNOTES),0,0,0,0,TRUE);
			updatestats(hDlg);
			return TRUE;

		case WM_USER:
			if (wParam!=WMU_CHANGE_FONT)
			{
				SetFocus(GetDlgItem(hDlg,IDC_STATSNOTES));
				return 0;
			}
			hdc=GetDC(hDlg);
			if (!menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,0,TRUE);
			else if (menu.bCompressed&&!menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,90,0,TRUE);
			else if (!menu.bCompressed&&menu.bBold)
				hCardFont=EzCreateFont(hdc,"Arial",100,100,EZ_ATTR_BOLD,TRUE);
			else
				hCardFont=EzCreateFont(hdc,"Arial",100,90,EZ_ATTR_BOLD,TRUE);
			SendMessage(GetDlgItem(hDlg,IDC_STATSLABEL),WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			SendMessage(GetDlgItem(hDlg,IDC_STATSLIST),WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			SendMessage(GetDlgItem(hDlg,IDC_STATSNOTES),WM_SETFONT,(WPARAM)hCardFont,MAKELPARAM(TRUE,0));
			ReleaseDC(hDlg,hdc);
			return 0;

		case WM_SIZE:
			if (menu.iStatsMode!=ID_STATS_NOTES) viewnotes(hDlg,FALSE);
			else viewnotes(hDlg,TRUE);
			break;

		case WM_MOVE:
			GetWindowRect(hDlg,&rStatsWin);
			break;

		case WM_COMMAND:
			/*if (LOWORD(wParam)==IDOK)
			{
				EndDialog(hDlg,0);
				return TRUE;
			}
			else*/
			if ((HIWORD(wParam)==LBN_SELCHANGE)&&(menu.iStatsMode!=ID_STATS_NOTES))
			{
				// see if selected stat is a card name. if it is, jump to it
				i=SendMessage(GetDlgItem(hDlg,IDC_STATSLIST),LB_GETTEXT,SendMessage(GetDlgItem(hDlg,IDC_STATSLIST),LB_GETCURSEL,0,0),(LPARAM)szBuf);
				sprintf(szBuf3,"%s",szBuf);
				// to save the card index, when a card is added to the stats list
				// it's padded with 11 \tabs. in some instances, there are 
				// columns before the card name. the following switch takes out
				// the previous column by moving the szBuf start to the start of
				// the card name

				switch (menu.iStatsMode)
				{
					case ID_STATS_DETAIL:
						for (i=1,k=0;i<(signed)strlen(szBuf);i++)
							if (szBuf[i-1]=='\t') {k=i; i=256;}
						sprintf(szBuf,"%s",szBuf+k);
						break;
					case ID_STATS_PROBABILITY:
						for (i=1,1,k=0,j=0;(j<3)&&(i<(signed)strlen(szBuf));i++)
							if (szBuf[i-1]=='\t') {k=i; j++;}
						sprintf(szBuf,"%s",szBuf+k);
						break;
				}

				for (i=0;i<iCListSize;i++)
				{
					
					sprintf(szBuf2,"%s\t\t\t\t\t\t\t\t\t\t\t%d",masterlist[cardlist[i].idx].name,cardlist[i].idx);
				
					if (strcmp(szBuf,szBuf2)==0)
					{
						SendMessage(GetParent(hDlg),WM_USER,i,0L);
						i=iCListSize; // get out of loop
					}
					else if ((menu.iStatsMode!=ID_STATS_FIND)&&(strcmp(szBuf2,szBuf3)==0))
					{
						SendMessage(GetParent(hDlg),WM_USER,i,0L);
						i=iCListSize; // get out of loop
					}
				}
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hDlg);
//			hDlgDeck = 0;
			break;

	}
	return FALSE;
}

BOOL CALLBACK JumpDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	int i,k;
	char szBuf[256],szEdit[256];

	switch (iMsg)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDOK)
			{
				EndDialog(hDlg,0);
				return TRUE;
			}
			else if (LOWORD(wParam)==IDCANCEL)
			{
				SendMessage(GetParent(hDlg),WM_USER,(WPARAM)iUndoSel,0L);
				EndDialog(hDlg,0);
				return TRUE;
			} 
			else if (HIWORD(wParam)==EN_CHANGE)
			{
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_JUMP),szEdit,256);

				for (i=0,k=0;(i<iCListSize)&&!k;i++)
				{
					sprintf(szBuf,"%s",masterlist[cardlist[i].idx].name);
					szBuf[strlen(szEdit)]='\0';
					if (_stricmp(szBuf,szEdit)==0)
					{
						k=1;
						if (strlen(szBuf)) SendMessage(GetParent(hDlg),WM_USER,(WPARAM)i,0L);
						else SendMessage(GetParent(hDlg),WM_USER,(WPARAM)iUndoSel,0L);
					}
				}
			}
		//if (masterlist[cardlist[j].idx].name[0]==(char)wParam)					if (strcmp(szEdit,
			break;

		case WM_CLOSE:
			DestroyWindow(hDlg);
//			hDlgDeck = 0;
			break;
	}

	return FALSE;
}

BOOL CALLBACK FindDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	char szBuf[256],szTitle[256];
	HMENU hMenu;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDNAME),BM_SETCHECK,menu.bFindName,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDLORE),BM_SETCHECK,menu.bFindLore,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDDESC),BM_SETCHECK,menu.bFindDesc,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDLIGHT),BM_SETCHECK,menu.bFindLight,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDDARK),BM_SETCHECK,menu.bFindDark,0);
			updatestats(hDlg);
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDCANCEL)
			{
				EndDialog(hDlg,0);
				return TRUE;
			}
			else if (LOWORD(wParam)==IDOK)
			{
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDNAME),BM_GETCHECK,0,0))
					menu.bFindName=TRUE; else menu.bFindName=FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDLORE),BM_GETCHECK,0,0))
					menu.bFindLore=TRUE; else menu.bFindLore=FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDDESC),BM_GETCHECK,0,0))
					menu.bFindDesc=TRUE; else menu.bFindDesc=FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDLIGHT),BM_GETCHECK,0,0))
					menu.bFindLight=TRUE; else menu.bFindLight=FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FINDDARK),BM_GETCHECK,0,0))
					menu.bFindDark=TRUE; else menu.bFindDark=FALSE;
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_FIND),szBuf,256);
				FindStats(hDlgStats,szBuf);
				sprintf(szTitle,"Find Results: %s",szBuf);
				SetWindowText(hDlgStats,szTitle);
				if (menu.bKeepFind)
				{
					hMenu = GetMenu(GetParent(hDlg));
					CheckMenuItem(hMenu,menu.iStatsMode,MF_UNCHECKED);
					menu.iStatsMode=ID_STATS_FIND;
					EnableMenuItem(hMenu,ID_STATS_FIND,MF_ENABLED);
					CheckMenuItem(hMenu,ID_STATS_FIND,MF_CHECKED);
				}
				EndDialog(hDlg,0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK QtyDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	char szBuf[32];

	switch (iMsg)
	{
		case WM_INITDIALOG:
			sprintf(szBuf,"%d",cardlist[iSel].deck);
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_QTY),szBuf);
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDOK)
			{
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_QTY),szBuf,31);
				cardlist[iSel].deck = min(atoi(szBuf),99999);
				UpdateSelQty(GetParent(hDlg));
				updatestats(hDlgStats);
				EndDialog(hDlg,0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}


void fillrarities(HWND hList,int cRarity,int cSubrarity,int cSide,int iQty)
{
	int i,j;
	int  nSSel;
	int	 aSSel[MAXSETS*sizeof(int)];

	nSSel = SendMessage(hList,LB_GETSELCOUNT,0,0);
	// if sets are selected
	if (nSSel>=1)
	{
		SendMessage(hList,LB_GETSELITEMS,(WPARAM)MAXSETS,(LPARAM)(LPINT)aSSel);
		// go through each selected set
		for (i=0;i<nSSel;i++)
		{
			// if selected set is valid
			if (sets[aSSel[i]][0]!='\0')
			{
				// go through each card
				for (j=0;j<iCListSize;j++)
				{
					// if set, side and rarity match
					if ((masterlist[cardlist[j].idx].attr[ATT_SET]==sets[aSSel[i]][0])&&
						(masterlist[cardlist[j].idx].attr[ATT_SIDE]==cSide)&&
						(masterlist[cardlist[j].idx].attr[ATT_RARITY]==cRarity)&&
						(masterlist[cardlist[j].idx].attr[ATT_SUBRARITY]==cSubrarity))
					{
						// adjust by iQty
						cardlist[j].deck = max(0,min(99999,cardlist[j].deck+iQty));
						cardlist[j].wb = max(0,min(99999,cardlist[j].wb+iQty));
					}
				}
			}
		}
	}
}

BOOL CALLBACK MassEntryDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hRList;
	HWND hSList;
	int	 i,iQty;
	int  nRSel;
	int	 aRSel[12];
	BOOL bLS,bDS; // whether LS and DS are filled
	char szBuf[10];

	switch (iMsg)
	{
		case WM_INITDIALOG:
			hRList = GetDlgItem(hDlg,IDC_LISTRARITY);
			hSList = GetDlgItem(hDlg,IDC_LISTSET);
			SendMessage(GetDlgItem(hDlg,IDC_CHECKLS),BM_SETCHECK,BST_CHECKED,0);
			SendMessage(GetDlgItem(hDlg,IDC_CHECKDS),BM_SETCHECK,BST_CHECKED,0);
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)0,(LPARAM)"R");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)1,(LPARAM)"R1");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)2,(LPARAM)"R2");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)3,(LPARAM)"U");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)4,(LPARAM)"U1");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)5,(LPARAM)"U2");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)6,(LPARAM)"C");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)7,(LPARAM)"C1");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)8,(LPARAM)"C2");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)9,(LPARAM)"C3");
			SendMessage(hRList,LB_INSERTSTRING,(WPARAM)10,(LPARAM)"PM");
			for (i=0;i<MAXSETS;i++)
				if (sets[i][0]!='\0')
					SendMessage(hSList,LB_INSERTSTRING,i,(LPARAM)(sets[i]+1));
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					hRList = GetDlgItem(hDlg,IDC_LISTRARITY);
					hSList = GetDlgItem(hDlg,IDC_LISTSET);
					GetWindowText(GetDlgItem(hDlg,IDC_EDITQTY),szBuf,8);
					iQty = atoi(szBuf);
					nRSel = SendMessage(hRList,LB_GETSELCOUNT,0,0);
					if (nRSel>=1)
					{
						if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECKLS),BM_GETCHECK,0,0))
							bLS=TRUE; else bLS=FALSE;
						if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECKDS),BM_GETCHECK,0,0))
							bDS=TRUE; else bDS=FALSE;
						SendMessage(hRList,LB_GETSELITEMS,(WPARAM)11,(LPARAM)(LPINT)aRSel);
						for (i=0;i<nRSel;i++)
						{
							switch (aRSel[i])
							{
								case 0: // R
									if (bLS) fillrarities(hSList,'R','0','l',iQty);
									if (bDS) fillrarities(hSList,'R','0','d',iQty);
									break;
								case 1: // R1
									if (bLS) fillrarities(hSList,'R','1','l',iQty);
									if (bDS) fillrarities(hSList,'R','1','d',iQty);
									break;
								case 2: // R2
									if (bLS) fillrarities(hSList,'R','2','l',iQty);
									if (bDS) fillrarities(hSList,'R','2','d',iQty);
									break;
								case 3: // U
									if (bLS) fillrarities(hSList,'U','0','l',iQty);
									if (bDS) fillrarities(hSList,'U','0','d',iQty);
									break;
								case 4: // U1
									if (bLS) fillrarities(hSList,'U','1','l',iQty);
									if (bDS) fillrarities(hSList,'U','1','d',iQty);
									break;
								case 5: // U2
									if (bLS) fillrarities(hSList,'U','2','l',iQty);
									if (bDS) fillrarities(hSList,'U','2','d',iQty);
									break;
								case 6: // C
									if (bLS) fillrarities(hSList,'C','0','l',iQty);
									if (bDS) fillrarities(hSList,'C','0','d',iQty);
									break;
								case 7: // C1
									if (bLS) fillrarities(hSList,'C','1','l',iQty);
									if (bDS) fillrarities(hSList,'C','1','d',iQty);
									break;
								case 8: // C2
									if (bLS) fillrarities(hSList,'C','2','l',iQty);
									if (bDS) fillrarities(hSList,'C','2','d',iQty);
									break;
								case 9: // C3
									if (bLS) fillrarities(hSList,'C','3','l',iQty);
									if (bDS) fillrarities(hSList,'C','3','d',iQty);
									break;
								case 10: // PM
									if (bLS) fillrarities(hSList,'N','A','l',iQty);
									if (bDS) fillrarities(hSList,'N','A','d',iQty);
									break;

							}

						}
					}
					if (nRSel)
					{
						updatestats(hDlgStats);
						InvalidateRect(GetParent(hDlg),NULL,TRUE);
					}
//					EndDialog(hDlg,0);
					return TRUE;
				case IDCANCEL:
					EndDialog(hDlg,0);
					return TRUE;
				case IDC_BUTTON_HELP:
					sprintf(pHelpText,HELP_MASSENTRY);
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,(DLGPROC)HelpDlgProc);
					return TRUE;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK WantDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	int i,iWant;
	char szBuf[32];

	switch (iMsg)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDOK)
			{
				bNeedSave = TRUE;
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_WANT),szBuf,8);
				iWant = atoi(szBuf);
				if (iWant)
				{
					for (i=0;i<iCListSize;i++)
					{
						cardlist[i].wb = 0;
						cardlist[i].deck = max(0,min(99999,iWant-cardlist[i].deck));
					}
					updatestats(hDlgStats);
					InvalidateRect(GetParent(hDlg),NULL,TRUE);
				}
				EndDialog(hDlg,0);
				return TRUE;
			}
			if (LOWORD(wParam)==IDCANCEL)
			{
				bNeedSave=FALSE;
				EndDialog(hDlg,0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK PrintDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:

			CheckRadioButton(hDlg,IDC_RADIO_DESTCLIP,IDC_RADIO_DESTPRINT,printopt.uDestination);
			if (printopt.bCardQty) CheckRadioButton(hDlg,IDC_RADIO_QTYCARD,IDC_RADIO_CARDQTY,IDC_RADIO_CARDQTY);
			else CheckRadioButton(hDlg,IDC_RADIO_QTYCARD,IDC_RADIO_CARDQTY,IDC_RADIO_QTYCARD);
			if (printopt.bNotes) SendMessage(GetDlgItem(hDlg,IDC_CHECK_NOTES),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bSummary) SendMessage(GetDlgItem(hDlg,IDC_CHECK_SUMMARY),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bDate) SendMessage(GetDlgItem(hDlg,IDC_CHECK_DATE),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bFSet) SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDSET),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bFSide) SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDSIDE),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bFRarity) SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDRARITY),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bFType) SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDTYPE),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bSSet) SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKSET),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bSSide) SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKSIDE),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bSRarity) SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKRARITY),BM_SETCHECK,BST_CHECKED,0);
			if (printopt.bSType) SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKTYPE),BM_SETCHECK,BST_CHECKED,0);
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDCANCEL)
			{
				EndDialog(hDlg,0);
				return TRUE;
			}
			if (LOWORD(wParam)==IDOK)
			{
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_RADIO_DESTCLIP),BM_GETCHECK,0,0)) printopt.uDestination = IDC_RADIO_DESTCLIP;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_RADIO_DESTFILE),BM_GETCHECK,0,0)) printopt.uDestination = IDC_RADIO_DESTFILE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_RADIO_DESTPRINT),BM_GETCHECK,0,0)) printopt.uDestination = IDC_RADIO_DESTPRINT;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_RADIO_CARDQTY),BM_GETCHECK,0,0)) printopt.bCardQty = TRUE;
				else printopt.bCardQty = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_NOTES),BM_GETCHECK,0,0))
					printopt.bNotes = TRUE;
				else printopt.bNotes = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_SUMMARY),BM_GETCHECK,0,0))
					printopt.bSummary = TRUE;
				else printopt.bSummary = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_DATE),BM_GETCHECK,0,0))
					printopt.bDate = TRUE;
				else printopt.bDate = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDSET),BM_GETCHECK,0,0))
					printopt.bFSet = TRUE;
				else printopt.bFSet = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDSIDE),BM_GETCHECK,0,0))
					printopt.bFSide = TRUE;
				else printopt.bFSide = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDRARITY),BM_GETCHECK,0,0))
					printopt.bFRarity = TRUE;
				else printopt.bFRarity = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_FIELDTYPE),BM_GETCHECK,0,0))
					printopt.bFType = TRUE;
				else printopt.bFType = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKSET),BM_GETCHECK,0,0))
					printopt.bSSet = TRUE;
				else printopt.bSSet = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKSIDE),BM_GETCHECK,0,0))
					printopt.bSSide = TRUE;
				else printopt.bSSide = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKRARITY),BM_GETCHECK,0,0))
					printopt.bSRarity = TRUE;
				else printopt.bSRarity = FALSE;
				if (BST_CHECKED==SendMessage(GetDlgItem(hDlg,IDC_CHECK_BREAKTYPE),BM_GETCHECK,0,0))
					printopt.bSType = TRUE;
				else printopt.bSType = FALSE;
				printdeck(GetParent(hDlg));
				EndDialog(hDlg,0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK OrderDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static int i,idx,nList;
	char szBuf[256];
	static HWND hList;
	static char szOrderList[32][32];
	FILE *fh;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			hList=GetDlgItem(hDlg,IDC_ORDERLIST);
			idx=0;
			nList=0;
			if (iOrderMode==ORDERSETS)
			{
				for (i=0;i<MAXSETS;i++)
				{
					if (sets[i][0]>'\0')
					{
						sprintf(szOrderList[nList++],"%s",sets[i]);
					}
				}
			}
			else if (iOrderMode==ORDERTYPE)
			{
				sprintf(szOrderList[nList++],"%s","LLocation");
				sprintf(szOrderList[nList++],"%s","CCharacter");
				sprintf(szOrderList[nList++],"%s","RCreature");
				sprintf(szOrderList[nList++],"%s","VVehicle");
				sprintf(szOrderList[nList++],"%s","SStarship");
				sprintf(szOrderList[nList++],"%s","IInterrupt");
				sprintf(szOrderList[nList++],"%s","EEffect");
				sprintf(szOrderList[nList++],"%s","WWeapon");
				sprintf(szOrderList[nList++],"%s","DDevice");
				sprintf(szOrderList[nList++],"%s","NEpic Event");
				sprintf(szOrderList[nList++],"%s","JJedi Test");
			}
			if (nList)
			{
				SendMessage(hList,WM_SETREDRAW,FALSE,0);
				SendMessage(hList,LB_RESETCONTENT,0,0);
				for (i=0;i<nList;i++)
					SendMessage(hList,LB_ADDSTRING,0,(LPARAM)(szOrderList[i]+1));
				SendMessage(hList,WM_SETREDRAW,TRUE,0);
				SendMessage(hList,LB_SETCURSEL,idx,0);
			}
			return TRUE;

		case WM_COMMAND:
			if (HIWORD(wParam)==LBN_SELCHANGE)
			{
				idx = SendMessage(hList,LB_GETCURSEL,0,0);
			}
			else
			switch (LOWORD(wParam))
			{
				case IDOK:
					if (iOrderMode==ORDERSETS)
					{
						if ((fh=fopen("SWCCG Sets.dm","w"))!=NULL)
						{
							for (i=0;i<nList;i++)
							{
								fprintf(fh,"%s\n",szOrderList[i]);
							}
							fclose(fh);
						}
					}
					if (iOrderMode==ORDERTYPE)
					{
						for (i=0;i<nList;i++)
						{
							szBuf[i]=szOrderList[i][0];
						}
						szBuf[i]='\0';
						WritePrivateProfileString("Sort","TypeOrder",szBuf,szIni);
					}
					EndDialog(hDlg,0);
					return TRUE;
					break;
				case IDCANCEL:
					iOrderMode=0;
					EndDialog(hDlg,0);
					return TRUE;
					break;
				case IDC_BUTTON_MOVEUP:
					if (idx>0)
					{
						sprintf(szBuf,szOrderList[idx]);
						sprintf(szOrderList[idx],szOrderList[idx-1]);
						sprintf(szOrderList[idx-1],szBuf);
						idx--;
						SendMessage(hList,WM_SETREDRAW,FALSE,0);
						SendMessage(hList,LB_RESETCONTENT,0,0);
						for (i=0;i<nList;i++)
							SendMessage(hList,LB_ADDSTRING,0,(LPARAM)(szOrderList[i]+1));
						SendMessage(hList,WM_SETREDRAW,TRUE,0);
						SendMessage(hList,LB_SETCURSEL,idx,0);
					}
					break;
				case IDC_BUTTON_MOVEDOWN:
					if ((idx+1)<nList)
					{
						sprintf(szBuf,szOrderList[idx]);
						sprintf(szOrderList[idx],szOrderList[idx+1]);
						sprintf(szOrderList[idx+1],szBuf);
						idx++;
						SendMessage(hList,WM_SETREDRAW,FALSE,0);
						SendMessage(hList,LB_RESETCONTENT,0,0);
						for (i=0;i<nList;i++)
							SendMessage(hList,LB_ADDSTRING,0,(LPARAM)(szOrderList[i]+1));
						SendMessage(hList,WM_SETREDRAW,TRUE,0);
						SendMessage(hList,LB_SETCURSEL,idx,0);
					}
					break;
			}				
			break;
	}
	return FALSE;
}

BOOL CALLBACK BrainiacDlgProc(HWND hDlg,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	int iLSHand,iDSHand,iLSForce,iDSForce;
	char szBuf[32];
	double fPower,fNum;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDOK)
			{
				EndDialog(hDlg,0);
				return TRUE;
			}
			if (LOWORD(wParam)==IDC_BUTTON_COMPUTE)
			{
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_LSHAND),szBuf,10);
				iLSHand=atoi(szBuf);
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_DSHAND),szBuf,10);
				iDSHand=atoi(szBuf);
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_LSFORCE),szBuf,10);
				iLSForce=atoi(szBuf);
				GetWindowText(GetDlgItem(hDlg,IDC_EDIT_DSFORCE),szBuf,10);
				iDSForce=atoi(szBuf);
				fNum=(double)(3*(iDSHand-iLSHand))+
					 (double)(2*(iDSForce-iLSForce))+
					 (double)3.141592653589;
				if (fNum>0.0) fPower=sqrt(fNum);
				else (fPower=1.0);
				if (fPower<1.0) fPower=1.0;
				sprintf(szBuf,"Power: %.1f",(float)fPower);
				SetWindowText(GetDlgItem(hDlg,IDC_STATIC_POWER),szBuf);
			}
			break;
	}
	return FALSE;
}
