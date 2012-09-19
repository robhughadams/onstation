// gridpage.cpp : implementation file
//
// This is the control which holds the editing "spreadsheet".  Due to the way
// messages are handled, most of the commands come throught here and are
// then passed into the grid control.
#include "stdafx.h"
#include "onstation.h"
#include "leggridpage.h"
#include "surveyleg.h"
#include "editablegrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLegGridPage property page

IMPLEMENT_DYNCREATE(CLegGridPage, CPropertyPage)

#define new DEBUG_NEW

static LPCSTR szFlagTypes[]={"S","W","L"};

CLegGridPage::CLegGridPage() : CPropertyPage(CLegGridPage::IDD)
{
	//{{AFX_DATA_INIT(CLegGridPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CLegGridPage::~CLegGridPage()
{
}

void CLegGridPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLegGridPage)
	DDX_Control(pDX, IDC_GRID1, m_myGridControl);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CLegGridPage, CPropertyPage)
	//{{AFX_MSG_MAP(CLegGridPage)
	ON_BN_CLICKED(IDC_AUTONUMBER, OnAutoNumber)
	ON_BN_CLICKED(IDC_UNDOBUTTON, OnUndobutton)
	ON_BN_CLICKED(IDC_INSERTLINE, OnInsertLine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLegGridPage message handlers

BOOL CLegGridPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLegGridPage::SetLeg(CSurveyLeg *Leg)
{
	m_Leg=Leg;
}


BOOL CLegGridPage::OnKillActive()
{	
//Save column widths
	for (int i=0;i<m_iNumStandardCols+4;i++)
	{
		m_Leg->m_iColWidth[i]=(short)m_myGridControl.GetColWidth(i);
	}

	SetCursor(::LoadCursor(NULL,IDC_WAIT));
	m_Leg->DeleteShots();
	CSurveyShot * pLastShot=NULL;
	for (i=0;i<MAX_GRID_ENTRIES;i++)
	{
		CSurveyShot *pShot;
		if (m_Leg->GetCaveDive())
		{
			if (!GetDiveRow(i,&pShot,pLastShot))
			{
				SetCursor(::LoadCursor(NULL,IDC_ARROW));
				return FALSE;
			}
		}
		else
		{
			if (!GetStandardRow(i,&pShot,pLastShot))
			{
				SetCursor(::LoadCursor(NULL,IDC_ARROW));
				return FALSE;
			}
		}
		if (pShot!=NULL)
		{
			m_Leg->GetShotArray()->Add(pShot);
			pLastShot=pShot;
		}
	}
	m_Leg->GetShotArray()->DoDeferredOffsetCalculation();
	SetCursor(::LoadCursor(NULL,IDC_ARROW));
	return CPropertyPage::OnKillActive();
}

BOOL CLegGridPage::OnSetActive()
{
	m_iNumStandardCols=9;
	if (m_Leg->HasBacksights())
	{
		m_iNumStandardCols+=2;
	}
	if (m_myGridControl.m_hWnd==NULL)
	{
		AfxMessageBox(IDS_NOGRIDCONTROL);
		return FALSE;
	}
	m_myGridControl.SetCols(m_iNumStandardCols+4);
	m_myGridControl.SetRows(MAX_GRID_ENTRIES+1);
	int i,j;
	BOOL bSetDefaultColWidths=(m_Leg->m_iColWidth[0]==-1);
	if (!bSetDefaultColWidths)
	{
		for (i=0;i<m_iNumStandardCols+4;i++)
		{
			m_myGridControl.SetColWidth(i,m_Leg->m_iColWidth[i]);
		}
	}
	CSurveyShotArray *List=m_Leg->GetShotArray();
	int iRowIndex=1;
	float f;
	for (i=0;i<List->GetSize();i++)
	{
		int iCol=2;
		if (m_Leg->GetCaveDive())
		{
	 		CCaveDiveSurveyShot *shot=dynamic_cast<CCaveDiveSurveyShot *>(List->GetAt(i));
			ASSERT(shot!=NULL);
			if (shot==NULL)
				return FALSE;	//major problem
			m_myGridControl.SetRow(iRowIndex);
			m_myGridControl.SetCol(0);
			m_myGridControl.SetText(shot->GetFromStationName());
			m_myGridControl.SetCol(1);
			m_myGridControl.SetText(shot->GetToStationName());
			for (j=0;j<7;j++)
			{
				CString CS;
				switch (m_Leg->m_szFieldOrder[j])
				{
					case 'T':
						FloatToString(CS,shot->GetLength());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'A':
						f=shot->GetCompass();
						if (f!=NAN && m_Leg->GetEuroDegrees())
						{
							f=f*400.0f/360.0f;
						}
						FloatToString(CS,f);
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'I':
						f=shot->GetDepthAtFromStation();
						FloatToString(CS,f);
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'U':
						FloatToString(CS,shot->GetRawUp());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'D':
						FloatToString(CS,shot->GetRawDown());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'L':
						FloatToString(CS,shot->GetRawLeft());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'R':
						FloatToString(CS,shot->GetRawRight());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					default:
						ASSERT(FALSE); //some sort of problem
				}
			}
			SetFlags(shot);
			iRowIndex=SetComments(iRowIndex,shot);
		}
		else
		{
	 		CStandardSurveyShot *shot=dynamic_cast<CStandardSurveyShot *>(List->GetAt(i));

			m_myGridControl.SetRow(iRowIndex);
			m_myGridControl.SetCol(0);
			m_myGridControl.SetText(shot->GetFromStationName());
			m_myGridControl.SetCol(1);
			m_myGridControl.SetText(shot->GetToStationName());
			for (j=0;j<m_iNumStandardCols;j++)
			{
				CString CS;
				switch (m_Leg->m_szFieldOrder[j])
				{
					case 'T':
						FloatToString(CS,shot->GetLength());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'A':
						f=shot->GetCompass();
						if (f!=NAN && m_Leg->GetEuroDegrees())
						{
							f=f*400.0f/360.0f;
						}
						FloatToString(CS,f);
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						if (m_Leg->HasBacksights())
						{
							f=shot->GetCompass2();
							if (f!=NAN && m_Leg->GetEuroDegrees())
							{
								f=f*400.0f/360.0f;
							}
							FloatToString(CS,f);
							m_myGridControl.SetCol(iCol++);
							m_myGridControl.SetText(CS);
						}
						break;
					case 'I':
						f=shot->GetClino();
						if (f!=NAN && m_Leg->GetEuroDegrees())
						{
							f=f*100.0f/90.0f;
						}
						FloatToString(CS,f);
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						if (m_Leg->HasBacksights())
						{
							f=shot->GetClino2();
							if (f!=NAN && m_Leg->GetEuroDegrees())
							{
								f=f*100.0f/90.0f;
							}
							FloatToString(CS,f);
							m_myGridControl.SetCol(iCol++);
							m_myGridControl.SetText(CS);
						}
						break;
					case 'U':
						FloatToString(CS,shot->GetRawUp());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'D':
						FloatToString(CS,shot->GetRawDown());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'L':
						FloatToString(CS,shot->GetRawLeft());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
					case 'R':
						FloatToString(CS,shot->GetRawRight());
						m_myGridControl.SetCol(iCol++);
						m_myGridControl.SetText(CS);
						break;
				}
			}
			SetFlags(shot);
			iRowIndex=SetComments(iRowIndex,shot);
		}
	}
	CRect r;
	m_myGridControl.GetClientRect(r);
	m_myGridControl.SetRow(0);
	CString CS;
	CS.LoadString(IDS_FROMSTATION);
	m_myGridControl.SetCol(0);
	m_myGridControl.SetText(CS);
	CS.LoadString(IDS_TOSTATION);
	m_myGridControl.SetCol(1);
	m_myGridControl.SetText(CS);
	int iIndex=2;
	for (j=0;j<7;j++)
	{
		m_myGridControl.SetCol(iIndex++);
		switch (m_Leg->m_szFieldOrder[j])
		{
		case 'T':
			CS.LoadString(IDS_LENGTH);
			m_myGridControl.SetText(CS);
			break;
		case 'I':
			if (m_Leg->GetCaveDive())
			{
				CS.LoadString(IDS_DEPTH);
				m_myGridControl.SetText(CS);
			}
			else
			{
				CS.LoadString(IDS_CLINOMETER);
				m_myGridControl.SetText(CS);
				if (m_Leg->HasBacksights())
				{
					if (bSetDefaultColWidths)
					{
						m_myGridControl.SetColWidth(iIndex-1,PixelsToTwips(REGULAR_COL_WIDTH));
					}
					m_myGridControl.SetCol(iIndex++);
					CS.LoadString(IDS_CLINOMETERBACK);
					m_myGridControl.SetText(CS);
				}
			}
			break;
		case 'A':
			CS.LoadString(IDS_COMPASS);
			m_myGridControl.SetText(CS);
			if (m_Leg->HasBacksights())
			{
				if (bSetDefaultColWidths)
				{
					m_myGridControl.SetColWidth(iIndex-1,PixelsToTwips(REGULAR_COL_WIDTH));
				}
				m_myGridControl.SetCol(iIndex++);
				CS.LoadString(IDS_COMPASSBACK);
				m_myGridControl.SetText(CS);
			}
			break;
		case 'U':
			CS.LoadString(IDS_UP);
			m_myGridControl.SetText(CS);
			break;
		case 'D':
			CS.LoadString(IDS_DOWN);
			m_myGridControl.SetText(CS);
			break;
		case 'L':
			CS.LoadString(IDS_LEFT);
			m_myGridControl.SetText(CS);
			break;
		case 'R':
			CS.LoadString(IDS_RIGHT);
			m_myGridControl.SetText(CS);
			break;
		default:
			ASSERT(!"BAD STRING VALUE");
			break;
		}
		if (bSetDefaultColWidths)
		{
			m_myGridControl.SetColWidth(iIndex-1,PixelsToTwips(REGULAR_COL_WIDTH));
		}
	}
	
	for (j=iIndex;j<iIndex+3;j++)
	{
		m_myGridControl.SetCol(j);
		m_myGridControl.SetText(szFlagTypes[j-iIndex]);	//surface;
		if (bSetDefaultColWidths)
		{
			m_myGridControl.SetColWidth(j,PixelsToTwips(CHECK_COL_WIDTH));
		}
	}	
	m_myGridControl.SetCol(j);
	CString csComment;
	csComment.LoadString(IDS_COMMENTS);
	m_myGridControl.SetText(csComment);
	if (bSetDefaultColWidths)
	{
		m_myGridControl.SetColWidth(j,PixelsToTwips(COMMENTS_COL_WIDTH));
		m_myGridControl.SetColWidth(0,PixelsToTwips(NAME_COLUMN_WIDTH));
		m_myGridControl.SetColWidth(1,PixelsToTwips(NAME_COLUMN_WIDTH));
	}
	m_myGridControl.SetRow(1);
	m_myGridControl.SetCol(0);

	CPropertyPage::OnSetActive();

	return TRUE;
}

void CLegGridPage::SetFlags(CSurveyShot * pShot)
{
	if (m_myGridControl.m_hWnd==NULL)
	{
		return;
	}
	m_myGridControl.SetCol(m_iNumStandardCols);
	if ((pShot->GetShotFlags()&SHOT_SURFACE)!=0)
	{
		m_myGridControl.SetText("X");
	}
	m_myGridControl.SetCol(m_iNumStandardCols+1);
	if ((pShot->GetShotFlags()&SHOT_SPLAY_SHOT)!=0)
	{
		m_myGridControl.SetText("X");
	}
	m_myGridControl.SetCol(m_iNumStandardCols+2);
	if ((pShot->GetShotFlags()&SHOT_EXCLUDE_FROM_LENGTH_TOTALS)!=0)
	{
		m_myGridControl.SetText("X");
	}
}

int CLegGridPage::SetComments(int iRow,CSurveyShot * pShot)
{
	int iNumShots=pShot->m_Comments.GetSize();
	for (int i=0;i<iNumShots;i++)
	{
		CComment * pComment=pShot->m_Comments.GetAt(i);
		m_myGridControl.SetRow(iRow);
		m_myGridControl.SetCol(m_iNumStandardCols+3);
		CString CS;
		if (!pComment->GetType()[0]==0)
		{
			CS=pComment->GetType();
			CS+=":";
		}
		CS+=pComment->GetBody();
		m_myGridControl.SetText(CS);
		iRow++;
	}
	if (iNumShots==0)
	{
		iRow++;
	}
	return iRow;
}


BOOL CLegGridPage::NeedRecalculate() const
{
	return TRUE;
}

BOOL CLegGridPage::GetDiveRow(int iLine,CSurveyShot ** ppShot,CSurveyShot * pPrevious)
{
	*ppShot=NULL;

	CString csLength;
	CString csCompass;
	CString csDepth;
	CString csUp;
	CString csDown;
	CString csLeft;
	CString csRight;
	CString csFrom;
	CString csTo;

	m_myGridControl.SetRow(iLine+1);
	m_myGridControl.SetCol(m_Leg->FindEntry('T'));
	csLength=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('A'));
	csCompass=m_myGridControl.GetText();
    m_myGridControl.SetCol(m_Leg->FindEntry('I'));
	csDepth=m_myGridControl.GetText();
	m_myGridControl.SetCol(0);
	csFrom=m_myGridControl.GetText();
	m_myGridControl.SetCol(1);
	csTo=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('L'));
	csLeft=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('R'));
	csRight=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('U'));
	csUp=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('D'));
	csDown=m_myGridControl.GetText();
	if (csCompass.IsEmpty() &&
		csDepth.IsEmpty() &&
		csLength.IsEmpty() && 
		csUp.IsEmpty() &&
		csDown.IsEmpty() &&
		csLeft.IsEmpty() &&
		csRight.IsEmpty() &&
		csFrom.IsEmpty() &&
		csTo.IsEmpty())
	{
		if (pPrevious!=NULL)
		{
			GetComments(pPrevious);
		}
		return TRUE;	//no problem.  Just an empty shot
	}

	if (csCompass.IsEmpty() ||
		csDepth.IsEmpty() ||
		csFrom.IsEmpty() ||
		csTo.IsEmpty() ||
		csLength.IsEmpty())
	{
		//From without other readings could be a wall dimension defining shot.
		if (csFrom.IsEmpty() || !csTo.IsEmpty() || !csCompass.IsEmpty() || csDepth.IsEmpty())
		{
			LegGridMessageBox(IDS_INCOMPLETESHOTDEPTH,iLine+1,1);
			return FALSE;
		}
    }

	if (strstr(csFrom," ")!=NULL)
	{
		LegGridMessageBox(IDS_BADSTATIONNAME,iLine+1,0);
		return FALSE;
	}
	if (strstr(csTo," ")!=NULL)
	{
		LegGridMessageBox(IDS_BADSTATIONNAME,iLine+1,1);
		return FALSE;
	}								

	float fUp,fDown,fLeft,fRight;		//validity check of field formats
	float fLength,fCompass,fDepth;
	if (!FloatFromString(csUp,fUp))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('U'));
		return FALSE;
	}
	if (!FloatFromString(csDown,fDown))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('D'));
		return FALSE;
	}
	if (!FloatFromString(csLeft,fLeft))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('L'));
		return FALSE;
	}
	if (!FloatFromString(csRight,fRight))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('R'));
		return FALSE;
	}
	if (!FloatFromString(csLength,fLength))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('T'));
		return FALSE;
	}
	if (!FloatFromString(csCompass,fCompass))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('A'));
		return FALSE;
	}
	if (m_Leg->GetEuroDegrees())
	{
		if (fCompass!=NAN && (fCompass<-400.0f || fCompass>400.0f) )
		{
			LegGridMessageBox(IDS_COMPASSRANGEEURO,iLine+1,m_Leg->FindEntry('A'));
			return FALSE;
		}
		if (fCompass!=NAN)
		{
			fCompass=fCompass*360.0f/400.0f;	//convert.  Always store in standard units
		}
	}
	else
	{
		if (fCompass!=NAN && (fCompass<-360.0f || fCompass>360.0) )
		{
			LegGridMessageBox(IDS_COMPASSRANGE,iLine+1,m_Leg->FindEntry('A'));
			return FALSE;
		}
	}
	if (!FloatFromString(csDepth,fDepth))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('I'));
		return FALSE;
	}

	CCaveDiveSurveyShot *pShot=new CCaveDiveSurveyShot(m_Leg);
	pShot->SetStationNames(csFrom,csTo);
	pShot->SetReadings(fLength,fCompass,fDepth);
	pShot->SetUDLR(fUp,fDown,fLeft,fRight);

	GetFlags(pShot);
	GetComments(pShot);
	*ppShot=pShot;

	return TRUE;
}

void CLegGridPage::GetComments(CSurveyShot * pShot)
{
	m_myGridControl.SetCol(m_iNumStandardCols+3);
	CString CS=m_myGridControl.GetText();
	if (!CS.IsEmpty())
	{
		pShot->AddComment(CS);
	}
}
void CLegGridPage::GetFlags(CSurveyShot * pShot)
{
	CString CS;
	int j=m_iNumStandardCols;
	m_myGridControl.SetCol(j);
	CS=m_myGridControl.GetText();
	if (!CS.IsEmpty())
	{
		pShot->SetShotFlags(pShot->GetShotFlags()|SHOT_SURFACE);
	}
	j++;
	m_myGridControl.SetCol(j);
	CS=m_myGridControl.GetText();
	if (!CS.IsEmpty())
	{
		pShot->SetShotFlags(pShot->GetShotFlags()|SHOT_SPLAY_SHOT);
	}
	j++;
	m_myGridControl.SetCol(j);
	CS=m_myGridControl.GetText();
	if (!CS.IsEmpty())
	{
		pShot->SetShotFlags(pShot->GetShotFlags()|SHOT_EXCLUDE_FROM_LENGTH_TOTALS);
	}
}



BOOL CLegGridPage::GetStandardRow(int iLine,CSurveyShot **ppShot,CSurveyShot * pPrevious)
{
	*ppShot=NULL;

	CString csLength;
	CString csCompass;
	CString csCompass2;
	CString csClino;
	CString csClino2;
	CString csUp;
	CString csDown;
	CString csLeft;
	CString csRight;
	CString csFrom;
	CString csTo;

	m_myGridControl.SetRow(iLine+1);
	m_myGridControl.SetCol(m_Leg->FindEntry('T'));
	csLength=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('A'));
	csCompass=m_myGridControl.GetText();
    m_myGridControl.SetCol(m_Leg->FindEntry('I'));
	csClino=m_myGridControl.GetText();
	m_myGridControl.SetCol(0);
	csFrom=m_myGridControl.GetText();
	m_myGridControl.SetCol(1);
	csTo=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('L'));
	csLeft=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('R'));
	csRight=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('U'));
	csUp=m_myGridControl.GetText();
	m_myGridControl.SetCol(m_Leg->FindEntry('D'));
	csDown=m_myGridControl.GetText();
	if (m_Leg->HasBacksights())
	{
		m_myGridControl.SetCol(m_Leg->FindEntry('A')+1);
		csCompass2=m_myGridControl.GetText();
		m_myGridControl.SetCol(m_Leg->FindEntry('I')+1);
		csClino2=m_myGridControl.GetText();
	}

	if (csCompass.IsEmpty() &&
		csCompass2.IsEmpty() &&
		csClino.IsEmpty() &&
		csClino2.IsEmpty() &&
		csUp.IsEmpty() &&
		csDown.IsEmpty() &&
		csLeft.IsEmpty() &&
		csRight.IsEmpty() &&
		csFrom.IsEmpty() &&
		csTo.IsEmpty())
	{
		if (pPrevious!=NULL)
		{
			GetComments(pPrevious);
		}
		return TRUE;	//no problem.  Just an empty shot
	}

	if ((csCompass.IsEmpty() && csCompass2.IsEmpty()) ||
		(csClino.IsEmpty() && csClino2.IsEmpty()) ||
		csFrom.IsEmpty() ||
		csTo.IsEmpty() ||
		csLength.IsEmpty())
	{
		//From without other readings could be a wall dimension defining shot.
		if (csFrom.IsEmpty() || !csTo.IsEmpty() || !csCompass.IsEmpty() || !csClino.IsEmpty())
		{
			LegGridMessageBox(IDS_INCOMPLETESHOT,iLine+1,1);
			return FALSE;
		}
    }

	if (strstr(csFrom," ")!=NULL)
	{
		LegGridMessageBox(IDS_BADSTATIONNAME,iLine+1,0);
		return FALSE;
	}
	if (strstr(csTo," ")!=NULL)
	{
		LegGridMessageBox(IDS_BADSTATIONNAME,iLine+1,1);
		return FALSE;
	}								

	float fUp,fDown,fLeft,fRight;		//validity check of field formats
	float fLength,fCompass,fClino,fCompass2,fClino2;
	if (!FloatFromString(csUp,fUp))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('U'));
		return FALSE;
	}
	if (!FloatFromString(csDown,fDown))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('D'));
		return FALSE;
	}
	if (!FloatFromString(csLeft,fLeft))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('L'));
		return FALSE;
	}
	if (!FloatFromString(csRight,fRight))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('R'));
	}
	if (!FloatFromString(csLength,fLength))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('T'));
		return FALSE;
	}
	if (!FloatFromString(csCompass,fCompass))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('A')+1);
		return FALSE;
	}
	if (m_Leg->GetEuroDegrees())
	{
		if (fCompass!=NAN && (fCompass<-400.0f || fCompass>400.0f) )
		{
			LegGridMessageBox(IDS_COMPASSRANGEEURO,iLine+1,m_Leg->FindEntry('A'));
			return FALSE;
		}
		if (fCompass!=NAN)
		{
			fCompass=fCompass*360.0f/400.0f;	//convert.  Always store in standard units
		}
	}
	else
	{
		if (fCompass!=NAN && (fCompass<-360.0f || fCompass>360.0) )
		{
			LegGridMessageBox(IDS_COMPASSRANGE,iLine+1,m_Leg->FindEntry('A'));
			return FALSE;
		}
	}
	if (!FloatFromString(csCompass2,fCompass2))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('A')+1);
		return FALSE;
	}
	if (m_Leg->GetEuroDegrees())
	{
		if (fCompass2!=NAN && (fCompass2<-400.0f || fCompass2>400.0f) )
		{
			LegGridMessageBox(IDS_COMPASSRANGEEURO,iLine+1,m_Leg->FindEntry('A')+1);
			return FALSE;
		}
		if (fCompass2!=NAN)
		{
			fCompass2=fCompass2*360.0f/400.0f;	//convert.  Always store in standard units
		}
	}
	else
	{
		if (fCompass2!=NAN && (fCompass2<-360.0f || fCompass2>360.0) )
		{
			LegGridMessageBox(IDS_COMPASSRANGE,iLine+1,m_Leg->FindEntry('A')+1);
			return FALSE;
		}
	}
	if (fCompass!=NAN && fCompass2!=NAN)
	{
		float fDiff;
		if (fCompass>fCompass2)
		{
			fDiff=fCompass-fCompass2;
		}
		else
		{
			fDiff=fCompass2-fCompass;
		}
		//Difference should ideally be 180.
		if (fDiff<160.0f || fDiff>200.0f)
		{
			LegGridMessageBox(IDS_COMPASSMISMATCH,iLine+1,m_Leg->FindEntry('A'));
			return FALSE;
		}
	}
	if (!FloatFromString(csClino,fClino))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('I'));
		return FALSE;
	}
	if (m_Leg->GetEuroDegrees())
	{
		if (fClino!=NAN && (fClino<-100.0f || fClino>100.0f))
		{
			LegGridMessageBox(IDS_CLINORANGEEURO,iLine+1,m_Leg->FindEntry('I'));
			return FALSE;
		}
		if (fClino!=NAN)
		{
			fClino=fClino*90.0f/100.0f;
		}
	}
	else
	{
		if (fClino!=NAN && (fClino<-90.0f || fClino>90.0f))
		{
			LegGridMessageBox(IDS_CLINORANGE,iLine+1,m_Leg->FindEntry('I'));
			return FALSE;
		}
	}
	if (!FloatFromString(csClino2,fClino2))
	{
		LegGridMessageBox(IDS_ENTERVALIDNUMBER,iLine+1,m_Leg->FindEntry('I')+1);
		return FALSE;
	}
	if (m_Leg->GetEuroDegrees())
	{
		if (fClino2!=NAN && (fClino2<-100.0f || fClino2>100.0f))
		{
			LegGridMessageBox(IDS_CLINORANGEEURO,iLine+1,m_Leg->FindEntry('I')+1);
			return FALSE;
		}
		if (fClino2!=NAN)
		{
			fClino2=fClino2*90.0f/100.0f;
		}
	}
	else
	{
		if (fClino2!=NAN && (fClino2<-90.0f || fClino2>90.0f))
		{
			LegGridMessageBox(IDS_CLINORANGE,iLine+1,m_Leg->FindEntry('I')+1);
			return FALSE;
		}
	}
	if (fClino!=NAN && fClino2!=NAN && fClino+fClino2>10.0f)
	{
		LegGridMessageBox(IDS_BACKSIGHTMISMATCH,iLine+1,m_Leg->FindEntry('I'));
		return FALSE;
	}

	CStandardSurveyShot *pShot=new CStandardSurveyShot(m_Leg);
	pShot->SetStationNames(csFrom,csTo);
	pShot->SetReadings(fLength,fCompass,fClino,fCompass2,fClino2);
	pShot->SetUDLR(fUp,fDown,fLeft,fRight);

	GetFlags(pShot);
	GetComments(pShot);
	*ppShot=pShot;
	return TRUE;
}

void CLegGridPage::OnInsertLine()
{
	CWaitCursor w;
	int iRow=m_myGridControl.GetRow();
	int iCol=m_myGridControl.GetCol();
	for (int y=MAX_GRID_ENTRIES;y>iRow;y--)
	{
		for (int x=0;x<m_iNumStandardCols+4;x++)
		{
			m_myGridControl.SetCol(x);
			m_myGridControl.SetRow(y-1);
			CString cs=m_myGridControl.GetText();
			m_myGridControl.SetRow(y);
			m_myGridControl.SetText(cs);
		}
	}
	m_myGridControl.SetRow(iRow);
	for (int x=0;x<m_iNumStandardCols+4;x++)
	{
		m_myGridControl.SetCol(x);
		m_myGridControl.SetText("");
	}
	m_myGridControl.SetCol(iCol);
}

void CLegGridPage::OnAutoNumber() 
{
	int iSavedRow=m_myGridControl.GetRow();
	int iSavedCol=m_myGridControl.GetCol();
	int iTopY=m_myGridControl.GetSelStartRow();
	int iBottomY=m_myGridControl.GetSelEndRow();
	int iLeftX=m_myGridControl.GetSelStartCol();
	int iRightX=m_myGridControl.GetSelEndCol();

	char szFromBase[MAX_STATION_NAME_LENGTH];
	char szToBase[MAX_STATION_NAME_LENGTH];
	szFromBase[0]=0;
	szToBase[0]=0;
	if (iTopY==iBottomY)
	{
		AfxMessageBox(IDS_SELECTFILLAREA);
		return;
	}

	int iFromColumn=0;
	int iToColumn=1;
	BOOL bFrom=iFromColumn>=iLeftX&& iFromColumn<=iRightX;
	BOOL bTo=iToColumn>=iLeftX && iToColumn<=iRightX;
	if (bFrom==FALSE && bTo==FALSE)
	{
		AfxMessageBox(IDS_FILLNEEDSFROMANDTO);
		return;
	}
	m_myGridControl.SetCol(iFromColumn);
	m_myGridControl.SetRow(iTopY);
	lstrcpyn(szFromBase,m_myGridControl.GetText(),MAX_STATION_NAME_LENGTH);
	m_myGridControl.SetCol(iToColumn);
	lstrcpyn(szToBase,m_myGridControl.GetText(),MAX_STATION_NAME_LENGTH);

	//Split the From station name to BASE+NUMBER
	int iStrLen=lstrlen(szFromBase);	
	LPSTR szFromDigits=szFromBase;
	for (int k=iStrLen-1;k>=0;k--)
	{
		if (strchr("0123456789",(int)szFromBase[k])==NULL)
		{
			szFromDigits=szFromBase+k+1;
			break;
		}
	}
	int iFromDigit=atoi(szFromDigits);
	szFromDigits[0]=0;

	//Split the To station name to BASE+NUMBER
	iStrLen=lstrlen(szToBase);	
	LPSTR szToDigits=szToBase;
	for (k=iStrLen-1;k>=0;k--)
	{
		if (strchr("0123456789",(int)szToBase[k])==NULL)
		{
			szToDigits=szToBase+k+1;
			break;
		}
	}
	int iToDigit=atoi(szToDigits);
	szToDigits[0]=0;

	if (bFrom && bTo)
	{
		if ((iToDigit!=iFromDigit-1 && iToDigit!=iFromDigit+1) || (lstrcmpi(szToBase,szFromBase)!=0))
		{
			AfxMessageBox(IDS_BADFILLPATTERN);
			return;
		}
	}


	char szWorking[MAX_STATION_NAME_LENGTH+4];
	//Now do all the numbering.
	for (int i=iTopY+1;i<=iBottomY;i++)
	{
		m_myGridControl.SetRow(i);
		if (bFrom)
		{
			m_myGridControl.SetCol(iFromColumn);
			iFromDigit++;
			wsprintf(szWorking,"%s%i",szFromBase,iFromDigit);
			m_myGridControl.SetText(szWorking);
		}
		if (bTo)
		{
			m_myGridControl.SetCol(iToColumn);
			iToDigit++;
			wsprintf(szWorking,"%s%i",szToBase,iToDigit);
			m_myGridControl.SetText(szWorking);
		}
	}
}


void CLegGridPage::LegGridMessageBox(int iMessageId,int iRow,int iCol)
{
	m_myGridControl.SetSelStartRow(iRow);
	m_myGridControl.SetSelEndRow(iRow);
	m_myGridControl.SetSelStartCol(iCol);
	m_myGridControl.SetSelEndCol(iCol);
	AfxMessageBox(iMessageId);
	m_myGridControl.SetFocus();
}

void CLegGridPage::OnUndobutton() 
{
	m_myGridControl.OnUndo();
	
}
