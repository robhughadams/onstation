// InfoByViewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "onstation.h"
#include "InfoByViewDialog.h"
#include "nodetree.h"
#include "settings.h"

extern COnStationApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoByViewDialog dialog

/*	float m_fDeepest;
	char m_szDeepest[MAX_STATION_NAME_LENGTH];
	float m_fHighest;
	char m_szHighest[MAX_STATION_NAME_LENGTH];
	float m_fSurfaceLength;
	float m_fCaveLength;
	int m_iCaveShots;
	int m_iSurfaceShots;
*/

CInfoByViewDialog::CInfoByViewDialog(CNodeTreeSummary *NTS,CWnd* pParent /*=NULL*/)
	: CDialog(CInfoByViewDialog::IDD, pParent)
{
	m_NTS=NTS;
	char szBuffer[64];
	char szBuffer2[64];
	char szBuffer3[64];
	char szBuffer4[64];
	char szBuffer5[64];
	char szBuffer6[64];
	char szBuffer7[64];
	wsprintf(szBuffer,"%i",NTS->m_iCaveShots);
	wsprintf(szBuffer2,"%i",NTS->m_iSurfaceShots);
	sprintf(szBuffer3,"%s\t%s",NTS->m_szDeepest,GetPreferedUnitString(NTS->m_fDeepest));
	sprintf(szBuffer4,"%s\t%s",NTS->m_szHighest,GetPreferedUnitString(NTS->m_fHighest));
	sprintf(szBuffer5,"%s",GetPreferedUnitString(NTS->m_fCaveLength));
	sprintf(szBuffer6,"%s",GetPreferedUnitString(NTS->m_fSurfaceLength));
	sprintf(szBuffer7,"%s",GetPreferedUnitString(NTS->m_fHorizontalLength));
	//{{AFX_DATA_INIT(CInfoByViewDialog)
	m_NumShots = szBuffer;
	m_NumSurfaceShots = szBuffer2;
	m_szDeepestPoint = szBuffer3;
	m_szHighestPoint = szBuffer4;
	m_TotalLength = szBuffer5;
	m_TotalSurfaceLength = szBuffer6;
	m_HorizontalLength=szBuffer7;
	//}}AFX_DATA_INIT
}


void CInfoByViewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoByViewDialog)
	DDX_Text(pDX, IDC_DEEPESTPOINT, m_szDeepestPoint);
	DDX_Text(pDX, IDC_HIGHESTPOINT, m_szHighestPoint);
	DDX_Text(pDX, IDC_NUMSHOTS, m_NumShots);
	DDX_Text(pDX, IDC_NUMSURFACESHOTS, m_NumSurfaceShots);
	DDX_Text(pDX, IDC_TOTALLENGTH, m_TotalLength);
	DDX_Text(pDX, IDC_HORIZONTALLENGTH,m_HorizontalLength);
	DDX_Text(pDX, IDC_TOTALSURFACELENGTH, m_TotalSurfaceLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoByViewDialog, CDialog)
	//{{AFX_MSG_MAP(CInfoByViewDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoByViewDialog message handlers
