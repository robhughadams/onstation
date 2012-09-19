#include "stdafx.h"
#include "comment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComment::CComment(LPCSTR szStationName,LPCSTR szType,LPCSTR szBody)
{
	SetValues(szStationName,szType,szBody);
}
CComment::~CComment()
{
};

void CComment::SetValues(LPCSTR szStationName,LPCSTR szType,LPCSTR szBody)
{
	if (szType!=NULL)
		m_szType=szType;
	if (szBody!=NULL)
		m_szBody=szBody;
	if (szStationName!=NULL)
		lstrcpyn(m_szStationName,szStationName,MAX_STATION_NAME_LENGTH);
}


