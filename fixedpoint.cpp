#include "stdafx.h"
#include "fixedpoint.h"
#include "surfacedata.h"
#include "filehelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//All units are in UTM units
void CFixedPoint::SetAllCoordinates(float fx,float fy,float fz)
{
	m_fXpos=fx;
	m_fYpos=fy;
	m_fZpos=fz;
}


CFixedPoint::CFixedPoint(LPCSTR szStationName)
{
	m_szName=szStationName;
	m_fXpos=0.0f;
	m_fYpos=0.0f;
	m_fZpos=0.0f;
	m_bWarnedMissing=FALSE;
}


void CFixedPoint::SetComment(LPCSTR szComment)
{
	m_szComment=szComment;
}

void CFixedPoint::Write(CFileHelper *h)
{
	h->WriteToken("StationName",m_szName);
	h->WriteToken("ConstraintComment",m_szComment);
	h->WritePartialToken("StationLocation");
	h->AppendFloat(m_fXpos);
	h->AppendFloat(m_fYpos);
	h->AppendFloat(m_fZpos);
	h->WriteEndOfLine();
}

void CFixedPoint::Read(CFileHelper *h)
{
	m_szName=h->ReadTokenString("StationName");
	m_szComment=h->ReadTokenString("ConstraintComment");
	h->ReadPartialToken("StationLocation");
	m_fXpos=h->ReadAppendedFloat();
	m_fYpos=h->ReadAppendedFloat();
	m_fZpos=h->ReadAppendedFloat();
}


CFixedPoint::~CFixedPoint()
{
}



