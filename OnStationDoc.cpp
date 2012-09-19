// OnStationDoc.cpp : implementation of the COnStationDoc class
//

#include "stdafx.h"
#include "OnStation.h"
#include "OnStationDoc.h"
#include "realfolder.h"
#include "surfacedata.h"
#include "surveyleg.h"
#include "colortool.h"
#include "nodetree.h"
#include "saveformattedtext.h"
#include "pagelayoutpreferences.h"
#include "overalldataview.h"
#include "mainfrm.h"
#include "filehelper.h"
#include "onstationview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern COnStationApp theApp;
COnStationDoc * pDocument_G=NULL;
extern COnStationView * pView_G;
extern CSettings * pSettings_G;
/////////////////////////////////////////////////////////////////////////////
// COnStationDoc

IMPLEMENT_DYNCREATE(COnStationDoc, CDocument)

BEGIN_MESSAGE_MAP(COnStationDoc, CDocument)
	//{{AFX_MSG_MAP(COnStationDoc)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_FILE_EXPORTFORMATTEDTEXT, OnFileSaveFormattedText)
	ON_COMMAND(ID_INSERT_FILE,OnInsertFile)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SENDMAIL,OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SENDMAIL,OnUpdateFileSendMail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnStationDoc construction/destruction

COnStationDoc::COnStationDoc()
{
#ifdef _DEBUG
	ASSERT(AfxCheckMemory());
#endif
	pDocument_G=this;
	m_SurveyDataFolder=new CRealFolder(this);
	CString CS;
	CS.LoadString(IDS_SURVEYROOT);
	m_SurveyDataFolder->SetName(CS);
	m_SurfaceData=new CSurfaceData(this);
	m_NodeTree=new CNodeTree(this);
#ifdef _DEBUG
	ASSERT(AfxCheckMemory());
#endif
}

COnStationDoc::~COnStationDoc()
{
#ifdef _DEBUG
	ASSERT(AfxCheckMemory());
#endif
	delete m_SurveyDataFolder;
	m_SurveyDataFolder=NULL;
	delete m_SurfaceData;
	m_SurfaceData=NULL;
	delete m_NodeTree;
	m_NodeTree=NULL;
}



void COnStationDoc::DeleteContents()
{
	pSettings_G->ResetContents();
	CMainFrame * pFrame=dynamic_cast<CMainFrame *>(AfxGetMainWnd());
	if (pFrame)
	{
		pFrame->m_dataView.RemoveAll();
	}
	m_SurveyDataFolder->DeleteContents();
	m_NodeTree->DeleteContents();
	m_SurfaceData->DeleteContents();
	if (pView_G!=NULL)
	{
		pView_G->DeleteContents();
	}
	CString CS;
	CS.LoadString(IDS_SURVEYROOT);
	m_SurveyDataFolder->SetName(CS);
}

/////////////////////////////////////////////////////////////////////////////
// COnStationDoc serialization

void COnStationDoc::Serialize(CArchive& ar)
{
	try
	{
		CFileHelper f(&ar);
		if (ar.IsStoring())
		{
			Write(&f);
		}
		else
		{
			Read(&f,FALSE);
		}
	}
	catch (void *)
	{
		AfxMessageBox(IDS_UNABLETOOPENFILE);
		DeleteContents();		
		CString CS;
		CS.LoadString(IDS_UNTITLED);
		SetTitle(CS);
	}
}					

void COnStationDoc::Write(CFileHelper *h)
{
	h->WriteToken("FileVersion","2.0");	
	h->WriteToken("Program","On Station 3.0a");
	m_SurveyDataFolder->Write(h);
	m_SurfaceData->WriteFixedPoints(h);
	m_SurfaceData->Write(h);
	pSettings_G->Write(h);
}




void COnStationDoc::Read(CFileHelper * h,BOOL bInsertFile)
{
	float fDataVersion=h->ReadTokenFloat("FileVersion");
	h->m_fVersion=fDataVersion;
	LPCSTR szProgram=h->ReadTokenString("Program");
	if (lstrcmpi(szProgram,"On Station")==0)
	{
		AfxMessageBox(IDS_OLDFORMATUSETRANSLATOR);
		return;
	}
	if (fDataVersion>CURRENT_VERSION)
	{
		AfxMessageBox(IDS_FILEOPENLATERVERSION);
	}
	if (bInsertFile)
	{
		CRealFolder * pNew=new CRealFolder(this);
		pNew->Read(h);
		pNew->SetParentFolder(m_SurveyDataFolder,NULL);
	}
	else
	{
		m_SurveyDataFolder->Read(h);
		m_SurfaceData->ReadFixedPoints(h);
		m_SurfaceData->ReadSurface(h);
		pSettings_G->Read(h);
		if (pView_G!=NULL)
		{
			pView_G->PostMessage(WM_COMMAND,ID_DRAWMODE_COLORSCHEME1,0l);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// COnStationDoc diagnostics

#ifdef _DEBUG
void COnStationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COnStationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



void COnStationDoc::OnFileImport()
{
	if (WinExec("importapp.exe",SW_SHOW)==ERROR_FILE_NOT_FOUND)
	{
		AfxMessageBox(IDS_NO_IMPORT_EXE);
	}
}


void COnStationDoc::OnFileSaveFormattedText() 
{
	CSaveFormattedText PSD(this);
	PSD.DoModal();
}


BOOL COnStationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	
	return TRUE;
}


void COnStationDoc::OnInsertFile()
{
	CFileDialog fd(TRUE,"*.cdi",NULL,OFN_FILEMUSTEXIST,"Cave Data Interchange|*.cdi||");
	if (fd.DoModal()==IDOK)
	{
		try
		{
			CString Cs=fd.GetPathName();
			CFile file(Cs,CFile::modeRead);
			CArchive archive(&file,CArchive::load);
			CFileHelper f(&archive);
			Read(&f,TRUE);
			CMainFrame * pFrame=dynamic_cast<CMainFrame *>(AfxGetMainWnd());
			if (pFrame)
			{
				pFrame->m_dataView.OnChangedDocumentData();
			}
		}
		catch (void *)
		{
			AfxMessageBox(IDS_UNABLETOOPENFILE);
			SetTitle("Untitled");
		}
	}
}
