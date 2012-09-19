// AboutWeb.cpp : implementation file
//

#include "stdafx.h"
#include "onstation.h"
#include "AboutWeb.h"
#include "winreg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutWeb property page

IMPLEMENT_DYNCREATE(CAboutWeb, CPropertyPage)

CAboutWeb::CAboutWeb() : CPropertyPage(CAboutWeb::IDD)
{
	//{{AFX_DATA_INIT(CAboutWeb)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	HKEY hKey;
	LONG lValue=RegOpenKeyEx(HKEY_CLASSES_ROOT,"htmlfile\\shell\\open\\command",0,KEY_READ,&hKey);
	m_szBrowser="";
	if (lValue==ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD dwDataSize=256;
		char byData[512];
		if (RegQueryValueEx(hKey,"",NULL,&dwType,(LPBYTE)byData,&dwDataSize)==ERROR_SUCCESS)
		{
			m_szBrowser=byData;
		}
	}
}

CAboutWeb::~CAboutWeb()
{
}

void CAboutWeb::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutWeb)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutWeb, CPropertyPage)
	//{{AFX_MSG_MAP(CAboutWeb)
	ON_BN_CLICKED(IDC_BUTTON1, OnHome)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutWeb message handlers

void CAboutWeb::OnHome() 
{
	if (m_szBrowser.GetLength()>1)
	{
		char szBuffer[512];
		lstrcpyn(szBuffer,m_szBrowser,sizeof(szBuffer)-1);
		lstrcat(szBuffer," http://members.home.net/tacovan/index.html");
		WinExec(szBuffer,SW_SHOWNORMAL);
	}
	else
	{
		AfxMessageBox(IDS_NOBROWSER);
	}
	// TODO: Add your control notification handler code here
	
}
