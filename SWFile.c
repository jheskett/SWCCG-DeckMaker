// SWFile.c

// File management functions for SWCCGDM

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include "attributes.h"
#include "SWFile.h"
#include "resource.h"
#include "SWDeck.h"
#include "SWStats.h"

static OPENFILENAME ofn;  // for open/save
static OPENFILENAME pofn; // for print to file

extern char *szAppName;

int	FileLength(FILE *fh)
{
	int iCurrentPos,iFileLength;

	iCurrentPos = ftell(fh);
	fseek(fh,0,SEEK_END);
	iFileLength = ftell(fh);

	fseek(fh,iCurrentPos,SEEK_SET);
	return iFileLength;
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

void DoCaption(HWND hwnd,char *szfName)
{
	char *szCaption = malloc(strlen(szfName)+128);

	sprintf(szCaption,"%s - %s",szAppName,
		szfName[0]?szfName:UNTITLED);
	SetWindowText(hwnd,szCaption);
}

int AskAboutSave(HWND hwnd,char *szfName)
{
	char szBuf[64+_MAX_FNAME+8];
	int	iReturn;

	sprintf(szBuf,"Save current changes in %s?",
		szfName[0]?szfName:UNTITLED);
	iReturn = MessageBox(hwnd,szBuf,szAppName,
		MB_YESNOCANCEL|MB_ICONQUESTION);

	if (iReturn==IDYES)
		if (!SendMessage(hwnd,WM_COMMAND,ID_FILE_SAVEDECK,0L))
			iReturn=IDCANCEL;
	return iReturn;
}

void SWFileInitialize(HWND hwnd)
{
	static char szFilter[] = "Deck Files (*.deck)\0*.deck\0" \
							 "All Files\0*.*\0\0";

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= hwnd;
	ofn.hInstance			= NULL;
	ofn.lpstrFilter			= szFilter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= NULL;
	ofn.nMaxFile			= _MAX_PATH;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= _MAX_FNAME+8;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= NULL;
	ofn.Flags				= 0;
	ofn.nFileOffset			= 0;
	ofn.nFileExtension		= 0;
	ofn.lpstrDefExt			= "deck";
	ofn.lCustData			= 0L;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;
}

BOOL SWFileOpenDlg(HWND hwnd,PSTR pstrFileName,PSTR pstrTitleName)
{
	BOOL bResult;
	char *p;

	ofn.hwndOwner			= hwnd;
	ofn.lpstrFile			= pstrFileName;
	ofn.lpstrFileTitle		= pstrTitleName;
	ofn.Flags				= OFN_HIDEREADONLY; //|OFN_FILEMUSTEXIST;

	bResult = GetOpenFileName(&ofn);

	if (!bResult) return bResult;

	p = strrchr(ofn.lpstrFile,'.');
	if (_stricmp(p,".dec")==0)
		sprintf(p,".deck");
	p = strrchr(ofn.lpstrFileTitle,'.');
	if (_stricmp(p,".dec")==0)
		sprintf(p,".deck");

	return bResult;
}

BOOL SWFileSaveDlg(HWND hwnd,PSTR pstrFileName,PSTR pstrTitleName)
{
	BOOL bResult;
	char *p;

	ofn.hwndOwner			= hwnd;
	ofn.lpstrFile			= pstrFileName;
	ofn.lpstrFileTitle		= pstrTitleName;
	ofn.Flags				= OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;

	bResult = GetSaveFileName(&ofn);

	if (!bResult) return bResult;

	p = strrchr(ofn.lpstrFile,'.');
	if (_stricmp(p,".dec")==0)
		sprintf(p,".deck");
	p = strrchr(ofn.lpstrFileTitle,'.');
	if (_stricmp(p,".dec")==0)
		sprintf(p,".deck");

	return bResult;
}

int matchcard(char *szinName,int cSide,int cSet)
{
	char szTmp[4096];
	unsigned int i,j,k;
	int	iResult;
	char *szName = (char*)malloc(strlen(szinName)+1);
	strcpy(szName,szinName);
/*	// truncate card to just before a unique dot (·,×)
	for (j=0,k=0;j<strlen(szTmp);j++)
	{
		if (((szName[j]=='×')||(szName[j]=='·'))&&(j))
		{
			szName[j-1]='\0';
			k=1;
		}
	}
	if (!k) szName[j-1]='\0';*/

	iResult = -1; // default to fail

//	if (cSide=='l') cSide='L';	// toupper side code
//	if (cSide=='d') cSide='D';
	
	for (i=0;i<(unsigned)iCListSize;i++)
	{
		sprintf(szTmp,"%s",masterlist[cardlist[i].idx].name);

		// truncate card to just before a unique dot (·,×)
		for (j=0,k=0;j<strlen(szTmp);j++)
		{
			if (((szTmp[j]=='×')||(szTmp[j]=='·'))&&(j))
			{
				szTmp[j-1]='\0';
				k=1;
			}
		}
		if (!k) szTmp[j-1]='\0';

		// if names are the same...
		if (strcmp(szTmp,szName)==0)
		{
			// and set and side are the same...
			if ((cSide==masterlist[cardlist[i].idx].attr[ATT_SIDE])&&
				(cSet==masterlist[cardlist[i].idx].attr[ATT_SET]))
			{
				// it's a match! set to return this index
				iResult=(signed)i;
				//for (k=0;k<(unsigned)iMListSize;k++)
				//{
				//	if (i==(unsigned)cardlist[k].idx) iResult=k;
				//}
				i=(unsigned)iMListSize; // exit loop
			}
		}

	}

	return iResult;
}

// this monolithic function opens the file pstrFileName
// derived from the GetOpenFileName common dialog, and
// goes through and fills in the .deck and .wb qty's for
// matching card names.
BOOL SWFileRead(HWND hwnd,PSTR pstrFileName)
{
	unsigned int i,j;
	char szBuf[4096],szName[256],szTmp[4096];
	int  cSide,cSet;
	int  iHave,iWant,iWB;
	int	 idx;
	FILE *fh;
	//FILE *fdebug;
	int	 cSideExclude;
	HMENU hMenu;
	BOOL bLoadBadFile = FALSE;

	//char testname[256]; char tside, tset; int tpos;

	hMenu = GetMenu(hwnd);

	if ((fh=fopen(pstrFileName,"r"))==NULL)
		return FALSE;

	//fdebug = fopen("debug.d","w");
//	fprintf(fdebug,"This file contains the cards not matched in SWFileRead\n");

	cSideExclude='\0';
	IncludeCList(ATT_SIDE,'*');			// pull all cards back in
	for (i=0;i<(unsigned)iCListSize;i++)// quickly clear the deck
	{
		cardlist[i].deck = 0;
		cardlist[i].wb = 0;
	}

	fgets(szBuf,4096,fh); // get first line (header)

	// set combine status (will combine later if needed)
	if (strstr(szBuf,"COMBINE=TRUE"))
	{
		menu.bCombineWB = TRUE;
		CheckMenuItem(GetMenu(hwnd),ID_VIEW_COMBINE,MF_CHECKED);
	}
	if (strstr(szBuf,"COMBINE=FALSE"))
	{
		menu.bCombineWB = FALSE;
		CheckMenuItem(GetMenu(hwnd),ID_VIEW_COMBINE,MF_UNCHECKED);
	}
	if (strstr(szBuf,"SIDEEXC=l")!=NULL) cSideExclude='l';
	if (strstr(szBuf,"SIDEEXC=d")!=NULL) cSideExclude='d';
	if (strstr(szBuf,"CREATORVERSION=3.20")!=NULL) bLoadBadFile = TRUE;
	// now read the cards one at a time
	while (!feof(fh))
	{
/*		tpos = ftell(fh);
		fscanf(fh,"%s;%c;%c*\n",testname,&tside,&tset);
		fseek(fh,tpos,SEEK_SET);
		fprintf(fdebug,"%s,%c;%c\n",testname,tside,tset);*/
		fgets(szBuf,4096,fh);		// read the line
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
					if (bLoadBadFile)
						szBuf[i+1]=szBuf[i+1]+('A'-'a');
					cSide = szBuf[i+1];
					//fprintf(fdebug,"side=%c\n",cSide);
					cSet = szBuf[i+3]; // <side>;<set>
					// find the idx if any (i+1=set,i+5=wb)
					idx = matchcard(szName,cSide,cSet);
					if (idx==-1) // if no match, abort processing
					{
						//fprintf(fdebug,"%s;%c;%c\n",szName,cSide,cSet);
						i=strlen(szBuf);  // skip to end
					}
					else
					{
						// we have a match, now grab have/want/wb
						iHave=0; iWant=0; iWB=0;

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
							iHave=atoi(szTmp);
							i=j+1; // skip to want column
							if (i<strlen(szBuf))
							{
								j=i;
								while ((szBuf[j]!=';')&&(j<strlen(szBuf)))
								{
									szTmp[j-i]=szBuf[j];
									j++;
								}
								szTmp[j-i]='\0';
								iWant=atoi(szTmp);
								i=j+1; // skip to wb column
								if (i<strlen(szBuf))
								{
									j=i;
									while ((szBuf[j]!=';')&&(j<strlen(szBuf)))
									{
										szTmp[j-i]=szBuf[j];
										j++;
									}
									szTmp[j-i]='\0';
									iWB=atoi(szTmp);
								}
							}
						}

						cardlist[idx].deck = iHave;
						cardlist[idx].wb = iWB;
					} // else to if a match/if not

				}	// else to if ; or if not
			} // end for loop

		} // end if not ENDCARDS
		else
		{
			for (i=0;i<4095;i++)
			{
				if (!feof(fh))
				{
					szBuf[i]=getc(fh);
					if (szBuf[i]=='\n')
					{
						szBuf[i++] = '\r';
						szBuf[i]='\n';
						szBuf[i+1]='\0';
					}
					if (feof(fh)) szBuf[i]='\0';
				}
			}
			while (!feof(fh)) i=getc(fh); // go to end of file
			SetWindowText(GetDlgItem(hDlgStats,IDC_STATSNOTES),szBuf);
		}
		} // end if !feof loop
	} // end while !feof loop
	fclose(fh);
	//fclose(fdebug);

	// if this was saved combined, combine but don't affect qty's
	if (menu.bCombineWB) LoadCombineWB();
	// if excluding one side, do it now
	if (cSideExclude)
	{
		ExcludeCList(ATT_SIDE,cSideExclude,TRUE);
		if (cSideExclude=='d')
		{
			menu.bExcludeDark=TRUE;
			menu.bExcludeLight=FALSE;
			UpdateOnlyMenu(hwnd);
		}
		else if (cSideExclude=='l')
		{
			menu.bExcludeLight=TRUE;
			menu.bExcludeDark=FALSE;
			UpdateOnlyMenu(hwnd);
		}
	}
	else
	{
			menu.bExcludeLight=FALSE;
			menu.bExcludeDark=FALSE;
			UpdateOnlyMenu(hwnd);
	}
	return TRUE;
}

BOOL SWFileWrite(HWND hwnd,PSTR pstrFileName)
{
	FILE *fh;
	int i;
	unsigned int j;
	char szTmp[4096];

	if ((fh=fopen(pstrFileName,"w"))==NULL)
		return FALSE;

	sprintf(szTmp,"GAME=swccg;CREATORNAME=DECKMAKER;CREATORVERSION=3.21;VERSION=2.000.1;COMBINE=%s\n",menu.bCombineWB?"TRUE":"FALSE");
	if (menu.bExcludeLight||menu.bExcludeDark)
	{
		i = strlen(szTmp)-1;
		if (menu.bExcludeLight) sprintf(szTmp+i,";SIDEEXC=l\n");
		if (menu.bExcludeDark) sprintf(szTmp+i,";SIDEEXC=d\n");
	}
	fprintf(fh,szTmp);
	for (i=0;i<iMListSize;i++)
	{
		if (cardlist[i].deck||cardlist[i].wb)
		{
			sprintf(szTmp,"%s",masterlist[cardlist[i].idx].name);
			for (j=0;j<strlen(szTmp);j++)
			{
				if (((szTmp[j]=='·')||(szTmp[j]=='×'))&&j)
					szTmp[j-1]='\0';
			}
			if (j) szTmp[j-1]='\0';
			fprintf(fh,"%s;%c;%c;%d;0;%d\n",szTmp,
				masterlist[cardlist[i].idx].attr[ATT_SIDE],
				masterlist[cardlist[i].idx].attr[ATT_SET],
				cardlist[i].deck,cardlist[i].wb);
		}
	}
	fprintf(fh,"ENDCARDS\n");
	GetWindowText(GetDlgItem(hDlgStats,IDC_STATSNOTES),szTmp,4095);
	stripcr(szTmp);
	fprintf(fh,"%s",szTmp);
	fclose(fh);

	return TRUE;
}

BOOL SWBinderRead(HWND hwnd,PSTR pstrFileName)
{
	unsigned int i,j;
	char szBuf[4096],szName[256],szTmp[4096];
	int  cSide,cSet;
	int  iHave,iWant,iWB;
	int	 idx;
	FILE *fh;
//	FILE *fdebug;
	HMENU hMenu;

	hMenu = GetMenu(hwnd);

	if ((fh=fopen(pstrFileName,"r"))==NULL)
		return FALSE;

//	fdebug = fopen("debug.d","w");
//	fprintf(fdebug,"This file contains the cards not matched in SWFileRead\n");

//	IncludeCList(ATT_SIDE,'*');			// pull all cards back in

	for (i=0;i<(unsigned)iCListSize;i++)// quickly clear the binder
	{
		cardlist[i].binder = 0;
	}

	fgets(szBuf,4096,fh); // get first line (header)

	// now read the cards one at a time
	while (!feof(fh))
	{
		fgets(szBuf,4096,fh);		// read the line
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
						// we have a match, now grab have/want/wb
						iHave=0; iWant=0; iWB=0;

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
							iHave=atoi(szTmp);
							i=j+1; // skip to want column
							if (i<strlen(szBuf))
							{
								j=i;
								while ((szBuf[j]!=';')&&(j<strlen(szBuf)))
								{
									szTmp[j-i]=szBuf[j];
									j++;
								}
								szTmp[j-i]='\0';
								iWant=atoi(szTmp);
								i=j+1; // skip to wb column
								if (i<strlen(szBuf))
								{
									j=i;
									while ((szBuf[j]!=';')&&(j<strlen(szBuf)))
									{
										szTmp[j-i]=szBuf[j];
										j++;
									}
									szTmp[j-i]='\0';
									iWB=atoi(szTmp);
								}
							}
						}

						cardlist[idx].binder = iHave;
					} // else to if a match/if not

				}	// else to if ; or if not
			} // end for loop

		} // end if not ENDCARDS
		else
		{
			while (!feof(fh)) i=getc(fh); // go to end of file
		}
		} // end if !feof loop
	} // end while !feof loop
	fclose(fh);
//	fclose(fdebug);

//	if (menu.bCombineWB) CombineBinderWB();

	return TRUE;
}

void SWPrintFileInitialize(HWND hwnd)
{
	static char szFilter[] = "Text Files (*.txt)\0*.txt\0" \
							 "All Files\0*.*\0\0";

	pofn.lStructSize			= sizeof(OPENFILENAME);
	pofn.hwndOwner			= hwnd;
	pofn.hInstance			= NULL;
	pofn.lpstrFilter			= szFilter;
	pofn.lpstrCustomFilter	= NULL;
	pofn.nMaxCustFilter		= 0;
	pofn.nFilterIndex		= 0;
	pofn.lpstrFile			= NULL;
	pofn.nMaxFile			= _MAX_PATH;
	pofn.lpstrFileTitle		= NULL;
	pofn.nMaxFileTitle		= _MAX_FNAME+8;
	pofn.lpstrInitialDir		= NULL;
	pofn.lpstrTitle			= NULL;
	pofn.Flags				= 0;
	pofn.nFileOffset			= 0;
	pofn.nFileExtension		= 0;
	pofn.lpstrDefExt			= "txt";
	pofn.lCustData			= 0L;
	pofn.lpfnHook			= NULL;
	pofn.lpTemplateName		= NULL;
}

BOOL SWPrintFileSaveDlg(HWND hwnd,PSTR pstrFileName,PSTR pstrTitleName)
{
	pofn.hwndOwner			= hwnd;
	pofn.lpstrFile			= pstrFileName;
	pofn.lpstrFileTitle		= pstrTitleName;
	pofn.Flags				= OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&pofn);
}

BOOL SWPrintFileWrite(HWND hwnd,PSTR pstrFileName,PSTR pstrPrintFile)
{
	FILE *fin,*fout;
	int  ch;

	if ((fout=fopen(pstrFileName,"w"))==NULL)
		return FALSE;

	if ((fin=fopen(pstrPrintFile,"r"))==NULL)
	{
		fclose(fout);
		return FALSE;
	}

	while (!feof(fin))
	{
		ch = getc(fin);
		if (!feof(fin)) putc(ch,fout);
	}

	fclose(fin);
	fclose(fout);
}
