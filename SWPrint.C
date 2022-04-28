/*----------------------------------------------
   POPPRNT.C -- Popup Editor Printing Functions
  ----------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <stdio.h>

BOOL bUserAbort ;
HWND hDlgPrint ;

// returns the number of lines in the file <sz>
int file_lines(char *sz)
{
	int  i;
	char szBuf[4096];
	FILE *fh;

	fh = fopen(sz,"r");
	for (i=0;!feof(fh);i++) fgets(szBuf,4095,fh);
	fclose(fh);
	i=max(0,i-1);

	return i;
}

BOOL PopPrntPrintFile (HWND hwnd, char *filen)
     {
     static DOCINFO  di = { sizeof (DOCINFO), "", NULL } ;
     static PRINTDLG pd ;
     BOOL            bSuccess ;
     char			 pstrBuffer[4096] ;
	 FILE			 *fh;
     int             yChar, iCharsPerLine, iLinesPerPage, iTotalLines,
                     iTotalPages, iPage, iLine, iLineNum ;
     TEXTMETRIC      tm ;
     WORD            iColCopy, iNoiColCopy ;
	 unsigned int	 i;

     pd.lStructSize         = sizeof (PRINTDLG) ;
     pd.hwndOwner           = hwnd ;
     pd.hDevMode            = NULL ;
     pd.hDevNames           = NULL ;
     pd.hDC                 = NULL ;
     pd.Flags               = PD_ALLPAGES | PD_COLLATE | PD_RETURNDC ;
     pd.nFromPage           = 0 ;
     pd.nToPage             = 0 ;
     pd.nMinPage            = 0 ;
     pd.nMaxPage            = 0 ;
     pd.nCopies             = 1 ;
     pd.hInstance           = NULL ;
     pd.lCustData           = 0L ;
     pd.lpfnPrintHook       = NULL ;
     pd.lpfnSetupHook       = NULL ;
     pd.lpPrintTemplateName = NULL ;
     pd.lpSetupTemplateName = NULL ;
     pd.hPrintTemplate      = NULL ;
     pd.hSetupTemplate      = NULL ;

     if (!PrintDlg (&pd))
          return TRUE ;

	 iTotalLines = (short) file_lines(filen);

     if (iTotalLines == 0)
          return TRUE ;

	 if ((fh=fopen(filen,"r"))==NULL)
		 return TRUE;

     GetTextMetrics (pd.hDC, &tm) ;
     yChar = tm.tmHeight + tm.tmExternalLeading ;

     iCharsPerLine = GetDeviceCaps (pd.hDC, HORZRES) / tm.tmAveCharWidth ;
     iLinesPerPage = GetDeviceCaps (pd.hDC, VERTRES) / yChar ;
     iTotalPages   = (iTotalLines + iLinesPerPage - 1) / iLinesPerPage ;

     //pstrBuffer = (LPCTSTR) HeapAlloc (GetProcessHeap (), 
	//	                               HEAP_NO_SERIALIZE, iCharsPerLine + 1) ;

     bSuccess   = TRUE ;
     bUserAbort = FALSE ;

     if (StartDoc (pd.hDC, &di) > 0)
          {
          for (iColCopy = 0 ;
               iColCopy < ((WORD) pd.Flags & PD_COLLATE ? pd.nCopies : 1) ;
               iColCopy++)
               {
               for (iPage = 0 ; iPage < iTotalPages ; iPage++)
                    {
                    for (iNoiColCopy = 0 ;
                         iNoiColCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies) ;
                         iNoiColCopy++)
                         {

                         if (StartPage (pd.hDC) < 0)
                              {
                              bSuccess = FALSE ;
                              break ;
                              }

                         for (iLine = 0 ; iLine < iLinesPerPage ; iLine++)
                              {
                              iLineNum = iLinesPerPage * iPage + iLine ;

                              if (iLineNum > iTotalLines)
                                   break ;

//                              *(int *) pstrBuffer = iCharsPerLine ;

							  fgets(pstrBuffer,4095,fh);
							  if (!feof(fh))
							  {
								for (i=0;i<strlen(pstrBuffer);i++)
								{
									if (pstrBuffer[i]=='\n') pstrBuffer[i]='\0';
									if ((pstrBuffer[i]<' ')||(pstrBuffer[i]>'z'))
										pstrBuffer[i]=' ';
								}
								TextOut (pd.hDC, 0, yChar * iLine, pstrBuffer, (int)strlen(pstrBuffer));
							  }
                              //TextOut (pd.hDC, 0, yChar * iLine, pstrBuffer,
                              //     (int) SendMessage (hwndEdit, EM_GETLINE,
                              //     (WPARAM) iLineNum, (LPARAM) pstrBuffer)) ;
                              }

                         if (EndPage (pd.hDC) < 0)
                              {
                              bSuccess = FALSE ;
                              break ;
                              }

                         if (bUserAbort)
                              break ;
                         }

                    if (!bSuccess || bUserAbort)
                         break ;
                    }

               if (!bSuccess || bUserAbort)
                    break ;
               }
          }
     else
          bSuccess = FALSE ;

     if (bSuccess)
          EndDoc (pd.hDC) ;

	 fclose(fh);

     //HeapFree (GetProcessHeap (), 0, (LPVOID) pstrBuffer) ;
     DeleteDC (pd.hDC) ;

     return bSuccess && !bUserAbort ;
     }
