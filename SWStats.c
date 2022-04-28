
#include <windows.h>
#include "attributes.h"
#include "SWDeck.h"
#include <string.h>
#include <stdio.h>
#include "resource.h"
#include "SWFile.h"
#include "SWStats.h"
#include <time.h>
#include "SWPrint.h"

BOOL findincard(char *szFind,char *szCard)
{
	char szBuf[4096],szCardBuf[4096];
	unsigned int i;

	// convert card to all lower case
	for (i=0;i<strlen(szCard);i++) szCardBuf[i]=tolower(szCard[i]);
	szCardBuf[i]='\0';
	// convert search text to all lower case
	for (i=0;i<strlen(szFind);i++) szBuf[i]=tolower(szFind[i]);
	szBuf[i]='\0';

	if (strstr(szCardBuf,szBuf)!=NULL) return TRUE;
	else return FALSE;

}

BOOL rightside(int i)
{
	if (!menu.bFindLight&&(masterlist[i].attr[ATT_SIDE]=='l')) return FALSE;
	if (!menu.bFindDark&&(masterlist[i].attr[ATT_SIDE]=='d')) return FALSE;
	return TRUE;
}

BOOL FindStats(HWND hDlg,char *szSearch)
{
	int i,k,total;
	char szBuf[4096];
	HWND	hList,hCards;

	hList = GetDlgItem(hDlg,IDC_STATSLIST);
	hCards = GetDlgItem(hDlg,IDC_STATSLABEL);

	if (!iCListSize)
	{
		SendMessage(hList,LB_RESETCONTENT,0,0);
		SetWindowText(hCards,"No Matches Found");
		return FALSE;
	}

	SendMessage(hList,WM_SETREDRAW,FALSE,0);
	SendMessage(hList,LB_RESETCONTENT,0,0);

	for (i=0,total=0;i<iCListSize;i++)
	{
		k=0;
		if (menu.bFindName&&rightside(cardlist[i].idx))
			if (findincard(szSearch,masterlist[cardlist[i].idx].name))
			{
				sprintf(szBuf,"%s\t\t\t\t\t\t\t\t\t\t\t%d",masterlist[cardlist[i].idx].name,cardlist[i].idx);
				SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szBuf);
				total++;
				k=1;
			}
		if (menu.bFindLore&&rightside(cardlist[i].idx)&&!k)
			if (findincard(szSearch,masterlist[cardlist[i].idx].lore))
			{
				sprintf(szBuf,"%s\t\t\t\t\t\t\t\t\t\t\t%d",masterlist[cardlist[i].idx].name,cardlist[i].idx);
				SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szBuf);
				total++;
				k=1;
			}
		if (menu.bFindDesc&&rightside(cardlist[i].idx)&&!k)
			if (findincard(szSearch,masterlist[cardlist[i].idx].desc))
			{
				sprintf(szBuf,"%s\t\t\t\t\t\t\t\t\t\t\t%d",masterlist[cardlist[i].idx].name,cardlist[i].idx);
				SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szBuf);
				total++;
			}

	}

	SendMessage(hList,WM_SETREDRAW,TRUE,0);

	sprintf(szBuf,"Matches Found: %d",total);
	SetWindowText(hCards,szBuf);

	return TRUE;			
}
/*
// calculates a low/middle/high value(of att) for all cards with a type of 'type1'
// and a subtype of 'type2'.  '*' can be used as a wildcard for either type.
// ie. spreadAtt(lpBuf,'C','D',ATT_POWER,&l,&m,&h) will fill l,m and h with a low,
// middle and high ATT_POWER for all 'C'haracter 'D'roids.  Replacing 'D' with '*'
// will give ATT_POWER values for all characters, regardless of subtype.
void spreadAtt(LPSTR lpBuf,int type1,int type2,int att,char *szBuf)
{
	int		i,ch1,ch2,value;
	long	total,number;
	int		low,mid,hi;

	low=9; mid=0; hi=0; total=0L; number=0L;
	for (i=0;i<iCListSize;i++)
	{
		if (cardlist[i].deck)
		{
			ch1 = masterlist[cardlist[i].idx].attr[type1];
			ch2 = [masterlist[cardlist[i].idx].attr[type2];
			if ( ((type1=='*')||(type1==ch1)) && ((type2=='*')||(type2==ch2)) )
			{
				value = masterlist[cardlist[i].idx].attr[att];
				if ((value<'0')||(value>'9')) value='0';
				value -= '0';
				if (value<=low) low=value;
				if (value>=hi)  hi =value;
				total += (long)value*(long)cardlist[i].deck;
				number += (long)cardlist[i].deck;
			}
		}
	}

	if (number)
	{
		mid=(int)((total*10L)/number);
	}
	else low=0;
	
	if (low==hi) sprintf(szBuf,"%i",low);
	else
	{
		switch (wRangeMode)
		{
			case ID_STATS_RANGE_LMH:
				sprintf(szBuf,"%i/%i.%i/%i",low,mid/10,mid%10,hi);
				break;
			case ID_STATS_RANGE_LH:
				sprintf(szBuf,"%i->%i",low,hi);
				break;
			case ID_STATS_RANGE_M:
				sprintf(szBuf,"%i.%i",mid/10,mid%10);
				break;
			default:
				sprintf(szBuf,"-");
				break;
		}
	}

}

// returns a grand total of values in the att field of all cards of _TYPE type1
// and _SUBTYPE type2.
long totalAtt(LPSTR lpBuf,int type1,int type2,int att)
{
	long	total;
	int		ch1,ch2;
	int		i,value;
	
	total = 0L;
	for (i=0;i<iListSize;i++)
	{
		if (cardlist[i].deck)
		{
			ch1 = lpBuf[masterlist[cardlist[i].idx].attributes+ATT_TYPE];
			ch2 = lpBuf[masterlist[cardlist[i].idx].attributes+ATT_SUBTYPE];
			if ( ((type1=='*')||(type1==ch1)) && ((type2=='*')||(type2==ch2)) )
			{
				value = lpBuf[masterlist[cardlist[i].idx].attributes+att];
				if (value=='Y') value='1';
				if ((value<'0')||(value>'9')) value = '0';
				value -= '0';
				total += (long)value*(long)cardlist[i].deck;
			}
		}
	}
	
	return total;
}

// calculates the probability of drawing one of "total" cards in a deck
// with "grand" cards, and also of drawing it in the starting hand of
// 8 cards.  returns TRUE if the both numbers are valid, FALSE if invalid.
// the results are written to the strings *draw and *hand
BOOL calc(long total,long grand,char *draw,char *hand)
{
	double	fHand,fDraw,fGrand,fTotal;
	int		idraw,ihand;

	idraw = 0;
	ihand = 0;	

	if (grand<1)
	{
		sprintf(draw,"---");
		sprintf(hand,"---");
		return FALSE;	// need at least 1 card for a %Draw
	}
	
	fTotal = (double)total;
	fGrand = (double)grand;
	
	fDraw = (fTotal/fGrand)*1000.0;
	idraw = (int)fDraw;

	if (grand<8)
	{
		sprintf(draw,"%i.%i",idraw/10,idraw%10);
		sprintf(hand,"---");
		return FALSE;
	}
	
	fHand = (1.0-(((fGrand-fTotal)/fGrand)*
				  ((fGrand-fTotal-1.0)/(fGrand-1.0))*
				  ((fGrand-fTotal-2.0)/(fGrand-2.0))*
				  ((fGrand-fTotal-3.0)/(fGrand-3.0))*
				  ((fGrand-fTotal-4.0)/(fGrand-4.0))*
				  ((fGrand-fTotal-5.0)/(fGrand-5.0))*
				  ((fGrand-fTotal-6.0)/(fGrand-6.0))*
				  ((fGrand-fTotal-7.0)/(fGrand-7.0)) ) )*1000.0;

	ihand = (int)fHand;

	sprintf(draw,"%i.%i",idraw/10,idraw%10);
	sprintf(hand,"%i.%i",ihand/10,ihand%10);
	
	return TRUE;
	
}

// counts the total of "type"s of the passed "att"ribute
// in the deck.
long countAtt(LPSTR lpBuf,int type,int att)
{
	long	total;
	int		i;

	for (total=0L,i=0;i<iListSize;i++)
	{
		if ((lpBuf[masterlist[cardlist[i].idx].attributes+att]==type)||
			((lpBuf[masterlist[cardlist[i].idx].attributes+att]=='T')&&(type=='P')))
			total += cardlist[i].deck;
	}

	return total;
}

long countAttAtt(LPSTR lpBuf,int type1,int att1,int type2,int att2)
{
	long	total;
	int		i;
	
	for (total=0L,i=0;i<iListSize;i++)
	{
		if ((lpBuf[masterlist[cardlist[i].idx].attributes+att1]==type1) &&
		    (lpBuf[masterlist[cardlist[i].idx].attributes+att2]==type2))
		{
			total += (long)cardlist[i].deck;
		}
	}
	
	return total;
}

// gives a "roll call" of a the number of cards in the list that have a
// 'type' at the given 'att' (ie. rollcall(lBuf,'l',ATT_SET) will give
// the number of Ltd/Unltd cards in the list. (not the number in the deck!)
int rollcall(LPSTR lpBuf,int type,int att)
{
	int i,total;
	
	for (total=0,i=0;i<iListSize;i++)
	{
		if (lpBuf[masterlist[cardlist[i].idx].attributes+att]==type)
			total++;
	}
	
	return total;
}

*/

// calculates the probability of drawing one of "total" cards in a deck
// with "grand" cards, and also of drawing it in the starting hand of
// 8 cards.  returns TRUE if the both numbers are valid, FALSE if invalid.
// the results are written to the strings *draw and *hand
BOOL calc(long total,long grand,char *draw,char *hand)
{
	double	fHand,fDraw,fGrand,fTotal;
	int		idraw,ihand;

	idraw = 0;
	ihand = 0;	

	if (grand<1)
	{
		sprintf(draw,"---");
		sprintf(hand,"---");
		return FALSE;	// need at least 1 card for a %Draw
	}
	
	fTotal = (double)total;
	fGrand = (double)grand;
	
	fDraw = (fTotal/fGrand)*1000.0;
	idraw = (int)fDraw;

	if (grand<8)
	{
		sprintf(draw,"%i.%i",idraw/10,idraw%10);
		sprintf(hand,"---");
		return FALSE;
	}
	
	fHand = (1.0-(((fGrand-fTotal)/fGrand)*
				  ((fGrand-fTotal-1.0)/(fGrand-1.0))*
				  ((fGrand-fTotal-2.0)/(fGrand-2.0))*
				  ((fGrand-fTotal-3.0)/(fGrand-3.0))*
				  ((fGrand-fTotal-4.0)/(fGrand-4.0))*
				  ((fGrand-fTotal-5.0)/(fGrand-5.0))*
				  ((fGrand-fTotal-6.0)/(fGrand-6.0))*
				  ((fGrand-fTotal-7.0)/(fGrand-7.0)) ) )*1000.0;

	ihand = (int)fHand;

	sprintf(draw,"%i.%i",idraw/10,idraw%10);
	sprintf(hand,"%i.%i",ihand/10,ihand%10);
	
	return TRUE;
	
}

// calculates a low/middle/high value(of att) for all cards with a type of 'type1'
// and a subtype of 'type2'.  '*' can be used as a wildcard for either type.
// ie. spreadAtt('C','D',ATT_POWER,&l,&m,&h) will fill l,m and h with a low,
// middle and high ATT_POWER for all 'C'haracter 'D'roids.  Replacing 'D' with '*'
// will give ATT_POWER values for all characters, regardless of subtype.
void spreadAtt(int type1,int type2,int att,char *szBuf)
{
	int		i,ch1,ch2,value;
	long	total,number;
	int		low,mid,hi;

	low=20; mid=0; hi=0; total=0L; number=0L;
	for (i=0;i<iCListSize;i++)
	{
		if (cardlist[i].deck)
		{
			ch1 = masterlist[cardlist[i].idx].attr[ATT_TYPE];
			ch2 = masterlist[cardlist[i].idx].attr[ATT_SUBTYPE];
			if ((((type1=='*')||(type1==ch1))&&((type2=='*')||(type2==ch2))) ||
				((type2=='z')&&((ch2=='I')||(ch2=='R')))) // make special case for imp/rebels
			{
				value = masterlist[cardlist[i].idx].attr[att];
				if ((value<'0')||(value>'9')) value='0';
				value -= '0';
				if (value<=low) low=value;
				if (value>=hi)  hi =value;
				total += (long)value*(long)cardlist[i].deck;
				number += (long)cardlist[i].deck;
			}
		}
	}

	if (number)
	{
		mid=(int)((total*10L)/number);
	}
	else low=0;
	
	if (low==hi) sprintf(szBuf,"%i",low);
	else
	{
		switch (menu.iRangeMode)
		{
			case ID_RANGES_LMH:
				sprintf(szBuf,"%i/%i.%i/%i",low,mid/10,mid%10,hi);
				break;
			case ID_RANGES_LH:
				sprintf(szBuf,"%i > %i",low,hi);
				break;
			case ID_RANGES_M:
				sprintf(szBuf,"%i.%i",mid/10,mid%10);
				break;
			default:
				sprintf(szBuf,"-");
				break;
		}
	}

}

long totalAtt(int type1,int type2,int att)
{
	long	total;
	int		ch1,ch2;
	int		i,value;
	
	total = 0L;
	for (i=0;i<iCListSize;i++)
	{
		if (cardlist[i].deck)
		{
			ch1 = masterlist[cardlist[i].idx].attr[ATT_TYPE];
			ch2 = masterlist[cardlist[i].idx].attr[ATT_SUBTYPE];
//			if ( ((type1=='*')||(type1==ch1)) && ((type2=='*')||(type2==ch2)) )
			if ((((type1=='*')||(type1==ch1))&&((type2=='*')||(type2==ch2))) ||
				((type2=='z')&&((ch2=='I')||(ch2=='R')))) // make special case for imp/rebels
			{
				value = masterlist[cardlist[i].idx].attr[att];
				if (value=='Y') value='1';
				if ((value<'0')||(value>'>')) value = '0';
				value -= '0';
				total += (long)value*(long)cardlist[i].deck;
			}
		}
	}
	
	return total;
}

long countAtt(int type,int att)
{
	long	total;
	int		i;

	for (total=0L,i=0;i<iCListSize;i++)
	{
		if (masterlist[cardlist[i].idx].attr[att]==type)
			total += cardlist[i].deck;
	}

	return total;
}

long countAttAtt(int type1,int att1,int type2,int att2)
{
	long	total;
	int		i;
	
	for (total=0L,i=0;i<iCListSize;i++)
	{
		if ((masterlist[cardlist[i].idx].attr[att1]==type1) &&
		    (masterlist[cardlist[i].idx].attr[att2]==type2))
		{
			total += (long)cardlist[i].deck;
		}
	}
	
	return total;
}

BOOL updatestats(HWND hDlg)
{
	int i,j,k,total;
	HWND hList,hCards;
	int iTop,iDListSize;
	char szBuf[256],szLine[4096];
	char szHand[32],szDraw[32];
	long li;
	char szRn1[32],szRn2[32],szRn3[32];
	float fhave=0,fneed=0;

	if (menu.iStatsMode==ID_STATS_FIND) return FALSE;

	hList = GetDlgItem(hDlg,IDC_STATSLIST);
	hCards = GetDlgItem(hDlg,IDC_STATSLABEL);

	switch (menu.iStatsMode)
	{
		case ID_STATS_DETAIL: sprintf(szBuf,"Deck Detail"); break;
		case ID_STATS_SUMMARY: sprintf(szBuf,"Deck Summary"); break;
		case ID_STATS_DESTINY: sprintf(szBuf,"Deck Destiny"); break;
		case ID_STATS_SET: sprintf(szBuf,"Set Statistics"); break;
		case ID_STATS_PROBABILITY: sprintf(szBuf,"Card Probabilities"); break;
		case ID_STATS_CHARACTERS: sprintf(szBuf,"Character Statistics"); break;
		case ID_STATS_LOCATIONS: sprintf(szBuf,"Location Statistics"); break;
		case ID_STATS_SHIPS: sprintf(szBuf,"Ship/Vehicle Statistics"); break;
		case ID_STATS_INTERRUPTS: sprintf(szBuf,"Interrupt/Effect Statistics"); break;
		case ID_STATS_WEAPONS: sprintf(szBuf,"Weapon/Device Statistics"); break;
		case ID_STATS_NOTES: sprintf(szBuf,"Deck Notes"); break;
		default: sprintf(szBuf,"Deck Information"); break;
	}
	SetWindowText(hDlg,szBuf);

	if (menu.iStatsMode==ID_STATS_NOTES)
	{
		SetWindowText(hCards,"Enter Deck Notes Here");
		return FALSE;
	}

	iTop = SendMessage(hList,LB_GETTOPINDEX,0,0);

	if (!iCListSize)
	{
		SendMessage(hList,LB_RESETCONTENT,0,0);
		SetWindowText(hCards,"no cards");
		return FALSE;
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

	sprintf(szBuf,"Cards: %d",total);
	SetWindowText(hCards,szBuf);

	switch (menu.iStatsMode)
	{
		case ID_STATS_DETAIL:
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"number\tcard");
			for (i=0;i<iDListSize;i++)
			{
				sprintf(szBuf,"%d\t%s\t\t\t\t\t\t\t\t\t\t\t%d",decklist[i].deck,masterlist[decklist[i].idx].name,decklist[i].idx);
				SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szBuf);
			}
			break;

		case ID_STATS_SUMMARY:
			j = countAtt('l',ATT_SIDE);
			sprintf(szBuf,"%d light side\t%d dark side",j,total-j);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szBuf);
			spreadAtt('*','*',ATT_DESTINY,szBuf);
			sprintf(szLine,"destiny: %s",szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"type\tnumber\tdestiny\tdraw\thand");

			spreadAtt('L','*',ATT_DESTINY,szBuf);
			li = countAtt('L',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"location\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('O','*',ATT_DESTINY,szBuf);
			li = countAtt('O',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"objective\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('C','*',ATT_DESTINY,szBuf);
			li = countAtt('C',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"char.\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('R','*',ATT_DESTINY,szBuf);
			li = countAtt('R',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"creature\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('S','*',ATT_DESTINY,szBuf);
			li = countAtt('S',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"starship\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('V','*',ATT_DESTINY,szBuf);
			li = countAtt('V',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"vehicle\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('I','*',ATT_DESTINY,szBuf);
			li = countAtt('I',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"interrupt\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('E','*',ATT_DESTINY,szBuf);
			li = countAtt('E',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"effect\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('W','*',ATT_DESTINY,szBuf);
			li = countAtt('W',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"weapon\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('D','*',ATT_DESTINY,szBuf);
			li = countAtt('D',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"device\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('N','*',ATT_DESTINY,szBuf);
			li = countAtt('N',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"event\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			spreadAtt('J','*',ATT_DESTINY,szBuf);
			li = countAtt('J',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"jedi test\t%li\t%s\t%s\t%s",li,szBuf,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			break;

		case ID_STATS_SET:
/*			for (i=0,k=0;i<MAXSETS;i++)
			{
				for (j=0;j<iCListSize;j++)
					if ((masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])&&
						cardlist[j].deck)
					{ k++; j=iCListSize; }
			}
			sprintf(szLine,"Sets Found: %d",k);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine); */
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"cards\tneed\tset");

			for (i=0;i<MAXSETS;i++)
			{
				for (j=0,li=0,k=0;j<iCListSize;j++)
				{
					if (masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])
					{
						li += cardlist[j].deck;
						if (cardlist[j].deck==0)
						{
								k++;
								fneed += masterlist[cardlist[j].idx].fPrice;
						}
						else
						{
								fhave += masterlist[cardlist[j].idx].fPrice * cardlist[j].deck;
						}
					}
				}
				sprintf(szLine,"%li\t%d\t%s",li,k,sets[i]+1);
				if (li||k) SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			}
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)" ");
			sprintf(szLine,"estimated value: %.2f",fhave);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"value of cards needed: %.2f",fneed);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			for (i=0;i<MAXSETS;i++)
			{
				for (j=0,k=0;!k&&(j<iCListSize);j++)
				{
					if (masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])
						k=1;
				}
				if (k)
				{
					SendMessage(hList,LB_ADDSTRING,0,(LPARAM)" ");
					sprintf(szLine,"%s cards needed:",sets[i]+1);
					SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
					for (j=0,k=0;j<iCListSize;j++)
					{
						if ((masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])&&
							!cardlist[j].deck)
						{
							sprintf(szLine,"%s\t\t\t\t\t\t\t\t\t\t\t%d",masterlist[cardlist[j].idx].name,cardlist[j].idx);
							SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
							k++;
						}
					}
					if (!k) SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"(none)");
				}

			}


			break;

		case ID_STATS_DESTINY:
			spreadAtt('*','*',ATT_DESTINY,szBuf);
			sprintf(szLine,"Destiny: %s",szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"destiny\tnumber\tdraw\thand");

			li = countAtt('0',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"0\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			li = countAtt('1',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"1\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			li = countAtt('2',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"2\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			li = countAtt('3',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"3\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			li = countAtt('4',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"4\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			li = countAtt('5',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"5\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			li = countAtt('6',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"6\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			li = countAtt('7',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			sprintf(szLine,"7\t%li\t%s\t%s",li,szDraw,szHand);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);

			break;

		case ID_STATS_PROBABILITY:
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"draw\thand\tnumber\tcard");
			if (!total) SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"(no cards)");
			for (i=0;i<iCListSize;i++)
			{
				if (cardlist[i].deck)
				{
					calc(cardlist[i].deck,total,szDraw,szHand);
					sprintf(szLine,"%s\t%s\t%d\t%s\t\t\t\t\t\t\t\t\t\t\t%d",szDraw,szHand,cardlist[i].deck,masterlist[cardlist[i].idx].name,cardlist[i].idx);
					SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
				}
			}
			break;

		case ID_STATS_CHARACTERS:
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"\timp/reb\talien\tdroid");
			sprintf(szLine,"number\t%li\t%li\t%li",
				countAttAtt('C',ATT_TYPE,'R',ATT_SUBTYPE)+countAttAtt('C',ATT_TYPE,'I',ATT_SUBTYPE),
				countAttAtt('C',ATT_TYPE,'A',ATT_SUBTYPE),
				countAttAtt('C',ATT_TYPE,'D',ATT_SUBTYPE));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('C','z',ATT_DESTINY,szRn1);
			spreadAtt('C','A',ATT_DESTINY,szRn2);
			spreadAtt('C','D',ATT_DESTINY,szRn3);
			sprintf(szLine,"destiny\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('C','z',ATT_POWER,szRn1);
			spreadAtt('C','A',ATT_POWER,szRn2);
			spreadAtt('C','D',ATT_POWER,szRn3);
			sprintf(szLine,"power\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('C','z',ATT_ABILITY,szRn1);
			spreadAtt('C','A',ATT_ABILITY,szRn2);
			spreadAtt('C','D',ATT_ABILITY,szRn3);
			sprintf(szLine,"ability*\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('C','z',ATT_DEPLOY,szRn1);
			spreadAtt('C','A',ATT_DEPLOY,szRn2);
			spreadAtt('C','D',ATT_DEPLOY,szRn3);
			sprintf(szLine,"deploy\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('C','z',ATT_FORFEIT,szRn1);
			spreadAtt('C','A',ATT_FORFEIT,szRn2);
			spreadAtt('C','D',ATT_FORFEIT,szRn3);
			sprintf(szLine,"forfeit\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"pilots\t%li\t%li\t%li",
				totalAtt('C','z',ATT_PILOT),
				totalAtt('C','A',ATT_PILOT),
				totalAtt('C','D',ATT_PILOT));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"warrior\t%li\t%li\t%li",
				totalAtt('C','z',ATT_WARRIOR),
				totalAtt('C','A',ATT_WARRIOR),
				totalAtt('C','D',ATT_WARRIOR));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"* armor for droids");
			break;

		case ID_STATS_SHIPS:
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"\tcapital\tfighter\tvehicle");
			sprintf(szLine,"number\t%li\t%li\t%li",
				countAttAtt('S',ATT_TYPE,'C',ATT_SUBTYPE)+countAttAtt('C',ATT_TYPE,'I',ATT_SUBTYPE),
				countAttAtt('S',ATT_TYPE,'F',ATT_SUBTYPE),
				countAttAtt('V',ATT_TYPE,'*',ATT_SUBTYPE));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('S','C',ATT_DESTINY,szRn1);
			spreadAtt('S','F',ATT_DESTINY,szRn2);
			spreadAtt('V','*',ATT_DESTINY,szRn3);
			sprintf(szLine,"destiny\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('S','C',ATT_POWER,szRn1);
			spreadAtt('S','F',ATT_POWER,szRn2);
			spreadAtt('V','*',ATT_POWER,szRn3);
			sprintf(szLine,"power\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('S','C',ATT_ABILITY,szRn1);
			spreadAtt('S','F',ATT_ABILITY,szRn2);
			spreadAtt('V','*',ATT_ABILITY,szRn3);
			sprintf(szLine,"armor*\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('S','C',ATT_SPEED,szRn1);
			spreadAtt('S','F',ATT_SPEED,szRn2);
			spreadAtt('V','*',ATT_SPEED,szRn3);
			sprintf(szLine,"speed\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('S','C',ATT_DEPLOY,szRn1);
			spreadAtt('S','F',ATT_DEPLOY,szRn2);
			spreadAtt('V','*',ATT_DEPLOY,szRn3);
			sprintf(szLine,"deploy\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('S','C',ATT_FORFEIT,szRn1);
			spreadAtt('S','F',ATT_FORFEIT,szRn2);
			spreadAtt('V','*',ATT_FORFEIT,szRn3);
			sprintf(szLine,"forfeit\t%s\t%s\t%s",szRn1,szRn2,szRn3);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"pilots\t%li\t%li\t%li",
				totalAtt('S','C',ATT_PPILOT),
				totalAtt('S','F',ATT_PPILOT),
				totalAtt('S','*',ATT_PPILOT));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"* maneuver for starfighters");
			break;

		case ID_STATS_LOCATIONS:
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"\tnumber\tds force\tls force");
			sprintf(szLine,"systems\t%li\t%li\t%li",
				countAttAtt('L',ATT_TYPE,'P',ATT_SUBTYPE)+countAttAtt('L',ATT_TYPE,'M',ATT_SUBTYPE),
				totalAtt('L','P',ATT_IFORCE)+totalAtt('L','M',ATT_IFORCE),
				totalAtt('L','P',ATT_RFORCE)+totalAtt('L','M',ATT_RFORCE));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"sites\t%li\t%li\t%li",
				countAttAtt('L',ATT_TYPE,'T',ATT_SUBTYPE),
				totalAtt('L','T',ATT_IFORCE),
				totalAtt('L','T',ATT_RFORCE));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"sectors\t%li\t%li\t%li",
				countAttAtt('L',ATT_TYPE,'S',ATT_SUBTYPE),
				totalAtt('L','S',ATT_IFORCE),
				totalAtt('L','S',ATT_RFORCE));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)" ");
			spreadAtt('L','P',ATT_PARSEC,szBuf);
			sprintf(szLine,"parsec\t%s",szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"interior\t%li\texterior\t%li",
				countAttAtt('L',ATT_TYPE,'N',ATT_ICON),
				countAttAtt('L',ATT_TYPE,'X',ATT_ICON));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"dark force generation\t%li",totalAtt('L','*',ATT_IFORCE));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			sprintf(szLine,"light force generation\t%li",totalAtt('l','*',ATT_RFORCE));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			break;

		case ID_STATS_INTERRUPTS:
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"interrupts    \tnumber\tdestiny");
			spreadAtt('I','U',ATT_DESTINY,szBuf);
			sprintf(szLine,"used\t\t%li\t%s",countAttAtt('I',ATT_TYPE,'U',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('I','l',ATT_DESTINY,szBuf);
			sprintf(szLine,"lost\t\t%li\t%s",countAttAtt('I',ATT_TYPE,'l',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('I','B',ATT_DESTINY,szBuf);
			sprintf(szLine,"used/lost    \t%li\t%s",countAttAtt('I',ATT_TYPE,'B',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('I','S',ATT_DESTINY,szBuf);
			sprintf(szLine,"starting\t\t%li\t%s",countAttAtt('I',ATT_TYPE,'S',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)" ");
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"effects\t\tnumber\tdestiny");
			spreadAtt('E','0',ATT_DESTINY,szBuf);
			sprintf(szLine,"normal\t\t%li\t%s",countAttAtt('E',ATT_TYPE,'0',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('E','U',ATT_DESTINY,szBuf);
			sprintf(szLine,"utinni\t\t%li\t%s",countAttAtt('E',ATT_TYPE,'U',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('E','I',ATT_DESTINY,szBuf);
			sprintf(szLine,"immediate\t%li\t%s",countAttAtt('E',ATT_TYPE,'I',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('E','M',ATT_DESTINY,szBuf);
			sprintf(szLine,"mobile\t\t%li\t%s",countAttAtt('E',ATT_TYPE,'M',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			break;

		case ID_STATS_WEAPONS:
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"weapons   \tnumber\tdestiny");
			spreadAtt('W','C',ATT_DESTINY,szBuf);
			sprintf(szLine,"character   \t%li\t%s",countAttAtt('W',ATT_TYPE,'C',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('W','S',ATT_DESTINY,szBuf);
			sprintf(szLine,"starship   \t%li\t%s",countAttAtt('W',ATT_TYPE,'S',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('W','A',ATT_DESTINY,szBuf);
			sprintf(szLine,"automated   \t%li\t%s",countAttAtt('W',ATT_TYPE,'A',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			spreadAtt('W','R',ATT_DESTINY,szBuf);
			sprintf(szLine,"artillery\t\t%li\t%s",countAttAtt('W',ATT_TYPE,'R',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)" ");
			spreadAtt('D','*',ATT_DESTINY,szBuf);
			sprintf(szLine,"devices\t\t%li\t%s",countAttAtt('D',ATT_TYPE,'*',ATT_SUBTYPE),szBuf);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine);
			break;
	}

	SendMessage(hList,LB_SETTOPINDEX,iTop,0);
	SendMessage(hList,WM_SETREDRAW,TRUE,0);

	return TRUE;
}

int getncard(long li)
{
	int i,j,r;
	long lj;

	r = 0;
	
	for (i=0,lj=0L;i<iCListSize;i++)
	{
		if (cardlist[i].deck)
		{
			for (j=0;j<cardlist[i].deck;j++)
			{
				if (lj==li) r=i;
				lj++;
			}
		}
	}

   return r;
}

void testdraw(HWND hwnd)
{
	HWND	hList;
	long	total,li;
	int		i,j,k;
	long	lDraw[8];

	hList = GetDlgItem(hwnd,IDC_STATSLIST);
	
	// calculate total number of cards in the deck
	for (total=0L,i=0;i<iCListSize;i++)
		total += cardlist[i].deck;

	if (total<8L)
	{
		SendMessage(hList,LB_RESETCONTENT,0,0L);
		SendMessage(hList,LB_ADDSTRING,0,(LONG)"The Test Draw will draw eight");
		SendMessage(hList,LB_ADDSTRING,0,(LONG)"cards from your deck.  There");
		SendMessage(hList,LB_ADDSTRING,0,(LONG)"are currently less than eight.");
		SendMessage(hList,LB_ADDSTRING,0,(LONG)"");
		SendMessage(hList,LB_ADDSTRING,0,(LONG)"Try again when you have at least");
		SendMessage(hList,LB_ADDSTRING,0,(LONG)"eight cards.");
	}
	else
	{

		SetWindowText(hwnd,"Test Draw");
		for (i=0;i<8;i++) lDraw[i]=-1L;
		for (i=0;i<8;i++)
		{
			do
			{
				k=0;
				li = rand()%(long)total;
				for (j=0;j<8;j++) if (lDraw[j]==li) k=1;
			} while (k);
			
			lDraw[i]=li;
			
		}

		do			// sort the drawn cards
		{
			k = 0;
			for (i=0;i<7;i++)
			{
				if (lDraw[i]>lDraw[i+1])
				{
					total = lDraw[i];
					lDraw[i] = lDraw[i+1];
					lDraw[i+1] = total;
					k=1;
				}
			}
		} while (k);


	   	SendMessage(hList,WM_SETREDRAW,FALSE,0L);
	   	SendMessage(hList,LB_RESETCONTENT,0,0L);
	   	
		for (i=0;i<8;i++)
		{
			SendMessage(hList,LB_ADDSTRING,0,
				(LONG)masterlist[cardlist[getncard(lDraw[i])].idx].name);
		}
		SendMessage(hList,WM_SETREDRAW,TRUE,0L);		
	}

			
}

// copies the filen file to the clipboard
BOOL sendtoclipboard(char *filen)
{
	HGLOBAL hGMem;
	PSTR	pGMem;
	FILE	*fh;
	long	iLength;
	char	*pTempFile;
	long	i;

	if ((fh=fopen(filen,"rb"))==NULL)
		return FALSE;

	iLength = FileLength(fh);

	if ((pTempFile=(char *)malloc(iLength+1))==NULL)
	{
		fclose(fh);
		return FALSE;
	}

	// read all data from the file into pCardData
	fread(pTempFile,1,iLength,fh);
	fclose(fh);
	DeleteFile(filen);

	hGMem = GlobalAlloc(GHND,iLength+1);
	pGMem = GlobalLock(hGMem);
	for (i=0;i<iLength;i++)
		*pGMem++ = pTempFile[i];
	GlobalUnlock(hGMem);
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT,hGMem);
	CloseClipboard();
	free(pTempFile);

	return TRUE;
}

BOOL sendtofile(HWND hwnd,char *filen)
{
	char szPrintFileName[_MAX_FNAME];
	char szPrintFile[_MAX_FNAME];
	char szDefaultName[_MAX_FNAME];

	sprintf(szPrintFile,filen);

	sprintf(szDefaultName,"%s",szTitleName);
	if (strlen(szDefaultName))
	{
		sprintf(strstr(szDefaultName,".deck"),".txt");
	}
	sprintf(szPrintFileName,szDefaultName);
	
	if (SWPrintFileSaveDlg(hwnd,(PSTR)szPrintFileName,(PSTR)szDefaultName))
		SWPrintFileWrite(hwnd,(PSTR)szPrintFileName,(PSTR)szPrintFile);

	return TRUE;
}

BOOL sendtoprinter(HWND hwnd,char *szFilen)
{
	return PopPrntPrintFile (hwnd, szFilen);
}

// this copies the stats to one of three places:
// ID_STATS_SEND_CLIPBOARD, ID_STATS_SEND_FILE,
// ID_STATS_SEND_PRINTER
// for simplicity's sake, it recreates the stats
// but the format is drastically different
BOOL printstats(HWND hDlg,UINT uDest)
{
	int i,j,k,total;
	HWND hList=NULL;
	int iDListSize;
	char szBuf[256],szLine[4096];
	char szHand[32],szDraw[32];
	long li;
	char szRn1[32],szRn2[32],szRn3[32];
	FILE *fh,*fo; // temporary file to save stats in
	float fneed=0,fhave=0;

	if ((fh=fopen("stats.tmp","w"))==NULL)
		return FALSE;

	// make a special case for if the stats list has
	// "Find Results" in the title bar. in this case
	// since the results aren't saved anywhere, just
	// copy them from the list itself (and take out
	// the end of the lines at the tab
	GetWindowText(hDlg,szBuf,256);
	if (strstr(szBuf,"Find Results")!=NULL)
	{
		fprintf(fh,"%s ",szBuf);
		iDListSize = SendMessage(GetDlgItem(hDlg,IDC_STATSLIST),LB_GETCOUNT,0,0);
		fprintf(fh,"(%d matches)\n",iDListSize);
		fprintf(fh,"search scope: ");
		if (menu.bFindName) fprintf(fh,"[name] ");
		if (menu.bFindLore) fprintf(fh,"[lore] ");
		if (menu.bFindDesc) fprintf(fh,"[game text] ");
		if (menu.bFindDark) fprintf(fh,"[dark] ");
		if (menu.bFindLight) fprintf(fh,"[light] ");
		fprintf(fh,"\n");
		for (i=0;i<iDListSize;i++)
		{
			SendMessage(GetDlgItem(hDlg,IDC_STATSLIST),LB_GETTEXT,(WPARAM)i,(LPARAM)szBuf);
			for (j=0;j<(signed)strlen(szBuf);j++)
				if (szBuf[j]=='\t') szBuf[j]='\0';
			fprintf(fh,"%s\n",szBuf);
		}
		fclose(fh);
	}
	else
	{
	switch (menu.iStatsMode)
	{
		case ID_STATS_DETAIL: sprintf(szBuf,"Deck Detail"); break;
		case ID_STATS_SUMMARY: sprintf(szBuf,"Deck Summary"); break;
		case ID_STATS_DESTINY: sprintf(szBuf,"Deck Destiny"); break;
		case ID_STATS_SET: sprintf(szBuf,"Set Statistics"); break;
		case ID_STATS_PROBABILITY: sprintf(szBuf,"Card Probabilities"); break;
		case ID_STATS_CHARACTERS: sprintf(szBuf,"Character Statistics"); break;
		case ID_STATS_LOCATIONS: sprintf(szBuf,"Location Statistics"); break;
		case ID_STATS_SHIPS: sprintf(szBuf,"Ship/Vehicle Statistics"); break;
		case ID_STATS_INTERRUPTS: sprintf(szBuf,"Interrupt/Effect Statistics"); break;
		case ID_STATS_WEAPONS: sprintf(szBuf,"Weapon/Device Statistics"); break;
		default: sprintf(szBuf,"Deck Information"); break;
	}
	fprintf(fh,"%s\n",szBuf);

	if (!iCListSize)
	{
		fprintf(fh,"no cards\n",szBuf);
		fclose(fh);
		return FALSE;
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

	sprintf(szBuf,"Cards: %d",total);
	fprintf(fh,"%s\n",szBuf);

	switch (menu.iStatsMode)
	{
		case ID_STATS_DETAIL:
			fprintf(fh,"%10s card\n","number");
			for (i=0;i<iDListSize;i++)
			{
				sprintf(szBuf,"%10d %s",decklist[i].deck,masterlist[decklist[i].idx].name);
				fprintf(fh,"%s\n",szBuf);
			}
			break;

		case ID_STATS_SUMMARY:
			j = countAtt('l',ATT_SIDE);
			sprintf(szBuf,"%d light side  %d dark side",j,total-j);
			fprintf(fh,"%s\n",szBuf);
			spreadAtt('*','*',ATT_DESTINY,szBuf);
			sprintf(szLine,"destiny: %s",szBuf);
			fprintf(fh,"%s\n",szLine);
			fprintf(fh,"%10s %10s %10s %6s %6s\n","type","number","destiny","draw","hand");

			spreadAtt('L','*',ATT_DESTINY,szBuf);
			li = countAtt('L',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","location",li,szBuf,szDraw,szHand);

			spreadAtt('O','*',ATT_DESTINY,szBuf);
			li = countAtt('O',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","objective",li,szBuf,szDraw,szHand);

			spreadAtt('C','*',ATT_DESTINY,szBuf);
			li = countAtt('C',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","character",li,szBuf,szDraw,szHand);

			spreadAtt('R','*',ATT_DESTINY,szBuf);
			li = countAtt('R',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","creature",li,szBuf,szDraw,szHand);

			spreadAtt('S','*',ATT_DESTINY,szBuf);
			li = countAtt('S',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","starship",li,szBuf,szDraw,szHand);

			spreadAtt('V','*',ATT_DESTINY,szBuf);
			li = countAtt('V',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","vehicle",li,szBuf,szDraw,szHand);

			spreadAtt('I','*',ATT_DESTINY,szBuf);
			li = countAtt('I',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","interrupt",li,szBuf,szDraw,szHand);

			spreadAtt('E','*',ATT_DESTINY,szBuf);
			li = countAtt('E',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","effect",li,szBuf,szDraw,szHand);

			spreadAtt('W','*',ATT_DESTINY,szBuf);
			li = countAtt('W',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","weapon",li,szBuf,szDraw,szHand);

			spreadAtt('D','*',ATT_DESTINY,szBuf);
			li = countAtt('D',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","device",li,szBuf,szDraw,szHand);

			spreadAtt('N','*',ATT_DESTINY,szBuf);
			li = countAtt('N',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","event",li,szBuf,szDraw,szHand);

			spreadAtt('J','*',ATT_DESTINY,szBuf);
			li = countAtt('J',ATT_TYPE);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %10s %6s %6s\n","jedi test",li,szBuf,szDraw,szHand);
			break;

		case ID_STATS_SET:
/*			for (i=0,k=0;i<MAXSETS;i++)
			{
				for (j=0;j<iCListSize;j++)
					if ((masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])&&
						cardlist[j].deck)
					{ k++; j=iCListSize; }
			}
			sprintf(szLine,"Sets Found: %d",k);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)szLine); */
			fprintf(fh,"%10s %10s %s\n","cards","need","set");

			for (i=0;i<MAXSETS;i++)
			{
				for (j=0,li=0,k=0;j<iCListSize;j++)
				{
					if (masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])
					{
						li += cardlist[j].deck;
						if (cardlist[j].deck==0)
						{
								k++;
								fneed += masterlist[cardlist[j].idx].fPrice;
						}
						else
						{
								fhave += masterlist[cardlist[j].idx].fPrice * cardlist[j].deck;
						}
					}
				}
				if (li||k) fprintf(fh,"%10li %10d %s\n",li,k,sets[i]+1);
			}
			fprintf(fh,"estimated value: %.2f\n",fhave);
			fprintf(fh,"value of cards needed: %.2f\n",fneed);

			for (i=0;i<MAXSETS;i++)
			{
				for (j=0,k=0;!k&&(j<iCListSize);j++)
				{
					if (masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])
						k=1;
				}
				if (k)
				{
					fprintf(fh,"\n%s cards needed:\n",sets[i]+1);
					for (j=0,k=0;j<iCListSize;j++)
					{
						if ((masterlist[cardlist[j].idx].attr[ATT_SET]==sets[i][0])&&
							!cardlist[j].deck)
						{
							fprintf(fh,"%s\n",masterlist[cardlist[j].idx].name);
							k++;
						}
					}
					if (!k) fprintf(fh,"(none)\n");
				}

			}
			break;

		case ID_STATS_DESTINY:
			spreadAtt('*','*',ATT_DESTINY,szBuf);
			fprintf(fh,"Destiny: %s\n",szBuf);
			fprintf(fh,"%10s %10s %6s %6s\n","destiny","number","draw","hand");

			li = countAtt('0',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","0",li,szDraw,szHand);

			li = countAtt('1',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","1",li,szDraw,szHand);

			li = countAtt('2',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","2",li,szDraw,szHand);

			li = countAtt('3',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","3",li,szDraw,szHand);

			li = countAtt('4',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","4",li,szDraw,szHand);

			li = countAtt('5',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","5",li,szDraw,szHand);

			li = countAtt('6',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","6",li,szDraw,szHand);

			li = countAtt('7',ATT_DESTINY);
			calc(li,total,szDraw,szHand);
			fprintf(fh,"%10s %10li %6s %6s\n","7",li,szDraw,szHand);
			break;

		case ID_STATS_PROBABILITY:
			fprintf(fh,"%6s %6s %10s %s\n","draw","hand","number","card");
			if (!total) fprintf(fh,"(no cards)\n");
			for (i=0;i<iCListSize;i++)
			{
				if (cardlist[i].deck)
				{
					calc(cardlist[i].deck,total,szDraw,szHand);
					fprintf(fh,"%6s %6s %10d %s\n",szDraw,szHand,cardlist[i].deck,masterlist[cardlist[i].idx].name);
				}
			}
			break;

		case ID_STATS_CHARACTERS:
			fprintf(fh,"%10s %10s %10s %10s\n"," ","imp/reb","alien","droid");
			fprintf(fh,"%10s %10li %10li %10li\n","number",
				countAttAtt('C',ATT_TYPE,'R',ATT_SUBTYPE)+countAttAtt('C',ATT_TYPE,'I',ATT_SUBTYPE),
				countAttAtt('C',ATT_TYPE,'A',ATT_SUBTYPE),
				countAttAtt('C',ATT_TYPE,'D',ATT_SUBTYPE));
			spreadAtt('C','z',ATT_DESTINY,szRn1);
			spreadAtt('C','A',ATT_DESTINY,szRn2);
			spreadAtt('C','D',ATT_DESTINY,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","destiny",szRn1,szRn2,szRn3);
			spreadAtt('C','z',ATT_POWER,szRn1);
			spreadAtt('C','A',ATT_POWER,szRn2);
			spreadAtt('C','D',ATT_POWER,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","power",szRn1,szRn2,szRn3);
			spreadAtt('C','z',ATT_ABILITY,szRn1);
			spreadAtt('C','A',ATT_ABILITY,szRn2);
			spreadAtt('C','D',ATT_ABILITY,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","ability *",szRn1,szRn2,szRn3);
			spreadAtt('C','z',ATT_DEPLOY,szRn1);
			spreadAtt('C','A',ATT_DEPLOY,szRn2);
			spreadAtt('C','D',ATT_DEPLOY,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","deploy",szRn1,szRn2,szRn3);
			spreadAtt('C','z',ATT_FORFEIT,szRn1);
			spreadAtt('C','A',ATT_FORFEIT,szRn2);
			spreadAtt('C','D',ATT_FORFEIT,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","forfeit",szRn1,szRn2,szRn3);
			fprintf(fh,"%10s %10li %10li %10li\n","pilot",
				totalAtt('C','z',ATT_PILOT),
				totalAtt('C','A',ATT_PILOT),
				totalAtt('C','D',ATT_PILOT));
			fprintf(fh,"%10s %10li %10li %10li\n","warrior",
				totalAtt('C','z',ATT_WARRIOR),
				totalAtt('C','A',ATT_WARRIOR),
				totalAtt('C','D',ATT_WARRIOR));
			fprintf(fh,"* armor for droids\n");
			break;

		case ID_STATS_SHIPS:
			fprintf(fh,"%10s %10s %10s %10s\n"," ","capital","fighter","vehicle");
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"\tcapital\tfighter\tvehicle");
			fprintf(fh,"%10s %10li %10li %10li\n","number",
				countAttAtt('S',ATT_TYPE,'C',ATT_SUBTYPE)+countAttAtt('C',ATT_TYPE,'I',ATT_SUBTYPE),
				countAttAtt('S',ATT_TYPE,'F',ATT_SUBTYPE),
				countAttAtt('V',ATT_TYPE,'*',ATT_SUBTYPE));
			spreadAtt('S','C',ATT_DESTINY,szRn1);
			spreadAtt('S','F',ATT_DESTINY,szRn2);
			spreadAtt('V','*',ATT_DESTINY,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","destiny",szRn1,szRn2,szRn3);
			spreadAtt('S','C',ATT_POWER,szRn1);
			spreadAtt('S','F',ATT_POWER,szRn2);
			spreadAtt('V','*',ATT_POWER,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","power",szRn1,szRn2,szRn3);
			spreadAtt('S','C',ATT_ABILITY,szRn1);
			spreadAtt('S','F',ATT_ABILITY,szRn2);
			spreadAtt('V','*',ATT_ABILITY,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","armor *",szRn1,szRn2,szRn3);
			spreadAtt('S','C',ATT_SPEED,szRn1);
			spreadAtt('S','F',ATT_SPEED,szRn2);
			spreadAtt('V','*',ATT_SPEED,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","speed",szRn1,szRn2,szRn3);
			spreadAtt('S','C',ATT_DEPLOY,szRn1);
			spreadAtt('S','F',ATT_DEPLOY,szRn2);
			spreadAtt('V','*',ATT_DEPLOY,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","deploy",szRn1,szRn2,szRn3);
			spreadAtt('S','C',ATT_FORFEIT,szRn1);
			spreadAtt('S','F',ATT_FORFEIT,szRn2);
			spreadAtt('V','*',ATT_FORFEIT,szRn3);
			fprintf(fh,"%10s %10s %10s %10s\n","forfeit",szRn1,szRn2,szRn3);
			fprintf(fh,"%10s %10li %10li %10li\n","pilot",
				totalAtt('S','C',ATT_PPILOT),
				totalAtt('S','F',ATT_PPILOT),
				totalAtt('S','*',ATT_PPILOT));
			fprintf(fh,"* maneuver for starfighters\n");
			break;

		case ID_STATS_LOCATIONS:
			fprintf(fh,"%10s %10s %6s %6s\n"," ","number","ds force","ls force");
			fprintf(fh,"%10s %10li %6li %6li\n","system",
				countAttAtt('L',ATT_TYPE,'P',ATT_SUBTYPE)+countAttAtt('L',ATT_TYPE,'M',ATT_SUBTYPE),
				totalAtt('L','P',ATT_IFORCE)+totalAtt('L','M',ATT_IFORCE),
				totalAtt('L','P',ATT_RFORCE)+totalAtt('L','M',ATT_RFORCE));
			fprintf(fh,"%10s %10li %6li %6li\n","site",
				countAttAtt('L',ATT_TYPE,'T',ATT_SUBTYPE),
				totalAtt('L','T',ATT_IFORCE),
				totalAtt('L','T',ATT_RFORCE));
			fprintf(fh,"%10s %10li %6li %6li\n","sector",
				countAttAtt('L',ATT_TYPE,'S',ATT_SUBTYPE),
				totalAtt('L','S',ATT_IFORCE),
				totalAtt('L','S',ATT_RFORCE));
			spreadAtt('L','P',ATT_PARSEC,szBuf);
			fprintf(fh,"\nparsec %s\n",szBuf);
			fprintf(fh,"interior %li   exterior %li\n",
				countAttAtt('L',ATT_TYPE,'N',ATT_ICON),
				countAttAtt('L',ATT_TYPE,'X',ATT_ICON));
			fprintf(fh,"dark force generation  %li\n",totalAtt('L','*',ATT_IFORCE));
			fprintf(fh,"light force generation %li\n",totalAtt('L','*',ATT_RFORCE));
			break;

		case ID_STATS_INTERRUPTS:
			fprintf(fh,"%15s %10s %10s\n","interrupt","number","destiny");
			spreadAtt('I','U',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","used",countAttAtt('I',ATT_TYPE,'U',ATT_SUBTYPE),szBuf);
			spreadAtt('I','L',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","lost",countAttAtt('I',ATT_TYPE,'L',ATT_SUBTYPE),szBuf);
			spreadAtt('I','B',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","used/lost",countAttAtt('I',ATT_TYPE,'B',ATT_SUBTYPE),szBuf);
			spreadAtt('I','S',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","starting",countAttAtt('I',ATT_TYPE,'S',ATT_SUBTYPE),szBuf);
			fprintf(fh,"\n%15s %10s %10s\n","effect","number","destiny");
			spreadAtt('E','0',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","normal",countAttAtt('E',ATT_TYPE,'0',ATT_SUBTYPE),szBuf);
			spreadAtt('E','U',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","utinni",countAttAtt('E',ATT_TYPE,'U',ATT_SUBTYPE),szBuf);
			spreadAtt('E','I',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","immediate",countAttAtt('E',ATT_TYPE,'I',ATT_SUBTYPE),szBuf);
			spreadAtt('E','M',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","mobile",countAttAtt('E',ATT_TYPE,'M',ATT_SUBTYPE),szBuf);
			break;

		case ID_STATS_WEAPONS:
			fprintf(fh,"%15s %10s %10s\n","weapon","number","destiny");
			spreadAtt('W','C',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","character",countAttAtt('W',ATT_TYPE,'C',ATT_SUBTYPE),szBuf);
			spreadAtt('W','S',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","starship",countAttAtt('W',ATT_TYPE,'S',ATT_SUBTYPE),szBuf);
			spreadAtt('W','A',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","automated",countAttAtt('W',ATT_TYPE,'A',ATT_SUBTYPE),szBuf);
			spreadAtt('W','R',ATT_DESTINY,szBuf);
			fprintf(fh,"%15s %10li %10s\n","artillery",countAttAtt('W',ATT_TYPE,'R',ATT_SUBTYPE),szBuf);
			spreadAtt('D','*',ATT_DESTINY,szBuf);
			fprintf(fh,"\n%15s %10li %10s\n","device",countAttAtt('D',ATT_TYPE,'*',ATT_SUBTYPE),szBuf);
			break;
	}
	fclose(fh);
	} // from if "find results" else

	// now go through and take out any · or × from the result
	fh = fopen("stats.tmp","rb");
	fo = fopen("print.out","wb");
	while (!feof(fh))
	{
		i = getc(fh);
		if ((i==183)||(i==215)) i=' ';
		if (!feof(fh)) putc(i,fo);

	}
	fclose(fo);
	fclose(fh);
	DeleteFile("stats.tmp"); // delete temporary file

	// now the file "print.out" can be sent to its destination
	switch (uDest)
	{
		case ID_STATS_SEND_CLIPBOARD:
			sendtoclipboard("print.out");
			break;
		case ID_STATS_SEND_FILE:
			sendtofile(GetParent(hDlg),"print.out");
			break;
		case ID_STATS_SEND_PRINTER:
			sendtoprinter(GetParent(hDlg),"print.out");
			break;
	}

	return TRUE;
}

void stripcr(char *s)
{
	char szTmp[4096];
	unsigned int i,j;

	for (i=0,j=0;i<strlen(s);i++)
	{
		if (s[i]!='\r') szTmp[j++]=s[i];
	}
	szTmp[j]='\0';
	sprintf(s,"%s",szTmp);
}

BOOL printdeck(HWND hwnd)
{
	int cSide,cSet,cRarity,cType;
	int i,j;
	char szBuf[4096];
	FILE *fh;
	time_t ltime;

	// like the stats printing, everything is saved to a file
	// first, then sent to its destination when done
	if ((fh=fopen("print.out","w"))==NULL)
		return FALSE;

	// print header if picked
	if (printopt.bDate)
	{
		GetWindowText(hwnd,szBuf,999);
		if (strstr(szBuf,UNTITLED)==NULL) fprintf(fh,"%s",strchr(szBuf,'-')+2);
		else fprintf(fh,"(untitled)");
		time( &ltime );
	    fprintf(fh," - %s\n", ctime( &ltime ) );
	}

	cSide='\0'; cSet='\0';		// for breaks
	cType='\0'; cRarity='\0';
	for (i=0;i<iCListSize;i++)
	{
		if (cardlist[i].deck)
		{
			// first check if this card is the start of a BREAK group
			// (definable in the Separate by checkboxes)
			if (printopt.bSSide&&(masterlist[cardlist[i].idx].attr[ATT_SIDE]!=cSide))
			{
				cSide=masterlist[cardlist[i].idx].attr[ATT_SIDE];
				if (cSide=='l') fprintf(fh,"\nLight Side ");
				else fprintf(fh,"\nDark Side ");
				fprintf(fh,"(%li)\n",countAtt(cSide,ATT_SIDE));
			}
			if (printopt.bSSet&&(masterlist[cardlist[i].idx].attr[ATT_SET]!=cSet))
			{
				cSet=masterlist[cardlist[i].idx].attr[ATT_SET];
				for (j=0;j<MAXSETS;j++)
				{
					if (sets[j][0]==cSet)
					{
						fprintf(fh,"\n%s (%li)\n",sets[j]+1,countAtt(cSet,ATT_SET));
					}
				}
				//if (j==MAXSETS) fprintf(fh,"\nUnknown Set\n\n");
			}
			if (printopt.bSRarity&&(masterlist[cardlist[i].idx].attr[ATT_RARITY]!=cRarity))
			{
				cRarity=masterlist[cardlist[i].idx].attr[ATT_RARITY];
				switch (cRarity)
				{
					case 'R': fprintf(fh,"\nRare (%li)\n",countAtt('R',ATT_RARITY)); break;
					case 'U': fprintf(fh,"\nUncommon (%li)\n",countAtt('U',ATT_RARITY)); break;
					case 'C': fprintf(fh,"\nCommon (%li)\n",countAtt('C',ATT_RARITY)); break;
					default:  fprintf(fh,"\nPromotional (%li)\n",countAtt('N',ATT_RARITY)); break;
				}
			}
			if (printopt.bSType&&(masterlist[cardlist[i].idx].attr[ATT_TYPE]!=cType))
			{
				cType=masterlist[cardlist[i].idx].attr[ATT_TYPE];
				switch (cType)
				{
					case 'l': fprintf(fh,"\nLocations (%li)\n",countAtt('l',ATT_TYPE)); break;
					case 'C': fprintf(fh,"\nCharacters (%li)\n",countAtt('C',ATT_TYPE)); break;
					case 'S': fprintf(fh,"\nStarships (%li)\n",countAtt('S',ATT_TYPE)); break;
					case 'V': fprintf(fh,"\nVehicles (%li)\n",countAtt('V',ATT_TYPE)); break;
					case 'R': fprintf(fh,"\nCreatures (%li)\n",countAtt('R',ATT_TYPE)); break;
					case 'E': fprintf(fh,"\nEffects (%li)\n",countAtt('E',ATT_TYPE)); break;
					case 'I': fprintf(fh,"\nInterrupts (%li)\n",countAtt('I',ATT_TYPE)); break;
					case 'D': fprintf(fh,"\nDevices (%li)\n",countAtt('D',ATT_TYPE)); break;
					case 'W': fprintf(fh,"\nWeapons (%li)\n",countAtt('W',ATT_TYPE)); break;
					case 'N': fprintf(fh,"\nEpic Events (%li)\n",countAtt('N',ATT_TYPE)); break;
					case 'J': fprintf(fh,"\nJedi Tests (%li)\n",countAtt('J',ATT_TYPE)); break;
					default:  fprintf(fh,"\nUnknown Type:\n"); break;
				}
			}

			if (printopt.bFSide) fprintf(fh,"%cS ",masterlist[cardlist[i].idx].attr[ATT_SIDE]);
			if (printopt.bFSet) fprintf(fh,"%c ",masterlist[cardlist[i].idx].attr[ATT_SET]);
			if (printopt.bFRarity) fprintf(fh,"%c%c ",masterlist[cardlist[i].idx].attr[ATT_RARITY],
				(masterlist[cardlist[i].idx].attr[ATT_SUBRARITY]=='0')?' ':masterlist[cardlist[i].idx].attr[ATT_SUBRARITY]);
			if (printopt.bFType) fprintf(fh,"%c ",masterlist[cardlist[i].idx].attr[ATT_TYPE]);
			if (!printopt.bCardQty)
			{
				if (cardlist[i].deck>1)	fprintf(fh,"%6i ",cardlist[i].deck);
				else fprintf(fh,"       ");
			}
			sprintf(szBuf,"%s",masterlist[cardlist[i].idx].name);
			for (j=0;j<(signed)strlen(szBuf);j++)
			{
				if (((szBuf[j]=='·')||(szBuf[j]=='×'))&&j)
				{
					szBuf[j-1]=' ';
					szBuf[j]='\0';
				}
			}
			fprintf(fh,"%s",szBuf);
			if (printopt.bCardQty)
			{
				if (cardlist[i].deck>1) fprintf(fh," x%i",cardlist[i].deck);
			}
			fprintf(fh,"\n");

		}
	}

	if (printopt.bNotes)
	{
		GetWindowText(GetDlgItem(hDlgStats,IDC_STATSNOTES),szBuf,4095);
		stripcr(szBuf);
		fprintf(fh,"\n%s\n",szBuf);
	}

	if (printopt.bSummary)
	{
		fprintf(fh,"\nType codes:\n");
		fprintf(fh,"L %-15s C %-15s S %-15s\n","Location","Character","Starship");
		fprintf(fh,"I %-15s E %-15s V %-15s\n","Interrupt","Effect","Vehicle");
		fprintf(fh,"N %-15s D %-15s W %-15s\n","Epic Event","Device","Weapon");
		fprintf(fh,"R %-15s J %-15s\n","Creature","Jedi Test");
		fprintf(fh,"Set codes:\n");
		for (i=0,j=0;i<MAXSETS;i++)
		{
			if (sets[i][0]!='\0')
			{
				fprintf(fh,"%c %-22s ",sets[i][0],sets[i]+1);
				j++;
				if (!(j%3)) fprintf(fh,"\n");
			}
		}
		fprintf(fh,"\n");
	}

	fclose(fh);

	switch (printopt.uDestination)
	{
		case IDC_RADIO_DESTCLIP: sendtoclipboard("print.out"); break;
		case IDC_RADIO_DESTFILE: sendtofile(hwnd,"print.out"); break;
		case IDC_RADIO_DESTPRINT: sendtoprinter(hwnd,"print.out"); break;
	}

	return TRUE;
}
