// Star Wars CCG DeckMaker 3.0
// for Windows 95

// started July 13, 1996

#include <windows.h>
#include <stdio.h>
#include <commctrl.h>

#include "resource.h"
#include "SWDeck.h"
#include "attributes.h"
#include "help.h"
#include "ezfont.h"

#define rWidth(r) (r.right-r.left)
#define rHeight(r) (r.bottom-r.top)

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK InfoWndProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK ExcludeDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK IncludeDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK HelpDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK EditDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK SplashDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK SortDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK DeckDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL	CALLBACK PriceDlgProc(HWND,UINT,WPARAM,LPARAM);

HWND	hDlgDeck;
HWND	hwndInfo;

HANDLE	hInst;
char	szAppName[] = "SWCCG DeckMaker 3.0";

char	pHelpText[10000];	// buffer for storing context-sensitive help

int		iBuf;		// global buffer for passing values back and forth

int		cyChar;		// height of a standard character
int		cxClient,cyClient; // dimensions of the main client window
int		iNdent;
#define NUMSPACE 46  // iNdent-NUMSPACE=right edge of icons
int		iVPos,iVMax;
int		iSel;		// cardlist[] index to currently selected card
int		iSelPos;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   PSTR szCmdLine,      int iCmdShow)
{
	HWND		hwnd;
	HWND		hwndSplash;	// handle of "splash" window
	MSG			msg;
	WNDCLASSEX	wndclass;
	int			i;
	HDC			hdc;
	int			cxWindow,cyWindow,xWindow,yWindow;

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
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= "Card Information";
	wndclass.hIconSm		= LoadImage(hInstance,MAKEINTRESOURCE(IDI_SWCCGDM),
							  IMAGE_ICON,16,16,0);

	RegisterClassEx(&wndclass);

	hInst = hInstance;

	hwndSplash = CreateDialog(hInst,MAKEINTRESOURCE(IDD_SPLASHDLG),NULL,SplashDlgProc);

	i = OpenSWCCGData();

	if (i>ALL_OK) DestroyWindow(hwndSplash);

    hdc = CreateIC("DISPLAY",NULL,NULL,NULL);
	cxWindow = GetDeviceCaps(hdc,HORZRES);
	cyWindow = GetDeviceCaps(hdc,VERTRES);
	DeleteDC(hdc);
	xWindow = cxWindow/20;					// main window's left is 10% in
	yWindow = cyWindow/10;					// main window's top is 6.7% down

	rMainWin.left = cxWindow/20;
	rMainWin.right = cxWindow/2+2*rMainWin.left;
	rMainWin.top = cyWindow/10;
	rMainWin.bottom = cyWindow-rMainWin.top;

	rDeckWin.left = rMainWin.right;
	rDeckWin.right = cxWindow-rMainWin.left;
	rDeckWin.top = rMainWin.top;
	rDeckWin.bottom = (rMainWin.bottom-rMainWin.top)/2+rMainWin.top;

	rInfoWin.left = rDeckWin.left;
	rInfoWin.right = rDeckWin.right;
	rInfoWin.top = rDeckWin.bottom;
	rInfoWin.bottom = rMainWin.bottom;

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
		MsgError(hwnd,i,"SWCCGDM.DAT Error");
	}
	else
	{

		DestroyWindow(hwndSplash);

		hDlgDeck = CreateDialog(hInst,MAKEINTRESOURCE(IDD_DECKDLG),
								hwnd,DeckDlgProc);


//		MoveWindow(hwnd,rMainWin.left,rMainWin.top,rMainWin.right-rMainWin.left,
//						rMainWin.bottom-rMainWin.top,TRUE);

		MoveWindow(hDlgDeck,rDeckWin.left,rDeckWin.top,rDeckWin.right-rDeckWin.left,
						rDeckWin.bottom-rDeckWin.top,TRUE);

  
//		MoveWindow(hwndInfo,rInfoWin.left,rInfoWin.right,rWidth(rInfoWin),rHeight(rInfoWin),TRUE);

		ShowWindow(hwndInfo,iCmdShow);
		UpdateWindow(hwndInfo);

		ShowWindow(hwnd,iCmdShow);
		UpdateWindow(hwnd);

		while (GetMessage(&msg,NULL,0,0))
		{
			if (hDlgDeck==0||!IsDialogMessage(hDlgDeck,&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		CloseSWCCGData();

	}

	DeleteICO();			// delete images loaded at start of program
	
	return msg.wParam;

}

void updateinfo(void)
{
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

	switch (iMsg)
	{
		case WM_COMMAND:
			switch (wParam)
			{

				case ID__EDITPRICE:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_PRICEDLG),hwnd,PriceDlgProc);
					updateinfo();
					return 0;

				case ID__EDITCARD:
					iBuf=cardlist[iSel].idx;
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_EDITDLG),hwnd,EditDlgProc);
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
					updatedeck(hDlgDeck);

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
					updatedeck(hDlgDeck);

					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_SORT_DEFINECUSTOM:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_SORTDLG),hwnd,SortDlgProc);
					if (iBuf) SendMessage(hwnd,WM_COMMAND,ID_SORT_BYCUSTOM,0);
					return 0;

				case ID_VIEW_DECKWINDOW:
					ToggleMenu(hMenu,ID_VIEW_DECKWINDOW,&menu.bDeckWindow);
					if (menu.bDeckWindow==FALSE)
					{
						if (hDlgDeck)
						{
							SendMessage(hDlgDeck,WM_CLOSE,0,0);
						}
					}
					else
					{
						if (!hDlgDeck)
						{
							hDlgDeck = CreateDialog(hInst,MAKEINTRESOURCE(IDD_DECKDLG),
									hwnd,DeckDlgProc);
							updatedeck(hDlgDeck);
						}
					}
					return 0;
					

				case ID_VIEW_DECKQUANTITY:
					ToggleMenu(hMenu,ID_VIEW_DECKQUANTITY,&menu.bDeckQty);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_FONT_COMPRESSED:
					ToggleMenu(hMenu,ID_VIEW_FONT_COMPRESSED,&menu.bCompressed);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_VIEW_FONT_BOLD:
					ToggleMenu(hMenu,ID_VIEW_FONT_BOLD,&menu.bBold);
					InvalidateRect(hwnd,NULL,TRUE);
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
					
				case ID_FILE_NEW_ALL:
				case ID_FILE_NEW_LIGHT:
				case ID_FILE_NEW_DARK:
					ResetSWCCGData();
					switch (wParam)
					{
						case ID_FILE_NEW_LIGHT:
							ExcludeCList(ATT_SIDE,'D',FALSE);
							break;
						case ID_FILE_NEW_DARK:
							ExcludeCList(ATT_SIDE,'L',FALSE);
							break;
					}
					UpdateRange(hwnd);
					newsort(szSort);
					updatedeck(hDlgDeck);
					InvalidateRect(hwnd,NULL,TRUE);
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

				case ID_VIEW_INCLUDE:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_INCLUDEDLG),hwnd,IncludeDlgProc);
					UpdateRange(hwnd);
					updatedeck(hDlgDeck);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;
					
				case ID_VIEW_EXCLUDE:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_EXCLUDEDLG),hwnd,ExcludeDlgProc);
					UpdateRange(hwnd);
					updatedeck(hDlgDeck);
					InvalidateRect(hwnd,NULL,TRUE);
					return 0;

				case ID_HELP_ABOUT:
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_ABOUTDLG),hwnd,AboutDlgProc);
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
			menu.bCompressed=FALSE;
			menu.bBold=FALSE;
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

			bPriceGuideChanged=FALSE;

			hCardMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDR_CARDMENU));
			hCardMenu = GetSubMenu(hCardMenu,0);

			sprintf(pHelpText,"");
			sprintf(szCustomSort,"STYZR");
			sprintf(szSort,"");

			uSortOrder = ID_SORT_BYNAME;

			iBuf = 0;
			iSel = 0;
			iSelPos = 0;

			bPopUp = FALSE;

			newsort(szSort);
			updateinfo();

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
					UpdateSelQty(hwnd);
					updatedeck(hDlgDeck);
				}
				else
				{
					cardlist[iSel].deck=max(0,cardlist[iSel].deck-1);
					UpdateSelQty(hwnd);
					updatedeck(hDlgDeck);
				}
			}
			else
			{
				UpdateSel(hwnd);
				iSelPos=iVPos;
				iSel = min(iCListSize-1,max(0,iVPos+HIWORD(lParam)/cyChar-1));
				UpdateSel(hwnd);
				updateinfo();
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
				updateinfo();
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
					break;

				case VK_END:
					UpdateSel(hwnd);
					iSel=max(0,iCListSize-1);
					iSelPos=iVMax;
					SendMessage(hwnd,WM_VSCROLL,SB_BOTTOM,0L);
					UpdateSel(hwnd);
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
					break;

				case VK_NEXT:
					UpdateSel(hwnd);
					iSel=min(iSel+cyClient/cyChar,iCListSize-1);
					SendMessage(hwnd,WM_VSCROLL,SB_PAGEDOWN,0L);
					iSelPos=min(iSelPos+cyClient/cyChar,iVMax);
					UpdateSel(hwnd);
					break;

				case VK_PRIOR:
					UpdateSel(hwnd);
					iSel=max(0,iSel-cyClient/cyChar);
					SendMessage(hwnd,WM_VSCROLL,SB_PAGEUP,0L);
					iSelPos=max(0,iSelPos-cyClient/cyChar);
					UpdateSel(hwnd);
					break;

				case VK_INSERT:
					cardlist[iSel].deck=min(cardlist[iSel].deck+1,99999);
					UpdateSelQty(hwnd);
					updatedeck(hDlgDeck);
					break;

				case VK_DELETE:
					cardlist[iSel].deck=max(0,cardlist[iSel].deck-1);
					UpdateSelQty(hwnd);
					updatedeck(hDlgDeck);
					break;

			}
			updateinfo();
			return 0;

		case WM_CHAR:
			if (iCListSize<=0) return 0;
			for (i=0;i<(int)LOWORD(lParam);i++)
			{
				switch (wParam)
				{
					case '\r':
						cardlist[iSel].deck = min(cardlist[iSel].deck+1,99999);
						UpdateSelQty(hwnd);
						updatedeck(hDlgDeck);
						break;

					case '\b':
						cardlist[iSel].deck = max(0,cardlist[iSel].deck-1);
						UpdateSelQty(hwnd);
						updatedeck(hDlgDeck);
						break;

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
							//	UpdateWindow(hwnd);
							}
						}
						break;
						
					
				}
			}
			updateinfo();
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
					if (masterlist[cardlist[i].idx].icons[6])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[6]],cx,cy);
					cx += 17;
					if (masterlist[cardlist[i].idx].icons[5])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[5]],cx,cy);
					cx += 17;
					if (masterlist[cardlist[i].idx].icons[4])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[4]],cx,cy);
					cx += 17;
				}
				if (menu.bSetIcons)
				{
					if (masterlist[cardlist[i].idx].icons[3])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[3]],cx,cy);
					cx += 17;
				}
				if (menu.bRareIcons)
				{
					if (masterlist[cardlist[i].idx].icons[2])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[2]],cx,cy);
					cx += 17;
				}
				if (menu.bDestIcons)
				{
					if (masterlist[cardlist[i].idx].icons[1])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[1]],cx,cy);
					cx += 17;
				}
				if (menu.bTypeIcons)
				{
					if (masterlist[cardlist[i].idx].icons[0])
						DrawBitmap(hdc,hIcons[masterlist[cardlist[i].idx].icons[0]],cx,cy);
					cx += 17;
				}

				iNdent = cx;

				if (menu.bDeckQty==TRUE)
					if (cardlist[i].deck>0)
					{
						sprintf(szBuf,"%i",cardlist[i].deck);
						SetTextAlign(hdc,TA_RIGHT);
						iNdent+=NUMSPACE-4;
						TextOut(hdc,iNdent,cy,szBuf,strlen(szBuf));
						SetTextAlign(hdc,TA_LEFT);
						iNdent+=4;
					} else iNdent+=NUMSPACE;

				if ((masterlist[cardlist[i].idx].attr[ATT_SIDE]=='D')&&menu.bDarkShaded&&(iSel!=i))
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
							case 'N': SetTextColor(hdc,RGB_PURPLE);	break;
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
							case 'N': hBrush=hBr[BR_PURPLE]; break;
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
			SendMessage(GetDlgItem(hDlg,IDC_LEAVE_USED),BM_SETCHECK,BST_CHECKED,0);
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
								case 0: iBuf+=ExcludeCList(ATT_SIDE,'D',bLeave); break;
								case 1:	iBuf+=ExcludeCList(ATT_SIDE,'L',bLeave); break;
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
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,HelpDlgProc);
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
								case 0: iBuf+=IncludeCList(ATT_SIDE,'D'); break;
								case 1:	iBuf+=IncludeCList(ATT_SIDE,'L'); break;
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
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,HelpDlgProc);
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
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,HelpDlgProc);
					return TRUE;

				case IDOK:
					fh=fopen("swccgdm.dat","wb");
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
		case 'S': *u=IDC_SORTSIDE; sprintf(label,"Side"); break;
		case 'T': *u=IDC_SORTSET; sprintf(label,"Set"); break;
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
		case IDC_SORTSIDE:		ch='S'; break;
		case IDC_SORTSET:		ch='T'; break;
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
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,HelpDlgProc);
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
			MoveWindow(GetDlgItem(hDlg,IDC_DECKCARDS),0,0,cxDlg/2,cyChar,TRUE);
			MoveWindow(GetDlgItem(hDlg,IDC_DECKDESTINY),cxDlg/2,0,cxDlg/2,cyChar,TRUE);
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
			hDlgDeck = 0;
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
	static HFONT	hInfoFont,hBackFont;
	HDC				hdc;
	char			szBuf[80];
	static char		szName[80];
	PAINTSTRUCT		ps;
	int				i,j,k;
	SIZE			size;
	
	switch (iMsg)
	{
		case WM_CREATE:
			hdc=GetDC(hwnd);
			hInfoFont=EzCreateFont(hdc,"Arial",80,80,0,TRUE);
			hBackFont=EzCreateFont(hdc,"Arial",90,90,EZ_ATTR_BOLD,TRUE);
			ReleaseDC(hwnd,hdc);
			hLore=CreateWindow("edit",NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|
								WS_BORDER|ES_LEFT|ES_MULTILINE, //|ES_READONLY,
								0,0,0,0,hwnd,(HMENU)1,hInst,NULL);
			hGT=CreateWindow("edit",NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|
								WS_BORDER|ES_LEFT|ES_MULTILINE, //|ES_READONLY,
								0,0,0,0,hwnd,(HMENU)1,hInst,NULL);
			SendMessage(hLore,WM_SETFONT,(WPARAM)hInfoFont,0);
			SendMessage(hGT,WM_SETFONT,(WPARAM)hInfoFont,0);
			cx=cyChar+4;
			sprintf(szName,"I Thought They Smelled Bad On The Outside");
			return 0;

		case WM_SIZE:
			cxIClient=LOWORD(lParam);
			cyIClient=HIWORD(lParam);

			rLore.left=cx;
			rLore.right=cxIClient-cx;
			rLore.top=cyChar;
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
			SelectObject(hdc,hBackFont);

			sprintf(szBuf,"%s",masterlist[cardlist[iSel].idx].name);

			j=rLore.right-rLore.left;
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

			i=masterlist[cardlist[iSel].idx].icons[CTYPE];
			if (i) DrawBitmap(hdc,hIcons[i],2,2);
			i=masterlist[cardlist[iSel].idx].icons[CDEST];
			if (i) DrawBitmap(hdc,hIcons[i],cxIClient-18,2);
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

			SetWindowText(hGT,masterlist[cardlist[iSel].idx].desc);

			switch (masterlist[cardlist[iSel].idx].attr[ATT_TYPE])
			{
				case 'C':
					sprintf(szBuf,"Power %c",masterlist[cardlist[iSel].idx].attr[ATT_POWER]);
					TextOut(hdc,rLore.left,rGT.top-cyChar,szBuf,strlen(szBuf));
					if (masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]!='0')
					{
						sprintf(szBuf,"Ability %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
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
						default:  sprintf(szBuf,"Character"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'S':
					sprintf(szBuf,"Power %c",masterlist[cardlist[iSel].idx].attr[ATT_POWER]);
					TextOut(hdc,rLore.left,rGT.top-cyChar,szBuf,strlen(szBuf));
					if (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE]!='C')
						sprintf(szBuf,"Manv %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
					else
						sprintf(szBuf,"Armor %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
					DrawBitmap(hdc,hIcons[ICO_DEPLOY0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_DEPLOY])],
						2,cyIClient-18-20);
					DrawBitmap(hdc,hIcons[ICO_FORFEIT0+
						digiticon(masterlist[cardlist[iSel].idx].attr[ATT_FORFEIT])],
						2,cyIClient-18);
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
					sprintf(szBuf,"Power %c",masterlist[cardlist[iSel].idx].attr[ATT_POWER]);
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
								case 'X': sprintf(szBuf,"Exterior Site"); break;
								case 'N': sprintf(szBuf,"Interior Site"); break;
								case 'D': sprintf(szBuf,"Dejarik Holosite"); break;
								default: sprintf(szBuf,"Site"); break;
							}
							break;
						case 'V':
							sprintf(szBuf,"Mobile Site");
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

				case 'I':
					switch (masterlist[cardlist[iSel].idx].attr[ATT_SUBTYPE])
					{
						case 'L': sprintf(szBuf,"Lost Interrupt"); break;
						case 'U': sprintf(szBuf,"Used Interrupt"); break;
						case 'B': sprintf(szBuf,"Used or Lost Interrupt"); break;
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
						default:  sprintf(szBuf,"Weapon"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;

				case 'V':
					sprintf(szBuf,"Power %c",masterlist[cardlist[iSel].idx].attr[ATT_POWER]);
					TextOut(hdc,rLore.left,rGT.top-cyChar,szBuf,strlen(szBuf));
					sprintf(szBuf,"Armor %c",masterlist[cardlist[iSel].idx].attr[ATT_ABILITY]);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rGT.top-cyChar,szBuf,strlen(szBuf));
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
						default:  sprintf(szBuf,"Vehicle"); break;
					}
					SetTextAlign(hdc,TA_CENTER);
					TextOut(hdc,rLore.left+(rWidth(rLore))/2,rLore.bottom,szBuf,strlen(szBuf));
					break;



			}

			SelectObject(hdc,GetStockObject(SYSTEM_FONT));
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
					DialogBox(hInst,MAKEINTRESOURCE(IDD_HELPDLG),hDlg,HelpDlgProc);
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
