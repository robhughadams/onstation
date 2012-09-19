#include "stdafx.h"
#include "onstation.h"
#include "query.h"
#include "surveyshot.h"

#define new DEBUG_NEW

//
//
// The following is the grammer we use for searches
// 
// String=
// String AND String
// String OR String
// String= (String)
// String= Token
// Token= "Text"
// Token= Text
// String= NOT String

// A OR (B AND NOT C)
// 
CQuery::CQuery()
{
	m_bUngotLastToken=FALSE;
	m_MyExpression=NULL;
}

#ifdef _DEBUG
void CQuery::Verify()
{
	LPCSTR szTest[]={"  A   AND B","  ANDcOR  or  ORD","  Hello Bob Hope  ","NOT Jason","((Billy) Bob)","(A or B) AND (C or D)"};

	for (int i=0;i<sizeof(szTest)/sizeof(LPCSTR);i++)
	{
		m_szQueryText=szTest[i];
		m_szWorkingPointer=szTest[i];
		switch (i)
		{
		case 0:
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_AND);
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_EOF);
			break;
		case 1:
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_OR);
			ASSERT(GetNextToken()==QT_TOKEN);
			UngetToken();
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_EOF);
			break;
		case 2:
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_EOF);
			break;
		case 3:
			ASSERT(GetNextToken()==QT_NOT);
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_EOF);
			break;
		case 4:
			ASSERT(GetNextToken()==QT_LBRACE);
			ASSERT(GetNextToken()==QT_LBRACE);
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_RBRACE);
			ASSERT(GetNextToken()==QT_TOKEN);
			ASSERT(GetNextToken()==QT_RBRACE);
			ASSERT(GetNextToken()==QT_EOF);
			break;
		}
	}
	for (i=0;i<sizeof(szTest)/sizeof(LPCSTR);i++)
	{
		BOOL bRet=SetQueryText(szTest[i]);
		ASSERT(bRet==TRUE);
		switch (i)
		{
		case 0:
			ASSERT(MatchesQuery("Oogie AND Boggies")==FALSE);
			ASSERT(MatchesQuery("A B C D E"));
			ASSERT(MatchesQuery("A C D F G")==FALSE);
			break;
		case 1:
			ASSERT(MatchesQuery("MYcORD Hello")==FALSE);
			ASSERT(MatchesQuery("ANDCOR ORD")==TRUE);
			break;
		case 2:
			ASSERT(MatchesQuery("Bob hope said hello to us yesterday")==TRUE);
			ASSERT(MatchesQuery("Godbye Bob, I hope it goes well")==FALSE);
			ASSERT(MatchesQuery("HelloBob, I hOPe")==FALSE);
			break;
		case 3:
			ASSERT(MatchesQuery("Jason was here")==FALSE);
			ASSERT(MatchesQuery("Bob was here")==TRUE);
			ASSERT(MatchesQuery("Nobody was here")==TRUE);
			break;
		case 4:
			ASSERT(MatchesQuery("Billy")==FALSE);
			ASSERT(MatchesQuery("BOB")==FALSE);
			break;
		case 5:
			ASSERT(MatchesQuery("A B")==FALSE);
			ASSERT(MatchesQuery("C D")==FALSE);
			ASSERT(MatchesQuery("A C")==TRUE);
			ASSERT(MatchesQuery("A D")==TRUE);
			break;
		}
	}
}
#endif


CExpression * CQuery::GetExpression()
{
	CExpression * Exp=NULL;
	CExpression * Exp2=NULL;
	switch (GetNextToken())
	{
	case QT_LBRACE:
		{
			Exp=GetExpression();
			if (Exp==NULL || GetNextToken()!=QT_RBRACE)
			{
				//Mismatched braces
				AfxMessageBox(IDS_MISSINGCLOSINGQUOTES);
				delete Exp;
				return NULL;
			}
		}
		break;
	case QT_AND:
	case QT_OR:
		AfxMessageBox(IDS_ILLEGALANDOR);
		return NULL;

	case QT_RBRACE:
		AfxMessageBox(IDS_MISSINGOPENINGPAREN);
		return NULL;

	case QT_EOF:
		return NULL;

	case QT_TOKEN:
		Exp=new CUnaryExpression(m_szTokenValue);
		break;

	case QT_BADQUOTEDSTRING:
		AfxMessageBox(IDS_MISSINGCLOSINGQUOTES);
		return NULL;
	
	case QT_NOT:
		Exp=GetExpression();
		if (Exp!=NULL)
		{
			Exp->HasNot();
		}
		break;
	default:
		ASSERT(FALSE);	//illegal value of enumerated type??  Weird
	}
	switch (GetNextToken())
	{
	case QT_EOF:
		return Exp;
	case QT_RBRACE:
		UngetToken();
		return Exp;
	case QT_OR:
		Exp2=GetExpression();
		if (Exp2!=NULL)
		{
			CExpression * ExpRet=new COrExpression(Exp,Exp2);
			return ExpRet;
		}
		else
		{
			delete Exp;
			return NULL;
		}
		break;
	case QT_LBRACE:   //Assume AND expressions if we get two tokens in a row
	case QT_TOKEN:
	case QT_NOT:
		UngetToken();
	case QT_AND:
		Exp2=GetExpression();
		if (Exp2!=NULL)
		{
			CExpression * ExpRet=new CAndExpression(Exp,Exp2);
			return ExpRet;
		}
		else
		{
			delete Exp;
			return NULL;
		}
		break;
	default:
		delete Exp;
		AfxMessageBox(IDS_UNABLETOPARSEEXPRESSION);
		return NULL;
		break;
	}
}

void CQuery::UngetToken()
{
	m_bUngotLastToken=TRUE;
}

QUERYTOKENTYPE CQuery::GetNextToken()
{

	//Each white space
	if (m_bUngotLastToken)
	{
		m_bUngotLastToken=FALSE;
		return m_LastToken;
	}
	while (*m_szWorkingPointer==' ' || *m_szWorkingPointer=='\t')
	{
		m_szWorkingPointer++;
	}

	if (*m_szWorkingPointer=='\0')
	{
		m_LastToken=QT_EOF;
		return QT_EOF;
	}
	if (*m_szWorkingPointer=='(')
	{
		m_szWorkingPointer++;
		m_LastToken=QT_LBRACE;
		return QT_LBRACE;
	}
	if (*m_szWorkingPointer==')')
	{
		m_szWorkingPointer++;
		m_LastToken=QT_RBRACE;
		return QT_RBRACE;
	}

	//Quoted value, search for the end quote
	if (*m_szWorkingPointer=='"')
	{
		//Read until an end quote is reached
		int iIndex=0;
		while (*m_szWorkingPointer!='"' && *m_szWorkingPointer!='\0')
		{
			m_szTokenValue[iIndex]=*m_szWorkingPointer;
			m_szWorkingPointer++;			
			iIndex++;
		}
		m_szTokenValue[iIndex]='\0';
		if (*m_szWorkingPointer=='\0')
		{
			m_LastToken=QT_BADQUOTEDSTRING;
			return QT_BADQUOTEDSTRING;
		}
		else
		{
			m_szWorkingPointer++;	//skip end quote
			m_LastToken=QT_TOKEN;

			return QT_TOKEN;
		}
	}

	int iIndex=0;
	while (*m_szWorkingPointer!=' ' && *m_szWorkingPointer!='"' && *m_szWorkingPointer!='(' && *m_szWorkingPointer!=')' && *m_szWorkingPointer!='\0')
	{
		m_szTokenValue[iIndex]=*m_szWorkingPointer;
		m_szWorkingPointer++;			
		iIndex++;
	}
	m_szTokenValue[iIndex]=0;

	if (lstrcmpi(m_szTokenValue,"NOT")==0)
	{
		m_LastToken=QT_NOT;
		return QT_NOT;
	}
	if (lstrcmpi(m_szTokenValue,"AND")==0)
	{
		m_LastToken=QT_AND;
		return QT_AND;
	}
	if (lstrcmpi(m_szTokenValue,"OR")==0)
	{
		m_LastToken=QT_OR;
		return QT_OR;
	}
	m_LastToken=QT_TOKEN;
	return QT_TOKEN;
}

BOOL CQuery::SetQueryText(LPCSTR szQueryText)
{
	delete m_MyExpression;
	m_MyExpression=NULL;
	m_szQueryText=szQueryText;
	m_szWorkingPointer=szQueryText;
	if (szQueryText[0]!='\0')
	{
		m_MyExpression=GetExpression();
		return m_MyExpression!=NULL;
	}
	else
	{
		m_MyExpression=NULL;
		return TRUE;
	}
}

BOOL CQuery::IsEmpty()
{
	return m_MyExpression==NULL;
}
BOOL CQuery::MatchesQuery(LPCSTR szData)
{
	if (IsEmpty())
	{
		return TRUE;
	}
	CString CS=szData;
	CS.MakeUpper();
	return m_MyExpression->Evaluate(CS);
}

BOOL CQuery::MatchesQuery(CSurveyShotArray * pShot)
{
	if (IsEmpty())
	{
		return TRUE;
	}
	return m_MyExpression->Evaluate(pShot);
}

CQuery::~CQuery()
{
	delete m_MyExpression;
	m_MyExpression=NULL;
}





CExpression::CExpression()
{
}

CExpression::~CExpression()
{
}

CBinaryExpression::CBinaryExpression(CExpression *left,CExpression * right)
{
	ASSERT(m_Left!=NULL && m_Right!=NULL);
	m_Left=left;
	m_Right=right;
}
CBinaryExpression::~CBinaryExpression()
{
	delete m_Left;
	delete m_Right;
	m_Left=NULL;
	m_Right=NULL;
}

void CBinaryExpression::HasNot()
{
	m_Left->HasNot();
}

CAndExpression::CAndExpression(CExpression *left,CExpression * right):CBinaryExpression(left,right)
{
}
CAndExpression::~CAndExpression()
{
}

BOOL CAndExpression::Evaluate(LPCSTR szData)
{
	return m_Left->Evaluate(szData) && m_Right->Evaluate(szData);
}
BOOL CAndExpression::Evaluate(CSurveyShotArray *pArray)
{
	return m_Left->Evaluate(pArray) && m_Right->Evaluate(pArray);
}


COrExpression::COrExpression(CExpression *left,CExpression * right):CBinaryExpression(left,right)
{
}
COrExpression::~COrExpression()
{
}
BOOL COrExpression::Evaluate(LPCSTR szData)
{
	return m_Left->Evaluate(szData) || m_Right->Evaluate(szData);
}
BOOL COrExpression::Evaluate(CSurveyShotArray * pData)
{
	return m_Left->Evaluate(pData) || m_Right->Evaluate(pData);
}

CUnaryExpression::CUnaryExpression(LPCSTR szData)
{
	m_szTokenValue=szData;
	m_szTokenValue.MakeUpper();
	m_bHasNot=FALSE;
}
CUnaryExpression::~CUnaryExpression()
{
}

BOOL CUnaryExpression::Evaluate(LPCSTR szData)
{									  
	int iLen=lstrlen(m_szTokenValue);
	LPCSTR szString=strstr(szData,m_szTokenValue);
	while (szString!=NULL)
	{
		//check for start of string or leading nonalpha character since
		//we want to match entire words
		if (szString==m_szTokenValue || !isalnum(*(szString-1)))
		{
			if (!isalnum(szString[iLen]))
			{
				return !m_bHasNot;	//TRUE normally
			}
		}
		szString=strstr(szString+1,m_szTokenValue);
	}
	return m_bHasNot;  //FALSE normally
}

BOOL CUnaryExpression::Evaluate(CSurveyShotArray * pShot)
{	
	return pShot->QueryStationName(m_szTokenValue);
}

void CUnaryExpression::HasNot()
{
	m_bHasNot=!m_bHasNot;
}