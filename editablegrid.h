#ifndef I_EDITABLEGRID__
#define I_EDITABLEGRID__
#include "gridctrl.h"
#include "afxtempl.h"

/////////////////////////////////////////////////////////////////////////////
// CEditableGrid window
class CUndoItem
{
public:
	CString m_szOldContents;
	int m_iXpos;
	int m_iYpos;
};
class CUndoStructure
{
public:
	CArray<CUndoItem *,CUndoItem *> m_ItemArray;

	~CUndoStructure();
};

class CEditableGrid : public CGridCtrl
{
protected:
	int m_iCurrentlyEditingRow;
	int m_iCurrentlyEditingCol;
	BOOL m_bIsModified;
// Construction
public:
	CEditableGrid();

	BOOL * m_pDirtyRows;

	BOOL IsModified() const {return m_bIsModified;}
// Attributes
public:
	CArray<CUndoStructure *,CUndoStructure*> m_UndoArray;

	void PushUndoEvent();
	void AddUndoItem(int iRow,int iCol);
	void SetRows(short iNumRows);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditableGrid)
	//}}AFX_VIRTUAL

// Implementation
	void OnCopy();
	void OnUndo();
	void OnPaste();
	void DeleteSelectedArea();
public:
	virtual ~CEditableGrid();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditableGrid)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif

