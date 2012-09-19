#include "stdafx.h"
#include "clipboard.h"
#include "surveyshot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern UINT uiCustomClipboardFormat_G;

BOOL CClipboard::StringToClipboard(CString& string,BOOL bCustomFormat)
{
	OpenClipboard(AfxGetMainWnd()->m_hWnd);
	if (EmptyClipboard()==FALSE)
		return FALSE;
	HGLOBAL hMem=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,string.GetLength()+1);
	LPBYTE byMem=(LPBYTE)GlobalLock(hMem);
	lstrcpy((LPSTR)byMem,(LPCSTR)string);
	GlobalUnlock(hMem);
	if (bCustomFormat)
	{
		if (SetClipboardData(uiCustomClipboardFormat_G,hMem)==NULL)
		{
			return FALSE;
		}
	}
	else
	{
		if (SetClipboardData(CF_TEXT,hMem)==NULL)
		{
			return FALSE;
		}
	}
	CloseClipboard();
	return TRUE;
}

BOOL CClipboard::IsClipboardEmpty()
{
	return (!IsClipboardFormatAvailable(CF_TEXT));
}


BOOL CClipboard::StringFromClipboard(CString& string,BOOL bCustomFormat)
{
	OpenClipboard(AfxGetMainWnd()->m_hWnd);
	HANDLE hMem;
	if (bCustomFormat)
	{
		hMem=GetClipboardData(uiCustomClipboardFormat_G);
	}
	else
	{
		hMem=GetClipboardData(CF_TEXT);
	}
	if (hMem==NULL)
		return FALSE;
	LPBYTE byMem=(LPBYTE)GlobalLock(hMem);
	string=(LPCSTR)byMem;
	GlobalUnlock(hMem);
	CloseClipboard();
	return TRUE;
}
