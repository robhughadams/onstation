 #ifndef I_FILEHELPER__
#define I_FILEHELPER__

//Allow for a huge line size since surface data
//may be all in one row.
#define MAX_LINE_SIZE  400000

//Because our file format is well defined, we have a pretty easy
//time of reading things.  Essentially, at any given location we
//are expecting either TOKEN=VALUE where TOKEN is know.  In some
//places TOKEN can be one of several types.  For this reason we
//allow lookahead to find out what the token is.  This is used most
//often in a list of survey shots where we can have a SHOT=
//or an END= token.
//We also allow the ProprietaryExtension tokens.  If we are
//looking for TOKEN=VALUE or doing a tokenlookahead, we skip the
//ProprietaryExtensionBlock until the ProprietaryEnd is reached.
//
//Our code works as follows:
//1)  If you are sure of the input, call ReadTokenInt etc... with the
//    token you are expecting.  It will throw an exception of type
//	  CString if there is a problem.  It will handle it's own error
//    reporting.
//2)  If the input is of an unknown type, call GetTokenLookaheadString().  This
//	  will return a pointer to the token.  You then go to step 1 for
//    the type that you want.
class CFileHelper
{
	protected:
		BOOL m_bBufferedLine;	//TRUE if we have done a lookahead.
		char m_szBuffer[MAX_LINE_SIZE];	//string input buffer;
		LPCSTR m_szValue;	//pointer to value part of buffer.
		BOOL ReadToEndOfScope();
		void ReportError(LPCSTR szMessage);
		
		//Converts the '(SCLXPY)' string into a proper
		//attributes string
		WORD ParseAttributes(LPCSTR szData);

		//Used by the SurfaceHeights code
		void WriteRawFloat(float);
		virtual void Write(LPCSTR sz);
		virtual BOOL ReadString(LPSTR szData,int iSize);
		void ReadLine();

		void AppendLines(int iPosition);
		void ReadCurrentLine(BOOL bEndOfFileError=TRUE);

		CArchive * m_Archive;
		int m_iCurrentLineNumber;
	
		BOOL m_bAppended;
		BOOL ParseFloat(float* test,LPCSTR szTest);
	public:
		CFileHelper(CArchive * ar);
		~CFileHelper();

		float m_fVersion;

		int GetCurrentLine() {return m_iCurrentLineNumber;}
		void UndoLastRead() {m_bBufferedLine=TRUE;}
		void WriteToken(LPCSTR szToken,LPCSTR szValue);
		void WriteToken(LPCSTR szToken,float fValue);
		void WriteToken(LPCSTR szToken,int iValue);

		//Used for lists consisting of multiple values such as the
		//height list in the surface data
		void WritePartialToken(LPCSTR szToken);
		void AppendFloat(float fValue);
		void WriteEndOfLine();

		void WriteTokenDate(LPCSTR szToken,DWORD dwDate);
		void WriteTokenColor(LPCSTR szToken,COLORREF crColor);
		void WriteStandardShot(LPCSTR szFrom,LPCSTR szTo,float length,float compass,float clino,float clinoback,float compassback,WORD wAttributes);
		void WriteWalls(LPCSTR szFrom,float fCompass,float fClino,float up,float down,float left,float right);
		void WriteDiveShot(LPCSTR szFrom,LPCSTR szTo,float length,float compass,float depth,WORD wAttributes);

		//These things CString as an exception if they run into a problem.
		int ReadTokenInt(LPCSTR szExpectedToken);
		float ReadTokenFloat(LPCSTR szExpectedToken);
		DWORD ReadTokenDate(LPCSTR szExpectedToken);
		COLORREF ReadTokenColor(LPCSTR szExpectedToken);

		//There must be a Begin=szValue
		void ReadTokenBegin(LPCSTR szValue);
		//There must be an End=szValue
		void ReadTokenEnd(LPCSTR szValue);

		BOOL CheckProprietaryStart(LPCSTR szValue,BOOL bEndOfFileAllowed=FALSE);
		void CheckProprietaryEnd(LPCSTR szValue);

		LPCSTR ReadTokenString(LPCSTR szExpectedToken);
		LPCSTR GetTokenLookaheadString(BOOL bSkipProprietaryData,BOOL bEndOfFileError=TRUE);
		LPCSTR GetTokenLookaheadValue();
		void ReadStandardShotVersion1(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* clino,float* up,float* down,float* left,float* right,float* compassback,float* clinoback,WORD * wAttributes);
		void ReadCaveDiveShotVersion1(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* depth,float* up,float* down,float* left,float* right,WORD * wAttributes);
		void ReadStandardShotVersion2(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* clino,float* compassback,float* clinoback,WORD * wAttributes);
		void ReadCaveDiveShotVersion2(LPSTR szFrom,LPSTR szTo,float* length,float* compass,float* depth,WORD * wAttributes);
		void ReadWalls(LPSTR szStation,float* fComp,float* fClino,float* fUp,float* fDown,float * fLeft,float *fRight);
		
		//Note, this assumes we are skipping over any Proprietary= blocks
		void ReadPartialToken(LPCSTR szTokenPart);
		float ReadAppendedFloat();
};

class CBufferFileHelper:public CFileHelper
{
protected:
	LPSTR m_szTextStoring;
	int m_iStoringLength;
	int m_iStoringCurrent;

	LPCSTR m_szReadPointer;

	virtual void Write(LPCSTR sz);
	virtual BOOL ReadString(LPSTR szData,int iSize);
public:
	CBufferFileHelper();
	CBufferFileHelper(LPCSTR szData);
	~CBufferFileHelper();
	
	LPCSTR GetTextStoring() {return m_szTextStoring;}
};

#endif
