// SWSpoilers.c

// createworkfile converts the data files maintained by Chris Gregg
// into a work file usable by the DeckMaker


#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include "attributes.h"
#include "SWSpoilers.h"
#include "SWFile.h"
#include "SWDeck.h"

#define MAXSPOILERLINE 4096

struct splitfields {
	char **szFields;
	int nFieldCount;
};

struct splitfields split(char *string,char *toks)
{
	struct splitfields rv = {0,0};
	char *copy;
	//rv.szFields = malloc(sizeof(char*));
	char *next;
	
	copy = (char*)malloc(strlen(string)+1);
	strcpy(copy,string);
	next = strtok(copy,toks);
	while (next != NULL) {
		rv.szFields = (char**)realloc(rv.szFields,sizeof(char*)*(rv.nFieldCount+1));
		rv.szFields[rv.nFieldCount] = malloc(strlen(next)+1);
		strcpy(rv.szFields[rv.nFieldCount++],next);
		strtok(NULL,toks);
	}
	return rv;
}

void getsplitidx(struct splitfields sf,int idx, char*szBuf)
{
	if (idx <sf.nFieldCount)
		strcpy(szBuf,sf.szFields[idx]);
}

void deletesf(struct splitfields sf)
{
	int i;
	for (i=0;i<sf.nFieldCount;i++)
		free(sf.szFields[i]);
	free(sf.szFields);
}

void getfieldstring(int field,char *in,char *out)
{
	unsigned int idx,odx;
	field--;

	
	idx=0;
	// skip to the character immediately after the ;
	// until skipped to the proper field
	while (field)
	{
		while ((in[idx]!=';')&&(idx<strlen(in))) idx++;
		idx++; // skip after the ;
		field--;
	}

	// field passed string's length
	if (idx==strlen(in)) { out[0]='\0'; return; }
	// field is empty if next char is a ;
	if (in[idx]==';') { out[0]='\0'; return; }

	// copy field to out[]
	odx = 0;
	while ((in[idx]!=';')&&(idx<strlen(in)))
	{
		out[odx++]=in[idx++];
	}

	// asciiz deliminate string
	out[odx] = '\0';

	for (idx=0;idx<strlen(out);idx++)
		if ((out[idx]<' ')||(out[idx]>'}')) out[idx]=' ';
	if (!strlen(out)) { sprintf(out," "); }
}

int getcharafter(int ch,char *in)
{
	unsigned int i;
	int chreturn;

	chreturn = '\0';
	for (i=0;i<strlen(in);i++)
		if ((in[i]==ch)&&((i+2)<strlen(in))) chreturn=in[i+2];

	return chreturn;
}

long parseattr(char *in,char *out,int setidx)
{
	char szBuf[MAXSPOILERLINE];
	int	 ch;
	unsigned int i;

	//sprintf(out,"000000000000000"); // initialize attribute
	memset(out,'0',ATT_MAX);
	out[ATT_MAX] = '\0';

	getfieldstring(1,in,szBuf);	// get side
//	if (szBuf[0]=='l') out[ATT_SIDE] = 'L';
//	else out[ATT_SIDE] = 'D';
	out[ATT_SIDE]=szBuf[0];

	getfieldstring(4,in,szBuf); // get set
	out[ATT_SET] = szBuf[setidx];
	if (!szBuf[0]) sprintf(szBuf,"z");

	getfieldstring(7,in,szBuf); // get rarity
	out[ATT_RARITY] = szBuf[0];
	if (szBuf[1]) out[ATT_SUBRARITY] = szBuf[1];
	switch (out[ATT_RARITY])
	{
		case 'R': case 'U': case 'C': case 'F':break;
		default: out[ATT_RARITY] = 'N';
				 out[ATT_SUBRARITY] = 'A';
				 break;
	}

	getfieldstring(2,in,szBuf);	// get type
	switch (szBuf[0])
	{
		case 'c':
			out[ATT_TYPE] = 'C';
			getfieldstring(3,in,szBuf);
			switch (szBuf[0])
			{
				case 'a': out[ATT_SUBTYPE]='A'; break;
				case 'd': out[ATT_SUBTYPE]='D'; break;
				case 'i': out[ATT_SUBTYPE]='I'; break;
				case 'r': out[ATT_SUBTYPE]='R'; break;
				case 'R': out[ATT_SUBTYPE]='A'; // chewie
						  out[ATT_ICON]='R';	break;
				case 'j': out[ATT_SUBTYPE]='J'; break;
				default:  out[ATT_SUBTYPE]='A'; break;
			}
			// now gather the rest of the info
			
			getfieldstring(9,in,szBuf); // statistics
			ch=getcharafter('P',szBuf);
			if (ch) out[ATT_POWER] = ch;
			ch=getcharafter('A',szBuf);
			if (ch) out[ATT_ABILITY] = ch;
			ch=getcharafter('R',szBuf);
			if (ch) out[ATT_ABILITY] = ch;
			ch=getcharafter('D',szBuf);
			if (ch) out[ATT_DEPLOY] = ch;
			ch=getcharafter('F',szBuf);
			if (ch) out[ATT_FORFEIT] = ch;
			getfieldstring(11,in,szBuf); // "icons" (strings)
			if (strstr(szBuf,"ilot")!=NULL)
				out[ATT_PILOT]='Y';
			if (strstr(szBuf,"arrior")!=NULL)
				out[ATT_WARRIOR]='Y';
			if (strstr(szBuf,"stromech")!=NULL)
				out[ATT_ASTRO]='Y';
			if (strstr(szBuf,"ermanent") && strstr(szBuf,"eapon"))
				out[ATT_ICON]='p';
			getfieldstring(12,in,szBuf); // get lore
			if ((strstr(szBuf,"ounty hunter")!=NULL)||(strstr(szBuf,"ounty hunter")!=NULL))
				out[ATT_ICON]='B';
			if ((strstr(szBuf,"{Leader")!=NULL)||(strstr(szBuf,"{leader")!=NULL))
				out[ATT_ICON]='L';
			if ((strstr(szBuf,"{Spy")!=NULL)||(strstr(szBuf,"{spy")!=NULL))
				out[ATT_ICON]='S';
			break;
		case 'd':
			out[ATT_TYPE] = 'D';
			break;
		case 'e':
			out[ATT_TYPE] = 'E';
			getfieldstring(3,in,szBuf);
			switch (szBuf[0])
			{
				case 'u': out[ATT_SUBTYPE]='U'; break;
				case 'i': out[ATT_SUBTYPE]='I'; break;
				case 'm': out[ATT_SUBTYPE]='M'; break;
			}
			getfieldstring(9,in,szBuf);
			if ((strstr(szBuf,"ejarik")!=NULL)||(strstr(szBuf,"ologram")!=NULL))
				out[ATT_ICON]='D';
			break;
		case 'E':
			out[ATT_TYPE] = 'N';
			break;
		case 'O':
			out[ATT_TYPE] = 'O';
			break;
		case 'i':
			out[ATT_TYPE] = 'I';
			getfieldstring(3,in,szBuf);
			switch (szBuf[0])
			{
				case 'l': out[ATT_SUBTYPE] = 'L'; break;
				case 'u': out[ATT_SUBTYPE] = 'U'; break;
				case 'o': out[ATT_SUBTYPE] = 'B'; break;
				case 'O': out[ATT_SUBTYPE] = 'S'; break;
			}
			getfieldstring(9,in,szBuf);
			if ((strstr(szBuf,"ejarik")!=NULL)||(strstr(szBuf,"ologram")!=NULL))
				out[ATT_ICON]='D';
			break;
		case 'j':
			out[ATT_TYPE] = 'J';
			break;
		case 'l':
			out[ATT_TYPE] = 'L';
			getfieldstring(3,in,szBuf);
			switch (szBuf[0])
			{
				case 'S': out[ATT_SUBTYPE]='P'; break;
				case 's': out[ATT_SUBTYPE]='T'; break;
				case 'e': out[ATT_SUBTYPE]='S'; break;
			}
			getfieldstring(9,in,szBuf);
			if (strstr(szBuf,"nterior")!=NULL)
				out[ATT_ICON]='N';
			if (strstr(szBuf,"xterior")!=NULL)
				out[ATT_ICON]='X';
			if ((strstr(szBuf,"comp Link")!=NULL)||(strstr(szBuf,"comp link")!=NULL))
				out[ATT_ASTRO]='S';
			// deal with specific locations chris' spoiler doesn't point out
			getfieldstring(6,in,szBuf);
			if ((strstr(szBuf,"Imperial Holotable")!=NULL)||(strstr(szBuf,"Dejarik Hologameboard")!=NULL))
				out[ATT_ICON]='D';
			if (strcmp(szBuf,"Death Star")==0) { out[ATT_SUBTYPE]='M'; out[ATT_PARSEC]='X'; }
			if (strstr(szBuf,"Sandcrawler:")!=NULL) out[ATT_SUBTYPE]='V';
			// get parsec number
			getfieldstring(8,in,szBuf);
			for (i=0;i<strlen(szBuf);i++)
				if ((szBuf[i]>='0')&&(szBuf[i]<='9')) out[ATT_PARSEC]=szBuf[i];
			// finally, get force icons
			getfieldstring(10,in,szBuf);
			out[ATT_RFORCE]=szBuf[0];
			getfieldstring(12,in,szBuf);
			out[ATT_IFORCE]=szBuf[0];
			break;
		case 'r':
			out[ATT_TYPE] = 'R';
			getfieldstring(9,in,szBuf); // statistics
			ch=getcharafter('P',szBuf);
			if (ch) out[ATT_POWER] = ch;
			ch=getcharafter('R',szBuf);
			if (ch) out[ATT_ABILITY] = ch;
			ch=getcharafter('D',szBuf);
			if (ch) out[ATT_DEPLOY] = ch;
			ch=getcharafter('F',szBuf);
			if (ch) out[ATT_FORFEIT] = ch;
			break;
		case 's':
			out[ATT_TYPE] = 'S';
			getfieldstring(3,in,szBuf);
			switch(szBuf[0])
			{
				case 'c': out[ATT_SUBTYPE]='C'; break;
				case 'f': out[ATT_SUBTYPE]='F'; break;
				case 's': out[ATT_SUBTYPE]='S'; break;
			}
			getfieldstring(9,in,szBuf); // statistics
			ch=getcharafter('P',szBuf);
			if (ch) out[ATT_POWER] = ch;
			ch=getcharafter('R',szBuf);
			if (ch) out[ATT_ABILITY] = ch;
			ch=getcharafter('M',szBuf);
			if (ch) out[ATT_ABILITY] = ch;
			ch=getcharafter('D',szBuf);
			if (ch) out[ATT_DEPLOY] = ch;
			ch=getcharafter('F',szBuf);
			if (ch) out[ATT_FORFEIT] = ch;
			ch=getcharafter('H',szBuf);
			if (ch) out[ATT_SPEED] = ch;
			getfieldstring(11,in,szBuf);
			if (strstr(szBuf,"ilot")!=NULL) out[ATT_PPILOT]='Y';
			if (strstr(szBuf,"stromech")!=NULL) out[ATT_ASTRO]='Y';
			if ((strstr(szBuf,"comp Link")!=NULL)||(strstr(szBuf,"comp link")!=NULL))
				out[ATT_ICON]='S';
			if (strstr(szBuf,"ndepend")!=NULL)
				out[ATT_ICON]='I';
			getfieldstring(6,in,szBuf);
			if (strstr(szBuf,"Executor")!=NULL)
			{
				out[ATT_POWER]='<'; out[ATT_ABILITY]='<';
				out[ATT_FORFEIT]='?'; out[ATT_DEPLOY]='?';
			}
			break;
		case 'v':
			out[ATT_TYPE] = 'V';
			getfieldstring(3,in,szBuf);
			switch (szBuf[0])
			{
				case 'c': out[ATT_SUBTYPE]='B'; break;
				case 'a': out[ATT_SUBTYPE]='C'; break;
				case 't': out[ATT_SUBTYPE]='T'; break;
				case 's': out[ATT_SUBTYPE]='S'; break;
			}
			getfieldstring(9,in,szBuf);
			ch=getcharafter('P',szBuf);
			if (ch) out[ATT_POWER] = ch;
			ch=getcharafter('M',szBuf);
			if (ch) out[ATT_DEFENSE] = ch;
			ch=getcharafter('A',szBuf);
			if (ch) out[ATT_DEFENSE] = ch;
			ch=getcharafter('R',szBuf);
			if (ch) out[ATT_DEFENSE] = ch;
			ch=getcharafter('D',szBuf);
			if (ch) out[ATT_DEPLOY] = ch;
			ch=getcharafter('F',szBuf);
			if (ch) out[ATT_FORFEIT] = ch;
			ch=getcharafter('L',szBuf);
			if (ch) out[ATT_SPEED] = ch;
			getfieldstring(11,in,szBuf);
			if (strstr(szBuf,"ilot")!=NULL) out[ATT_PPILOT]='Y';
			getfieldstring(12,in,szBuf);
			if (strstr(szBuf,"nclosed")!=NULL) out[ATT_ICON]='E';
			break;
		case 'w':
			out[ATT_TYPE] = 'W';
			getfieldstring(3,in,szBuf);
			switch (szBuf[0])
			{
				case 'c': out[ATT_SUBTYPE] = 'C'; break;
				case 'a': out[ATT_SUBTYPE] = 'A'; break;
				case 's': out[ATT_SUBTYPE] = 'S'; break;
				case 'v': out[ATT_SUBTYPE] = 'V'; break;
				case 'r': out[ATT_SUBTYPE] = 'R'; break;
				case 'D': out[ATT_SUBTYPE] = 'D'; break;
			}
			getfieldstring(9,in,szBuf);
			ch=getcharafter('D',szBuf);
			if (ch) out[ATT_DEPLOY] = ch;
			ch=getcharafter('F',szBuf);
			if (ch) out[ATT_FORFEIT] = ch;
			break;
	}
	if (out[ATT_TYPE] != 'L') {
			getfieldstring(8,in,szBuf); // destiny
			//figure out how many destinies are listed:
			out[ATT_DESTINY]=szBuf[0];
			out[ATT_DESTINY2]='N';
			if (strlen(szBuf)>1) {
				for (i=1;i<strlen(szBuf);i++)
					if (szBuf[i]>='0'&&szBuf[i]<='9') {
						out[ATT_DESTINY2]=szBuf[i];
						break;
					}
			}
			if (out[ATT_DESTINY]=='\0') out[ATT_DESTINY]='0';
			if (out[ATT_DESTINY]=='.') out[ATT_DESTINY]='0';
			if (out[ATT_DESTINY]=='p') // for brainiac
			{ out[ATT_DESTINY]='3'; out[ATT_ICON]='P'; }
			
			if (out[ATT_DESTINY2]=='.') out[ATT_DESTINY2]='0';
			if (out[ATT_DESTINY2]=='p') // for brainiac
			{ out[ATT_DESTINY2]='3'; out[ATT_ICON]='P'; }
	}
		if (out[ATT_DESTINY]=='\0') out[ATT_DESTINY]='0';
		if (out[ATT_DESTINY2]=='\0') out[ATT_DESTINY2]='0';

	getfieldstring(4,in,szBuf); // get set
	if (setidx != 0)
		out[ATT_LIMITED] = szBuf[0];
	if (strlen(szBuf)-1 > (UINT)setidx)
		return TRUE;
	return FALSE;
}

void parsecardname(char *in,char *out)
{
	unsigned int i,k;

	char szName[MAXSPOILERLINE];
	char szDots[32];

	getfieldstring(6,in,szName); // get the card name itself
	getfieldstring(5,in,szDots); // get any unique . or <>'s

	// convert *'s to unique dots (·)
	// and <>'s to system unique dots (×)
	for (i=0;i<strlen(szDots);i++)
	{
		if (szDots[i]=='*') szDots[i]='·';
		if (szDots[i]=='<')
		{
			// pull rest of dots string up to clear '>'
			for (k=i;(k+1)<strlen(szDots);k++)
				szDots[k]=szDots[k+1];
			szDots[k] = '\0'; // truncate last char off
			szDots[i]='×'; // overwite '>' with '×'
		}
	}

	sprintf(out,"%s %s",szName,szDots);

}

void parselore(char *in,char *out)
{
	char szBuf[MAXSPOILERLINE];
	sprintf(out,"Card Lore");
	getfieldstring(2,in,szBuf);
	switch (szBuf[0])
	{
		case 'c': getfieldstring(12,in,out); break;
		case 's': getfieldstring(12,in,out); break;
		case 'v': getfieldstring(12,in,out); break;
		case 'r': getfieldstring(12,in,out); break;
		case 'd': getfieldstring(11,in,out); break;
		case 'w': getfieldstring(11,in,out); break;
		case 'e': getfieldstring(9,in,out); break;
		case 'E': getfieldstring(9,in,out); break;
		case 'i': getfieldstring(9,in,out); break;
		case 'j': getfieldstring(9,in,out); break;
		case 'l':
			getfieldstring(1,in,szBuf);
			if (szBuf[0]=='d')
			{
				getfieldstring(11,in,szBuf);
				sprintf(out,"Light Side Text: %s ",strlen(szBuf)?szBuf:"none");
			} else
			{
				getfieldstring(13,in,szBuf);
				sprintf(out,"Dark Side Text: %s ",strlen(szBuf)?szBuf:"none");
			}
			break;
		case 'O':
			getfieldstring(9,in,szBuf);
			sprintf(out,"First Side Text: %s",strlen(szBuf)?szBuf:"none");
			break;
		default: break;
	}

}

void parsedesc(char *in,char *out)
{
	char szBuf[MAXSPOILERLINE];
	sprintf(out,"Game Text ");
	getfieldstring(2,in,szBuf);
	switch (szBuf[0])
	{
		case 'c': getfieldstring(13,in,out); break;
		case 's': getfieldstring(13,in,out); break;
		case 'v': getfieldstring(13,in,out); break;
		case 'r': getfieldstring(13,in,out); break;
		case 'd': getfieldstring(12,in,out); break;
		case 'w': getfieldstring(12,in,out); break;
		case 'e': getfieldstring(10,in,out); break;
		case 'E': getfieldstring(10,in,out); break;
		case 'i': getfieldstring(10,in,out); break;
		case 'j': getfieldstring(10,in,out); break;
		case 'l':
			getfieldstring(1,in,szBuf);
			if (szBuf[0]=='l')
			{
				getfieldstring(11,in,szBuf);
				sprintf(out,"Light Side Text: %s ",strlen(szBuf)?szBuf:"none");
			} else
			{
				getfieldstring(13,in,szBuf);
				sprintf(out,"Dark Side Text: %s ",strlen(szBuf)?szBuf:"none");
			}
			break;
		case 'O':
			getfieldstring(10,in,szBuf);
			sprintf(out,"Second Side Text: %s",strlen(szBuf)?szBuf:"none");
			break;
		default: break;
	}
//	if (strlen(out)) out[strlen(out)-1]='\0';
}

// this functions scans all the files in the /data directory
// for any spoilers (.dat files) made after the last workfile
// was made.  if there are any (and the workfile needs to be
// updated) changes or problems, it return TRUE.  If there is
// a current workfile, and all data files have a write date
// earlier than the workfile, it returns FALSE.
BOOL needtocreateworkfile(void)
{
	struct	_finddata_t c_file;
	time_t	datatm;
	long	hFile;
	char search[_MAX_PATH];

	sprintf(search,"%s\\%s",szApp,DATAFILEN);

	if ( (hFile=_findfirst(search,&c_file))==-1L)
		return TRUE; // no SWCCG Cards.dat file, definitely make it
	datatm = c_file.time_write;
	_findclose(hFile);

	sprintf(search,"%s\\*.dat",szApp);

	if ( (hFile=_findfirst(search,&c_file))==-1L)
		return TRUE; // leave if no files can be found

	// if a file was created after the data file, one needs to be made
	if (c_file.time_write>datatm) { _findclose(hFile); return TRUE; }
	
	// check the rest of the .dat files
	while (_findnext(hFile,&c_file)==0)
		if (c_file.time_write>datatm) { _findclose(hFile); return TRUE; }

	_findclose(hFile);

	return FALSE; // if we've gotten here, must be because nothing changed
}



// this function parses all data in the /data directory
// into a data file 'SWCCG Cards.dat' which is used by
// the program for all data.
BOOL createworkfile(HWND hwndProgress)
{

	struct _finddata_t c_file;
	long	hFile;
	long	number_of_files=0,i=0,z,check=TRUE;
	FILE	*fin,*fout;
	char	szBuf[MAXSPOILERLINE],szParsed[MAXSPOILERLINE];
	BOOL	bNoCont;
	char datsearch[_MAX_PATH];

	if (hwndProgress!=NULL) SetWindowText(hwndProgress,"checking for updates...");


	memset(&c_file,0,sizeof(c_file));

	sprintf(datsearch,"%s\\*.dat",szApp);

	if ( (hFile=_findfirst(datsearch,&c_file))==-1L)
		return FALSE; // leave if no files can be found

	// count the number of dat files
	number_of_files++;
	while (_findnext(hFile,&c_file)==0) number_of_files++;
	_findclose(hFile);

	MAXLIST=number_of_files*640;
	masterlist = malloc(sizeof(struct mcard)*640*number_of_files);
	cardlist = malloc(sizeof(struct card)*640*number_of_files);
	decklist = malloc(sizeof(struct card)*640*number_of_files);

	if (!needtocreateworkfile())
	{
		if (hwndProgress!=NULL) SetWindowText(hwndProgress,"creating database...");
		return TRUE; // no changes, don't bother updating
	}

	sprintf(szBuf,"loading %li sets",number_of_files);
	if (hwndProgress!=NULL) SetWindowText(hwndProgress,szBuf);

	if ((fout= fopen(DATAFILEN,"wb"))==NULL)
		return FALSE;

	// go through each file and process
	for (i=0L;i<number_of_files;i++)
	{
		if (i==0L) hFile=_findfirst("*.dat",&c_file);
		else _findnext(hFile,&c_file);
		sprintf(szBuf,"%s",c_file.name);
		if ((fin = fopen(szBuf,"r"))==NULL)
		{
			fclose(fout);
			return FALSE;
		}
//		fprintf(fout,"#%s\n",szBuf);
		sprintf(szParsed,"loading %s",szBuf);
		if (hwndProgress!=NULL) SetWindowText(hwndProgress,szParsed);

		fgets(szBuf,MAXSPOILERLINE,fin);
		while (!feof(fin))
		{
			if ((szBuf[0]=='l')||(szBuf[0]=='d'))
//					fprintf(fout,"%s",szBuf);				
			{
				
				for (z=0,check=TRUE;check;z++) {
					check = parseattr(szBuf,szParsed,z);
					stwrite(fout,szParsed);
					parsecardname(szBuf,szParsed);
					stwrite(fout,szParsed);
					parselore(szBuf,szParsed);
					stwrite(fout,szParsed);
					parsedesc(szBuf,szParsed);
					stwrite(fout,szParsed);
					getbool("View","NoUnlimited",&bNoCont);
					if (bNoCont) break;
				}
			}
			fgets(szBuf,MAXSPOILERLINE,fin);
		}
		fclose(fin);
	}

	fclose(fout);

	if (hwndProgress!=NULL) SetWindowText(hwndProgress,"updates found. creating database...");

	return TRUE;
}
