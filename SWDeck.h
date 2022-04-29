// SWDeck.h

// MAXLIST is the number of unique cards loadable.  There are just under 500
// cards in existence(as of ANH), so 1200 should be safe for some time to come.
// Any masterlist idx that = MAXLIST is an invalid card.
extern int MAXLIST;
#define MAXLIKELY 15000

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
	char *lore;			// pointer to lore text string in pCardData
	char *desc;			// pointer to game text string in pCardData
	BOOL bValid;		// TRUE=card is valid, FALSE=card is not valid

	char  szPrice[8];	// string representation of the card's value
	float fPrice;		// numerical representation of the card's value

	int	 rank;			// card's ranking in the current sort order
	int  icons[8];		// list of indexes into hIcons[], not the UINT id

};
struct mcard *masterlist;

struct card
{
	int	 idx;		// index into masterlist
	int  deck;		// qty in the deck
	int  wb;		// placeholder for combining wb
	int  binder;	// qty in binder
	BOOL bValid;
};
struct card *cardlist;
struct card *decklist;

#define MAXSETS 25
char sets[MAXSETS][30];	// defined in WM_CREATE from swccgdm.set: set names
						// and codes. sets[][0]==code, sets[][1]==name

HWND    hDlgStats;

int  OpenSWCCGData(HWND hwndProgress);
void CloseSWCCGData(void);
void ResetSWCCGData(void);

int  IncludeCList(int att,int c);
int  ExcludeCList(int att,int c,BOOL bLeave);

void ExportData(void);
void ImportData(void);
void CreateOldData(void);

void MsgError(HWND hwnd,int err,char *caption);

BOOL getsets(HWND hwnd);

void DefineICO(HANDLE hInst);
void DeleteICO(void);

//HBITMAP	hIcons[ICO_MAX];
HIMAGELIST hImageLists[10];
HBITMAP hBlankRow;
HBRUSH	hDarkBrush;
BOOL	bHighColorMode;
HBRUSH	hBr[8];
#define	BR_RED		0
#define BR_BLACK	1
#define BR_GREY		2
#define BR_BLUE		3
#define BR_GREEN	4
#define BR_PURPLE	5
#define BR_DKGREY	6
#define BR_DKGREEN  7

void DrawBitmap(HDC hdc,HBITMAP hBitmap,int xStart,int yStart);
void DrawBackBitmap(HDC hdc,HBITMAP hBitmap,int xStart, int yStart);
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
	BOOL	bFindName;
	BOOL	bFindLore;
	BOOL	bFindDesc;
	BOOL	bFindLight;
	BOOL	bFindDark;
	BOOL	bKeepFind;
	UINT	iStatsMode;
	UINT	iRangeMode;
	BOOL    bNoUnlimited;
	BOOL	bCombineWB;
	BOOL	bExcludeLight;
	BOOL	bExcludeDark;
	BOOL	bBinderQty;
	BOOL	bSaveStates;
} menu;

UINT	uSortOrder;
UINT	uPrevStats;

BOOL ToggleMenu(HMENU hMenu,UINT id,BOOL *state);

#define MAXSORT 16
char  szCustomSort[MAXSORT];
char  szSort[MAXSORT];

void newsort(char *sortkey);

int  updatedeck(HWND hDlg);
void parsename(char *newname, char *oldname);

RECT	rMainWin,rDeckWin,rInfoWin,rStatsWin;

char  szPriceGuideSource[512];
char  szPriceGuideDate[16];

BOOL  bPriceGuideChanged;

#define DATAFILEN  "SWCCG Cards.dm"
#define PRICEFILEN "SWCCG Price Guide.dm"
#define SETSFILEN  "SWCCG Sets.dm"

BOOL  CombineWB(void);
BOOL  LoadCombineWB(void);

void UpdateOnlyMenu(HWND);

char szApp[_MAX_PATH];
char szIni[_MAX_PATH];

void WriteMenuProfile(void);
void GetMenuProfile(HMENU);

#define MAXORDERSIZE 32

int	 iOrderMode;
#define ORDERTYPE 1
#define ORDERSETS 2

char szTypeOrder[32];

HBITMAP hPalBmp;

void getbool(char *szSect,char *szItem,BOOL *b);
