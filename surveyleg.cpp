//
#include "stdafx.h"
#include "onstation.h"
#include "surveyleg.h"
#include "folder.h"
#include "colortool.h"
#include "settings.h"
#include "filedump.h"
#include "filehelper.h"
#include "node.h"
#include "onstationdoc.h"
#include "settings.h"
#include "comment.h"
#include "query.h"
#include "legquery.h"
#include "nodetree.h"

extern COnStationApp theApp;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CGlobalColorManager Colors_G;
extern CSettings * pSettings_G;
// Reading and writing folders:
//
CSurveyLeg::CSurveyLeg(COnStationDoc *doc):CFolder(doc)
{
    m_bMeters=TRUE;
	m_bBacksights=FALSE;
	for (int i=0;i<TOTAL_COLOR_SCHEMES;i++)
	{
	    m_iMyGlobalColorReferences[i]=RGB(255,255,255);
	}
	m_bEuroDegrees=FALSE;
    m_bRenderThis=TRUE;
	m_bCaveDive=FALSE;
	m_bDiveMeters=FALSE;	//most dives are in feet
    m_fDeclination=0.0f;
    m_fCompass1Error=0.0f;
    m_fClino1Error=0.0f;
	m_fStandardTapeError=0.05f;
	m_fStandardCompassError=2.0f;
	m_fStandardClinoError=2.0f;
    m_fCompass2Error=0.0f;
    m_fClino2Error=0.0f;
	m_fTapeError=0.0f;
	m_fDepthGaugeError=0.0f;
	m_fDepthGaugeStandardError=0.5f;
	m_dwDate=0;
	for (i=0;i<6;i++)
	{
		m_szPersonName[i][0]=0;
		m_szDuty[i][0]=0;
	}
	for (i=0;i<sizeof(m_iColWidth)/sizeof(short);i++)
	{
		m_iColWidth[i]=-1;
	}
	m_szCompass1Name[0]=0;
	m_szCompass2Name[0]=0;
	m_szClino1Name[0]=0;
	m_szClino2Name[0]=0;
	m_szTapeName[0]=0;
	m_szSystem[0]=0;
	m_szDepthGaugeName[0]=0;
	m_szDescription[0]=0;
	GenerateDefaultFieldOrder();
};



BOOL CSurveyLeg::QuerySurveyComments(CPtrArray * result,CQuery * pTypeQuery,CQuery * pCommentQuery)
{
	return m_MyShots.QuerySurveyComments(result,pTypeQuery,pCommentQuery);
}



//Returns TRUE if these things are not different enough to effect the
//calculation of a final system based on this data.
BOOL CSurveyLeg::IsColorSchemeDataSimilar(int First,int Second)
{
	if (m_iMyGlobalColorReferences[First]!=COLOR_INACTIVE && m_iMyGlobalColorReferences[Second]!=COLOR_INACTIVE)
	{
		return TRUE;
	}
	return (m_iMyGlobalColorReferences[First]==m_iMyGlobalColorReferences[Second]);
}

CSurveyLeg::~CSurveyLeg()
{
	DeleteContents();
};


void CSurveyLeg::DeleteShots()
{
	m_MyShots.DeleteContents();
}
void CSurveyLeg::DeleteContents()
{
	DeleteShots();
}

void CSurveyLeg::Read(CFileHelper *f)
{
	try
	{
	//Write out a tag for the survey leg.  This should
	//include all of the leg data, followed by the shots.

		f->ReadTokenBegin("Survey");
		SetName(f->ReadTokenString("SurveyName"));
		m_dwDate=f->ReadTokenDate("SurveyDate");
		if ((m_dwDate==0) || (m_dwDate==0xffffffff))
		{
			m_dwDate=MakeMyDate(1900,1,1);
		}
		lstrcpyn(m_szDescription,f->ReadTokenString("SurveyDescription"),sizeof(m_szDescription));

		m_fDeclination=f->ReadTokenFloat("Declination");
		CString CS=f->ReadTokenString("DataOrder");
		lstrcpyn(m_szFieldOrder,CS,8);
		//Do a minimal sanity check on the data
		for (int i=0;i<7;i++)
		{
			switch (m_szFieldOrder[i])
			{
			case 'T':
			case 'A':
			case 'I':
			case 'U':
			case 'D':
			case 'R':
			case 'L':
				break;
			default:
				GenerateDefaultFieldOrder();
				i=7;
				break;
			}
		}


		LPCSTR szTemp=f->ReadTokenString("LengthUnits");
		if (szTemp[0]=='M')
		{
			m_bMeters=TRUE;
		}
		else
		{
			m_bMeters=FALSE;
		}
		szTemp=f->ReadTokenString("AzimuthUnits");
		if (szTemp[0]=='G')
		{
			m_bEuroDegrees=TRUE;
		}
		else
		{
			m_bEuroDegrees=FALSE;
		}
		
		//other values not used.  Assumed tobe same as previous ones
		szTemp=f->ReadTokenString("InclinationUnits");
		szTemp=f->ReadTokenString("DepthUnits");
		if (szTemp[0]=='M')
		{
			m_bDiveMeters=TRUE;
		}
		else
		{
			m_bDiveMeters=FALSE;
		}

		lstrcpyn(m_szCompass1Name,f->ReadTokenString("FrontCompass"),sizeof(m_szCompass1Name));
		lstrcpyn(m_szCompass2Name,f->ReadTokenString("BackCompass"),sizeof(m_szCompass2Name));
		m_fCompass1Error=f->ReadTokenFloat("FrontCompassCorrection");
		if (m_fCompass1Error==NAN)
		{
			m_fCompass1Error=0.0f;
		}
		m_fCompass2Error=f->ReadTokenFloat("BackCompassCorrection");
		if (m_fCompass2Error==NAN)
		{
			m_fCompass2Error=0.0f;
		}
		m_fStandardCompassError=f->ReadTokenFloat("FrontCompassStandardError");
		if (m_fStandardCompassError==NAN)
		{
			m_fStandardCompassError=2.0f;
		}
		//value not used
		f->ReadTokenFloat("BackCompassStandardError");
		lstrcpyn(m_szClino1Name,f->ReadTokenString("FrontClino"),sizeof(m_szClino1Name));
		lstrcpyn(m_szClino2Name,f->ReadTokenString("BackClino"),sizeof(m_szClino2Name));
		m_fClino1Error=f->ReadTokenFloat("FrontClinoCorrection");
		if (m_fClino1Error==NAN)
		{
			m_fClino1Error=0.0f;
		}
		m_fClino2Error=f->ReadTokenFloat("BackClinoCorrection");
		if (m_fClino2Error==NAN)
		{
			m_fClino2Error=0.0f;
		}
		m_fStandardClinoError=f->ReadTokenFloat("FrontClinoStandardError");
		if (m_fStandardClinoError==NAN)
		{
			m_fStandardClinoError=1.0f;
		}

		//value not used
		f->ReadTokenFloat("BackClinoStandardError");
		lstrcpyn(m_szTapeName,f->ReadTokenString("Tape"),sizeof(m_szTapeName));
		m_fTapeError=f->ReadTokenFloat("TapeCorrection");
		if (m_fTapeError==NAN)
			m_fTapeError=0.0f;
		m_fStandardTapeError=f->ReadTokenFloat("TapeStandardError");
		if (m_fStandardTapeError==NAN)
			m_fStandardTapeError=0.05f;
		lstrcpyn(m_szDepthGaugeName,f->ReadTokenString("DepthGauge"),sizeof(m_szDepthGaugeName));
		m_fDepthGaugeError=f->ReadTokenFloat("DepthCorrection");
		m_fDepthGaugeStandardError=f->ReadTokenFloat("DepthStandardError");

		for (i=0;i<6;i++)
		{
			char szBuffer[16];
			wsprintf(szBuffer,"Person%i",i+1);
			lstrcpyn(m_szPersonName[i],f->ReadTokenString(szBuffer),sizeof(m_szPersonName[i]));
			wsprintf(szBuffer,"Duty%i",i+1);
			lstrcpyn(m_szDuty[i],f->ReadTokenString(szBuffer),sizeof(m_szDuty[i]));;
		}
		m_bBacksights=FALSE;
		BOOL bIsDive=m_MyShots.Read(f,this,&m_bBacksights);
		m_bCaveDive=bIsDive;
		ReadProprietaryStuff(f);
		f->ReadTokenEnd("Survey");
	}
	catch (void *)  //something went wrong so delete everything we just read in.
	{
		DeleteContents();
		throw;
	}
}


void CSurveyLeg::Write(CFileHelper * f)
{
//Write out a tag for the survey leg.  This should
//include all of the leg data, followed by the shots.
	f->WriteToken("Begin","Survey");
	f->WriteToken("SurveyName",GetName());
	f->WriteTokenDate("SurveyDate",m_dwDate);
	f->WriteToken("SurveyDescription",m_szDescription);

	f->WriteToken("Declination",m_fDeclination);
	f->WriteToken("DataOrder",m_szFieldOrder);
	f->WriteToken("LengthUnits",m_bMeters?"M":"F");
	f->WriteToken("AzimuthUnits",m_bEuroDegrees?"G":"D");
	f->WriteToken("InclinationUnits",m_bEuroDegrees?"G":"D");
	f->WriteToken("DepthUnits",m_bDiveMeters?"M":"F");

	f->WriteToken("FrontCompass",m_szCompass1Name);
	f->WriteToken("BackCompass",m_szCompass2Name);
	f->WriteToken("FrontCompassCorrection",m_fCompass1Error);
	f->WriteToken("BackCompassCorrection",m_fCompass2Error);
	f->WriteToken("FrontCompassStandardError",m_fStandardCompassError);
	f->WriteToken("BackCompassStandardError",m_fStandardCompassError);
	f->WriteToken("FrontClino",m_szClino1Name);
	f->WriteToken("BackClino",m_szClino2Name);
	f->WriteToken("FrontClinoCorrection",m_fClino1Error);
	f->WriteToken("BackClinoCorrection",m_fClino2Error);
	f->WriteToken("FrontClinoStandardError",m_fStandardClinoError);
	f->WriteToken("BackClinoStandardError",m_fStandardClinoError);
	f->WriteToken("Tape",m_szTapeName);
	f->WriteToken("TapeCorrection",m_fTapeError);
	f->WriteToken("TapeStandardError",m_fStandardTapeError);
	f->WriteToken("DepthGauge",m_szDepthGaugeName);
	f->WriteToken("DepthCorrection",m_fDepthGaugeError);
	f->WriteToken("DepthStandardError",m_fDepthGaugeStandardError);

	for (int i=0;i<6;i++)
	{
		char szBuffer[16];
		wsprintf(szBuffer,"Person%i",i+1);
		f->WriteToken(szBuffer,m_szPersonName[i]);
		wsprintf(szBuffer,"Duty%i",i+1);
		f->WriteToken(szBuffer,m_szDuty[i]);
	}
	m_MyShots.Write(f);
	WriteProprietaryStuff(f);
	f->WriteToken("End","Survey");
}

void CSurveyLeg::WriteProprietaryStuff(CFileHelper * f)
{
	f->WriteToken("ProprietaryStart","On Station Color Schemes");
	for (int i=0;i<TOTAL_COLOR_SCHEMES;i++)
	{
		char szBuffer[16];
		wsprintf(szBuffer,"Scheme%i",i+1);
		f->WriteTokenColor(szBuffer,m_iMyGlobalColorReferences[i]);
	}
	f->WriteToken("ProprietaryEnd","On Station Color Schemes");

	f->WriteToken("ProprietaryStart","On Station GridEditorSizes");
	char szBuffer[128];
	wsprintf(szBuffer,"%i %i %i %i %i %i %i %i %i %i %i %i %i",
		m_iColWidth[0],m_iColWidth[1],m_iColWidth[2],m_iColWidth[3],m_iColWidth[4],
		m_iColWidth[5],m_iColWidth[6],m_iColWidth[7],m_iColWidth[8],m_iColWidth[9],
		m_iColWidth[10],m_iColWidth[11],m_iColWidth[12],m_iColWidth[13],m_iColWidth[14]);
	f->WriteToken("Widths",szBuffer);
	f->WriteToken("ProprietaryEnd","On Station GridEditorSizes");
}
void CSurveyLeg::ReadProprietaryStuff(CFileHelper * file)
{
	if (!file->CheckProprietaryStart("On Station Color Schemes"))
	{
		return;
	}
	//The last call just did a lookahead.  Now get it
	file->ReadTokenString("ProprietaryStart");

	for (int i=0;i<TOTAL_COLOR_SCHEMES;i++)
	{
		char szBuffer[16];
		wsprintf(szBuffer,"Scheme%i",i+1);
		m_iMyGlobalColorReferences[i]=file->ReadTokenColor(szBuffer);
	}
	file->CheckProprietaryEnd("On Station Color Schemes");
	if (!file->CheckProprietaryStart("GridEditorSizes"))
	{
		return;
	}
	//The last call just did a lookahead.  Now get it
	file->ReadTokenString("On Station GridEditorSizes");
	CString szSizes=file->ReadTokenString("Widths");
	sscanf(szSizes,"%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i\n",
		&m_iColWidth[0],&m_iColWidth[1],&m_iColWidth[2],&m_iColWidth[3],&m_iColWidth[4],
		&m_iColWidth[5],&m_iColWidth[6],&m_iColWidth[7],&m_iColWidth[8],&m_iColWidth[9],
		&m_iColWidth[10],&m_iColWidth[11],&m_iColWidth[12],&m_iColWidth[13],&m_iColWidth[14]);
	file->CheckProprietaryEnd("On Station GridEditorSizes");
}



COLORREF CSurveyLeg::GetAssignedColor(int iColorSchemeIndex)
{
	ASSERT(iColorSchemeIndex<TOTAL_COLOR_SCHEMES);
	return m_iMyGlobalColorReferences[iColorSchemeIndex];
}

COLORREF CSurveyLeg::GetDrawColor(int iColorSchemeIndex)
{
	ASSERT(iColorSchemeIndex<TOTAL_COLOR_SCHEMES);
	if (iColorSchemeIndex!=COLORSCHEME_AGE)
	{
		return m_iMyGlobalColorReferences[iColorSchemeIndex];
	}
	else
	{
		COLORREF crColor=m_iMyGlobalColorReferences[iColorSchemeIndex];
		if (crColor==COLOR_INACTIVE || crColor==COLOR_INVISIBLE)
		{
			return crColor;
		}
		else
		{
			return pSettings_G->GetColorByAge(m_dwDate);
		}
	}
}

COLORREF CSurveyLeg::SetColor(COLORREF crColor,int iColorSchemeIndex,BOOL bRainbow,BOOL * bDisabled,CLegQuery * pLegQuery)
{
	ASSERT(pLegQuery==NULL || pLegQuery->MatchesQuery(this));
	ASSERT(iColorSchemeIndex<TOTAL_COLOR_SCHEMES && iColorSchemeIndex>=0);

	if (m_iMyGlobalColorReferences[iColorSchemeIndex]==COLOR_INACTIVE && crColor!=COLOR_INACTIVE)
	{
		*bDisabled=TRUE;
	}
	if (m_iMyGlobalColorReferences[iColorSchemeIndex]!=COLOR_INACTIVE && crColor==COLOR_INACTIVE)
	{
		*bDisabled=TRUE;
	}
	m_iMyGlobalColorReferences[iColorSchemeIndex]=crColor;
	//In rainbow mode we cycle through all legal colors
	if (bRainbow)
	{
		crColor=CGlobalColorManager::CycleColor(crColor);
	}
	return crColor;
}

void CSurveyLeg::GatherFolderData(CFolderData *FD,int iViewColor,BOOL bClosedView,CLegQuery * pLegQuery)
{
	//We don't count this guy!
	if (m_iMyGlobalColorReferences[iViewColor]==COLOR_INACTIVE)
		return;

	if (pLegQuery!=NULL && !pLegQuery->MatchesQuery(this))
	{
		return;	//does not match query
	}

	FD->iNumSurveys++;

	float fLength=0.0f;
	int iNumShots=m_MyShots.GetSize();
	for (int i=0;i<iNumShots;i++)
	{
		CSurveyShot *shot=m_MyShots.GetAt(i);
		FD->iTotalShots++;
		if (shot->GetShotFlags()&SHOT_EXCLUDE_FROM_ALL_PROCESSING)
		{
			FD->iNumExcludedShots++;
		}
		else
		{
			if (shot->GetShotFlags()&SHOT_EXCLUDE_FROM_LENGTH_TOTALS)
			{
				FD->iNumLengthExcludedShots++;
			}
			if (shot->GetShotFlags()&SHOT_SPLAY_SHOT)
			{
				FD->iNumSplayShots++;
			}
			if (shot->GetShotFlags()&SHOT_SURFACE)
			{
				FD->iNumSurfaceShots++;
				if ((shot->GetShotFlags()&SHOT_EXCLUDE_FROM_LENGTH_TOTALS)==0)
				{
					FD->fTotalShotLengthSurface+=shot->GetCalculationLength();
				}
			}
			else
			{
				FD->iNumCaveShots++;
				if ((shot->GetShotFlags()&SHOT_EXCLUDE_FROM_LENGTH_TOTALS)==0)
				{
					FD->fTotalShotLengthCave+=shot->GetCalculationLength();
					ASSERT(FD->fTotalShotLengthCave<500000.0f && FD->fTotalShotLengthCave>=0.0f);
				}
			}
		}
	}

	//Set namelist stuff which records who surveyed over what
	//period of time
	FD->m_NameList.SetYear(GetYear(m_dwDate));
	for (i=0;i<6;i++)
	{
		if (m_szPersonName[i][0]!=0)
		{
			FD->m_NameList.AddName(m_szPersonName[i],GetYear(m_dwDate));
		}
	}
}


void CSurveyLeg::SaveFormattedText(CFileDumpHelper *FDH,int iColorScheme,BOOL bClosedLoops,BOOL bCoordinatesOnly)
{
	CString szBuffer;
	char szBuffer2[16];
	CString CSFloats;
	szBuffer.GetBuffer(256);
	szBuffer.Format(IDS_SAVEFSURVEY,(LPCSTR)GetName());
	FDH->OutputText(szBuffer);
	szBuffer.Format(IDS_SAVEFSYSTEM,(LPCSTR)m_szSystem);
	FDH->OutputText(szBuffer);
	szBuffer.Format(IDS_SAVEFDESCRIPTION,(LPCSTR)m_szDescription);
	FDH->OutputText(szBuffer);
	if (m_dwDate!=0xffffffff)
	{
		DWordToDate(m_dwDate,szBuffer2);
	}
	else
	{
		szBuffer2[0]=0;
	}
	szBuffer.Format(IDS_SAVEFDATE,szBuffer2);
	FDH->OutputText(szBuffer);
	if (bCoordinatesOnly==FALSE)
	{
		szBuffer.LoadString(IDS_SAVEFTECHNICAL);
		FDH->OutputText(szBuffer);

		if (m_bMeters)
		{
			szBuffer.LoadString(IDS_SAVEFMETERS);
		}
		else
		{
			szBuffer.LoadString(IDS_SAVEFFEET);
		}
		FDH->OutputText(szBuffer);
		FloatToString(CSFloats,m_fDeclination);
		szBuffer.Format(IDS_SAVEFDECLINATION,(LPCSTR)CSFloats);
		FDH->OutputText(szBuffer);
		for (int i=0;i<6;i++)
		{
			szBuffer.Format(IDS_SAVEFNAMEDUTY,m_szPersonName[i],m_szDuty[i]);
			FDH->OutputText(szBuffer);
		}
		FDH->OutputText("\n");

		CString S1,S2,S3,S4,S5,S6,S7;
		if (GetCaveDive())
		{
			szBuffer.LoadString(IDS_SAVEFDIVEHEADING);
			FDH->OutputText(szBuffer);
		}
		else
		{
			szBuffer.LoadString(IDS_SAVEFLEGHEADING);
			FDH->OutputText(szBuffer);
		}
		for (i=0;i<m_MyShots.GetSize();i++)
		{
			if (GetCaveDive())
			{
				CCaveDiveSurveyShot *shot=dynamic_cast<CCaveDiveSurveyShot *>(m_MyShots.GetAt(i));
				ASSERT(shot!=NULL);
				if (shot==NULL)
					return;
				FloatToString(S1,shot->GetLength());
				FloatToString(S2,shot->GetCompass());
				FloatToString(S3,shot->GetDepthAtFromStation());
				FloatToString(S4,shot->GetRawUp());
				FloatToString(S5,shot->GetRawDown());
				FloatToString(S6,shot->GetRawLeft());
				FloatToString(S7,shot->GetRawRight());
				szBuffer.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\r\n",shot->GetFromStationName(),shot->GetToStationName(),(LPCSTR)S1,(LPCSTR)S2,(LPCSTR)S3,(LPCSTR)S4,(LPCSTR)S5,(LPCSTR)S6,(LPCSTR)S7);
		//			wsprintf(szBuffer,"%s  %s  %s  %s  %s  %s  %s  %s  %s  %s\n",shot->GetFromStationName(),shot->GetToStationName(),(LPCSTR)S1,(LPCSTR)S2,(LPCSTR)S3,(LPCSTR)S4,(LPCSTR)S5,(LPCSTR)S6,(LPCSTR)S7);
				FDH->OutputText(szBuffer);
			}
			else
			{
				CStandardSurveyShot *shot=dynamic_cast<CStandardSurveyShot*>(m_MyShots.GetAt(i));
				ASSERT(shot!=NULL);
				if (shot==NULL)
					return;
				FloatToString(S1,shot->GetLength());
				FloatToString(S2,shot->GetCompass());
				FloatToString(S3,shot->GetClino());
				FloatToString(S4,shot->GetRawUp());
				FloatToString(S5,shot->GetRawDown());
				FloatToString(S6,shot->GetRawLeft());
				FloatToString(S7,shot->GetRawRight());
				szBuffer.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\r\n",shot->GetFromStationName(),shot->GetToStationName(),(LPCSTR)S1,(LPCSTR)S2,(LPCSTR)S3,(LPCSTR)S4,(LPCSTR)S5,(LPCSTR)S6,(LPCSTR)S7);
		//			wsprintf(szBuffer,"%s  %s  %s  %s  %s  %s  %s  %s  %s  %s\n",shot->GetFromStationName(),shot->GetToStationName(),(LPCSTR)S1,(LPCSTR)S2,(LPCSTR)S3,(LPCSTR)S4,(LPCSTR)S5,(LPCSTR)S6,(LPCSTR)S7);
				FDH->OutputText(szBuffer);
			}
			CSurveyShot *shot=m_MyShots.GetAt(i);
			for (int c=0;c<shot->m_Comments.GetSize();c++)
			{
				szBuffer.Format("%s:%s\n",shot->m_Comments[c]->GetType(),shot->m_Comments[c]->GetBody());
				FDH->OutputText(szBuffer);
			}
		}
		FDH->OutputText("\r\n");
		CFolderData FD;
		GatherFolderData(&FD,iColorScheme,bClosedLoops,NULL);
		szBuffer.Format(IDS_SAVEFTOTALS,(LPCSTR)GetName());
		FDH->OutputText(szBuffer);
		if (FD.iNumCaveShots==0 && FD.iNumSurfaceShots==0)
		{
			szBuffer.LoadString(IDS_SAVEFNOSHOTS);
			FDH->OutputText(szBuffer);
		}
		else
		{
			CString CS1;
			CString CS2;
			szBuffer.Format(IDS_SAVEFTOTALS2,FD.iNumSurfaceShots,FD.iNumCaveShots);
			FDH->OutputText(szBuffer);
			FloatToString(CS1,FD.fTotalShotLengthSurface);
			FloatToString(CS2,FD.fTotalShotLengthCave);
			szBuffer.Format(IDS_SAVEFTOTALS3,CS1,CS2);
			FDH->OutputText(szBuffer);
		}
		szBuffer.Format("\r\n----------------------------------------------\r\n");
		FDH->OutputText(szBuffer);
	}
	else
	{
		BOOL bMetric=theApp.GetSettings()->IsMetric();
		if (bMetric)
		{
			szBuffer.LoadString(IDS_SAVEFMETERS);
		}
		else
		{
			szBuffer.LoadString(IDS_SAVEFFEET);
		}
		FDH->OutputText(szBuffer);
		for (int i=0;i<m_MyShots.GetSize();i++)
		{
			CSurveyShot *shot=m_MyShots.GetAt(i);
			if (i==0)
			{
				CPosMatrix * pPos=shot->m_FromNode->GetPosition(bClosedLoops);
				szBuffer.Format(IDS_LOCATION,shot->GetFromStationName(),bMetric?(pPos->GetX()):MetersToFeet(pPos->GetX()),bMetric?(pPos->GetY()):MetersToFeet(pPos->GetY()),bMetric?(pPos->GetZ()):MetersToFeet(pPos->GetZ()));
				FDH->OutputText(szBuffer);
				FDH->OutputText("\r\n");
			}
			CPosMatrix * pPos=shot->m_ToNode->GetPosition(bClosedLoops);
			szBuffer.Format(IDS_LOCATION,shot->GetToStationName(),bMetric?(pPos->GetX()):MetersToFeet(pPos->GetX()),bMetric?(pPos->GetY()):MetersToFeet(pPos->GetY()),bMetric?(pPos->GetZ()):MetersToFeet(pPos->GetZ()));
			FDH->OutputText(szBuffer);
			FDH->OutputText("\r\n");
		}
		szBuffer.Format("\r\n----------------------------------------------\r\n");
		FDH->OutputText(szBuffer);
	}
}

//TODO:  Make this a member of CSurveyLeg
int CSurveyLeg::FindEntry(char c) const
{
	int iCounter=2;	//skip from and to
	for (int i=0;i<7;i++)
	{
		if (m_szFieldOrder[i]==c)
		{
			return iCounter;
		}
		iCounter++;
		if (m_szFieldOrder[i]=='A' || m_szFieldOrder[i]=='I')
		{
			if (m_bBacksights)
			{
				iCounter++;
			}
		}
	}
	return -1;
}


void CSurveyLeg::GenerateDefaultFieldOrder()
{
	lstrcpy(m_szFieldOrder,"TAIUDLR");
}

void CSurveyLeg::SetBacksights(BOOL bBacksights)
{
	m_bBacksights=bBacksights;
}


void CSurveyLeg::RefreshLegNumbers()
{
	m_MyShots.DoDeferredOffsetCalculation();
}

void CSurveyLeg::SetCompass1Error(float f)
{
	if (f==NAN)
		f=0.0f;
	if (m_fCompass1Error!=f)
	{
		m_fCompass1Error=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetCompass2Error(float f)
{
	if (f==NAN)
		f=0.0f;
	if (m_fCompass2Error!=f)
	{
		m_fCompass2Error=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetClino1Error(float f)
{
	if (f==NAN)
		f=0.0f;
	if (m_fClino1Error!=f)
	{
		m_fClino1Error=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetClino2Error(float f)
{
	if (f==NAN)
		f=0.0f;
	if (m_fClino2Error!=f)
	{
		m_fClino2Error=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetTapeError(float f)
{
	if (m_fTapeError!=f)
	{
		m_fTapeError=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetStandardTapeError(float f)
{
	if (m_fStandardTapeError!=f)
	{
		m_fStandardTapeError=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetDeclination(float f)
{
	if (m_fDeclination!=f)
	{
		m_fDeclination=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetStandardCompassError(float f)
{
	if (m_fStandardCompassError!=f)
	{
		m_fStandardCompassError=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetStandardClinoError(float f)
{
	if (m_fStandardClinoError!=f)
	{
		m_fStandardClinoError=f;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetDiveMeters(BOOL bDiveMeters)
{
	if (m_bDiveMeters!=bDiveMeters)
	{
		m_bDiveMeters=bDiveMeters;
		RefreshLegNumbers();
	}
}

void CSurveyLeg::SetMeters(BOOL bMeters)
{
	if (m_bMeters!=bMeters)
	{
		m_bMeters=bMeters;
		RefreshLegNumbers();
	}
}

BOOL CSurveyLeg::DoesAnythingMatchTheQuery(CLegQuery *pLegQuery)
{
	return pLegQuery->MatchesQuery(this);
}

BOOL CSurveyLeg::HasDisabledLegs(int iColorScheme)
{
	return m_iMyGlobalColorReferences[iColorScheme]==COLOR_INACTIVE;
}



