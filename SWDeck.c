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

BOOL LoadPriceGuide(void);
BOOL SavePriceGuide(void);

char err_msg[10][20] = { "Version 1.0 format", "Version 2.0 format",
					   "Version 2.1 format", "Version 3.0 format",
					   "All OK", "Can't Open Data!", "Out of Memory",
					   "Corrupt Data File", "Reached MAXLIST",
					   "Invalid Format" };

// this function reads the swccgdm data files into the pCardData
// declared in SWDeck.h.  It then fills the masterlist[] with
// the pointers and defines iMListSize as the number of masterlist[]
// entries.  It also defines cardlist[] and iCListSize to include
// all cards in masterlist[].  Use ExcludeCList() and IncludeCList()
// too add/remove cards from the list.  This program also fills out
// the icons[] array in masterlist[], to speed up WM_PAINTs
int	OpenSWCCGData(void)
{
	int		iDataSize;
	int		iRot;
	int		iIdx;
	int		i,j;
	FILE	*fh;

	if ((fh=fopen("swccgdm.dat","rb"))==NULL)
		return CANT_OPEN_DATA;

	if (iMListSize>0) CloseSWCCGData();

	iDataSize = FileLength(fh);

	if ((pCardData=(char *)malloc(iDataSize+1))==NULL)
	{
		fclose(fh);
		return OUT_OF_MEMORY;
	}

	// read all data from the file into pCardData
	fread(pCardData,1,(iDataSize-1),fh);

	for (i=0;i<MAXLIST;i++)
	{
		masterlist[i].attr = NULL;
		masterlist[i].name = NULL;
		masterlist[i].desc = NULL;
		masterlist[i].bValid = FALSE;
		masterlist[i].rank = MAXLIST;
		for (j=0;j<7;j++) masterlist[i].icons[j] = 0;
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
					masterlist[iIdx].desc = pCardData+i+1;
					masterlist[iIdx].rank = iIdx;
					masterlist[iIdx].bValid = TRUE;
					iIdx++;
			}
			iRot = (iRot+1)%3;
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
		cardlist[i].inventory = 0;
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
				if (masterlist[i].attr[ATT_SUBRARITY]=='1')	masterlist[i].icons[CRARITY] = ICO_R1;
				else masterlist[i].icons[CRARITY] = ICO_R2;
				break;
			case 'U':
				if (masterlist[i].attr[ATT_SUBRARITY]=='1') masterlist[i].icons[CRARITY] = ICO_U1;
				else masterlist[i].icons[CRARITY] = ICO_U2;
				break;
			case 'C':
				if (masterlist[i].attr[ATT_SUBRARITY]=='1') masterlist[i].icons[CRARITY] = ICO_C1;
				else if (masterlist[i].attr[ATT_SUBRARITY]=='2') masterlist[i].icons[CRARITY] = ICO_C2;
				else masterlist[i].icons[CRARITY] = ICO_C3;
				break;
			default:
				masterlist[i].icons[CRARITY] = ICO_NA;
		}

		// all cards belong to a set
		switch (masterlist[i].attr[ATT_SET])
		{
			case 'L': masterlist[i].icons[CSET] = ICO_LTD; break;
			case 'P': masterlist[i].icons[CSET] = ICO_2P; break;
			case 'U': masterlist[i].icons[CSET] = ICO_ULTD; break;
			case 'A': masterlist[i].icons[CSET] = ICO_ANH; break;
			case 'R': masterlist[i].icons[CSET] = ICO_RL; break;
			case 'J': masterlist[i].icons[CSET] = ICO_RL; break;
			case 'H': masterlist[i].icons[CSET] = ICO_HOTH; break;
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
		}

		switch (masterlist[i].attr[ATT_TYPE])
		{
			case 'L':
				// all locations have force icons
				masterlist[i].icons[CEXTRA1]=
					masterlist[i].attr[ATT_RFORCE]-'0'+ICO_RFORCE0;
				masterlist[i].icons[CEXTRA2]=
					masterlist[i].attr[ATT_IFORCE]-'0'+ICO_IFORCE0;
				// swap force icons if this is a Dark card
				if (masterlist[i].attr[ATT_SIDE]=='D')
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
				}

				switch (masterlist[i].attr[ATT_SUBTYPE])
				{
					case 'R': masterlist[i].icons[CTYPE]=ICO_REBEL; break;
					case 'I': masterlist[i].icons[CTYPE]=ICO_IMP; break;
					case 'D': masterlist[i].icons[CTYPE]=ICO_DROID; break;
					case 'A': masterlist[i].icons[CTYPE]=ICO_ALIEN; break;
				}
				break;

			case 'R':
				masterlist[i].icons[CTYPE]=ICO_CRITTER;
				break;

			case 'V':
				masterlist[i].icons[CTYPE]=ICO_VEHICLE;
				if (masterlist[i].attr[ATT_PPILOT]=='Y')
					masterlist[i].icons[CEXTRA2]=ICO_PILOT;
				break;

			case 'S':
				masterlist[i].icons[CTYPE]=ICO_SHIP;
				if (masterlist[i].attr[ATT_ASTRO]=='Y')
					masterlist[i].icons[CEXTRA1]=ICO_ASTRO;
				if (masterlist[i].attr[ATT_PPILOT]=='Y')
					masterlist[i].icons[CEXTRA2]=ICO_PILOT;
				if (masterlist[i].attr[ATT_ICON]=='S')
					masterlist[i].icons[CEXTRA3]=ICO_SCOMP;
				break;

			case 'I':
				masterlist[i].icons[CTYPE]=ICO_INT;
				switch (masterlist[i].attr[ATT_SUBTYPE])
				{
					case 'L': masterlist[i].icons[CEXTRA1]=ICO_LOST; break;
					case 'B': masterlist[i].icons[CEXTRA1]=ICO_UORL; break;
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
				}
				break;

			case 'D':
				masterlist[i].icons[CTYPE]=ICO_DEVICE;
				break;

			case 'N':
				masterlist[i].icons[CTYPE]=ICO_EVENT;
				break;

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
	OpenSWCCGData();
}
		
void ImportData(void)
{
	FILE *fin,*fout;
	char szBuf1[80],szBuf2[80],szBuf3[500];

	fin = fopen("swccgdat.asc","rb");
	fout = fopen("swccgdm.dat","wb");

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

int IncludeCList(int att,int c)
{
	int i,j,cAdded;

	for (i=0,cAdded=0;i<iMListSize;i++)
		if ((masterlist[i].attr[att]==c))
			for (j=0;j<iMListSize;j++)
				if ((cardlist[j].idx==i)&&(cardlist[j].bValid==FALSE)&&
					!((masterlist[i].attr[ATT_SET]=='U')&&(menu.bUnlimited)))
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
	char szBuf[20];

	if ((fh=fopen("swccgdm.set","r"))==NULL)
	{
		if (IDYES == MessageBox(hwnd,HELP_NOSETS,"SWCCGDM.SET not found",MB_YESNO))
		{
			fh = fopen("swccgdm.set","w");
			fprintf(fh,"LPremiere\n");
			fprintf(fh,"PPB 2-Player\n");
			fprintf(fh,"UUnlimited\n");
			fprintf(fh,"AA New Hope\n");
			fprintf(fh,"RRebel Leader\n");
			fprintf(fh,"JJedi Pack\n");
			fprintf(fh,"HHoth\n");
			fprintf(fh,"DDagobah\n");
			fprintf(fh,"BBespin\n");
			fprintf(fh,"\n");
			fclose(fh);
			if ((fh=fopen("swccgdm.set","r"))==NULL)
			{
				MessageBox(hwnd,"Can't create swccgdm.set\r\t","DeckMaker Aborting",MB_OK);
				return FALSE;
			}
		} else return FALSE;
	}

	for (i=0;i<MAXSETS;i++)
	{
		fgets(szBuf,19,fh);
		if (!feof(fh)&&(strlen(szBuf)>2)&&(szBuf[0]>='A')&&(szBuf[0]<='Z'))
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
	
	hBr[BR_RED] = CreateSolidBrush(RGB_RED);
	hBr[BR_BLACK] = CreateSolidBrush(RGB_BLACK);
	hBr[BR_GREY] = CreateSolidBrush(RGB_GREY);
	hBr[BR_BLUE] = CreateSolidBrush(RGB_BLUE);
	hBr[BR_GREEN] = CreateSolidBrush(RGB_GREEN);
	hBr[BR_PURPLE] = CreateSolidBrush(RGB_PURPLE);
	hBr[BR_DKGREY] = CreateSolidBrush(RGB_DKGREY);

	hBlankRow=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BLANKROW));

}

void DeleteICO(void)
{
	int i;
	DeleteObject(hDarkBrush);
	DeleteObject((HGDIOBJ)hBlankRow);
	for (i=0;i<7;i++) DeleteObject((HGDIOBJ)hBr[i]);
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
// L - deploy	F - forfeit	C - price
void createkey(char *key,char *sort,int idx)
{
	unsigned int i,j;

	if (!masterlist[idx].bValid||!strlen(sort))
	{
		key[0]='\0';
	}
	else
	{

	for (i=0;i<strlen(sort);i++)
	{
		switch (sort[i])
		{
			case 'S':
				switch (masterlist[idx].attr[ATT_SIDE])
				{
					case 'L': key[i]='A'; break;
					case 'D': key[i]='B'; break;
					default:  key[i]='C'; break;
				}
				break;

			case 'T':
				key[i]='\0';
				for (j=0;(j<MAXSETS)&&(key[i]=='\0');j++)
				{
					if (sets[j][0]==masterlist[idx].attr[ATT_SET])
						key[i]='A'+j;
				}
				if (key[i]=='\0') key[i]='Z';
				break;

			case 'R':
				switch (masterlist[idx].attr[ATT_RARITY])
				{
					case 'R':
						key[i]='A'+masterlist[idx].attr[ATT_SUBRARITY]-'1';
						break;
					case 'U':
						key[i]='D'+masterlist[idx].attr[ATT_SUBRARITY]-'1';
						break;
					case 'C':
						key[i]='H'+masterlist[idx].attr[ATT_SUBRARITY]-'1';
						break;
					default:
						key[i]='Z';
						break;
				}
				break;
			
			case 'D':
				key[i]=masterlist[idx].attr[ATT_DESTINY];
				break;
			
			case 'Y':
				switch (masterlist[idx].attr[ATT_TYPE])
				{
					case 'L': key[i]='A'; break;
					case 'C': key[i]='B'; break;
					case 'R': key[i]='C'; break;
					case 'V': key[i]='D'; break;
					case 'S': key[i]='E'; break;
					case 'I': key[i]='F'; break;
					case 'E': key[i]='G'; break;
					case 'W': key[i]='H'; break;
					case 'D': key[i]='I'; break;
					case 'N': key[i]='J'; break;
					default: key[i]='Z'; break;
				}
				break;

			case 'Z':
				key[i]=masterlist[idx].attr[ATT_SUBTYPE];
				break;

			case 'P':
				key[i]=masterlist[idx].attr[ATT_POWER];
				break;

			case 'A':
				key[i]=masterlist[idx].attr[ATT_ABILITY];
				break;

			case 'L':
				key[i]=masterlist[idx].attr[ATT_DEPLOY];
				break;

			case 'F':
				key[i]=masterlist[idx].attr[ATT_FORFEIT];
				break;

			default:
				key[i]='Z';
				break;
		}
	}
	key[i]='\0';

	}

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
	char szKey[16];
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
	char	szBuf[100],szRange[20];
	HWND	hList,hCards,hDest;

	if (!hDlg) return 0; // leave if hDlgDeck isn't visible

	hList = GetDlgItem(hDlg,IDC_DECKLIST);
	hCards = GetDlgItem(hDlg,IDC_DECKCARDS);
	hDest = GetDlgItem(hDlg,IDC_DECKDESTINY);

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
			// if past first card and it's the same as previous
			// one, then add it to the previous and don't increment
			if (!iDListSize||(iDListSize&&
					strcmp(masterlist[decklist[iDListSize-1].idx].name,
							masterlist[cardlist[i].idx].name)))
			{
				decklist[iDListSize]=cardlist[i];
				total += decklist[iDListSize].deck;
				iDListSize++;
			}
			else
			{
				decklist[iDListSize-1].deck+=cardlist[i].deck;
				total += cardlist[i].deck;
			}
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

	calcRange(szRange,ATT_DESTINY);
	sprintf(szBuf,"Destiny: %s",szRange);
	SetWindowText(hDest,szBuf);

	return total;
}

void stread(FILE *fh,char *sz,int max)
{
	int i;

	i=0;
	do
	{
		sz[i]=getc(fh);
		if (feof(fh)) sz[i]='\0';
		i++;
	} while (!feof(fh)&&(i<max)&&sz[i-1]);

}

void stwrite(FILE *fh,char *sz)
{
	fprintf(fh,"%s",sz);
	fputc('\0',fh);
}

BOOL LoadPriceGuide(void)
{
	FILE *fh;
	char szBuf[80];
	int  i;

	if (iMListSize<1) return FALSE;

	if ((fh=fopen("SWCCG Price Guide.dat","r"))==NULL)
	{
		fh=fopen("SWCCG Price Guide.dat","w");
		stwrite(fh,"SWCCGDMPG3.00");
		sprintf(szPriceGuideSource,"No Price Data Available");
		stwrite(fh,szPriceGuideSource);
		sprintf(szPriceGuideDate,"n/a");
		stwrite(fh,szPriceGuideDate);
		fclose(fh);
		return FALSE;
	}

	stread(fh,szBuf,60); // version
	stread(fh,szPriceGuideSource,255); // source
	stread(fh,szPriceGuideDate,19);    // source date

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

	if ((fh=fopen("SWCCG Price Guide.dat","w"))!=NULL)
	{
		fh=fopen("SWCCG Price Guide.dat","w");
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
