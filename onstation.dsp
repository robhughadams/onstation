# Microsoft Developer Studio Project File - Name="onstation" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=onstation - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "onstation.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "onstation.mak" CFG="onstation - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "onstation - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "onstation - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "onstation - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "onstation - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "onstation - Win32 Release"
# Name "onstation - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutBoxSummary.cpp
# End Source File
# Begin Source File

SOURCE=.\aboutcopyrightpage.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutCreditsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutWeb.cpp
# End Source File
# Begin Source File

SOURCE=.\AgeColorHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\agecolorpage.cpp
# End Source File
# Begin Source File

SOURCE=.\BlunderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\bookmark.cpp
# End Source File
# Begin Source File

SOURCE=.\BookmarkDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\centerpage.cpp
# End Source File
# Begin Source File

SOURCE=.\CenterStationNameDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\clipboard.CPP
# End Source File
# Begin Source File

SOURCE=.\ColorSchemeComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\colortool.CPP
# End Source File
# Begin Source File

SOURCE=.\colorwidget.CPP
# End Source File
# Begin Source File

SOURCE=.\comment.cpp
# End Source File
# Begin Source File

SOURCE=.\compasswidget.CPP
# End Source File
# Begin Source File

SOURCE=.\DebugStationLinksDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DepthColorHelperDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\depthcolorpage.cpp
# End Source File
# Begin Source File

SOURCE=.\DepthLevelDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DistanceBetweenDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\editablegrid.cpp
# End Source File
# Begin Source File

SOURCE=.\editfixedpointdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\filedump.cpp
# End Source File
# Begin Source File

SOURCE=.\filehelper.cpp
# End Source File
# Begin Source File

SOURCE=.\fixedpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Folder.cpp
# End Source File
# Begin Source File

SOURCE=.\folderinfodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\gworld.cpp
# End Source File
# Begin Source File

SOURCE=.\gworldopengl.cpp
# End Source File
# Begin Source File

SOURCE=.\Hitlist.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoByViewDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\jpclose.cpp
# End Source File
# Begin Source File

SOURCE=.\junctionbox.cpp
# End Source File
# Begin Source File

SOURCE=.\legeditorsettingspage.cpp
# End Source File
# Begin Source File

SOURCE=.\legeditsheet.cpp
# End Source File
# Begin Source File

SOURCE=.\leggridpage.cpp
# End Source File
# Begin Source File

SOURCE=.\leginstrumentspage.cpp
# End Source File
# Begin Source File

SOURCE=.\legpeoplepage.cpp
# End Source File
# Begin Source File

SOURCE=.\legquery.cpp
# End Source File
# Begin Source File

SOURCE=.\legsummarypage.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\mathstuff.CPP
# End Source File
# Begin Source File

SOURCE=.\namelist.cpp
# End Source File
# Begin Source File

SOURCE=.\newfolderdlg.CPP
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\nodepositioninfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Nodetree.cpp
# End Source File
# Begin Source File

SOURCE=.\onstation.cpp
# End Source File
# Begin Source File

SOURCE=.\onstation.rc
# End Source File
# Begin Source File

SOURCE=.\onstationDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\OnStationSurveyTree.cpp
# End Source File
# Begin Source File

SOURCE=.\onstationView.cpp
# End Source File
# Begin Source File

SOURCE=.\OrphanedShotDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OverallDataView.cpp
# End Source File
# Begin Source File

SOURCE=.\PageLayoutPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\PassageOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\pdate32.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\Preferences.cpp
# End Source File
# Begin Source File

SOURCE=.\query.cpp
# End Source File
# Begin Source File

SOURCE=.\realfolder.cpp
# End Source File
# Begin Source File

SOURCE=.\Ruler.cpp
# End Source File
# Begin Source File

SOURCE=.\saveformattedtext.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchCommentsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\searchresultdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetExactViewSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\surfacedata.cpp
# End Source File
# Begin Source File

SOURCE=.\surfaceeditpage.cpp
# End Source File
# Begin Source File

SOURCE=.\surfacepropertysheet.cpp
# End Source File
# Begin Source File

SOURCE=.\surfacesizepage.cpp
# End Source File
# Begin Source File

SOURCE=.\surveyleg.cpp
# End Source File
# Begin Source File

SOURCE=.\SurveySearchDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\surveyshot.CPP
# End Source File
# Begin Source File

SOURCE=.\TipDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleBoxSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\Treehelper.cpp
# End Source File
# Begin Source File

SOURCE=.\viewanglepage.CPP
# End Source File
# Begin Source File

SOURCE=.\ViewScalePage.cpp
# End Source File
# Begin Source File

SOURCE=.\Viewtab.cpp
# End Source File
# Begin Source File

SOURCE=.\WaitForClosureDialog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BlunderDialog.h
# End Source File
# Begin Source File

SOURCE=.\DebugStationLinksDialog.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\onstation.h
# End Source File
# Begin Source File

SOURCE=.\onstationDoc.h
# End Source File
# Begin Source File

SOURCE=.\onstationView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmappi.bmp
# End Source File
# Begin Source File

SOURCE=.\Bmfolder.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00008.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00008.bmp
# End Source File
# Begin Source File

SOURCE=.\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\Dragdrop.cur
# End Source File
# Begin Source File

SOURCE=.\res\Dragdrop.cur
# End Source File
# Begin Source File

SOURCE=.\res\help.cur
# End Source File
# Begin Source File

SOURCE=.\idr_onst.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_onst.ico
# End Source File
# Begin Source File

SOURCE=.\res\move4way.cur
# End Source File
# Begin Source File

SOURCE=.\res\nodrop.cur
# End Source File
# Begin Source File

SOURCE=.\res\onstation.ico
# End Source File
# Begin Source File

SOURCE=.\res\onstation.rc2
# End Source File
# Begin Source File

SOURCE=.\OnStationDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\onstationDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\sarrows.cur
# End Source File
# Begin Source File

SOURCE=.\res\splith.cur
# End Source File
# Begin Source File

SOURCE=.\res\splitv.cur
# End Source File
# Begin Source File

SOURCE=.\res\Titleima.bmp
# End Source File
# Begin Source File

SOURCE=.\Titleima.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\trck4way.cur
# End Source File
# Begin Source File

SOURCE=.\res\trcknesw.cur
# End Source File
# Begin Source File

SOURCE=.\res\trckns.cur
# End Source File
# Begin Source File

SOURCE=.\res\trcknwse.cur
# End Source File
# Begin Source File

SOURCE=.\res\trckwe.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
