#include "stdafx.h"
#include "onstation.h"
#include "settings.h"
#include "onstationdoc.h"
#include "folder.h"
#include "realfolder.h"
#include "colorwidget.h"
#include "filehelper.h"
#include "nodetree.h"
#include "onstationview.h"
#include "colortool.h"
#include "bookmark.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern COnStationApp theApp;
CSettings * pSettings_G=NULL;

void CAppSettings::WriteToIniFile()
{
//	theApp.WriteProfileInt("Client","normleft",m_wndPlacement.rcNormalPosition.left);
///	theApp.WriteProfileInt("Client","normtop",m_wndPlacement.rcNormalPosition.top);
//	theApp.WriteProfileInt("Client","normright",m_wndPlacement.rcNormalPosition.right);
//	theApp.WriteProfileInt("Client","normbottom",m_wndPlacement.rcNormalPosition.bottom);
//	theApp.WriteProfileInt("Client","flags",m_wndPlacement.flags);
//	theApp.WriteProfileInt("Client","showcmd",m_wndPlacement.showCmd);
//	theApp.WriteProfileInt("Client","minposx",m_wndPlacement.ptMinPosition.x);
//	theApp.WriteProfileInt("Client","minposy",m_wndPlacement.ptMinPosition.y);
//	theApp.WriteProfileInt("Client","maxposx",m_wndPlacement.ptMaxPosition.x);
//	theApp.WriteProfileInt("Client","maxposy",m_wndPlacement.ptMaxPosition.y);

	theApp.WriteProfileString("Font","FontName",m_szFontName);
	theApp.WriteProfileInt("Font","FontSize",m_iFontSize);
	theApp.WriteProfileInt("Font","FontWeight",m_iFontWeight);
	
	theApp.WriteProfileInt("Bars","StatusBars",m_bStatusBar);
	theApp.WriteProfileInt("Bars","Toolbars",m_bToolBar);
	theApp.WriteProfileInt("Bars","Ruler",m_bRuler);
	theApp.WriteProfileInt("Bars","IndexTabs",m_bBottomTabs);
	theApp.WriteProfileInt("Bars","Compass",m_bCompass);

	theApp.WriteProfileInt("Units","Prefered",m_PreferedUnits);
	theApp.WriteProfileInt("Printing","Scale",m_bPrintShowScale);
	theApp.WriteProfileInt("Printing","North",m_bPrintShowNorth);
	theApp.WriteProfileInt("Printing","Overlap",m_iPrintOverlap);
	theApp.WriteProfileInt("Printing","ThickLines",m_bPrintThickLines);
	theApp.WriteProfileInt("Printing","SideScale",m_bShowSideScale);
	theApp.WriteProfileInt("Printing","TitlePage",m_bPrintTitlePage);
	theApp.WriteProfileInt("Printing","PeopleByYear",m_bPrintPeopleByYear);

	pSettings_G->WriteToIniFile();
}

CAppSettings::CAppSettings()
{
}

CAppSettings::~CAppSettings()
{
}

BOOL CAppSettings::IsMetric() const
{
	return (m_PreferedUnits==METERS || m_PreferedUnits==METERSANDKM);
}

void CAppSettings::ReadFromIniFile()
{
//	m_wndPlacement.length=sizeof(WINDOWPLACEMENT);
//	m_wndPlacement.rcNormalPosition.left=theApp.GetProfileInt("Client","normleft",10);
//	m_wndPlacement.rcNormalPosition.top=theApp.GetProfileInt("Client","normtop",10);
//	m_wndPlacement.rcNormalPosition.right=theApp.GetProfileInt("Client","normright",630);
//	m_wndPlacement.rcNormalPosition.bottom=theApp.GetProfileInt("Client","normbottom",450);
//	m_wndPlacement.flags=theApp.GetProfileInt("Client","flags",WPF_RESTORETOMAXIMIZED);
//	m_wndPlacement.showCmd=theApp.GetProfileInt("Client","showcmd",SW_SHOWNORMAL);
//	m_wndPlacement.ptMinPosition.x=theApp.GetProfileInt("Client","minposx",50);
//	m_wndPlacement.ptMinPosition.y=theApp.GetProfileInt("Client","minposy",50);
//	m_wndPlacement.ptMaxPosition.x=theApp.GetProfileInt("Client","maxposx",0);
//	m_wndPlacement.ptMaxPosition.y=theApp.GetProfileInt("Client","maxposy",0);

	m_bStatusBar=theApp.GetProfileInt("Bars","StatusBars",TRUE);
	m_bToolBar=theApp.GetProfileInt("Bars","Toolbars",TRUE);
	m_bRuler=theApp.GetProfileInt("Bars","Ruler",TRUE);
	m_bBottomTabs=theApp.GetProfileInt("Bars","IndexTabs",TRUE);
	m_bCompass=theApp.GetProfileInt("Bars","Compass",TRUE);

	m_PreferedUnits=(PREFEREDUNITS)theApp.GetProfileInt("Units","Prefered",METERS);

	m_bPrintShowScale=theApp.GetProfileInt("Printing","Scale",TRUE);
	m_bPrintShowNorth=theApp.GetProfileInt("Printing","North",TRUE);
	m_iPrintOverlap=theApp.GetProfileInt("Printing","Overlap",20);
	m_bPrintThickLines=theApp.GetProfileInt("Printing","ThickLines",FALSE);
	m_bShowSideScale=theApp.GetProfileInt("Printing","SideScale",TRUE);
	m_bPrintTitlePage=theApp.GetProfileInt("Printing","TitlePage",FALSE);
	m_bPrintPeopleByYear=theApp.GetProfileInt("Printing","PeopleByYear",FALSE);

	m_szFontName=theApp.GetProfileString("Font","FontName","Arial");
	m_iFontSize=theApp.GetProfileInt("Font","FontSize",8);
	m_iFontWeight=theApp.GetProfileInt("Font","FontWeight",100);

	pSettings_G->ReadFromIniFile();
}



void CSettings::WriteToIniFile()
{
	theApp.WriteProfileInt("ViewSettings","ShowQueryFromShot",m_bQueryFrom);
	theApp.WriteProfileInt("ViewSettings","ShowQueryToShot",m_bQueryTo);
	theApp.WriteProfileInt("ViewSettings","ShowQueryBox",m_bQueryBox);
	theApp.WriteProfileInt("ViewSettings","QueryColor",m_crQueryColor);
}

void CSettings::ReadFromIniFile()
{
	m_bQueryFrom=theApp.GetProfileInt("ViewSettings","ShowQueryFromShot",m_bQueryFrom);
	m_bQueryTo=theApp.GetProfileInt("ViewSettings","ShowQueryToShot",m_bQueryTo);
	m_bQueryBox=theApp.GetProfileInt("ViewSettings","ShowQueryBox",m_bQueryBox);
	m_crQueryColor=theApp.GetProfileInt("ViewSettings","QueryColor",m_crQueryColor);
}

CSettings::~CSettings()
{
	int iIndex=GetNumberOfBookMarks();
	for (int i=0;i<iIndex;i++)
	{
		CBookMark * BM=GetBookMark(i);
		delete BM;
	}
	m_BookMarkList.RemoveAll();
}


CSettings::CSettings()
{
	ResetContents();

	m_iColorSchemeIndex=0;

	m_crQueryColor=RGB(255,0,0);
	m_bQueryFrom=TRUE;
	m_bQueryTo=TRUE;
	m_bQueryBox=TRUE;
}

void CSettings::Write(CFileHelper *file)
{
	file->WriteToken("ProprietaryStart","On Station Editor Settings");
	file->WriteToken("Begin","ColorSchemes");
	for (int i=0;i<TOTAL_COLOR_SCHEMES;i++)
	{
		file->WriteToken("Name",m_szColorSchemeNames[i]);
		file->WriteTokenColor("SurfaceColor",m_cSurfaceColors[i]);
		file->WriteToken("SurfaceSurveys",m_bSurfaceSurveys[i]);
		file->WriteToken("ShowClosureErrors",m_bShowClosureErrors[i]);
		file->WriteToken("ShowClosedLoops",m_bShowClosedLoops[i]);
		file->WriteToken("ShowFixedPoints",m_bBoxFixedPoints[i]);
		file->WriteToken("ShowStationNames",(int)m_ShowStationNames[i]);
		file->WriteToken("Perspective",m_bPerspective[i]);
		file->WriteToken("ViewType",m_ViewTypes[i]);
		//Passage walls stuff
		file->WriteToken("PassageType",(int)m_PassageType[i]);
		file->WriteToken("OpaquePassages",m_bOpaquePassages[i]);
		file->WriteToken("DepthFog",m_bDepthFogging[i]);
		file->WriteToken("Lights",m_bMultipleLights[i]);
		file->WriteToken("NoLightRotate",m_bRotateNoLighting[i]);
		file->WriteToken("NoSolidRotate",m_bRotateNoSolids[i]);
	}
	file->WriteToken("End","ColorSchemes");
	file->WriteToken("NumBookmarks",GetNumberOfBookMarks());
	for (i=0;i<GetNumberOfBookMarks();i++)
	{
		CBookMark *BM=GetBookMark(i);
		BM->Write(file);
	}
	file->WriteToken("Begin","DepthColors");
	file->WriteToken("NumberOfDepthColors",m_iNumDepthColors);
	file->WriteToken("StartingDepth",m_fStartDepth);
	file->WriteToken("DepthIncrements",m_fDepthIncrements);
	for (i=0;i<m_iNumDepthColors;i++)
	{
		file->WriteTokenColor("Color",m_crDepthColors[i]);
	}
	file->WriteToken("End","DepthColors");

	file->WriteToken("Begin","AgeColors");
	file->WriteToken("NumberOfAgeColors",m_iNumAgeColors);
	for (i=0;i<m_iNumAgeColors;i++)
	{
		file->WriteTokenDate("Date",m_dwAgeDates[i]);
		file->WriteTokenColor("Color",m_crAgeColors[i]);
	}
	file->WriteToken("End","AgeColors");

	file->WriteToken("ProprietaryEnd","On Station Editor Settings");
}

void CSettings::Read(CFileHelper *file)
{
	if (!file->CheckProprietaryStart("On Station Editor Settings",TRUE))
	{
		return;
	}
	//The last call just did a lookahead.  Now get it
	file->ReadTokenString("ProprietaryStart");
	file->ReadTokenBegin("ColorSchemes");
	for (int i=0;i<TOTAL_COLOR_SCHEMES;i++)
	{
		if (i<MAX_REGULAR_COLOR_SCHEMES)
		{
			m_szColorSchemeNames[i]=file->ReadTokenString("Name");
		}
		else
		{
			file->ReadTokenString("Name");	//ignore the name
		}
		m_cSurfaceColors[i]=file->ReadTokenColor("SurfaceColor");
		m_bSurfaceSurveys[i]=file->ReadTokenInt("SurfaceSurveys");
		m_bShowClosureErrors[i]=file->ReadTokenInt("ShowClosureErrors");
		m_bShowClosedLoops[i]=file->ReadTokenInt("ShowClosedLoops");
		m_bShowClosedLoops[i]=FALSE;	//bug if we show them on startup
		m_bShowReferenceGrid[i]=FALSE;
		m_bBoxFixedPoints[i]=file->ReadTokenInt("ShowFixedPoints");
		m_ShowStationNames[i]=(STATIONSHOW)(file->ReadTokenInt("ShowStationNames"));
		m_bPerspective[i]=(int)file->ReadTokenInt("Perspective");
		m_ViewTypes[i]=(VIEWTYPE)file->ReadTokenInt("ViewType");

		//Passage walls stuff
		m_PassageType[i]=(PASSAGE_TYPE)file->ReadTokenInt("PassageType");
		m_bOpaquePassages[i]=file->ReadTokenInt("OpaquePassages");
		m_bDepthFogging[i]=file->ReadTokenInt("DepthFog");
		m_bMultipleLights[i]=file->ReadTokenInt("Lights");
		m_bRotateNoLighting[i]=file->ReadTokenInt("NoLightRotate");
		m_bRotateNoSolids[i]=file->ReadTokenInt("NoSolidRotate");
	}
	file->ReadTokenEnd("ColorSchemes");
	m_bShowClosedLoops[COLORSCHEME_BLUNDER]=TRUE;
	int iNumBookMarks=file->ReadTokenInt("NumBookmarks");
	for (i=0;i<iNumBookMarks;i++)
	{
		CBookMark *pBM=new CBookMark("",0.0f,0.0f,0.0f,0.0f,0.0f,10.0f,0);
		pBM->Read(file);
		AddBookMark(pBM);
	}
	file->ReadTokenBegin("DepthColors");
	m_iNumDepthColors=file->ReadTokenInt("NumberOfDepthColors");
	m_fStartDepth=file->ReadTokenFloat("StartingDepth");
	m_fDepthIncrements=file->ReadTokenFloat("DepthIncrements");
	for (i=0;i<m_iNumDepthColors;i++)
	{
		m_crDepthColors[i]=file->ReadTokenColor("Color");
	}
	file->ReadTokenEnd("DepthColors");

	file->ReadTokenBegin("AgeColors");
	m_iNumAgeColors=file->ReadTokenInt("NumberOfAgeColors");
	for (i=0;i<m_iNumAgeColors;i++)
	{
		m_dwAgeDates[i]=file->ReadTokenDate("Date");
		m_crAgeColors[i]=file->ReadTokenColor("Color");
	}
	file->ReadTokenEnd("AgeColors");

	file->CheckProprietaryEnd("On Station Editor Settings");
}

CBookMark * CSettings::GetBookMark(int iIndex)
{
	POSITION P;
	P=m_BookMarkList.FindIndex(iIndex);
	if (P!=NULL)
	{
		return (CBookMark *)m_BookMarkList.GetAt(P);
	}
	else
	{
		return NULL;
	}
}

void CSettings::RemoveBookMark(int iIndex)
{
	POSITION P;
	P=m_BookMarkList.FindIndex(iIndex);
	if (P!=NULL)
	{
		m_BookMarkList.RemoveAt(P);
	}
}

int CSettings::GetNumberOfBookMarks() const
{
	return m_BookMarkList.GetCount();
}

void CSettings::AddBookMark(CBookMark *Mark)
{
	m_BookMarkList.AddHead((CObject *)Mark);
}


void CSettings::SetDefaultAgeColors()
{
	for (int i=0;i<MAX_AGE_COLORS;i++)
	{
		m_crAgeColors[i]=CGlobalColorManager::StandardVGAColors[i];
	}
	CTime time=CTime::GetCurrentTime();
	SetAgeColors(MakeMyDate(time.GetYear()-20,1,1),10,2);
}


COLORREF CSettings::GetColorByAge(DWORD dwAge)
{
	for (int i=0;i<m_iNumAgeColors;i++)
	{
		if (dwAge<=m_dwAgeDates[i])
		{
			return m_crAgeColors[i];
		}
	}
	return m_crAgeColors[m_iNumAgeColors-1];
}

void CSettings::SetAgeColors(DWORD dwDate,UINT iNumIntervals,UINT iYears)
{
	m_iNumAgeColors=iNumIntervals;
	for (UINT i=0;i<iNumIntervals;i++)
	{
		m_dwAgeDates[i]=dwDate;
		int iYear=GetYear(dwDate);
		int iDay=GetDay(dwDate);
		int iMonth=GetMonth(dwDate);
		dwDate=MakeMyDate(iYear+iYears,iMonth,iDay);
	}
}


void CSettings::ResetContents()
{
//Remove all of the bookmarks
	int iIndex=GetNumberOfBookMarks();
	for (int i=0;i<iIndex;i++)
	{
		CBookMark * BM=GetBookMark(i);
		delete BM;
	}
	m_BookMarkList.RemoveAll();

	m_fDepthIncrements=NAN;	//Default could be generated from the data
	m_fStartDepth=NAN;		//or can be set manually.  Set to NAN to identify the non-initialized case
	m_iNumAgeColors=0;

	for (i=0;i<MAX_REGULAR_COLOR_SCHEMES;i++)
	{
		CString CS;
		CS.Format(IDS_COLORSCHEMENAME,i+1);
		m_szColorSchemeNames[i]=CS;
	}
	m_szColorSchemeNames[COLORSCHEME_AGE].LoadString(IDS_COLORSCHEMEAGE);
	m_szColorSchemeNames[COLORSCHEME_DEPTH].LoadString(IDS_COLORSCHEMEDEPTH);
	m_szColorSchemeNames[COLORSCHEME_BLUNDER].LoadString(IDS_COLORSCHEMEBLUNDER);

	for (i=0;i<TOTAL_COLOR_SCHEMES;i++)
	{
		m_cSurfaceColors[i]=RGB(128,128,128);
		m_bSurfaceSurveys[i]=TRUE;
		m_bShowClosureErrors[i]=TRUE;
		m_bShowClosedLoops[i]=FALSE;
		m_bBoxFixedPoints[i]=TRUE;
		m_ShowStationNames[i]=SHOW_INTERSECT;
		m_bPerspective[i]=FALSE;
		m_ViewTypes[i]=VT_FREEFORM;

		//Passage walls stuff
		m_PassageType[i]=PT_LINE_PLOT;
		m_bOpaquePassages[i]=FALSE;
		m_bDepthFogging[i]=FALSE;
		m_bMultipleLights[i]=TRUE;
		m_bRotateNoLighting[i]=TRUE;
		m_bRotateNoSolids[i]=TRUE;
	}

	for (i=0;i<MAX_AGE_COLORS;i++)
	{
		m_crAgeColors[i]=CGlobalColorManager::StandardVGAColors[i];
	}
	for (i=0;i<MAX_DEPTH_COLORS;i++)
	{
		m_crDepthColors[i]=CGlobalColorManager::StandardVGAColors[i];
	}
	SetDocumentDepthGrid();
	SetDefaultAgeColors();
	m_iColorSchemeIndex=0;
	m_bShowClosedLoops[COLORSCHEME_BLUNDER]=TRUE;
}


void CSettings::SetDocumentDepthGrid(COnStationDoc *document,COnStationView *View,int iNumDepthColors)
{
	//We will crash if this variable is out of bounds, so do a validity
	//check on it.
	ASSERT(iNumDepthColors>1 && iNumDepthColors<=MAX_DEPTH_COLORS);
	iNumDepthColors=max(1,iNumDepthColors);
	iNumDepthColors=min(MAX_DEPTH_COLORS,iNumDepthColors);

	int iDepthDelta=100;
	m_fStartDepth=0.0f;
	if (View!=NULL)
	{
		CNodeTreeSummary NTS;
		CNodeTree *NT=document->GetNodeTree();
		NT->GetViewInfo(&NTS);

		iDepthDelta=(int)(NTS.m_fHighest-NTS.m_fDeepest);
		m_fStartDepth=(float)((int)NTS.m_fHighest);
	}
	//	CFolderData Data;
	m_iNumDepthColors=iNumDepthColors;
	iDepthDelta=(iDepthDelta/DEPTH_ROUNDING+1)*DEPTH_ROUNDING;	//Round up to nearest 50 meters
	m_fDepthIncrements=(float)iDepthDelta/(float)m_iNumDepthColors;
	for (int i=0;i<m_iNumDepthColors;i++)
	{
		m_crDepthColors[i]=CGlobalColorManager::StandardVGAColors[i];		//index in order
	}
}

COLORREF CSettings::GetDepthColor(float fDepth)
{
	int iZone=(int)((m_fStartDepth-fDepth)/m_fDepthIncrements);
	return GetDepthColor(iZone);
}

COLORREF CSettings::GetDepthColor(int iDepthInterval)
{
	if (iDepthInterval<0)
		iDepthInterval=0;
	if (iDepthInterval>=m_iNumDepthColors)
		iDepthInterval=m_iNumDepthColors-1;
	return m_crDepthColors[iDepthInterval];
}


CFont* CAppSettings::GetFont()
{
	CFont * pFont=new CFont();
	pFont->CreateFont(m_iFontSize,0,0,0,m_iFontWeight,FALSE,FALSE,0,DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,m_szFontName);
	return pFont;
}

CFont* CAppSettings::GetPrinterFont()
{
	CFont * pFont=new CFont();
	pFont->CreateFont(m_iFontSize*2,0,0,0,m_iFontWeight,FALSE,FALSE,0,DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,m_szFontName);
	return pFont;
}

COLORREF CSettings::GetBlunderColor(int iDeviation)
{
	switch (iDeviation)
	{
	case 0:
		return RGB(128,128,128);
	case 1:
		return RGB(192,192,129);
	case 2:
		return RGB(255,255,255);
	case 3:
		return RGB(128,0,0);
	default:
		return RGB(255,0,0);
	}
}