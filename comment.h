#ifndef COMMENT_H_
#define COMMENT_H_

class CComment:public CObject
{
public:
	CComment::CComment(LPCSTR szStationName,LPCSTR szType,LPCSTR szBody);
	CComment::~CComment();

	LPCSTR GetType() const {return m_szType;}
	LPCSTR GetBody() const {return m_szBody;}
	LPCSTR GetStationName() const {return m_szStationName;}
	void SetValues(LPCSTR szStationName,LPCSTR szType,LPCSTR szBody);
private:
	CString m_szType;
	CString m_szBody;
	char m_szStationName[MAX_STATION_NAME_LENGTH];
};

#endif

