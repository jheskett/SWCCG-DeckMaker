/*void newsort(int sortby)
{
	LPSTR	lpBuffer;
	int		i,k,ch,ix,iMasterlistSize;
	char	szBuf1[MAXNAME],szBuf2[MAXNAME];
	int		tmpRank[MAXLIST];
	int		chSet1,chSet2;
	//FILE *fh;
	HCURSOR hCursor;
	
	hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));	// this could take a couple seconds
	ShowCursor(TRUE);								// make the pointer the "hourglass"

	for (i=0;i<MAXLIST;i++) tmpRank[i]=i; // start with a unique sequence
	
	lpBuffer=GlobalLock(hCardData);

	// The masterlist has no missing entries between the first and last one,
	// so it'll save a lot of time to ignore everything after it.  Find the
	// last entry.
	for (i=0,iMasterlistSize=0;i<MAXLIST;i++)
		if (masterlist[i].name!=NULL) iMasterlistSize=i;
		
	do
	{
		for (i=0,k=0;i<iMasterlistSize;i++)
		{
			// if bSortSets==TRUE, then sort the sets apart by making the
			// first character of each sort string the set code.  If ==FALSE,
			// make them all the same set sort with code 'z'
			if (bSortSets==FALSE)
			{
				chSet1 = 'z';
				chSet2 = 'z';
			}
			else
			{
				if (masterlist[tmpRank[i]].name!=NULL)
					chSet1 = lpBuffer[masterlist[tmpRank[i]].attributes+ATT_SET];
				else chSet1 = 'z';
				if (masterlist[tmpRank[i+1]].name!=NULL)
					chSet2 = lpBuffer[masterlist[tmpRank[i+1]].attributes+ATT_SET];
				else chSet2 = 'z';
			}

			if ((masterlist[tmpRank[i]].name!=NULL)&&(masterlist[tmpRank[i+1]].name!=NULL))
			{
				// If both tmpRank's are valid, do a check for swap
				switch (sortby)		// sort method determines what would swap
				{
					case BYNAME:
						if (masterlist[tmpRank[i]].name!=NULL) wsprintf(szBuf1,"%c%s",chSet1,lpBuffer+masterlist[tmpRank[i]].name);
						else wsprintf(szBuf1,"zzz");	// empty list entries are sorted out
						if (masterlist[tmpRank[i+1]].name!=NULL) wsprintf(szBuf2,"%c%s",chSet2,lpBuffer+masterlist[tmpRank[i+1]].name);
						else wsprintf(szBuf2,"zzz");
						// lstrcmpi is a case-INSENSITIVE string compare (windows.h)
						if (lstrcmpi(szBuf1,szBuf2)>0) k=intswap(&tmpRank[i],&tmpRank[i+1]);
						break;

					case BYTYPE:
						ch = typeweight(lpBuffer[masterlist[tmpRank[i]].attributes+ATT_TYPE]);
						wsprintf(szBuf1,"%c%c%s",chSet1,ch,lpBuffer+masterlist[tmpRank[i]].name);
						ch = typeweight(lpBuffer[masterlist[tmpRank[i+1]].attributes+ATT_TYPE]);
						wsprintf(szBuf2,"%c%c%s",chSet2,ch,lpBuffer+masterlist[tmpRank[i+1]].name);
						if (lstrcmpi(szBuf1,szBuf2)>0) k=intswap(&tmpRank[i],&tmpRank[i+1]);
						break;

					case BYDEST:
						wsprintf(szBuf1,"%c%c%s",chSet1,lpBuffer[masterlist[tmpRank[i]].attributes+ATT_DESTINY],
											lpBuffer+masterlist[tmpRank[i]].name);
						wsprintf(szBuf2,"%c%c%s",chSet2,lpBuffer[masterlist[tmpRank[i+1]].attributes+ATT_DESTINY],
											lpBuffer+masterlist[tmpRank[i+1]].name);
						if (lstrcmpi(szBuf1,szBuf2)>0) k=intswap(&tmpRank[i],&tmpRank[i+1]);
						break;
					
					case BYRARE:
						wsprintf(szBuf1,"%c%c%s",chSet1,rareweight(lpBuffer[masterlist[tmpRank[i]].attributes+ATT_RARITY],
										lpBuffer[masterlist[tmpRank[i]].attributes+ATT_SUBRARITY],
										lpBuffer[masterlist[tmpRank[i]].attributes+ATT_SET]),
										lpBuffer+masterlist[tmpRank[i]].name);
						wsprintf(szBuf2,"%c%c%s",chSet2,rareweight(lpBuffer[masterlist[tmpRank[i+1]].attributes+ATT_RARITY],
										lpBuffer[masterlist[tmpRank[i+1]].attributes+ATT_SUBRARITY],
										lpBuffer[masterlist[tmpRank[i+1]].attributes+ATT_SET]),
										lpBuffer+masterlist[tmpRank[i+1]].name);
						if (lstrcmpi(szBuf1,szBuf2)>0) k=intswap(&tmpRank[i],&tmpRank[i+1]);
						break;

					case BYDEPL:
					case BYPOWR:
						if (sortby==BYDEPL) ix=ATT_DEPLOY; else ix=ATT_POWER;
						ch = typeweight(lpBuffer[masterlist[tmpRank[i]].attributes+ATT_TYPE]);
						if ((ch=='A')&&(ix==ATT_POWER)&&(iDeckSide==DARKSIDE)) ix++;
						wsprintf(szBuf1,"%c%c%c%s",chSet1,ch,lpBuffer[masterlist[tmpRank[i]].attributes+ix],
											lpBuffer+masterlist[tmpRank[i]].name);
						ch = typeweight(lpBuffer[masterlist[tmpRank[i+1]].attributes+ATT_TYPE]);
						wsprintf(szBuf2,"%c%c%c%s",chSet2,ch,lpBuffer[masterlist[tmpRank[i+1]].attributes+ix],
											lpBuffer+masterlist[tmpRank[i+1]].name);
						if (lstrcmpi(szBuf1,szBuf2)>0) k=intswap(&tmpRank[i],&tmpRank[i+1]);
						break;
				}
			}

		} // end for
	} while (k);

	GlobalUnlock(hCardData); // done with card data
	
	// Here, tmpRank[] contains the new sequence.
	
	// Now go through and renumber the masterlist[]
	for (i=0;i<MAXLIST;i++) masterlist[tmpRank[i]].rank=i;
	
	iSort = sortby; // made it this far? Keep this sort method

	ShowCursor(FALSE); 	// restore cursor to original shape
	SetCursor(hCursor);
	
}
/*