#include "stdafx.h"
#include "onstation.h"
#include "filehelper.h"
#include "stdio.h"
#include "string.h"
#include "surveyshot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CFileHelper::ReadString(LPSTR szBuffer,int iSize)
{
	return (m_Archive->ReadString(szBuffer,iSize)!=NULL);
}

//This function reads the current line assuming a format of TOKEN=VALUE.
//The Token portion is stored in the string m_szToken.
//The Value portion is stored in the string m_szValue.
//The input is read until a \r\n is encountered or until our raw
//input buffer overflows.  If the last character of the input line
//is a \ we assume a continuation of the input onto the next line.
//in this case, we read the next line as well and add it to the m_szValue
//portion of the input.
void CFileHelper::ReadCurrentLine(BOOL bEndOfFileError)
{
start:
//	TRACE("Reading line %i\n",m_iCurrentLineNumber);
	if (m_bBufferedLine)
	{
		m_bBufferedLine=FALSE;
		return;
	}
	m_iCurrentLineNumber++;
	if (!ReadString(m_szBuffer,MAX_LINE_SIZE))
	{
		if (bEndOfFileError)
		{
			AfxMessageBox(IDS_ENDOFFILEERROR);
		}
		throw "end of file error";
	}
	if (m_szBuffer[0]==0)	//empty line.  just skip it
	{
		goto start;
	}
	
	//Find the = to part of the string.
	for (int i=0;i<MAX_LINE_SIZE;i++)
	{
		if (m_szBuffer[i]==0)
		{
			CString CS;
			CS.GetBuffer(512);
			m_szBuffer[450]=0;	//stop overflow
			CS.Format(IDS_BADLINEFORMAT,m_iCurrentLineNumber,m_szBuffer);
			AfxMessageBox(CS);
			throw "BadFormatError";
		}
		if (m_szBuffer[i]=='=')
		{
			m_szBuffer[i]=0;
			m_szValue=m_szBuffer+i+1;
			for (int j=i+1;j<MAX_LINE_SIZE+1;j++)
			{
				if (m_szBuffer[j]==0)
				{
					if (m_szBuffer[j-1]=='//')
					{
						//line continues.  Read next line and append it.
						AppendLines(j);
						return;
					}
					return;
				}
			}
			return;
		}
	}
	CString CS;
	CS.GetBuffer(512);
	m_szBuffer[450]=0;	//stop overflow
	CS.Format(IDS_LINETOOLONG,m_iCurrentLineNumber);
	AfxMessageBox(CS);
	throw "Overrun exception";
}

void CFileHelper::AppendLines(int iPosition)
{
	m_iCurrentLineNumber++;
	if (iPosition>=MAX_LINE_SIZE-10)
	{
		CString CS;
		CS.Format(IDS_LINETOOLONG,m_iCurrentLineNumber);
		ReportError(CS);
		throw "Overrun exception";
		return;
	}
	if (ReadString(m_szBuffer+iPosition,MAX_LINE_SIZE-iPosition))
	{
		for (int j=iPosition;j<MAX_LINE_SIZE+1;j++)
		{
			if (m_szBuffer[j]=='0')
			{
				if (m_szBuffer[j-1]=='\\')
				{
					AppendLines(j);
				}
				return;
			}
		}
		CString CS;
		CS.Format(IDS_LINETOOLONG,m_iCurrentLineNumber);
		ReportError(CS);
		throw "Overrun exception";
	}
	return;
}


//ASSUMES THAT THE CURRENT LINE IS THE BEGIN BLOCK.
//This is a little bit harder then it looks because these
//things can be nested.  We use this function recursively
//to take care of that.
BOOL CFileHelper::ReadToEndOfScope()
{
	CString szBlockToken=m_szBuffer;
	CString szBlockValue=m_szValue;

restart:	
	ReadCurrentLine();
	if (lstrcmpi(m_szBuffer,"Begin")==0 || lstrcmpi(m_szBuffer,"ProprietaryExtension")==0)
	{
		if (!ReadToEndOfScope())
		{
			return FALSE;
		}
		goto restart;
	}
	if (lstrcmpi(m_szBuffer,"End")==0)
	{
		if (lstrcmpi(szBlockToken,"Begin")!=0)
		{
			CString CS;
			CS.Format(IDS_MISMATCHEDEND,m_iCurrentLineNumber,m_szBuffer,m_szValue);
			ReportError(CS);
			return FALSE;
		}
		if (lstrcmpi(szBlockValue,m_szValue)==0)
		{
			return TRUE;
		}
		else
		{
			CString CS;
			CS.Format(IDS_WRONGEND,szBlockToken,szBlockValue,m_szBuffer,m_szValue);
			ReportError(CS);
			return FALSE;
		}
	}
	if (lstrcmpi(m_szBuffer,"ProprietaryEnd")==0)
	{
		if (lstrcmpi(szBlockToken,"ProprietaryStart")!=0)
		{
			CString CS;
			CS.Format(IDS_MISMATCHEDEND,m_iCurrentLineNumber,m_szBuffer,m_szValue);
			ReportError(CS);
			return FALSE;
		}
		if (lstrcmpi(szBlockValue,m_szValue)==0)
		{
			return TRUE;
		}
		else
		{
			CString CS;
			CS.Format(IDS_WRONGEND,szBlockToken,szBlockValue,m_szBuffer,m_szValue);
			ReportError(CS);
			return FALSE;
		}
	}
	goto restart; 
}

void CFileHelper::ReportError(LPCSTR szError)
{
	AfxMessageBox(szError);
}

//Reads a token which may have a very large line following it.
//Internally, we use the standard read.
void CFileHelper::ReadPartialToken(LPCSTR szExpectedToken)
{
	ReadTokenString(szExpectedToken);
}

float CFileHelper::ReadAppendedFloat()
{
	//skip whitespace
	while (m_szValue[0]==' ' || m_szValue[0]=='\t')
	{
		m_szValue++;
	}
	if (m_szValue[0]==0)	//empty string:
	{
		return NAN;
	}
	//Look for trailing whitespace
	char szFloat[128];
	int i=0;
	while (m_szValue[0]!='\t' && m_szValue[0]!='\r' && m_szValue[0]!='\n' && m_szValue[0]!=' ' && m_szValue[0]!=0 && i<128)
	{
		szFloat[i]=m_szValue[0];
		m_szValue++;
		i++;
	}
	szFloat[i]=0;
	if (szFloat[0]==0)
	{
		return NAN;
	}
	if (lstrcmpi(szFloat,"NAN")==0)
	{
		return NAN;
	}
	float f;
	if (!FloatFromString(szFloat,f))
	{
		return NAN;
	}
	return f;
}

int CFileHelper::ReadTokenInt(LPCSTR szExpectedToken)
{
restart:
	ReadCurrentLine();
	if (lstrcmpi(szExpectedToken,"ProprietaryExtension")!=0 && lstrcmpi(m_szBuffer,"ProprietaryExtension")==0)
	{
		if (!ReadToEndOfScope())
		{
			throw "extension error";
		}
		goto restart;
	}
	if (!lstrcmpi(szExpectedToken,m_szBuffer)==0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKEN,m_iCurrentLineNumber,szExpectedToken,m_szBuffer,m_szValue);
		ReportError(CS);
		throw "unexpected token error";
	}
	return atoi(m_szValue);
}

float CFileHelper::ReadTokenFloat(LPCSTR szExpectedToken)
{
restart:
	ReadCurrentLine();
	if (lstrcmpi(szExpectedToken,"ProprietaryExtension")!=0 && lstrcmpi(m_szBuffer,"ProprietaryExtension")==0)
	{
		if (!ReadToEndOfScope())
		{
			throw "extension error";
		}
		goto restart;
	}
	if (!lstrcmpi(szExpectedToken,m_szBuffer)==0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKEN,m_iCurrentLineNumber,szExpectedToken,m_szBuffer,m_szValue);
		ReportError(CS);
		throw "unexpected token error";
	}
	//a little bit lazy here.
	if ((m_szValue[0]=='N' || m_szValue[0]=='n') && (m_szValue[1]=='A' || m_szValue[1]=='a'))
	{
		return NAN;
	}
	return (float)atof(m_szValue);
}

DWORD CFileHelper::ReadTokenDate(LPCSTR szExpectedToken)
{
restart:
	ReadCurrentLine();
	if (lstrcmpi(szExpectedToken,"ProprietaryExtension")!=0 && lstrcmpi(m_szBuffer,"ProprietaryExtension")==0)
	{
		if (!ReadToEndOfScope())
		{
			throw "extension error";
		}
		goto restart;
	}
	if (!lstrcmpi(szExpectedToken,m_szBuffer)==0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKEN,m_iCurrentLineNumber,szExpectedToken,m_szBuffer,m_szValue);
		ReportError(CS);
		throw "unexpected token error";
	}
	int iYear,iMonth,iDay;
	if (sscanf(m_szValue,"%i/%i/%i",&iYear,&iMonth,&iDay)!=3)
	{
		return 0xffffffff;
	}
	if (iYear<1000 || iYear>3000 || iMonth<1 || iMonth>12 || iDay<1 || iDay>31)
	{
		return 0xffffffff;
	}
	return MakeMyDate(iYear,iMonth,iDay);
}

void CFileHelper::ReadTokenBegin(LPCSTR szValue)
{
	LPCSTR szData=ReadTokenString("Begin");
	if (lstrcmpi(szValue,szData)!=0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKENBEGIN,m_iCurrentLineNumber,szValue,m_szBuffer);
		ReportError(CS);
		throw "unexpected token error";
	}
}

void CFileHelper::WritePartialToken(LPCSTR szToken)
{
	Write(szToken);
	Write("=");
	m_bAppended=FALSE;;
}

void CFileHelper::AppendFloat(float fValue)
{
	if (m_bAppended)
	{
		Write(" ");
	}
	WriteRawFloat(fValue);
	m_bAppended=TRUE;
}
void CFileHelper::WriteEndOfLine()
{
	Write("\r\n");
}

void CFileHelper::ReadTokenEnd(LPCSTR szValue)
{
	LPCSTR szData=ReadTokenString("End");
	if (lstrcmpi(szValue,szData)!=0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKENEND,m_iCurrentLineNumber,szValue,m_szBuffer);
		ReportError(CS);
		throw "unexpected token error";
	}
}


BOOL CFileHelper::CheckProprietaryStart(LPCSTR szValue,BOOL bEndOfFileAllowed)
{
start:
	try {
		LPCSTR szData=GetTokenLookaheadString(FALSE,!bEndOfFileAllowed);
		if (lstrcmpi(szData,"ProprietaryStart")!=0)
		{
			return FALSE;
		}
		if (lstrcmpi(m_szValue,szValue)!=0)
		{
			ReadToEndOfScope();
			//somebody else's proprietary data
			goto start;
		}
	}
	catch (LPCSTR szData)
	{
		if (bEndOfFileAllowed && lstrcmpi(szData,"end of file error")==0)
		{
			return FALSE;
		}
		else
		{
			throw;
		}
	}
	return TRUE;
}

void CFileHelper::CheckProprietaryEnd(LPCSTR szValue)
{
	LPCSTR szData=ReadTokenString("ProprietaryEnd");
	if (lstrcmpi(szValue,szData)!=0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKEN,m_iCurrentLineNumber,szValue,m_szBuffer,m_szValue);
		ReportError(CS);
		throw "unexpected token error";
	}
}


LPCSTR CFileHelper::ReadTokenString(LPCSTR szExpectedToken)
{
restart:
	ReadCurrentLine();
	if (lstrcmpi(szExpectedToken,"ProprietaryExtension")!=0 && lstrcmpi(m_szBuffer,"ProprietaryExtension")==0)
	{
		if (!ReadToEndOfScope())
		{
			throw "extension error";
		}
		goto restart;
	}

	if (!lstrcmpi(szExpectedToken,m_szBuffer)==0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKEN,m_iCurrentLineNumber,szExpectedToken,m_szBuffer,m_szValue);
		ReportError(CS);
		throw "unexpected token error";
	}
	return m_szValue;
}

LPCSTR CFileHelper::GetTokenLookaheadString(BOOL bSkipProprietaryStart,BOOL bEndOfFileError)
{
start:
	ReadCurrentLine(bEndOfFileError);
	m_bBufferedLine=TRUE;
	if (bSkipProprietaryStart && (lstrcmpi(m_szBuffer,"ProprietaryStart")==0))
	{
		ReadToEndOfScope();
		goto start;
	}
	return m_szBuffer;
}

LPCSTR CFileHelper::GetTokenLookaheadValue()
{
	ReadCurrentLine();
	m_bBufferedLine=TRUE;
	return m_szValue;
}


void CFileHelper::ReadStandardShotVersion1(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* clino,float* up,float* down,float* left,float* right,float* compassback,float* clinoback,WORD * wAttributes)
{
	char stringfrom[128];
	char stringto[128];
	char stringlen[128];
	char stringcom[128];
	char stringclino[128];
	char stringup[128];
	char stringdn[128];
	char stringleft[128];
	char stringright[128];
	char stringcomback[128];
	char stringclinoback[128];
	char stringattributes[128];
	ReadCurrentLine();
	if (lstrcmpi(m_szBuffer,"Shot")!=0)
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}

	int iNum=sscanf(m_szValue,"%s %s %s %s %s %s %s %s %s %s %s %s",stringfrom,stringto,stringlen,stringcom,stringclino,stringcomback,stringclinoback,stringup,stringdn,stringleft,stringright,stringattributes);
	if (iNum<12)
	{		   
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	
	WORD wShotType=ParseAttributes(stringattributes);
	if (wShotType==0xffff)
	{
		CString CS;
		CS.Format(IDS_BADFORMATSTRING,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	*wAttributes=wShotType;

	//Parse all the information
	if (!ParseFloat(length,stringlen) || !ParseFloat(compass,stringcom) || !(ParseFloat(clino,stringclino)))
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		ReportError(CS);
		throw ("Bad shot format");
	}

	if (*length!=NAN)
	{
		if (*clino==NAN && *clinoback==NAN)
		{
			TRACE("CLINO ERROR");
			*clino=0.0f;
		}
		if (*compass==NAN && *compassback==NAN)
		{
			TRACE("COMPASS ERROR");
			*compass=0.0f;
		}
	}
	

	ParseFloat(up,stringup);
	ParseFloat(down,stringdn);
	ParseFloat(left,stringleft);
	ParseFloat(right,stringright);
	ParseFloat(compassback,stringcomback);
	ParseFloat(clinoback,stringclinoback);
	ASSERT(lstrlen(szFrom)<MAX_STATION_NAME_LENGTH);
	lstrcpyn(szFrom,stringfrom,MAX_STATION_NAME_LENGTH);
	//From and To equal means that we have a wall only
	//shot
	if (lstrcmpi(stringfrom,stringto)==0)
	{
		lstrcpy(szTo,"");
	}
	else
	{
		ASSERT(lstrlen(szTo)<MAX_STATION_NAME_LENGTH);
		lstrcpyn(szTo,stringto,MAX_STATION_NAME_LENGTH);
	}
}


void CFileHelper::ReadCaveDiveShotVersion1(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* depth,float* up,float* down,float* left,float* right,WORD * wAttributes)
{
	char szFromBuffer[128];
	char szToBuffer[128];
	char szLength[128];
	char szCompass[128];
	char szDepth[128];
	char szUp[128];
	char szDown[128];
	char szLeft[128];
	char szRight[128];
	char szAttributes[128];

	ReadCurrentLine();
	if (lstrcmpi(m_szBuffer,"DiveShot")!=0)
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}

	int iNum=sscanf(m_szValue,"%s %s %s %s %s %s %s %s %s %s",szFromBuffer,szToBuffer,szLength,szCompass,szDepth,szUp,szDown,szLeft,szRight,szAttributes);
	if (iNum<10)
	{		   
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szBuffer);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	
	WORD wShotType=ParseAttributes(szAttributes);
	if (wShotType==0xffff)
	{
		CString CS;
		CS.Format(IDS_BADFORMATSTRING,m_iCurrentLineNumber,m_szBuffer);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	*wAttributes=wShotType;
	//Parse all the information
	if (!ParseFloat(length,szLength) || !ParseFloat(compass,szCompass) || !(ParseFloat(depth,szDepth)))
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szBuffer);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	ParseFloat(up,szUp);
	ParseFloat(down,szDown);
	ParseFloat(left,szLeft);
	ParseFloat(right,szRight);
	ASSERT(lstrlen(szFromBuffer)<MAX_STATION_NAME_LENGTH);
	lstrcpyn(szFrom,szFromBuffer,MAX_STATION_NAME_LENGTH);
	//From and To equal means that we have a wall only
	//shot
	ASSERT(lstrlen(szToBuffer)<MAX_STATION_NAME_LENGTH);
	if (lstrcmpi(szFromBuffer,szToBuffer)==0)
	{
		lstrcpy(szTo,"");
	}
	else
	{
		lstrcpyn(szTo,szToBuffer,MAX_STATION_NAME_LENGTH);
	}
}
void CFileHelper::ReadWalls(LPSTR szStation,float* fCompass,float* fClino,float* fUp,float* fDown,float * fLeft,float *fRight)
{
	char stringfrom[128];
	char stringcom[128];
	char stringclino[128];
	char stringup[128];
	char stringdown[128];
	char stringleft[128];
	char stringright[128];
	ReadCurrentLine();

	ASSERT(lstrcmpi(m_szBuffer,"Wall")==0);

	int iNum=sscanf(m_szValue,"%s %s %s %s %s %s %s",stringfrom,stringcom,stringclino,stringup,stringdown,stringleft,stringright);
	if (iNum<7)
	{		   
		CString CS;
		CS.Format(IDS_BADWALLFORMAT,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	
	//Parse all the information
	ParseFloat(fCompass,stringcom);
	ParseFloat(fClino,stringclino);
	ParseFloat(fUp,stringup);
	ParseFloat(fDown,stringdown);
	ParseFloat(fLeft,stringleft);
	ParseFloat(fRight,stringright);
	ASSERT(lstrlen(stringfrom)<MAX_STATION_NAME_LENGTH);
	lstrcpyn(szStation,stringfrom,MAX_STATION_NAME_LENGTH);
}
void CFileHelper::ReadStandardShotVersion2(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* clino,float* compassback,float* clinoback,WORD * wAttributes)
{
	char stringfrom[128];
	char stringto[128];
	char stringlen[128];
	char stringcom[128];
	char stringclino[128];
	char stringcomback[128];
	char stringclinoback[128];
	char stringattributes[128];
	ReadCurrentLine();
	if (lstrcmpi(m_szBuffer,"Shot")!=0)
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}

	int iNum=sscanf(m_szValue,"%s %s %s %s %s %s %s %s",stringfrom,stringto,stringlen,stringcom,stringclino,stringcomback,stringclinoback,stringattributes);
	if (iNum<8)
	{		   
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	
	WORD wShotType=ParseAttributes(stringattributes);
	if (wShotType==0xffff)
	{
		CString CS;
		CS.Format(IDS_BADFORMATSTRING,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	*wAttributes=wShotType;

	//Parse all the information
	if (!ParseFloat(length,stringlen) || !ParseFloat(compass,stringcom) || !(ParseFloat(clino,stringclino)))
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		ReportError(CS);
		throw ("Bad shot format");
	}

	if (*length!=NAN)
	{
		if (*clino==NAN && *clinoback==NAN)
		{
			TRACE("CLINO ERROR");
			*clino=0.0f;
		}
		if (*compass==NAN && *compassback==NAN)
		{
			TRACE("COMPASS ERROR");
			*compass=0.0f;
		}
	}
	
	ParseFloat(compassback,stringcomback);
	ParseFloat(clinoback,stringclinoback);
	ASSERT(lstrlen(szFrom)<MAX_STATION_NAME_LENGTH);
	lstrcpyn(szFrom,stringfrom,MAX_STATION_NAME_LENGTH);
	//From and To equal means that we have a wall only
	//shot
	if (lstrcmpi(stringfrom,stringto)==0)
	{
		lstrcpy(szTo,"");
	}
	else
	{
		ASSERT(lstrlen(szTo)<MAX_STATION_NAME_LENGTH);
		lstrcpyn(szTo,stringto,MAX_STATION_NAME_LENGTH);
	}
}


void CFileHelper::ReadCaveDiveShotVersion2(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* depth,WORD * wAttributes)
{
	char szFromBuffer[128];
	char szToBuffer[128];
	char szLength[128];
	char szCompass[128];
	char szDepth[128];
	char szAttributes[128];

	ReadCurrentLine();
	if (lstrcmpi(m_szBuffer,"DiveShot")!=0)
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szValue);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}

	int iNum=sscanf(m_szValue,"%s %s %s %s %s %s",szFromBuffer,szToBuffer,szLength,szCompass,szDepth,szAttributes);
	if (iNum<6)
	{		   
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szBuffer);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	
	WORD wShotType=ParseAttributes(szAttributes);
	if (wShotType==0xffff)
	{
		CString CS;
		CS.Format(IDS_BADFORMATSTRING,m_iCurrentLineNumber,m_szBuffer);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	*wAttributes=wShotType;
	//Parse all the information
	if (!ParseFloat(length,szLength) || !ParseFloat(compass,szCompass) || !(ParseFloat(depth,szDepth)))
	{
		CString CS;
		CS.Format(IDS_BADSHOTFORMAT,m_iCurrentLineNumber,m_szBuffer);
		AfxMessageBox(CS);
		throw ("Bad shot format");
	}
	ASSERT(lstrlen(szFromBuffer)<MAX_STATION_NAME_LENGTH);
	lstrcpyn(szFrom,szFromBuffer,MAX_STATION_NAME_LENGTH);
	//From and To equal means that we have a wall only
	//shot
	ASSERT(lstrlen(szToBuffer)<MAX_STATION_NAME_LENGTH);
	if (lstrcmpi(szFromBuffer,szToBuffer)==0)
	{
		lstrcpy(szTo,"");
	}
	else
	{
		lstrcpyn(szTo,szToBuffer,MAX_STATION_NAME_LENGTH);
	}
}
  
WORD CFileHelper::ParseAttributes(LPCSTR szData)
{
	if (szData[0]!='(')
	{
		return 0xffff;	//bad format
	}
	szData++;
	WORD wReturn=0;
	while (szData[0]!=0)
	{
		switch (szData[0])
		{
		case 'X':
			wReturn|=SHOT_EXCLUDE_FROM_ALL_PROCESSING;
			break;
		case 'S':
			wReturn|=SHOT_SURFACE;
			break;
		case 'C':
			wReturn|=SHOT_EXCLUDE_FROM_CLOSING;
			break;
		case 'L':
			wReturn|=SHOT_EXCLUDE_FROM_LENGTH_TOTALS;
			break;
		case 'P':
			wReturn|=SHOT_EXCLUDE_FROM_PLOTTING;
			break;
		case 'Y':
			wReturn|=SHOT_SPLAY_SHOT;
			break;
		default:
			break;	//ignore anything else
		}
		szData++;
	}
	return wReturn;
}


CFileHelper::CFileHelper(CArchive * ar)
{
	m_Archive=ar;
	m_iCurrentLineNumber=0;
	m_bBufferedLine=FALSE;
	m_fVersion=2.0;
}

CFileHelper::~CFileHelper()
{
}

void CFileHelper::Write(LPCSTR szBuffer)
{
	m_Archive->Write((LPVOID)szBuffer,lstrlen(szBuffer));
}

void CFileHelper::WriteRawFloat(float f)
{
	char szBuffer[16];
	if (f!=NAN)
	{
		sprintf(szBuffer,"%.2f",f);
		Write(szBuffer);
	}
	else
	{
		Write("NAN");
	}
}

void CFileHelper::WriteToken(LPCSTR szToken,float f)
{
	Write(szToken);
	Write("=");
	WriteRawFloat(f);
	Write("\r\n");
}
		

void CFileHelper::WriteToken(LPCSTR szToken,LPCSTR szValue)
{
	Write(szToken);
	Write("=");
	Write(szValue);
	Write("\r\n");
}

void CFileHelper::WriteToken(LPCSTR szToken,int iValue)
{
	Write(szToken);
	Write("=");

	char szBuffer[16];
	wsprintf(szBuffer,"%i",iValue);
	Write(szBuffer);
	Write("\r\n");
}

void CFileHelper::WriteTokenDate(LPCSTR szToken,DWORD dwDate)
{
	Write(szToken);
	Write("=");

	if (dwDate!=0xffffffff)
	{
		char szBuffer[32];
		wsprintf(szBuffer,"%i/%i/%i",GetYear(dwDate),GetMonth(dwDate),GetDay(dwDate));
		Write(szBuffer);
	}
	Write("\r\n");
}

void CFileHelper::WriteWalls(LPCSTR szFrom,float fCompass,float fClino,float up,float down,float left,float right)
{
	if (up==NAN && down==NAN && left==NAN && right==NAN)
	{
		return;
	}
	Write("Wall=");
	Write(szFrom);
	Write(" ");
	WriteRawFloat(fCompass);
	Write(" ");
	WriteRawFloat(fClino);
	Write(" ");
	WriteRawFloat(up);
	Write(" ");
	WriteRawFloat(down);
	Write(" ");
	WriteRawFloat(left);
	Write(" ");
	WriteRawFloat(right);
	Write("\r\n");
}
void CFileHelper::WriteStandardShot(LPCSTR szFrom,LPCSTR szTo,float length,float compass,float clino,float clinoback,float compassback,WORD wAttributes)
{
	//An endpoint of the leg
	if (szTo[0]==0)
	{
		return;
	}
	Write("Shot=");
	Write(szFrom);
	Write(" ");
	Write(szTo);
	Write(" ");
	WriteRawFloat(length);
	Write(" ");
	WriteRawFloat(compass);
	Write(" ");
	WriteRawFloat(clino);
	Write(" ");
	WriteRawFloat(clinoback);
	Write(" ");
	WriteRawFloat(compassback);
	Write(" (");
	if (wAttributes&SHOT_SURFACE)
	{
		Write("S");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_CLOSING)
	{
		Write("C");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_LENGTH_TOTALS)
	{
		Write("L");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_ALL_PROCESSING)
	{
		Write("X");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_PLOTTING)
	{
		Write("P");
	}
	if (wAttributes&SHOT_SPLAY_SHOT)
	{
		Write("Y");
	}
	Write(")\r\n");
}

void CFileHelper::WriteTokenColor(LPCSTR szToken,COLORREF crColor)
{
	Write(szToken);
	Write("=");
	if (crColor==COLOR_INACTIVE)
	{
		Write("-2");
	}
	else if (crColor==COLOR_INVISIBLE)
	{
		Write("-1");
	}
	else
	{
		char szBuffer[20];
		wsprintf(szBuffer,"%i %i %i",GetRValue(crColor),GetGValue(crColor),GetBValue(crColor));
		Write(szBuffer);
	}
	Write("\r\n");
}

COLORREF CFileHelper::ReadTokenColor(LPCSTR szExpectedToken)
{
restart:
	ReadCurrentLine();
	if (lstrcmpi(szExpectedToken,"ProprietaryExtension")!=0 && lstrcmpi(m_szBuffer,"ProprietaryExtension")==0)
	{
		if (!ReadToEndOfScope())
		{
			throw "extension error";
		}
		goto restart;
	}

	if (!lstrcmpi(szExpectedToken,m_szBuffer)==0)
	{
		CString CS;
		CS.Format(IDS_UNEXPECTEDTOKEN,m_iCurrentLineNumber,szExpectedToken,m_szBuffer,m_szValue);
		ReportError(CS);
		throw "unexpected token error";
	}
	int iRed,iGreen,iBlue;
	int iArgs=sscanf(m_szValue,"%i %i %i",&iRed,&iGreen,&iBlue);
	if (iArgs==1)
	{
		if (iRed==-1)
		{
			return COLOR_INVISIBLE;
		}
		if (iRed==-2)
		{
			return COLOR_INACTIVE;
		}
	}
	if (iRed<0 || iGreen<0 || iBlue<0 || iRed>255 || iGreen>255 || iBlue>255)
	{
		return RGB(255,255,255);	//white;
	}
	return (RGB(iRed,iGreen,iBlue));
}

void CFileHelper::WriteDiveShot(LPCSTR szFrom,LPCSTR szTo,float length,float compass,float fDepth,WORD wAttributes)
{
	Write("DiveShot=");
	Write(szFrom);
	Write(" ");
	if (szTo[0]==0)
	{
		Write(szFrom);
	}
	else
	{
		Write(szTo);
	}
	Write(" ");
	WriteRawFloat(length);
	Write(" ");
	WriteRawFloat(compass);
	Write(" ");
	WriteRawFloat(fDepth);
	Write(" (");
	if (wAttributes&SHOT_SURFACE)
	{
		Write("S");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_CLOSING)
	{
		Write("C");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_LENGTH_TOTALS)
	{
		Write("L");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_ALL_PROCESSING)
	{
		Write("X");
	}
	if (wAttributes&SHOT_EXCLUDE_FROM_PLOTTING)
	{
		Write("P");
	}
	if (wAttributes&SHOT_SPLAY_SHOT)
	{
		Write("Y");
	}
	Write(")\r\n");
}


BOOL CFileHelper::ParseFloat(float* test,LPCSTR szTest)
{
	if (lstrcmpi(szTest,"NAN")==0)
	{
		*test=NAN;
		return TRUE;
	}
	else
	{
		return FloatFromString(szTest,*test);
	}
}

CBufferFileHelper::CBufferFileHelper(LPCSTR szData):CFileHelper(NULL)
{
	m_szReadPointer=szData;
	m_szTextStoring=NULL;
	m_iStoringLength=0;
	m_iStoringCurrent=0;
}
CBufferFileHelper::CBufferFileHelper():CFileHelper(NULL)
{
	//I used to concatenate CStrings for this but it
	//was too slow because the CString would have to
	//realloc every time it grew which could be millions
	//of times in some cases.
	m_szReadPointer=NULL;
	m_szTextStoring=(LPSTR)malloc(10000);
	m_iStoringLength=10000;
	m_iStoringCurrent=0;
}

CBufferFileHelper::~CBufferFileHelper()
{
	if (m_szTextStoring!=NULL)
	{
		free(m_szTextStoring);
	}
}

BOOL CBufferFileHelper::ReadString(LPSTR szData,int iSize)
{
	if (m_szReadPointer==NULL)
	{
		return FALSE;
	}
	int iCount=0;
	while ((m_szReadPointer[iCount]!='\n') && (m_szReadPointer[iCount]!='\0'))
	{
		iCount++;
	}
	lstrcpyn(szData,m_szReadPointer,min(iSize,iCount));
	if (m_szReadPointer[iCount]==0)
	{
		m_szReadPointer=NULL;
	}
	else
	{
		m_szReadPointer=m_szReadPointer+iCount+1;
	}

	return TRUE;
}

void CBufferFileHelper::Write(LPCSTR szData)
{
	int iExtra=lstrlen(szData);
	if (m_iStoringCurrent+iExtra>=m_iStoringLength)
	{
		m_szTextStoring=(LPSTR)realloc((LPVOID)m_szTextStoring,m_iStoringLength+10000);
		m_iStoringLength+=10000;
	}
	lstrcpy(m_szTextStoring+m_iStoringCurrent,szData);
	m_iStoringCurrent+=iExtra;
}
