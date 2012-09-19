#ifndef I_QUERY__
#define I_QUERY__

class CSurveyShotArray;
class CExpression
{
private:
public:
	virtual ~CExpression();
	CExpression();
	virtual BOOL Evaluate(LPCSTR szString)=0;
	virtual BOOL Evaluate(CSurveyShotArray *)=0;
	virtual void HasNot()=0;	//
};

class CBinaryExpression:public CExpression
{
protected:
	CExpression *m_Left;
	CExpression *m_Right;
public:
	virtual ~CBinaryExpression();
	CBinaryExpression(CExpression *left,CExpression *right);

	virtual BOOL Evaluate(LPCSTR szString)=0;
	virtual BOOL Evaluate(CSurveyShotArray *)=0;
	virtual void HasNot();
};

class COrExpression:public CBinaryExpression
{
public:
	virtual ~COrExpression();
	COrExpression(CExpression *left,CExpression *right);

	virtual BOOL Evaluate(LPCSTR szString);
	virtual BOOL Evaluate(CSurveyShotArray *);
};

class CAndExpression:public CBinaryExpression
{
public:
	virtual ~CAndExpression();
	CAndExpression(CExpression *left,CExpression *right);

	virtual BOOL Evaluate(LPCSTR szString);
	virtual BOOL Evaluate(CSurveyShotArray *);
};

class CUnaryExpression:public CExpression
{
private:
	CString m_szTokenValue;
	BOOL m_bHasNot;
public:
	CUnaryExpression(LPCSTR szData);
	virtual ~CUnaryExpression();
	
	virtual BOOL Evaluate(LPCSTR szString);
	virtual BOOL Evaluate(CSurveyShotArray *);
	virtual void HasNot();
};



typedef enum QUERYTOKENTYPE {QT_EOF,QT_LBRACE,QT_RBRACE,QT_BADQUOTEDSTRING,QT_AND,QT_OR,QT_TOKEN,QT_NOT};
class CQuery
{
private:
	CString m_szQueryText;
	LPCSTR m_szWorkingPointer;
	char m_szTokenValue[256];

	CExpression * m_MyExpression;
	CExpression * GetExpression();
	QUERYTOKENTYPE GetNextToken();
	void UngetToken();
	BOOL m_bUngotLastToken;
	QUERYTOKENTYPE m_LastToken;  //for the ungetlasttoken call
public:
	CQuery();
	~CQuery();

	BOOL IsEmpty();
#ifdef _DEBUG
	void Verify();
#endif
	BOOL SetQueryText(LPCSTR szQueryText);	//returns FALSE if syntax error in text
	BOOL MatchesQuery(LPCSTR szData);
	BOOL MatchesQuery(CSurveyShotArray * pShots);
	LPCSTR GetQueryText() {return m_szQueryText;}
};

#endif

