// SWDeck.DLL

// this library contains all the necessary functions for
// loading and accesing the SW:CCG data

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include "SWDeck.h"
#include "SWFile.h"
#include "attributes.h"
#include "help.h"
#include "resource.h"
#include <commctrl.h>
#include "SWSpoilers.h"
#include "SWStats.h"

// Variables declared in SWDeck.h
char *pCardData;		// a long string of the loaded swccgdm data
int iMListSize;			// number of masterlist entries in pCardData
int iCListSize;			// number of entries in the cardlist
int iDListSize;			// number of entries in the decklist

struct mcard masterlist[MAXLIST];

struct card cardlist[MAXLIST];
struct card decklist[MAXLIST];
char sets[MAXSETS][30];	// defined in WM_CREATE from swccgdm.set: set names
						// and codes. sets[][0]==code, sets[][1]==name
HWND    hDlgStats;

HBITMAP	hIcons[NUMICONS];
HBITMAP hBlankRow;
HBRUSH	hDarkBrush;
BOOL	bHighColorMode;
HBRUSH	hBr[8];

struct menustates menu;

UINT	uSortOrder;
UINT	uPrevStats;

char  szCustomSort[MAXSORT];
char  szSort[MAXSORT];
RECT	rMainWin,rDeckWin,rInfoWin,rStatsWin;

char  szPriceGuideSource[512];
char  szPriceGuideDate[16];

BOOL  bPriceGuideChanged;

char szIni[_MAX_PATH];

int	 iOrderMode;

char szTypeOrder[32];

// End Variables declared in SWDeck.h


BOOL LoadPriceGuide(void);
BOOL SavePriceGuide(void);

#define ISNUMBER(x) (((x>='0')&&(x<='9'))?1:0)

char err_msg[10][20] = { "Version 1.0 format", "Version 2.0 format",
					   "Version 2.1 format", "Version 3.0 format",
					   "All OK", "Can't Open Data!", "Out of Memory",
					   "Corrupt Data File", "Reached MAXLIST",
					   "Invalid Format"};

// this function reads the swccgdm data files into the pCardData
// declared in SWDeck.h.  It then fills the masterlist[] with
// the pointers and defines iMListSize as the number of masterlist[]
// entries.  It also defines cardlist[] and iCListSize to include
// all cards in masterlist[].  Use ExcludeCList() and IncludeCList()
// too add/remove cards from the list.  This program also fills out
// the icons[] array in masterlist[], to speed up WM_PAINTs
int	OpenSWCCGData(HWND hwndSplash)
{
	int		iDataSize;
	int		iRot;
	int		iIdx;
	int		i,j;
	FILE	*fh;
	struct mcard cheatsheet;

	if (createworkfile(GetDlgItem(hwndSplash,IDC_LOADPROGRESS))==FALSE)
		return CANT_OPEN_DATA;

	if (iMListSize>0) CloseSWCCGData();

	if ((fh=fopen(DATAFILEN,"rb"))==NULL)
		return CANT_OPEN_DATA;

	iDataSize = FileLength(fh);

	if ((pCardData=(char *)malloc(iDataSize+1))==NULL)
	{
		fclose(fh);
		return OUT_OF_MEMORY;
	}

	// read all data from the file into pCardData
	fread(pCardData,1,(iDataSize-1),fh);
	fclose(fh);

	memset(&cheatsheet,0,sizeof(struct mcard));
	cheatsheet.rank = MAXLIST;

	for (i=0;i<MAXLIST;i++)
	{
		memcpy(&masterlist[i],&cheatsheet,sizeof(struct mcard));
	}

	masterlist[0].attr = pCardData;
	for (i=0,iIdx=0,iRot=1;(i<iDataSize)&&(iIdx<MAXLIST);i++)
	{
		if (*(pCardData+i)=='\0')
		{
			switch (iRot)
			{
				case 0:
					masterlist[iIdx].attr = pCardData+i+1;
					break;
				case 1:
					masterlist[iIdx].name = pCardData+i+1;
					break;
				case 2:
					masterlist[iIdx].lore = pCardData+i+1;
					break;
				case 3:
					masterlist[iIdx].desc = pCardData+i+1;
					masterlist[iIdx].rank = iIdx;
					masterlist[iIdx].bValid = TRUE;
					iIdx++;
					break;
			}
			iRot = (iRot+1)%4;
		}

		if (iIdx>=MAXLIST)
		{
			free(pCardData);
			fclose(fh);
			return REACHED_MAX;
		}
		iMListSize = iIdx;
	}

	for (i=iMListSize;i<MAXLIST;i++) masterlist[i].bValid = FALSE;

	// by default, fill out cardlist[] to include all of masterlist[]
	for (i=0;i<iMListSize;i++)
	{
		cardlist[i].idx = i;
		cardlist[i].deck = 0;
		cardlist[i].wb = 0;
		cardlist[i].binder = 0;
		cardlist[i].bValid = TRUE;
		decklist[i] = cardlist[i];
		sprintf(masterlist[i].szPrice,"---");
		masterlist[i].fPrice=(float)0.0;
	}
	iCListSize = i;
	iDListSize = 0;

	// this monolithic loop fills out all seven masterlist[].icons, so
	// WM_PAINT won't have to run this loop every time it displays a card
	// icons[] values are ICO_ index values into hIcons[]
	for (i=0;i<iMListSize;i++)
	{
		for (j=0;j<7;j++) masterlist[i].icons[j] = 0;

		// all cards have a rarity
		switch (masterlist[i].attr[ATT_RARITY])
		{
			case 'R':
				switch (masterlist[i].attr[ATT_SUBRARITY])
				{
				case '1': masterlist[i].icons[CRARITY] = ICO_R1; break;
				case '2': masterlist[i].icons[CRARITY] = ICO_R2; break;
				default:  masterlist[i].icons[CRARITY] = ICO_R0; break;
				}
				break;
			case 'U':
				switch (masterlist[i].attr[ATT_SUBRARITY])
				{
				case '1': masterlist[i].icons[CRARITY] = ICO_U1; break;
				case '2': masterlist[i].icons[CRARITY] = ICO_U2; break;
				default:  masterlist[i].icons[CRARITY] = ICO_U0; break;
				}
				break;
			case 'C':
				switch (masterlist[i].attr[ATT_SUBRARITY])
				{
				case '1': masterlist[i].icons[CRARITY] = ICO_C1; break;
				case '2': masterlist[i].icons[CRARITY] = ICO_C2; break;
				case '3': masterlist[i].icons[CRARITY] = ICO_C3; break;
				default:  masterlist[i].icons[CRARITY] = ICO_C0; break;
				}
				break;
			case 'F':
				masterlist[i].icons[CRARITY] = ICO_FIXED; break;
				break;
			default:
				masterlist[i].icons[CRARITY] = ICO_NA;
		}

		// all cards belong to a set. the "promotional" expansions all share
		// the same icon, so for simplicity if the set is not listed here,
		// it will get the "rebel leader" icon
		switch (masterlist[i].attr[ATT_SET])
		{
			case 'p': masterlist[i].icons[CSET] = ICO_LTD; break;
			case 'i': masterlist[i].icons[CSET] = ICO_2P; break;
			case 'U': masterlist[i].icons[CSET] = ICO_ULTD; break;
			case 'a': masterlist[i].icons[CSET] = ICO_ANH; break;
			case 'h': masterlist[i].icons[CSET] = ICO_HOTH; break;
			case 'E': masterlist[i].icons[CSET] = ICO_2P; break; // ESB 2P
			case 'd': masterlist[i].icons[CSET] = ICO_DAGOBAH; break;
			case 'c': masterlist[i].icons[CSET] = ICO_BESPIN; break;
			case 'J': masterlist[i].icons[CSET] = ICO_JABBA; break;
			default:  masterlist[i].icons[CSET] = ICO_RL; break;
		}

		// all non-location cards have a destiny
		if (masterlist[i].attr[ATT_TYPE]=='L')
			masterlist[i].icons[CDEST]=ICO_BLANK;
		else
		{
			switch (masterlist[i].attr[ATT_DESTINY])
			{
				case '0': masterlist[i].icons[CDEST]=ICO_DEST0; break;
				case '1': masterlist[i].icons[CDEST]=ICO_DEST1; break;
				case '2': masterlist[i].icons[CDEST]=ICO_DEST2; break;
				case '3': masterlist[i].icons[CDEST]=ICO_DEST3; break;
				case '4': masterlist[i].icons[CDEST]=ICO_DEST4; break;
				case '5': masterlist[i].icons[CDEST]=ICO_DEST5; break;
				case '6': masterlist[i].icons[CDEST]=ICO_DEST6; break;
				case '7': masterlist[i].icons[CDEST]=ICO_DEST7; break;
				case '*': masterlist[i].icons[CDEST]=ICO_DESTX; break;
				default:  masterlist[i].icons[CDEST]=ICO_BLANK; break;
			}
			switch (masterlist[i].attr[ATT_DESTINY2])
			{
				case '0': masterlist[i].icons[CDEST2]=ICO_DEST0; break;
				case '1': masterlist[i].icons[CDEST2]=ICO_DEST1; break;
				case '2': masterlist[i].icons[CDEST2]=ICO_DEST2; break;
				case '3': masterlist[i].icons[CDEST2]=ICO_DEST3; break;
				case '4': masterlist[i].icons[CDEST2]=ICO_DEST4; break;
				case '5': masterlist[i].icons[CDEST2]=ICO_DEST5; break;
				case '6': masterlist[i].icons[CDEST2]=ICO_DEST6; break;
				case '7': masterlist[i].icons[CDEST2]=ICO_DEST7; break;
				case '*': masterlist[i].icons[CDEST2]=ICO_DESTX; break;
				default:  masterlist[i].icons[CDEST2]=ICO_NONE; break;
			}
		}

		// here the type icon is determined, and anything pertinent to it
		switch (masterlist[i].attr[ATT_TYPE])
		{
			case 'L':
				// all locations have force icons
				masterlist[i].icons[CEXTRA1]=
					masterlist[i].attr[ATT_RFORCE]-'0'+ICO_RFORCE0;
				masterlist[i].icons[CEXTRA2]=
					masterlist[i].attr[ATT_IFORCE]-'0'+ICO_IFORCE0;
				// swap force icons if this is a Dark card
				if (masterlist[i].attr[ATT_SIDE]=='d')
				{
					j = masterlist[i].icons[CEXTRA1];
					masterlist[i].icons[CEXTRA1]=masterlist[i].icons[CEXTRA2];
					masterlist[i].icons[CEXTRA2]=j;
				}
				switch (masterlist[i].attr[ATT_SUBTYPE])
				{
					case 'P':
						masterlist[i].icons[CTYPE]=ICO_SYSTEM;
						break;
					case 'T':
						if (masterlist[i].attr[ATT_ASTRO]=='S')
							masterlist[i].icons[CEXTRA3]=ICO_SCOMP;
						switch (masterlist[i].attr[ATT_ICON])
						{
							case 'N': masterlist[i].icons[CTYPE]=ICO_INTSITE; break;
							case 'X': masterlist[i].icons[CTYPE]=ICO_EXTSITE; break;
							case 'D': masterlist[i].icons[CTYPE]=ICO_DEJARIK; break;
						}
						break;
					case 'M':
						masterlist[i].icons[CTYPE]=ICO_MOBILE;
						break;
					case 'V':
						masterlist[i].icons[CTYPE]=ICO_INTSITE;
						masterlist[i].icons[CEXTRA3]=ICO_VEHICLE;
						break;
					case 'S':
						masterlist[i].icons[CTYPE]=ICO_SECTOR;
						break;
				}
				break;

			case 'C':
				if (masterlist[i].attr[ATT_PILOT]=='Y')
					masterlist[i].icons[CEXTRA2]=ICO_PILOT;
				if (masterlist[i].attr[ATT_WARRIOR]=='Y')
					masterlist[i].icons[CEXTRA1]=ICO_WARRIOR;
				if (masterlist[i].attr[ATT_ASTRO]=='Y')
					masterlist[i].icons[CEXTRA1]=ICO_ASTRO;

				switch (masterlist[i].attr[ATT_ICON])
				{
					case 'A': masterlist[i].icons[CEXTRA3]=ICO_ALIEN; break;
					case 'W': masterlist[i].icons[CEXTRA3]=ICO_WARRIOR; break;
					case '2': masterlist[i].icons[CEXTRA3]=ICO_DEST2; break;
					case 'P': masterlist[i].icons[CDEST]=ICO_DESTPI; break;
					case 'L': masterlist[i].icons[CEXTRA3]=ICO_LEADER; break;
					case 'S': masterlist[i].icons[CEXTRA3]=ICO_SPY; break;
					case 'B': masterlist[i].icons[CEXTRA3]=ICO_BOUNTY; break;
				}

				switch (masterlist[i].attr[ATT_SUBTYPE])
				{
					case 'R': masterlist[i].icons[CTYPE]=ICO_REBEL; break;
					case 'I': masterlist[i].icons[CTYPE]=ICO_IMP; break;
					case 'D': masterlist[i].icons[CTYPE]=ICO_DROID; break;
					case 'A': masterlist[i].icons[CTYPE]=ICO_ALIEN; break;
					case 'J': masterlist[i].icons[CTYPE]=ICO_JEDIMAST; break;
				}
				break;

			case 'R':
				masterlist[i].icons[CTYPE]=ICO_CRITTER;
				break;

			case 'V':
				masterlist[i].icons[CTYPE]=ICO_VEHICLE;
				if (masterlist[i].attr[ATT_PPILOT]=='Y')
					masterlist[i].icons[CEXTRA2]=ICO_PILOT;
				if (masterlist[i].attr[ATT_ICON]=='E')
					masterlist[i].icons[CEXTRA3]=ICO_ENCLOSED;
				break;

			case 'S':
				masterlist[i].icons[CTYPE]=ICO_SHIP;
				if (masterlist[i].attr[ATT_ASTRO]=='Y')
					masterlist[i].icons[CEXTRA1]=ICO_ASTRO;
				if (masterlist[i].attr[ATT_PPILOT]=='Y')
					masterlist[i].icons[CEXTRA2]=ICO_PILOT;
				if (masterlist[i].attr[ATT_ICON]=='S')
					masterlist[i].icons[CEXTRA3]=ICO_SCOMP;
				if (masterlist[i].attr[ATT_ICON]=='I')
					masterlist[i].icons[CEXTRA3]=ICO_INDEPENDENT;
				break;

			case 'I':
				masterlist[i].icons[CTYPE]=ICO_INT;
				switch (masterlist[i].attr[ATT_SUBTYPE])
				{
					case 'L': masterlist[i].icons[CEXTRA1]=ICO_LOST; break;
					case 'B': masterlist[i].icons[CEXTRA1]=ICO_UORL; break;
					case 'S': masterlist[i].icons[CEXTRA1]=ICO_STARTING; break;
				}
				if (masterlist[i].attr[ATT_ICON]=='D')
					masterlist[i].icons[CEXTRA3]=ICO_DEJARIK;
				break;

			case 'E':
				masterlist[i].icons[CTYPE]=ICO_EFFECT;
				switch (masterlist[i].attr[ATT_SUBTYPE])
				{
					case 'U': masterlist[i].icons[CEXTRA1]=ICO_UTINNI; break;
					case 'I': masterlist[i].icons[CEXTRA1]=ICO_IMMED; break;
					case 'M': masterlist[i].icons[CEXTRA1]=ICO_MOBEFFECT; break;
				}
				if (masterlist[i].attr[ATT_ICON]=='D')
					masterlist[i].icons[CEXTRA3]=ICO_DEJARIK;
				break;

			case 'W':
				masterlist[i].icons[CTYPE]=ICO_WEAPON;
				switch (masterlist[i].attr[ATT_SUBTYPE])
				{
					case 'C':
						if (masterlist[i].attr[ATT_SIDE]=='L')
							masterlist[i].icons[CEXTRA3]=ICO_REBEL;
						else masterlist[i].icons[CEXTRA3]=ICO_IMP;
						break;
					case 'D':
						masterlist[i].icons[CEXTRA3]=ICO_MOBILE;
						break;
					case 'S':
						masterlist[i].icons[CEXTRA3]=ICO_SHIP;
						break;
					case 'V':
						masterlist[i].icons[CEXTRA3]=ICO_VEHICLE;
						break;
				}
				break;

			case 'D':
				masterlist[i].icons[CTYPE]=ICO_DEVICE;
				break;

			case 'N':
				masterlist[i].icons[CTYPE]=ICO_EVENT;
				break;
			
			case 'O':
				masterlist[i].icons[CTYPE]=ICO_OBJECTIVE;
				break;

			case 'J':
				masterlist[i].icons[CTYPE]=ICO_JEDITEST;
		}

	}

	LoadPriceGuide();

	return ALL_OK;

}

void CloseSWCCGData(void)
{
	if (iMListSize>0)
	{
		if (bPriceGuideChanged==TRUE) SavePriceGuide();
		iMListSize = 0;
		free(pCardData);
	}
}


// sets all data back to the original
void ResetSWCCGData(void)
{
	CloseSWCCGData();
	OpenSWCCGData(NULL);
}
		
void ImportData(void)
{
	FILE *fin,*fout;
	char szBuf1[80],szBuf2[80],szBuf3[500];

	fin = fopen("swccgdat.asc","r");
	fout = fopen(DATAFILEN,"wb");

	while (!feof(fin))
	{
		fgets(szBuf1,80,fin);
		if (!feof(fin))
		{
			szBuf1[strlen(szBuf1)-1]='\0';
			fgets(szBuf2,80,fin);
			szBuf2[strlen(szBuf2)-1]='\0';
			fgets(szBuf3,500,fin);
			szBuf3[strlen(szBuf3)-1]='\0';

			fprintf(fout,"%s",szBuf1);
			fputc(0,fout);
			fprintf(fout,"%s",szBuf2);
			fputc(0,fout);
			fprintf(fout,"%s",szBuf3);
			fputc(0,fout);
		}
	}

	fclose(fout);
	fclose(fin);

}

void ExportData(void)
{
	FILE	*fh;
	int		i;

	if (iMListSize<=0) return;
	
	fh = fopen("swccgdat.asc","wb");
	for (i=0;i<iMListSize;i++)
	{
		fprintf(fh,"%s\n%s\n%s\n",masterlist[i].attr,
								masterlist[i].name,
								masterlist[i].desc );
	}
	fclose(fh);

}

// this creates data files for the 2.11 version, which don't contain all
// the fields and have a slightly different way of labeling locations
void CreateOldData(void)
{
	FILE *fhLgt,*fhDrk,*fhErr,*fhWhich;
	int i;
	char szAttr[20],szName[80],szDesc[500];

	if (iMListSize<=0) return;

	fhLgt = fopen("swccgdat.lgt","wb");
	fhDrk = fopen("swccgdat.drk","wb");
	fhErr = fopen("swccgdat.err","wb");
	for (i=0;i<iMListSize;i++)
	{
		sprintf(szAttr,"%s",masterlist[i].attr);
		sprintf(szName,"%s",masterlist[i].name);
		sprintf(szDesc,"%s",masterlist[i].desc);

		if (szAttr[ATT_TYPE]=='L')
		{
			switch (szAttr[ATT_SUBTYPE])
			{
				case 'P': case 'M':
					szAttr[ATT_TYPE]='P';
					szAttr[ATT_SUBTYPE]='0';
					break;
				default:
					szAttr[ATT_TYPE]='T';
					szAttr[ATT_SUBTYPE]='0';
					break;
			}
		}

		switch (szAttr[ATT_SIDE])
		{
			case 'l': fhWhich = fhLgt; break;
			case 'd': fhWhich = fhDrk; break;
			default:  fhWhich = fhErr; break;
		}

		if (szAttr[ATT_SET]!='U')
		{
			szAttr[ATT_SIDE]='\0';
			fprintf(fhWhich,"%s",szAttr);
			fputc(0,fhWhich);
			fprintf(fhWhich,"%s",szName);
			fputc(0,fhWhich);
			fprintf(fhWhich,"%s",szDesc);
			fputc(0,fhWhich);
		}
	}
			

	fclose(fhErr);
	fclose(fhLgt);
	fclose(fhDrk);

}

int cardvalid(int idx)
{
	if (cardlist[idx].bValid==TRUE) return 0;
	else return MAXLIST;
}

void sortcardlist(void)
{
	int i,k;
	struct card tmpCard;

	if (iCListSize>1)
	{
		do
		{
			k = 0;
			for (i=0;i<(iCListSize-1);i++)
			{
				if ( (masterlist[cardlist[i].idx].rank+cardvalid(i)) >
					(masterlist[cardlist[i+1].idx].rank+cardvalid(i+1)) )
				{
					tmpCard = cardlist[i];
					cardlist[i] = cardlist[i+1];
					cardlist[i+1] = tmpCard;
					k=1;
				}
			}
		} while (k);

		for (i=0;i<iCListSize;i++)
			if (cardlist[i].bValid==FALSE) iCListSize = i;
	}
}

// in only one case will cardlist[] need to be sorted among
// the entire masterlist[]: in the Include functions.  To
// improve speed, the normal sortcardlist() will only go up
// to iCListSize.  In the Include special case, change
// iCListSize to encompass all cards.  (iCListSize is
// redefined in sortcardlist()
void msortcardlist(void)
{
	iCListSize=iMListSize;
	sortcardlist();
}

// the opposite of ExcludeCList, this function puts cards back into the
// cardlist if att==c, or if c=='*' (to include all back in)
int IncludeCList(int att,int c)
{
	int i,j,cAdded;

	for (i=0,cAdded=0;i<iMListSize;i++)
		if ((masterlist[i].attr[att]==c)||(c=='*'))
			for (j=0;j<iMListSize;j++)
				if ((cardlist[j].idx==i)&&(cardlist[j].bValid==FALSE))
				{
					cardlist[j].bValid=TRUE;
					j=iMListSize;
					cAdded++;
				}

	msortcardlist();

	return cAdded;
}	

// removes(sets bValid to FALSE) every card in cardlist[] whose
// attribute 'att'=='c'.  Then resorts the list.  Returns the number
// of cards excluded.
int ExcludeCList(int att,int c,BOOL bLeave)
{
	int i,cKilled;

	for (i=0,cKilled=0;i<iCListSize;i++)
	{
		if ((masterlist[cardlist[i].idx].attr[att]==c)&&
			!((bLeave==TRUE)&&(cardlist[i].deck)))
		{
			cardlist[i].bValid = FALSE;
			cKilled++;
		}
	}

	sortcardlist();

	return cKilled;

}

void MsgError(HWND hwnd,int err,char *caption)
{
	MessageBox(hwnd,err_msg[err-1],caption,MB_ICONERROR);
}

BOOL getsets(HWND hwnd)
{
	FILE *fh;
	int	 i;
	char szBuf[30];

	if ((fh=fopen(SETSFILEN,"r"))==NULL)
	{
		fh = fopen(SETSFILEN,"w");
		fprintf(fh,"pPremiere\n");
		fprintf(fh,"UUnlimited Premiere\n");
		fprintf(fh,"aA New Hope\n");
		fprintf(fh,"AUnlimited A New Hope\n");
		fprintf(fh,"hHoth\n");
		fprintf(fh,"HUnlimited Hoth\n");
		fprintf(fh,"dDagobah\n");
		fprintf(fh,"cCloud City\n");
		fprintf(fh,"JJabba's Palace\n");
		fprintf(fh,"1First Anthology (Unlimited SE)\n");
		fprintf(fh,"sSpecial Edition\n");
		fprintf(fh,"PEnhanced Premiere Pack\n");
		fprintf(fh,"iPremiere 2-Player\n");
		fprintf(fh,"EESB 2-Player\n");
		fprintf(fh,"rRebel Leader\n");
		fprintf(fh,"jJedi Pack\n");
		fprintf(fh,"SOfficial Sealed Deck\n");
		fprintf(fh,"eEndor\n");
		fprintf(fh,"DDeath Star\n");
		fprintf(fh,"\n");
		fclose(fh);
		if ((fh=fopen(SETSFILEN,"r"))==NULL)
		{
			MessageBox(hwnd,"Can't create SWCCG Sets.dm\r\t","DeckMaker Aborting",MB_OK);
			return FALSE;
		}
	}

	for (i=0;i<MAXSETS;i++)
	{
		fgets(szBuf,29,fh);
		if (!feof(fh)&&(strlen(szBuf)>2)&&(szBuf[0]>='0')&&(szBuf[0]<='z'))
		{
			if (szBuf[strlen(szBuf)-1]=='\n') szBuf[strlen(szBuf)-1]='\0';
			sprintf(sets[i],szBuf);
		}
		else sets[i][0]='\0';
	}

	fclose(fh);

	return TRUE;
}

void DefineICO(HANDLE hInst)
{
	hIcons[0]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BLANK));
	hIcons[1]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BLANK));
	hIcons[2]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST0));
	hIcons[3]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST1));
	hIcons[4]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST2));
	hIcons[5]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST3));
	hIcons[6]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST4));
	hIcons[7]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST5));
	hIcons[8]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST6));
	hIcons[9]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEST7));
	hIcons[10]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SYSTEM));
	hIcons[11]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_INTERIOR));
	hIcons[12]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_EXTERIOR));
	hIcons[13]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_MOBILE));
	hIcons[14]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEJARIK));
	hIcons[15]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_REBEL));
	hIcons[16]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMPERIAL));
	hIcons[17]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DROID));
	hIcons[18]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ALIEN));
	hIcons[19]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_CREATURE));
	hIcons[20]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_VEHICLE));
	hIcons[21]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SHIP));
	hIcons[22]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_INTERRUPT));
	hIcons[23]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_UORL));
	hIcons[24]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_EFFECT));
	hIcons[25]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_UTINNI));
	hIcons[26]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMMEDIATE));
	hIcons[27]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORCE0));
	hIcons[28]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_RFORCE1));
	hIcons[29]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_RFORCE2));
	hIcons[30]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_RFORCE3));
	hIcons[31]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORCE0));
	hIcons[32]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IFORCE1));
	hIcons[33]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IFORCE2));
	hIcons[34]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IFORCE3));
	hIcons[35]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_WEAPON));
	hIcons[36]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEVICE));
	hIcons[37]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_PILOT));
	hIcons[38]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ASTRO));
	hIcons[39]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_WARRIOR));
	hIcons[40]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_R1));
	hIcons[41]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_R2));
	hIcons[42]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_U1));
	hIcons[43]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_U2));
	hIcons[44]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_C1));
	hIcons[45]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_C2));
	hIcons[46]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_C3));
	hIcons[47]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_NA));
	hIcons[48]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SCOMP));
	hIcons[49]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_LIMITED));
	hIcons[50]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_UNLIMITED));
	hIcons[51]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_PARKERBROS));
	hIcons[52]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ANH));
	hIcons[53]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_REBELLEADER));
	hIcons[54]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_EVENT));
	hIcons[55]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_LOST));
	hIcons[56]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_HOTH));
	hIcons[57]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DESTPI));
	hIcons[58]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DESTX));
	hIcons[59]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY0));
	hIcons[60]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY1));
	hIcons[61]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY2));
	hIcons[62]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY3));
	hIcons[63]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY4));
	hIcons[64]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY5));
	hIcons[65]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY6));
	hIcons[66]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY7));
	hIcons[67]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY8));
	hIcons[68]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY9));
	hIcons[69]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOYX));
	hIcons[70]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT0));
	hIcons[71]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT1));
	hIcons[72]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT2));
	hIcons[73]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT3));
	hIcons[74]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT4));
	hIcons[75]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT5));
	hIcons[76]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT6));
	hIcons[77]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT7));
	hIcons[78]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT8));
	hIcons[79]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT9));
	hIcons[80]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEITX));
	hIcons[81]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_R0));
	hIcons[82]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_U0));
	hIcons[83]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_C0));
	hIcons[84]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DAGOBAH));
	hIcons[85]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BESPIN));
	hIcons[86]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_JABBA));
	hIcons[87]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_JEDITEST));
	hIcons[88]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_JEDIMASTER));;
	hIcons[89]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_STARTING));
	hIcons[90]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_INDEPENDENT));
	hIcons[91]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SECTOR));
	hIcons[92]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_MOBEFFECT));
	hIcons[93]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SPY));
	hIcons[94]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_LEADER));
	hIcons[95]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BOUNTY));
	hIcons[96]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DEPLOY15));
	hIcons[97]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FORFEIT15));
	hIcons[98]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ENCLOSED));

	hIcons[99]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_LSBACK));
	hIcons[100]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DSBACK));

	hIcons[101]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_OBJECTIVE));

	hIcons[102]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_FIXED));
	
	hBr[BR_RED] = CreateSolidBrush(RGB_RED);
	hBr[BR_BLACK] = CreateSolidBrush(RGB_BLACK);
	hBr[BR_GREY] = CreateSolidBrush(RGB_GREY);
	hBr[BR_BLUE] = CreateSolidBrush(RGB_BLUE);
	hBr[BR_GREEN] = CreateSolidBrush(RGB_GREEN);
	hBr[BR_PURPLE] = CreateSolidBrush(RGB_PURPLE);
	hBr[BR_DKGREY] = CreateSolidBrush(RGB_DKGREY);
	hBr[BR_DKGREEN] = CreateSolidBrush(RGB_DKGREEN);

	hBlankRow=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BLANKROW));

}

void DeleteICO(void)
{
	int i;
	DeleteObject(hDarkBrush);
	DeleteObject((HGDIOBJ)hBlankRow);
	for (i=0;i<8;i++) DeleteObject((HGDIOBJ)hBr[i]);
	for (i=0;i<NUMICONS;i++) DeleteObject((HGDIOBJ)hIcons[i]);
}

void DrawBitmap(HDC hdc,HBITMAP hBitmap,int xStart,int yStart)
{
//	BITMAP	bm;
	HDC		hdcMem;

	hdcMem=CreateCompatibleDC(hdc);
	SelectObject(hdcMem,hBitmap);
	SetMapMode(hdcMem,GetMapMode(hdc));

	//GetObject(hBitmap,sizeof(BITMAP),(LPVOID)&bm);

	BitBlt(hdc,xStart,yStart,16,16,hdcMem,0,0,SRCCOPY);

	DeleteDC(hdcMem);
}

void DrawBackBitmap(HDC hdc,HBITMAP hBitmap,int xStart,int yStart)
{
//	BITMAP	bm;
	HDC		hdcMem;

	hdcMem=CreateCompatibleDC(hdc);
	SelectObject(hdcMem,hBitmap);
	SetMapMode(hdcMem,GetMapMode(hdc));

	//GetObject(hBitmap,sizeof(BITMAP),(LPVOID)&bm);

	BitBlt(hdc,xStart,yStart,16,22,hdcMem,0,0,SRCCOPY);

	DeleteDC(hdcMem);
}

void DrawCardIcons(HDC hdc,int card,int xStart,int yStart)
{
	HDC		hdcMemIco[7];
	HDC		hdcMemAll;
	int		i;

	for (i=0;i<7;i++)
	{
		hdcMemIco[i]=CreateCompatibleDC(hdc);
		SelectObject(hdcMemIco[i],hIcons[masterlist[card].icons[i]]);
	}

	hdcMemAll=CreateCompatibleDC(hdc);
	SelectObject(hdcMemAll,hBlankRow);

	for (i=0;i<7;i++)
	{
		if (masterlist[card].icons[i])
		{
			BitBlt(hdcMemAll,i*17,0,16,16,hdcMemIco[i],0,0,SRCCOPY);
		}
	}

	BitBlt(hdc,xStart,yStart,119,16,hdcMemAll,0,0,SRCCOPY);


	DeleteDC(hdcMemAll);
	for (i=0;i<7;i++)
		DeleteDC(hdcMemIco[i]);
}

BOOL ToggleMenu(HMENU hMenu,UINT id,BOOL *state)
{
	if (*state==TRUE)
	{
		*state=FALSE;
		CheckMenuItem(hMenu,id,MF_UNCHECKED);
	}
	else
	{
		*state=TRUE;
		CheckMenuItem(hMenu,id,MF_CHECKED);
	}
	return *state;
}

// fills *key with a string weighted based on the
// passed *sort.  The characters in *sort must be
// one of the following, or the sort name is assumed
// S - side		T - set		R - rarity	D - destiny
// Y - type		Z - subtype	P - power	A - ability
// L - deploy	F - forfeit	C - price   V - Value
// 2 - Subrarity
void createkey(char *oldkey,char *sort,int idx)
{
	unsigned int i,j,k,x;
	char	key[25];

	if (!masterlist[idx].bValid||!strlen(sort))
	{
		key[0]='\0';
	}
	else
	{

	for (i=0,k=0;i<strlen(sort);i++)
	{
		switch (sort[i])
		{
			case 'V':
				sprintf(key+k,"%3.2f",(float)900.0-masterlist[idx].fPrice);
				k+=6;
				break;

			case 'S':
				switch (masterlist[idx].attr[ATT_SIDE])
				{
					case 'l': key[k]='A'; break;
					case 'd': key[k]='B'; break;
					default:  key[k]='C'; break;
				}
				k++;
				break;

			case 'T':
				key[k]='\0';
				for (j=0;(j<MAXSETS)&&(key[k]=='\0');j++)
				{
					if (sets[j][0]==masterlist[idx].attr[ATT_SET])
						key[k]='A'+j;
				}
				if (key[k]=='\0') key[k]='Z';
				k++;
				break;

			case 'R':
				switch (masterlist[idx].attr[ATT_RARITY])
				{
					case 'R': key[k]='A'; break;
					case 'U': key[k]='B'; break;
					case 'C': key[k]='C'; break;
					default:  key[k]='D'; break;
				}
				k++;
				break;

			case '2':
				key[k]=masterlist[idx].attr[ATT_SUBRARITY];
				k++;
				break;

			case 'D':
				key[k]=masterlist[idx].attr[ATT_DESTINY];
				k++;
				break;
			
			case 'Y':
				for (x=0;x<strlen(szTypeOrder);x++)
				{
					if (masterlist[idx].attr[ATT_TYPE]==szTypeOrder[x])
					{
						key[k]='A'+x;
						x=99;
					}
				}
				if (x<99) key[k]='Z';
				k++;
				break;

			case 'Z':
				key[k]=masterlist[idx].attr[ATT_SUBTYPE];
				k++;
				break;

			case 'P':
				if (ISNUMBER(masterlist[idx].attr[ATT_POWER]))				
					key[k]=':'-masterlist[idx].attr[ATT_POWER];
				else key[k]='0';
				k++;
				break;

			case 'A':
				if (ISNUMBER(masterlist[idx].attr[ATT_ABILITY]))
					key[k]=':'-masterlist[idx].attr[ATT_ABILITY];
				else key[k]='0';
				k++;
				break;

			case 'L':
				key[k]=masterlist[idx].attr[ATT_DEPLOY];
				k++;
				break;

			case 'F':
				key[k]=masterlist[idx].attr[ATT_FORFEIT];
				k++;
				break;

			default:
				key[k]='Z';
				k++;
				break;
		}
	}
	key[k]='\0';

	}

	sprintf(oldkey,"%s",key);

}

void sortclist(void)
{
	int i,k;
	struct card swapcard;

	do
	{
		for (i=0,k=0;i<iCListSize-1;i++)
		{
			if (masterlist[cardlist[i].idx].rank>
				masterlist[cardlist[i+1].idx].rank)
			{
				swapcard = cardlist[i];
				cardlist[i] = cardlist[i+1];
				cardlist[i+1] = swapcard;
				k=1;
			}
		}
	} while (k);

}

void newsort(char *sort)
{
	int i,j,k;
	int tmpRank[MAXLIST];	// temporary list of ranks
	char key[MAXLIST][MAXSORT+80];		// sort sequence string
	char szKey[25];
	HCURSOR hCursor;

	if (iMListSize>1)
	{
	
	hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
	ShowCursor(TRUE);								// make the pointer the "hourglass"

	// copy rankings into a temporary array
	for (i=0;i<MAXLIST;i++)
	{
		tmpRank[i]=i;
		createkey(szKey,sort,i);
		sprintf(key[i],"%s%s",szKey,masterlist[i].name);
	}

	do
	{
		for (i=0,k=0;(i<(iMListSize-1));i++)
		{
			if (_stricmp(key[tmpRank[i]],key[tmpRank[i+1]])>0)
			{
				j=tmpRank[i];
				tmpRank[i]=tmpRank[i+1];
				tmpRank[i+1]=j;
				k=1;
			}
		}
	} while (k);

	for (i=0;i<iMListSize;i++)
	{
		masterlist[tmpRank[i]].rank=i;
	}

	sortcardlist();

	ShowCursor(FALSE); 	// restore cursor to original shape
	SetCursor(hCursor);

	}

}

void calcRange(char *szBuf,int att)
{
	int i,k,ch,total;
	int low,med,high;

	if (iCListSize>0)
	{
		low=999; med=0; high=0;
		for (i=0,k=0,total=0;i<iCListSize;i++)
		{
			if (cardlist[i].deck)
			{
				ch = masterlist[cardlist[i].idx].attr[att]-'0';
				if ((ch<0)||(ch>9)) ch=0;
				if (ch<low) low=ch;
				if (ch>high) high=ch;
				total += ch*cardlist[i].deck;
				k += cardlist[i].deck;
			}
		}
		if (k)
		{
			med = (total*10/k);
			sprintf(szBuf,"%d/%d.%d/%d",low,med/10,med%10,high);
		}
		else sprintf(szBuf,"---");
	}
	else sprintf(szBuf,"---");

}



// decklist[] is used only to maintain the hDlgDeck IDC_DECKLIST
// listbox.  To improve speed, simply scan through and recreate
// it everytime it's possibly updated.
int updatedeck(HWND hDlg)
{
	int		i,total,iTop;
	char	szBuf[100];
	HWND	hList,hCards;

	if (!hDlg) return 0; // leave if hDlgDeck isn't visible

	hList = GetDlgItem(hDlg,IDC_DECKLIST);
	hCards = GetDlgItem(hDlg,IDC_DECKCARDS);

	iTop = SendMessage(hList,LB_GETTOPINDEX,0,0);

	if (!iCListSize)
	{
		SendMessage(hList,LB_RESETCONTENT,0,0);
		return 0;
	}

	for (i=0,iDListSize=0,total=0;i<iCListSize;i++)
	{
		// if this card is populated, add it to decklist
		if ((cardlist[i].deck>0)&&(cardlist[i].bValid))
		{
			decklist[iDListSize]=cardlist[i];
			total += decklist[iDListSize].deck;
			iDListSize++;
		}

	}

	SendMessage(hList,WM_SETREDRAW,FALSE,0);
	SendMessage(hList,LB_RESETCONTENT,0,0);
	
	for (i=0;i<iDListSize;i++)
	{
		sprintf(szBuf,"%d\t%s",decklist[i].deck,masterlist[decklist[i].idx].name);
		SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szBuf);
	}
	SendMessage(hList,LB_SETTOPINDEX,iTop,0);
	SendMessage(hList,WM_SETREDRAW,TRUE,0);

	sprintf(szBuf,"Cards: %d",total);
	SetWindowText(hCards,szBuf);

	return total;
}

void parsename(char *newname, char *oldname)
{
	char szBuf[80],szUnique[16];
	unsigned int  i,k;

	// move any unique dots (· or ×) to the beginning
	sprintf(szBuf,oldname);
	for (i=0,k=0;i<strlen(szBuf);i++)
	{
		if ((szBuf[i]=='·')||(szBuf[i]=='×'))
		{
			szBuf[i-1]='\0';
			sprintf(szUnique,"%s",szBuf+i);
			sprintf(newname," %s %s",szUnique,szBuf);
			k=1;
		}
	}
	if (!k) sprintf(newname," %s",oldname);

}

int setoccurance(int ch)
{
	int iResult,i;

	iResult=0;
	for (i=0,iResult=0;i<MAXSETS;i++)
		if (ch==sets[i][0]) iResult = i;

	return iResult;
}


int getidx(int x)
{
	int i,iResult;
	for (i=0,iResult=0;i<iMListSize;i++)
		if (cardlist[i].idx==x)
		{
			iResult=i;
			i=iMListSize;
		}

	return iResult;
}

/*BOOL CombineWB()
{
	int i=0,idx=0;
	BOOL k;
	
	while (i < iMListSize)
	{
		if (masterlist[i].attr[ATT_LIMITED] != '0') {
			idx = matchcard(masterlist[i].name,masterlist[i].attr[ATT_SIDE],masterlist[i].attr[ATT_LIMITED]);
			//jdx = i;
			if (idx <0) continue;
	
			cardlist[idx].wb = cardlist[idx].deck;
			cardlist[idx].deck += cardlist[i].deck;
			cardlist[i].wb = cardlist[i].deck;
			cardlist[i].deck = 0;
			cardlist[i].bValid = FALSE;
		}
	}

	msortcardlist();
	return TRUE;
}*/



BOOL CombineWB(void)
{
	int i,j,idx,jdx;
	BOOL k;

	if (!iCListSize) return FALSE;

	k = FALSE;

	// if turning combine off, then bring those cards back
	if (!menu.bCombineWB)
	{
		for (i=iMListSize-1;i;i--)
		{
			for (j=0;j<i;j++)
			{
			if ((strcmp(masterlist[i].name,masterlist[j].name)==0)&&
				(masterlist[i].attr[ATT_SIDE]==masterlist[j].attr[ATT_SIDE])&&
				(masterlist[i].attr[ATT_LIMITED]==masterlist[j].attr[ATT_SET]||
				 masterlist[i].attr[ATT_SET]==masterlist[j].attr[ATT_SET])&&
				(masterlist[i].attr[ATT_LIMITED]!='0'||
				 masterlist[j].attr[ATT_LIMITED]!='0'))
				{
					idx = getidx(i);
					jdx = getidx(j);
					//if (setoccurance(masterlist[i].attr[ATT_SET])
					//	<setoccurance(masterlist[j].attr[ATT_SET]))
					if (masterlist[j].attr[ATT_LIMITED]!='0')
					{
						// i comes before j
						cardlist[idx].deck = cardlist[idx].wb;
						cardlist[idx].wb = 0;
						cardlist[jdx].deck = cardlist[jdx].wb;
						cardlist[jdx].wb = 0;
						if (!(((masterlist[j].attr[ATT_SIDE]=='l')&&(menu.bExcludeLight))||
							((masterlist[j].attr[ATT_SIDE]=='d')&&(menu.bExcludeDark))))
						{
							cardlist[jdx].bValid = TRUE;
						}
						k = TRUE;
					}
					else
					{
						// j comes before i
						cardlist[jdx].deck = cardlist[jdx].wb;
						cardlist[jdx].wb = 0;
						cardlist[idx].deck = cardlist[idx].wb;
						cardlist[idx].wb = 0;
						if (!(((masterlist[i].attr[ATT_SIDE]=='l')&&(menu.bExcludeLight))||
							((masterlist[i].attr[ATT_SIDE]=='d')&&(menu.bExcludeDark))))
						{
							cardlist[idx].bValid = TRUE;
						}
						k = TRUE;
					}
				}
			}
		}
		msortcardlist();

		return k;
	}
	for (i=iMListSize-1;i>-1;i--)
	{
		for (j=0;j<i;j++)
			if ((strcmp(masterlist[i].name,masterlist[j].name)==0)&&
				(masterlist[i].attr[ATT_SIDE]==masterlist[j].attr[ATT_SIDE])&&
				(masterlist[i].attr[ATT_LIMITED]==masterlist[j].attr[ATT_SET]||
				 masterlist[i].attr[ATT_SET]==masterlist[j].attr[ATT_SET])&&
				(masterlist[i].attr[ATT_LIMITED]!='0'||
				 masterlist[j].attr[ATT_LIMITED]!='0'))
			{
				idx = getidx(i);
				jdx = getidx(j);
//				if (setoccurance(masterlist[i].attr[ATT_SET])
//					<setoccurance(masterlist[j].attr[ATT_SET]))
				if (masterlist[j].attr[ATT_LIMITED]!='0')
				{
					// i comes before j
					cardlist[idx].wb = cardlist[idx].deck;
					cardlist[idx].deck += cardlist[jdx].deck;
					cardlist[jdx].wb = cardlist[jdx].deck;
					cardlist[jdx].deck = 0;
					cardlist[jdx].bValid = FALSE;
					k = TRUE;
				}
				else
				{
					// j comes before i
					cardlist[jdx].wb = cardlist[jdx].deck;
					cardlist[jdx].deck += cardlist[idx].deck;
					cardlist[idx].wb = cardlist[idx].deck;
					cardlist[idx].deck = 0;
					cardlist[idx].bValid = FALSE;
					k = TRUE;
				}
			}
	}

	msortcardlist();

	return k;
}

BOOL LoadCombineWB(void)
{
	int i,j,idx,jdx;
	BOOL k;

	if (!iCListSize) return FALSE;

	k = FALSE;

	for (i=iMListSize-1;i;i--)
	{
		for (j=0;j<i;j++)
//			if ((strcmp(masterlist[i].name,masterlist[j].name)==0)&&
//				(masterlist[i].attr[ATT_SIDE]==masterlist[j].attr[ATT_SIDE])&&
//				(masterlist[i].attr[ATT_LIMITED]==masterlist[j].attr[ATT_SET]||
//				 masterlist[i].attr[ATT_SET]==masterlist[j].attr[ATT_SET])&&
//				(masterlist[i].attr[ATT_LIMITED]!='0'||
//				 masterlist[j].attr[ATT_LIMITED]!='0'))
			if ((strcmp(masterlist[i].name,masterlist[j].name)==0)&&
				(masterlist[i].attr[ATT_SIDE]==masterlist[j].attr[ATT_SIDE]))
			{
				idx = getidx(i);
				jdx = getidx(j);
				if (setoccurance(masterlist[i].attr[ATT_SET])
					<setoccurance(masterlist[j].attr[ATT_SET]))
				if (masterlist[j].attr[ATT_LIMITED]!='0')
				{
					// i comes before j
					cardlist[jdx].bValid = FALSE;
					k = TRUE;
				}
				else
				{
					// j comes before i
					cardlist[idx].bValid = FALSE;
					k = TRUE;
				}
			}
	}
	msortcardlist();

	return k;
}

void UpdateOnlyMenu(HWND hwnd)
{
	HMENU hMenu;

	hMenu = GetMenu(hwnd);
	if (menu.bExcludeLight)
	{
		CheckMenuItem(hMenu,ID_VIEW_LSONLY,MF_UNCHECKED);
		CheckMenuItem(hMenu,ID_VIEW_DSONLY,MF_CHECKED);
		CheckMenuItem(hMenu,ID_VIEW_BOTHONLY,MF_UNCHECKED);
	}
	else if (menu.bExcludeDark)
	{
		CheckMenuItem(hMenu,ID_VIEW_LSONLY,MF_CHECKED);
		CheckMenuItem(hMenu,ID_VIEW_DSONLY,MF_UNCHECKED);
		CheckMenuItem(hMenu,ID_VIEW_BOTHONLY,MF_UNCHECKED);
	}
	else
	{
		CheckMenuItem(hMenu,ID_VIEW_LSONLY,MF_UNCHECKED);
		CheckMenuItem(hMenu,ID_VIEW_DSONLY,MF_UNCHECKED);
		CheckMenuItem(hMenu,ID_VIEW_BOTHONLY,MF_CHECKED);
	}

}

/*BOOL LoadPriceGuide(void)
{
	FILE *fh;
	char szBuf[80];
	int  i;

	if (iMListSize<1) return FALSE;


	while (!feof(fh))
	{
		stread(fh,szBuf,6);
		if (!feof(fh))
		{
			i=atoi(szBuf);
			if ((i<iMListSize)&&(i>=0))
			{
				stread(fh,masterlist[i].szPrice,9);
				masterlist[i].fPrice=(float)atof(masterlist[i].szPrice);
			}
		}
	}

	fclose(fh);

	return TRUE;
}

BOOL SavePriceGuide(void)
{
	FILE *fh;
	char szBuf[80];
	int  i;

	if (iMListSize<1) return FALSE;

	if ((fh=fopen(PRICEFILEN,"w"))!=NULL)
	{
		fh=fopen(PRICEFILEN,"w");
		stwrite(fh,"SWCCGDMPG3.00");
		stwrite(fh,szPriceGuideSource);
		stwrite(fh,szPriceGuideDate);
		for (i=0;i<iMListSize;i++)
		{
			if (masterlist[i].fPrice>(float)0.0)
			{
				sprintf(szBuf,"%i",i);
				stwrite(fh,szBuf);
				stwrite(fh,masterlist[i].szPrice);
			}
		}
		fclose(fh);
		return TRUE;
	}

	return FALSE;
}
*/
BOOL SavePriceGuide(void)
{
	FILE *fh;
	int i;
	unsigned int j,k;
	char szTmp[256];

	if ((fh=fopen(PRICEFILEN,"w"))==NULL)
		return FALSE;

	fprintf(fh,"%s\n",szPriceGuideDate);

	for (i=0;i<iMListSize;i++)
	{
		if (masterlist[i].fPrice>0.0)
		{
			sprintf(szTmp,"%s",masterlist[i].name);
			for (j=0,k=0;j<strlen(szTmp);j++)
			{
				if (((szTmp[j]=='×')||(szTmp[j]=='·'))&&(j))
				{
					szTmp[j-1]='\0';
					k=1;
				}
			}
			if (!k) szTmp[j-1]='\0';
			fprintf(fh,"%s;%c;%c;%s\n",szTmp,
				//(masterlist[i].attr[ATT_SIDE]=='L')?'l':'d',
				masterlist[i].attr[ATT_SIDE],
				masterlist[i].attr[ATT_SET],masterlist[i].szPrice);
		}
	}
	fprintf(fh,"ENDCARDS\n");
	fprintf(fh,"%s",szPriceGuideSource);
	fclose(fh);
	return TRUE;
}


BOOL LoadPriceGuide(void)
{
	unsigned int i,j;
	char szBuf[4096],szName[256],szTmp[4096];
	int  cSide,cSet;
	int	 idx;
	FILE *fh;

	if ((fh=fopen(PRICEFILEN,"r"))==NULL)
	{
		if ((fh=fopen("SWCCG Price Guide.dm","w"))!=NULL)
		{
			sprintf(szPriceGuideDate,"n/a");
			fprintf(fh,"%s\n",szPriceGuideDate);
			fprintf(fh,"ENDCARDS\n");
			sprintf(szPriceGuideSource,"No source information available");
			fprintf(fh,"%s",szPriceGuideSource);
		}
		return FALSE;
	}

	fgets(szBuf,4096,fh); // get first line (header)
	for (i=0;i<14;i++)
	{
		if (szBuf[i]=='\n') szBuf[i]='\0';
		szPriceGuideDate[i]=szBuf[i];
	}
	szPriceGuideDate[i]='\0';
	
	// now read the cards one at a time
	while (!feof(fh))
	{
		fgets(szBuf,4096,fh);	// read the line

		if (!feof(fh))
		{
		for (i=0;i<strlen(szBuf);i++)
			if (szBuf[i]=='\n') szBuf[i]='\0'; // strip newlines
		// szBuf now contains possibly a card in the format
		// name;side;set;have;want
		// or an ENDCARDS
		if (strcmp(szBuf,"ENDCARDS")!=0)
		{
			// this is now a name;side;set;have;want
			for (i=0;i<strlen(szBuf);i++)
			{
				szName[i]=szBuf[i];
				// if the end of the card name is reached
				if (szName[i]==';')
				{
					szName[i]='\0';
					cSide = szBuf[i+1]; // name;<side>
					cSet = szBuf[i+3]; // <side>;<set>
					// find the idx if any (i+1=set,i+5=wb)
					idx = matchcard(szName,cSide,cSet);
					if (idx==-1) // if no match, abort processing
					{
//						fprintf(fdebug,"%s;%c;%c\n",szName,cSide,cSet);
						i=strlen(szBuf);  // skip to end
					}
					else
					{
						i += 5; // skip to have column
						j = szBuf[i];
						if (i<strlen(szBuf))
						{
							j=i;
							while ((szBuf[j]!=';')&&(j<strlen(szBuf)))
							{
								szTmp[j-i]=szBuf[j];
								j++;
							}
							szTmp[j-i]='\0';
							sprintf(masterlist[cardlist[idx].idx].szPrice,"%s",szTmp);
							masterlist[cardlist[idx].idx].fPrice=(float)atof(szTmp);
						}
					} // else to if a match/if not

				}	// else to if ; or if not
			} // end for loop

		} // end if not ENDCARDS
		else
		{
			for (i=0;i<511;i++)
			{
				if (!feof(fh))
				{
					szPriceGuideSource[i]=getc(fh);
					if (szPriceGuideSource[i]=='\n')
					{
						szPriceGuideSource[i++] = '\r';
						szPriceGuideSource[i]='\n';
						szPriceGuideSource[i+1]='\0';
					}
					if (feof(fh)) szPriceGuideSource[i]='\0';
				}
			}
			while (!feof(fh)) i=getc(fh); // go to end of file
		}
		} // end if !feof loop
	} // end while !feof loop
	fclose(fh);

	return TRUE;
}

void WriteMenuProfile(void)
{
	char sz[32];

	WritePrivateProfileString("View","Compressed",menu.bCompressed?"1":"0",szIni);
	WritePrivateProfileString("View","Bold",menu.bBold?"1":"0",szIni);
	WritePrivateProfileString("View","Type",menu.bTypeIcons?"1":"0",szIni);
	WritePrivateProfileString("View","Destiny",menu.bDestIcons?"1":"0",szIni);
	WritePrivateProfileString("View","Rarity",menu.bRareIcons?"1":"0",szIni);
	WritePrivateProfileString("View","Set",menu.bSetIcons?"1":"0",szIni);
	WritePrivateProfileString("View","Extra",menu.bExtraIcons?"1":"0",szIni);
	WritePrivateProfileString("View","Color",menu.bColor?"1":"0",szIni);
	WritePrivateProfileString("View","Shaded",menu.bDarkShaded?"1":"0",szIni);
	WritePrivateProfileString("View","DeckQty",menu.bDeckQty?"1":"0",szIni);
	WritePrivateProfileString("View","BinderQty",menu.bBinderQty?"1":"0",szIni);
	WritePrivateProfileString("Sort","Set",menu.bSortSets?"1":"0",szIni);
	WritePrivateProfileString("Sort","Side",menu.bSortSides?"1":"0",szIni);
	WritePrivateProfileString("Find","Name",menu.bFindName?"1":"0",szIni);
	WritePrivateProfileString("Find","Lore",menu.bFindLore?"1":"0",szIni);
	WritePrivateProfileString("Find","Desc",menu.bFindDesc?"1":"0",szIni);
	WritePrivateProfileString("Find","Light",menu.bFindLight?"1":"0",szIni);
	WritePrivateProfileString("Find","Dark",menu.bFindDark?"1":"0",szIni);
	WritePrivateProfileString("Find","Keep",menu.bKeepFind?"1":"0",szIni);
	WritePrivateProfileString("View","NoUnlimited",menu.bNoUnlimited?"1":"0",szIni);
	WritePrivateProfileString("View","CombineWB",menu.bCombineWB?"1":"0",szIni);
	WritePrivateProfileString("View","ExLight",menu.bExcludeLight?"1":"0",szIni);
	WritePrivateProfileString("View","ExDark",menu.bExcludeDark?"1":"0",szIni);
	WritePrivateProfileString("Tools","SaveStates",menu.bSaveStates?"1":"0",szIni);
	if ((menu.iStatsMode!=ID_STATS_FIND)&&(menu.iStatsMode!=ID_STATS_NOTES))
	{
		sprintf(sz,"%i",menu.iStatsMode);
		WritePrivateProfileString("Stats","Mode",sz,szIni);
	}
	sprintf(sz,"%i",menu.iRangeMode);
	WritePrivateProfileString("Stats","Range",sz,szIni);
	sprintf(sz,"%i",uSortOrder);
	WritePrivateProfileString("Sort","Order",sz,szIni);
	sprintf(sz,"%i",uPrevStats);
	WritePrivateProfileString("Sort","Previous",sz,szIni);
	sprintf(sz,"%s",szCustomSort);
	WritePrivateProfileString("Sort","CustomKey",sz,szIni);
	sprintf(sz,"%i",printopt.uDestination);
	WritePrivateProfileString("Print","Destination",sz,szIni);
	WritePrivateProfileString("Print","CardQty",printopt.bCardQty?"1":"0",szIni);
	WritePrivateProfileString("Print","Notes",printopt.bNotes?"1":"0",szIni);
	WritePrivateProfileString("Print","Legend",printopt.bSummary?"1":"0",szIni);
	WritePrivateProfileString("Print","Header",printopt.bDate?"1":"0",szIni);
	WritePrivateProfileString("Print","FSet",printopt.bFSet?"1":"0",szIni);
	WritePrivateProfileString("Print","FSide",printopt.bFSide?"1":"0",szIni);
	WritePrivateProfileString("Print","FRarity",printopt.bFRarity?"1":"0",szIni);
	WritePrivateProfileString("Print","FType",printopt.bFType?"1":"0",szIni);
	WritePrivateProfileString("Print","SSet",printopt.bSSet?"1":"0",szIni);
	WritePrivateProfileString("Print","SSide",printopt.bSSide?"1":"0",szIni);
	WritePrivateProfileString("Print","SRarity",printopt.bSRarity?"1":"0",szIni);
	WritePrivateProfileString("Print","SType",printopt.bSType?"1":"0",szIni);

}

void getbool(char *szSect,char *szItem,BOOL *b)
{
	*b=GetPrivateProfileInt(szSect,szItem,*b?1:0,szIni)?TRUE:FALSE;
}

void checkitem(HWND hMenu,UINT uItem,BOOL b)
{
	if (b) CheckMenuItem(hMenu,uItem,MF_CHECKED);
	else CheckMenuItem(hMenu,uItem,MF_UNCHECKED);
}

void GetMenuProfile(HMENU hMenu)
{
	char sz[32];

	getbool("View","Compressed",&menu.bCompressed);
	checkitem(hMenu,ID_VIEW_FONT_COMPRESSED,menu.bCompressed);
	getbool("View","Bold",&menu.bBold);
	checkitem(hMenu,ID_VIEW_FONT_BOLD,menu.bBold);
	getbool("View","Type",&menu.bTypeIcons);
	checkitem(hMenu,ID_VIEW_ICONS_TYPE,menu.bTypeIcons);
	getbool("View","Destiny",&menu.bDestIcons);
	checkitem(hMenu,ID_VIEW_ICONS_DESTINY,menu.bDestIcons);
	getbool("View","Rarity",&menu.bRareIcons);
	checkitem(hMenu,ID_VIEW_ICONS_RARITY,menu.bRareIcons);
	getbool("View","Set",&menu.bSetIcons);
	checkitem(hMenu,ID_VIEW_ICONS_SET,menu.bSetIcons);
	getbool("View","Extra",&menu.bExtraIcons);
	checkitem(hMenu,ID_VIEW_ICONS_EXTRA,menu.bExtraIcons);
	getbool("View","Color",&menu.bColor);
	checkitem(hMenu,ID_VIEW_CARDNAMES_COLOR,menu.bColor);
	getbool("View","Shaded",&menu.bDarkShaded);
	checkitem(hMenu,ID_VIEW_CARDNAMES_DARKSHADED,menu.bDarkShaded);
	getbool("View","DeckQty",&menu.bDeckQty);
	checkitem(hMenu,ID_VIEW_DECKQUANTITY,menu.bDeckQty);
	getbool("View","BinderQty",&menu.bBinderQty);
	checkitem(hMenu,ID_VIEW_BINDER,menu.bBinderQty);
	getbool("Sort","Set",&menu.bSortSets);
	checkitem(hMenu,ID_SORT_BYSETSFIRST,menu.bSortSets);
	getbool("Sort","Side",&menu.bSortSides);
	checkitem(hMenu,ID_SORT_BYSIDEFIRST,menu.bSortSides);
	getbool("Find","Name",&menu.bFindName);
	getbool("Find","Lore",&menu.bFindLore);
	getbool("Find","Desc",&menu.bFindDesc);
	getbool("Find","Light",&menu.bFindLight);
	getbool("Find","Dark",&menu.bFindDark);
	getbool("Find","Keep",&menu.bKeepFind);
	checkitem(hMenu,ID_STATS_KEEPFIND,menu.bKeepFind);
	getbool("View","NoUnlimited",&menu.bNoUnlimited);
	checkitem(hMenu,ID_VIEW_NOUNLIMITED,menu.bNoUnlimited);
	getbool("View","CombineWB",&menu.bCombineWB);
	checkitem(hMenu,ID_VIEW_COMBINE,menu.bCombineWB);
//	getbool("View","ExLight",&menu.bExcludeLight);
//	getbool("View","ExDark",&menu.bExcludeDark);
	menu.bExcludeLight=FALSE;	// never exclude one side
	menu.bExcludeDark=FALSE;	// when opening
	checkitem(hMenu,ID_VIEW_LSONLY,menu.bExcludeDark);
	checkitem(hMenu,ID_VIEW_DSONLY,menu.bExcludeLight);
	if (!menu.bExcludeLight&&!menu.bExcludeDark)
		checkitem(hMenu,ID_VIEW_BOTHONLY,TRUE);
	getbool("Tools","SaveStates",&menu.bSaveStates);
	checkitem(hMenu,ID_TOOLS_SAVESTATES,menu.bSaveStates);

	menu.iStatsMode=GetPrivateProfileInt("Stats","Mode",menu.iStatsMode,szIni);
	checkitem(hMenu,menu.iStatsMode,TRUE);
	menu.iRangeMode=GetPrivateProfileInt("Stats","Range",menu.iRangeMode,szIni);
	checkitem(hMenu,menu.iRangeMode,TRUE);
	uSortOrder=GetPrivateProfileInt("Sort","Order",uSortOrder,szIni);
	checkitem(hMenu,uSortOrder,TRUE);
	uPrevStats=GetPrivateProfileInt("Sort","Previous",uPrevStats,szIni);

	GetPrivateProfileString("Sort","CustomKey",szCustomSort,sz,16,szIni);
	sprintf(szCustomSort,sz);

	printopt.uDestination=GetPrivateProfileInt("Print","Destination",printopt.uDestination,szIni);
	getbool("Print","CardQty",&printopt.bCardQty);
	getbool("Print","Notes",&printopt.bNotes);
	getbool("Print","Legend",&printopt.bSummary);
	getbool("Print","Header",&printopt.bDate);
	getbool("Print","FSet",&printopt.bFSet);
	getbool("Print","FSide",&printopt.bFSide);
	getbool("Print","FRarity",&printopt.bFRarity);
	getbool("Print","FType",&printopt.bFType);
	getbool("Print","SSet",&printopt.bSSet);
	getbool("Print","SSide",&printopt.bSSide);
	getbool("Print","SRarity",&printopt.bSRarity);
	getbool("Print","SType",&printopt.bSType);

}
