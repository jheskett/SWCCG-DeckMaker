# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=SWCCG DeckMaker - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to SWCCG DeckMaker - Win32\
 Debug.
!ENDIF 

!IF "$(CFG)" != "SWCCG DeckMaker - Win32 Release" && "$(CFG)" !=\
 "SWCCG DeckMaker - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SWCCG DeckMaker.mak" CFG="SWCCG DeckMaker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SWCCG DeckMaker - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SWCCG DeckMaker - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "SWCCG DeckMaker - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SWCCG DeckMaker - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SWCCG_De"
# PROP BASE Intermediate_Dir "SWCCG_De"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SWCCG_De"
# PROP Intermediate_Dir "SWCCG_De"
# PROP Target_Dir ""
OUTDIR=.\SWCCG_De
INTDIR=.\SWCCG_De

ALL : "$(OUTDIR)\SWCCG DeckMaker.exe"

CLEAN : 
	-@erase ".\SWCCG_De\SWCCG DeckMaker.exe"
	-@erase ".\SWCCG_De\SWDeck.obj"
	-@erase ".\SWCCG_De\SWFile.obj"
	-@erase ".\SWCCG_De\Ezfont.obj"
	-@erase ".\SWCCG_De\SWCCG DeckMaker.obj"
	-@erase ".\SWCCG_De\resource.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/SWCCG DeckMaker.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\SWCCG_De/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SWCCG DeckMaker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/SWCCG DeckMaker.pdb" /machine:I386\
 /out:"$(OUTDIR)/SWCCG DeckMaker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/SWDeck.obj" \
	"$(INTDIR)/SWFile.obj" \
	"$(INTDIR)/Ezfont.obj" \
	"$(INTDIR)/SWCCG DeckMaker.obj" \
	"$(INTDIR)/resource.res"

"$(OUTDIR)\SWCCG DeckMaker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWCCG DeckMaker - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SWCCG_D0"
# PROP BASE Intermediate_Dir "SWCCG_D0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SWCCG_D0"
# PROP Intermediate_Dir "SWCCG_D0"
# PROP Target_Dir ""
OUTDIR=.\SWCCG_D0
INTDIR=.\SWCCG_D0

ALL : "$(OUTDIR)\SWCCG DeckMaker.exe"

CLEAN : 
	-@erase ".\SWCCG_D0\vc40.pdb"
	-@erase ".\SWCCG_D0\vc40.idb"
	-@erase ".\SWCCG_D0\SWCCG DeckMaker.exe"
	-@erase ".\SWCCG_D0\SWDeck.obj"
	-@erase ".\SWCCG_D0\SWCCG DeckMaker.obj"
	-@erase ".\SWCCG_D0\SWFile.obj"
	-@erase ".\SWCCG_D0\Ezfont.obj"
	-@erase ".\SWCCG_D0\resource.res"
	-@erase ".\SWCCG_D0\SWCCG DeckMaker.ilk"
	-@erase ".\SWCCG_D0\SWCCG DeckMaker.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/SWCCG DeckMaker.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\SWCCG_D0/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/resource.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SWCCG DeckMaker.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/SWCCG DeckMaker.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/SWCCG DeckMaker.exe" 
LINK32_OBJS= \
	"$(INTDIR)/SWDeck.obj" \
	"$(INTDIR)/SWCCG DeckMaker.obj" \
	"$(INTDIR)/SWFile.obj" \
	"$(INTDIR)/Ezfont.obj" \
	"$(INTDIR)/resource.res"

"$(OUTDIR)\SWCCG DeckMaker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "SWCCG DeckMaker - Win32 Release"
# Name "SWCCG DeckMaker - Win32 Debug"

!IF  "$(CFG)" == "SWCCG DeckMaker - Win32 Release"

!ELSEIF  "$(CFG)" == "SWCCG DeckMaker - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=".\SWCCG DeckMaker.c"

!IF  "$(CFG)" == "SWCCG DeckMaker - Win32 Release"

DEP_CPP_SWCCG=\
	".\SWDeck.h"\
	".\attributes.h"\
	".\help.h"\
	".\Ezfont.h"\
	

"$(INTDIR)\SWCCG DeckMaker.obj" : $(SOURCE) $(DEP_CPP_SWCCG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SWCCG DeckMaker - Win32 Debug"

DEP_CPP_SWCCG=\
	".\SWDeck.h"\
	".\attributes.h"\
	".\help.h"\
	".\Ezfont.h"\
	
NODEP_CPP_SWCCG=\
	".\..\..\..\\updateinfo(hDlgInfo,cardlist[iSel].idx);"\
	

"$(INTDIR)\SWCCG DeckMaker.obj" : $(SOURCE) $(DEP_CPP_SWCCG) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.rc
DEP_RSC_RESOU=\
	".\Swccgdm.ico"\
	".\pics\weapon.bmp"\
	".\pics\creature.bmp"\
	".\pics\dejarik.bmp"\
	".\pics\dest0.bmp"\
	".\pics\dest1.bmp"\
	".\pics\dest2.bmp"\
	".\pics\dest3.bmp"\
	".\pics\dest4.bmp"\
	".\pics\dest5.bmp"\
	".\pics\dest6.bmp"\
	".\pics\device.bmp"\
	".\pics\droid.bmp"\
	".\pics\event.bmp"\
	".\pics\exterior.bmp"\
	".\pics\force0.bmp"\
	".\pics\hyperdrive.bmp"\
	".\pics\iforce1.bmp"\
	".\pics\iforce2.bmp"\
	".\pics\iforce3.bmp"\
	".\pics\immediate.bmp"\
	".\pics\imperial.bmp"\
	".\pics\interior.bmp"\
	".\pics\interrupt.bmp"\
	".\pics\limited.bmp"\
	".\pics\lost.bmp"\
	".\pics\mobile.bmp"\
	".\pics\parker.bmp"\
	".\pics\pi.bmp"\
	".\pics\pilot.bmp"\
	".\pics\rarec1.bmp"\
	".\pics\rarec2.bmp"\
	".\pics\rarec3.bmp"\
	".\pics\rarena.bmp"\
	".\pics\rarer1.bmp"\
	".\pics\rarer2.bmp"\
	".\pics\rareu1.bmp"\
	".\pics\rareu2.bmp"\
	".\pics\rebel.bmp"\
	".\pics\rforce1.bmp"\
	".\pics\rforce2.bmp"\
	".\pics\rforce3.bmp"\
	".\pics\rl.bmp"\
	".\pics\scomp.bmp"\
	".\pics\ship.bmp"\
	".\pics\system.bmp"\
	".\pics\unlimited.bmp"\
	".\pics\usedorlost.bmp"\
	".\pics\utinni.bmp"\
	".\pics\vehicle.bmp"\
	".\pics\warrior.bmp"\
	".\pics\blank.bmp"\
	".\pics\effect.bmp"\
	".\pics\alien.bmp"\
	".\pics\anh.bmp"\
	".\pics\bitmap1.bmp"\
	".\swdm256.bmp"\
	".\pics\hoth.bmp"\
	".\pics\dest7.bmp"\
	".\pics\deploy1.bmp"\
	".\pics\deploy2.bmp"\
	".\pics\deploy3.bmp"\
	".\pics\deploy4.bmp"\
	".\pics\deploy5.bmp"\
	".\pics\deploy6.bmp"\
	".\pics\deploy7.bmp"\
	".\pics\deploy8.bmp"\
	".\pics\deploy9.bmp"\
	".\pics\deployx.bmp"\
	".\pics\deploy0.bmp"\
	".\pics\destx.bmp"\
	".\pics\forfeit0.bmp"\
	".\pics\forfeit1.bmp"\
	".\pics\forfeit2.bmp"\
	".\pics\forfeit3.bmp"\
	".\pics\forfeit4.bmp"\
	".\pics\forfeit5.bmp"\
	".\pics\forfeit6.bmp"\
	".\pics\forfeit7.bmp"\
	".\pics\forfeit8.bmp"\
	".\pics\forfeit9.bmp"\
	".\pics\forfeitx.bmp"\
	

"$(INTDIR)\resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SWDeck.c
DEP_CPP_SWDEC=\
	".\SWDeck.h"\
	".\SWFile.h"\
	".\attributes.h"\
	".\help.h"\
	

"$(INTDIR)\SWDeck.obj" : $(SOURCE) $(DEP_CPP_SWDEC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SWFile.c

"$(INTDIR)\SWFile.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ezfont.c
DEP_CPP_EZFON=\
	".\Ezfont.h"\
	

"$(INTDIR)\Ezfont.obj" : $(SOURCE) $(DEP_CPP_EZFON) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
