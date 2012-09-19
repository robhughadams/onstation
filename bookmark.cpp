#include "stdafx.h"
#include "bookmark.h"
#include "filehelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBookMark::CBookMark(LPCSTR szName,float X,float Y,float Z,float fRot,float fTilt,float fZoom,int iColorSchemeIndex)
{
	m_szName=szName;
	m_fPanX=X;
	m_fPanY=Y;
	m_fPanZ=Z;
	m_fRot=fRot;
	m_fTilt=fTilt;
	m_fZoom=fZoom;
	m_iColorSchemeIndex=iColorSchemeIndex;
}

void CBookMark::Write(CFileHelper * FH)
{
	FH->WriteToken("Begin","BookMark");
	FH->WriteToken("BookmarkName",m_szName);
	FH->WriteToken("PanX",m_fPanX);
	FH->WriteToken("PanY",m_fPanY);
	FH->WriteToken("PanZ",m_fPanZ);
	FH->WriteToken("Rotate",m_fRot);
	FH->WriteToken("Tilt",m_fTilt);
	FH->WriteToken("Scale",m_fZoom);
	FH->WriteToken("ColorSchemeIndex",m_iColorSchemeIndex);
	FH->WriteToken("End","BookMark");
}

void CBookMark::Read(CFileHelper *FH)
{
	FH->ReadTokenBegin("BookMark");
	m_szName=FH->ReadTokenString("BookmarkName");
	m_fPanX=FH->ReadTokenFloat("PanX");
	m_fPanY=FH->ReadTokenFloat("PanY");
	m_fPanZ=FH->ReadTokenFloat("PanZ");
	m_fRot=FH->ReadTokenFloat("Rotate");
	m_fTilt=FH->ReadTokenFloat("Tilt");
	m_fZoom=FH->ReadTokenFloat("Scale");
	m_iColorSchemeIndex=FH->ReadTokenInt("ColorSchemeIndex");
	FH->ReadTokenEnd("BookMark");
}

CBookMark::~CBookMark()
{
}

