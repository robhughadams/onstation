// 
#include "stdafx.h"
#include "onstation.h"
#include "filedump.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFileDumpHelper::CFileDumpHelper()
{
	m_pFile=NULL;
};

CFileDumpHelper::~CFileDumpHelper()
{
}

void CFileDumpHelper::EndDump()
{
	m_pFile->Close();
	delete m_pFile;
	m_pFile=NULL;
}
BOOL CFileDumpHelper::BeginDump()
{
	char szReturn[512];
	szReturn[0]=0;

	OPENFILENAME of;
	of.lStructSize=sizeof(OPENFILENAME);
	of.hwndOwner=AfxGetMainWnd()->m_hWnd;
	of.hInstance=AfxGetInstanceHandle();
	of.lpstrFilter="(Text Files)\0*.txt\0\0";
	of.lpstrCustomFilter=NULL;
	of.nFilterIndex=0;
	of.lpstrFile=szReturn;
	of.nMaxFile=sizeof(szReturn);
	of.lpstrFileTitle=NULL;
	of.lpstrInitialDir=NULL;
	of.lpstrTitle="Export Formatted Text File";
	of.Flags=OFN_HIDEREADONLY;
	of.lpstrDefExt="txt"; 
	
	if (GetOpenFileName(&of)==TRUE)
	{
		try
		{
			m_pFile=new CFile(szReturn,CFile::modeCreate|CFile::modeWrite);
		}
		catch (CFileException *)
		{
			AfxMessageBox(IDS_UNABLETODUMPFORMATTEDTEXT);
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CFileDumpHelper::OutputText(LPCSTR szData)
{
	m_pFile->Write(szData,lstrlen(szData));
}

