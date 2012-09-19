#include "stdafx.h"
#include "onstation.h"
#include "editablegrid.h"
#include "clipboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CEditableGrid

CEditableGrid::CEditableGrid()
{
	m_iCurrentlyEditingRow=-1;
	m_iCurrentlyEditingCol=-1;
	m_pDirtyRows=NULL;
	m_bIsModified=FALSE;
}

void CEditableGrid::SetRows(short iNumRows)
{
	CGridCtrl::SetRows(iNumRows);
	delete[] m_pDirtyRows;
	m_pDirtyRows=new BOOL[iNumRows];
	for (int i=0;i<iNumRows;i++)
	{
		m_pDirtyRows[i]=FALSE;
	}
}

CEditableGrid::~CEditableGrid()
{
	for (int i=0;i<m_UndoArray.GetSize();i++)
	{
		CUndoStructure * pUndo=m_UndoArray.GetAt(i);
		delete pUndo;
	}
	m_UndoArray.RemoveAll();
	
	delete[] m_pDirtyRows;
	m_pDirtyRows=FALSE;
}


BEGIN_MESSAGE_MAP(CEditableGrid, CGridCtrl)
	//{{AFX_MSG_MAP(CEditableGrid)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditableGrid message handlers

void CEditableGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_DELETE:
		DeleteSelectedArea();
		break;
	default:
		break;
	}
	CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEditableGrid::PushUndoEvent()
{
	m_bIsModified=TRUE;
	CUndoStructure * pUndo=new CUndoStructure();
	m_UndoArray.Add(pUndo);
	if (m_UndoArray.GetSize()>GRID_UNDO_DEPTH)
	{
		TRACE("Undo buffer full, removing oldest item\n");
		delete m_UndoArray[0];
		m_UndoArray.RemoveAt(0);
	}
}

void CEditableGrid::OnUndo()
{
	if (m_UndoArray.GetSize()==0)
	{
		MessageBeep(0);
		return;	//nothing to UNDO
	}
	CUndoStructure * pUndoStruct=m_UndoArray[m_UndoArray.GetSize()-1];
	for (int i=0;i<pUndoStruct->m_ItemArray.GetSize();i++)
	{
		CUndoItem * pItem=pUndoStruct->m_ItemArray[i];
		SetCol(pItem->m_iXpos);
		SetRow(pItem->m_iYpos);
		//Don't need to set dirty on undo
		SetText(pItem->m_szOldContents);
	}
	delete pUndoStruct;
	m_UndoArray.RemoveAt(m_UndoArray.GetSize()-1);
}

void CEditableGrid::AddUndoItem(int iRow,int iCol)
{
	int iSavedCol=GetCol();
	int iSavedRow=GetRow();
	SetCol(iCol);
	SetRow(iRow);
	CUndoItem * pItem=new CUndoItem();
	pItem->m_iXpos=iCol;
	pItem->m_iYpos=iRow;
	pItem->m_szOldContents=GetText();
	TRACE("Adding Row %i Col %i text %s to Undo\n",iRow,iCol,(LPCSTR)pItem->m_szOldContents);
	SetCol(iSavedCol);
	SetRow(iSavedRow);
	//This fails if we forgot to call PushUndoEvent before calling AddUndoItem
	ASSERT(m_UndoArray.GetSize()>=1);
	CUndoStructure * pUndoStruct=m_UndoArray[m_UndoArray.GetSize()-1];
	pUndoStruct->m_ItemArray.Add(pItem);
}

void CEditableGrid::DeleteSelectedArea()
{
	PushUndoEvent();
	int iSavedRow=GetRow();
	int iSavedCol=GetCol();
	int iLeftX=GetSelStartRow();
	int iRightX=GetSelEndRow();
	int iTopY=GetSelStartCol();
	int iBottomY=GetSelEndCol();
	for (int x=iLeftX;x<=iRightX;x++)
	{
		for (int y=iTopY;y<=iBottomY;y++)
		{
			SetRow(x);
			SetCol(y);
			AddUndoItem(x,y);
			SetText("");
		}
	}
	SetRow(iSavedRow);
	SetCol(iSavedCol);
}


void CEditableGrid::OnPaste()
{
	int iSavedRow=GetRow();
	int iSavedCol=GetCol();
	int iLeftX=GetSelStartCol();
	int iRightX=GetSelEndCol();
	int iTopY=GetSelStartRow();
	int iBottomY=GetSelEndRow();
	if (iBottomY==iTopY && iRightX==iLeftX)
	{
		iRightX=GetCols();
		iBottomY=GetRows();
	}

	int iRow=iTopY;
	TRACE("CGenericDataPump::PasteToArea()\n");
	char szTemp[128];
	CString CS;
	if (CClipboard::StringFromClipboard(CS,FALSE))
	{
		PushUndoEvent();
		LPCSTR szData=(LPCSTR)CS;
		while (szData[0]!=NULL && iRow<=iBottomY)
		{
			TRACE1("Doing Row %i\n",iRow);
			if (szData[0]=='\n')		//empty row
			{
				TRACE(" empty\n");
				iRow++;
				szData++;
			}
			else
			{
				int iIndexThisString=0;
				int iXPosition=iLeftX;
				szTemp[0]=0;
				//For each line seperate out all of the \t seperated fields
				while (szData[0]!=NULL && szData[0]!='\n')
				{
					szTemp[iIndexThisString]=szData[0];
					if (szData[0]=='\t')		//Got a field
					{
						szTemp[iIndexThisString]=0;
						TRACE2(" Item %i is %s\n",iXPosition,szTemp);
						if (iXPosition<=iRightX)
						{
							SetCol(iXPosition);
							SetRow(iRow);
							AddUndoItem(iRow,iXPosition);
							m_pDirtyRows[iRow]=TRUE;
							SetText(szTemp);
						}
						else	//Too many columns for this line
						{
							TRACE("Column lost due to width limit\n");
						}
						iXPosition++;
						iIndexThisString=-1;
						//just so we start with an empty string.  If not \t\n would generate
						//a second identical string
						szTemp[0]=0;
					}
					szData++;
					iIndexThisString++;
					if (iIndexThisString>sizeof(szTemp))
					{
						iIndexThisString=0;
					}
				}
				if (iXPosition<=iRightX)
				{
					if (iIndexThisString>0 && szTemp[iIndexThisString-1]=='\r')
					{
						szTemp[iIndexThisString-1]=0;
					}
					szTemp[iIndexThisString]=0;
					TRACE2(" Set 2Item %i is %s\n",iXPosition,(LPCSTR)szTemp);
					SetCol(iXPosition);
					SetRow(iRow);
					AddUndoItem(iRow,iXPosition);
					SetText(szTemp);
					m_pDirtyRows[iRow]=TRUE;
					iXPosition++;
				}
				else
				{
					TRACE("Column lost due to width limit(2)\n");
				}
				iRow++;
				if (szData[0]!=NULL)
				{
					szData++;
				}
			}

		}	//end of while szData[0]!=NULL
		SetRow(iSavedRow);
		SetCol(iSavedCol);
	}
	else
	{
		AfxMessageBox(IDS_PASTEFAILED);
	}
}

void CEditableGrid::OnCopy()
{
	int iSavedRow=GetRow();
	int iSavedCol=GetCol();
	int iLeftX=GetSelStartCol();
	int iRightX=GetSelEndCol();
	int iTopY=GetSelStartRow();
	int iBottomY=GetSelEndRow();
	CString CS;
	for (int y=iTopY;y<=iBottomY;y++)
	{
 		BOOL bDidOne=FALSE;
		for (int x=iLeftX;x<=iRightX;x++)
		{
			//By adding a tab to the begining of the next elements, we
			//avoid the problem of having a tab as the character before the
			//line feed.  The \t\n combination was causing some
			//pasting range problems.
			if (bDidOne)
			{
				CS=CS+"\t";
			}
			SetRow(y);
			SetCol(x);
			CS=CS+GetText();
			bDidOne=TRUE;
		}
		CS=CS+"\n";
	}
	SetRow(iSavedRow);
	SetCol(iSavedCol);
//	TRACE("String to Clipboard %s\n",(LPCSTR)CS);
	VERIFY(CClipboard::StringToClipboard(CS,FALSE));
}

void CEditableGrid::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
		case 3:
			OnCopy();
			break;
		case 22:
			OnPaste();
			break;
		case 26:
			OnUndo();
			break;
		case 24:
			OnCopy();
			DeleteSelectedArea();
			break;
		default:
			break;
	}

	//Start a mini-edit (arrow keys still move us forwards and backwards)
	char szBuffer[128];
	lstrcpyn(szBuffer,CGridCtrl::GetText(),sizeof(szBuffer));
	if (GetRow()==m_iCurrentlyEditingRow && GetCol()==m_iCurrentlyEditingCol)
	{
		int iIndex=lstrlen(szBuffer);
		if (nChar==8)	//backspace
		{
			if (iIndex>0)
			{
				szBuffer[iIndex-1]=0;
			}
		}
		else
		{
			if (nChar>26 && iIndex<127)
			{
				szBuffer[iIndex]=nChar;
				szBuffer[iIndex+1]=0;
			}
		}
	}
	else
	{
		if (nChar>26)
		{
			PushUndoEvent();
			AddUndoItem(GetRow(),GetCol());
			m_iCurrentlyEditingRow=GetRow();
			m_iCurrentlyEditingCol=GetCol();
			szBuffer[0]=nChar;
			szBuffer[1]=0;
		}
		else
		{
//			szBuffer[0]=0;	//no change
		}
	}
	CGridCtrl::SetText(szBuffer);
	m_pDirtyRows[GetRow()]=TRUE;
//	CGridCtrl::OnChar(nChar, nRepCnt, nFlags);
}

UINT CEditableGrid::OnGetDlgCode() 
{
//return DLGC_WANTALLKEYS|DLGC_WANTARROWS|DLGC_WANTCHARS|DLGC_WANTMESSAGE;
	return DLGC_WANTALLKEYS|DLGC_WANTCHARS|DLGC_WANTMESSAGE;
}

CUndoStructure::~CUndoStructure()
{
	for (int i=0;i<m_ItemArray.GetSize();i++)
	{
		delete m_ItemArray[i];
		m_ItemArray[i]=0;
	}
}

