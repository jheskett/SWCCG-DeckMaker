// SWFile.h
#include <windows.h>
#include <stdio.h>

static char			szFileName[_MAX_PATH];
static char			szTitleName[_MAX_FNAME+8];


#define UNTITLED "(untitled)"

int FileLength(FILE *fh);
void stread(FILE *fh,char *sz,int max);
void stwrite(FILE *fh,char *sz);
void DoCaption(HWND hwnd,char *p);
int AskAboutSave(HWND,char *p);

void SWFileInitialize(HWND);
BOOL SWFileOpenDlg(HWND,PSTR,PSTR);
BOOL SWFileSaveDlg(HWND,PSTR,PSTR);
BOOL SWFileRead(HWND,PSTR);
BOOL SWFileWrite(HWND,PSTR);

void SWPrintFileInitialize(HWND);
BOOL SWPrintFileSaveDlg(HWND,PSTR,PSTR);
BOOL SWPrintFileWrite(HWND,PSTR,PSTR);

BOOL SWBinderRead(HWND,PSTR);
int matchcard(char *szName,int cSide,int cSet);
