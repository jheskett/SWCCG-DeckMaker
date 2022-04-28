# Microsoft Developer Studio Project File - Name="SWCCG DeckMaker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SWCCG DeckMaker - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SWCCG DeckMaker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SWCCG DeckMaker.mak" CFG="SWCCG DeckMaker - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SWCCG DeckMaker - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SWCCG DeckMaker - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SWCCG DeckMaker - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\SWCCG_De"
# PROP BASE Intermediate_Dir ".\SWCCG_De"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "SWCCG DeckMaker - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\SWCCG_D0"
# PROP BASE Intermediate_Dir ".\SWCCG_D0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /O1 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "SWCCG DeckMaker - Win32 Release"
# Name "SWCCG DeckMaker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Ezfont.c
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=".\SWCCG DeckMaker.c"
# End Source File
# Begin Source File

SOURCE=.\SWDeck.c
# End Source File
# Begin Source File

SOURCE=.\SWFile.c
# End Source File
# Begin Source File

SOURCE=.\SWPrint.C
# End Source File
# Begin Source File

SOURCE=.\SWSpoilers.c
# End Source File
# Begin Source File

SOURCE=.\SWStats.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\attributes.h
# End Source File
# Begin Source File

SOURCE=.\Ezfont.h
# End Source File
# Begin Source File

SOURCE=.\help.h
# End Source File
# Begin Source File

SOURCE=.\SWDeck.h
# End Source File
# Begin Source File

SOURCE=.\SWFile.h
# End Source File
# Begin Source File

SOURCE=.\SWPrint.h
# End Source File
# Begin Source File

SOURCE=.\SWSpoilers.h
# End Source File
# Begin Source File

SOURCE=.\SWStats.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\pics\alien.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\anh.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\anhul.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\anthology.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bespin.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\blank.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00008.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00009.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00010.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00011.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00012.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00013.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00014.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00015.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00016.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00017.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bmp00018.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\bounty.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\cardback.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\creature.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\dagobah.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\deathstar.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\dejarik.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\deploy0.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\deploy1.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\deploy3.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\dest0.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\device.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\droid.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\ds_back.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\effect.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\enclosed.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\event.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\exterior.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\fixed.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\forfeit1.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\hoth.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\hothul.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\hyperdrive.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\immediate.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\imperial.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\independent.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\interior.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\interrupt.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\jabba.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\jedimaster.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\jeditest.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\leader.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\limited.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\lost.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\mobile.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\objective.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\parker.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\permweapon.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\pi.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\pilot.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\rarena.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\rebel.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\rl.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\scomp.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\sector.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\ship.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\spy.bmp
# End Source File
# Begin Source File

SOURCE=.\Swccgdm.ico
# End Source File
# Begin Source File

SOURCE=.\pics\swdm256.bmp
# End Source File
# Begin Source File

SOURCE=.\swdm256.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\system.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\unlimited.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\usedorlost.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\utinni.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\vehicle.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\warrior.bmp
# End Source File
# Begin Source File

SOURCE=.\pics\weapon.bmp
# End Source File
# End Group
# End Target
# End Project
