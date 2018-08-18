// SWDeck.h

#include <windows.h>

// MAXLIST is the number of unique cards loadable.  There are just under 500
// cards in existence(as of ANH), so 1200 should be safe for some time to come.
// Any masterlist idx that = MAXLIST is an invalid card.
#define MAXLIST			1200

#define MAXATTR			16
#define MAXNAME			60
#define MAXDESC			800

#define LIGHT_SIDE		1
#define DARK_SIDE		2
#define ALL_SIDES		3

#define VER_10_FORMAT	1
#define VER_20_FORMAT	2
#define VER_21_FORMAT	3
#define VER_30_FORMAT	4
#define	ALL_OK			5
#define CANT_OPEN_DATA	6
#define OUT_OF_MEMORY	7
#define CORRUPT_DATA	8
#define REACHED_MAX		9
#define INVALID_FORMAT	10

char *pCardData;		// a long string of the loaded swccgdm data
int iMListSize;			// number of masterlist entries in pCardData
int iCListSize;			// number of entries in the cardlist
int iDListSize;			// number of entries in the decklist

struct mcard
{
	char *attr;			// pointer to attribute string in pCardData
	char *name;			// pointer to card name string in pCardData
	char *desc;			// pointer to game text string in pCardData
	BOOL bValid;		// TRUE=card is valid, FALSE=card is not valid

	char  szPrice[8];	// string representation of the card's value
	float fPrice;		// numerical representation of the card's value

	int	 rank;			// card's ranking in the current sort order
	int  icons[7];		// list of indexes into hIcons[], not the UINT id

};
struct mcard masterlist[MAXLIST];

struct card
{
	int	 idx;
	int  deck;
	int  inventory;
	BOOL bValid;
};
struct card cardlist[MAXLIST];
struct card decklist[MAXLIST];

#define MAXSETS 16
char sets[MAXSETS][20];	// defined in WM_CREATE from swccgdm.set: set names
						// and codes. sets[][0]==code, sets[][1]==name

int  OpenSWCCGData(void);
void CloseSWCCGData(void);
void ResetSWCCGData(void);

int  IncludeCList(int att,int c);
int  ExcludeCList(int att,int c,BOOL bLeave);

void ExportData(void);
void ImportData(void);

void MsgError(HWND hwnd,int err,char *caption);

BOOL getsets(HWND hwnd);

void DefineICO(HANDLE hInst);
void DeleteICO(void);

#define NUMICONS 81
HBITMAP	hIcons[NUMICONS];
HBITMAP hBlankRow;
HBRUSH	hDarkBrush;
BOOL	bHighColorMode;
HBRUSH	hBr[7];
#define	BR_RED		0
#define BR_BLACK	1
#define BR_GREY		2
#define BR_BLUE		3
#define BR_GREEN	4
#define BR_PURPLE	5
#define BR_DKGREY	6

void DrawBitmap(HDC hdc,HBITMAP hBitmap,int xStart,int yStart);
void DrawCardIcons(HDC hdc,int card,int xStart,int yStart);

struct menustates
{
	BOOL	bCompressed;
	BOOL	bBold;
	BOOL	bTypeIcons;
	BOOL	bDestIcons;
	BOOL	bRareIcons;
	BOOL	bSetIcons;
	BOOL	bExtraIcons;
	BOOL	bColor;
	BOOL	bDarkShaded;
	BOOL	bSortSets;
	BOOL	bSortSides;
	BOOL	bUnlimited;
	BOOL	bDeckQty;
	BOOL	bDeckWindow;
	BOOL	bInfoWindow;
} menu;

UINT	uSortOrder;

BOOL ToggleMenu(HMENU hMenu,UINT id,BOOL *state);

#define MAXSORT 16
char  szCustomSort[MAXSORT];
char  szSort[MAXSORT];

void newsort(char *sortkey);

int  updatedeck(HWND hDlg);

static RECT	rMainWin,rDeckWin,rInfoWin;

char  szPriceGuideSource[256];
char  szPriceGuideDate[16];

BOOL  bPriceGuideChanged;
