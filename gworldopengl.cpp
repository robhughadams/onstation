#include "stdafx.h"
#include "onstation.h"
#include "gworldopengl.h"
#include "colortool.h"
#include "onstationdoc.h"
#include "settings.h"	//used for print line thickness

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern COnStationApp theApp;
extern CGlobalColorManager Colors_G;


CGWorldOpenGL::CGWorldOpenGL()
{
}

CGWorldOpenGL::~CGWorldOpenGL()
{
	Empty();
}

void CGWorldOpenGL::Empty()
{
	m_WireframeLineArray.RemoveAll();
	m_LineArray.RemoveAll();
	m_LabelArray.RemoveAll();
	m_ConstraintArray.RemoveAll();
	int iTriangleGroupCount=m_TriangleGroupArray.GetSize();
	for (int i=0;i<iTriangleGroupCount;i++)
	{
		CGWorldTriangleGroup * pGroup=m_TriangleGroupArray.GetAt(i);
		delete pGroup;
	}
	m_TriangleGroupArray.RemoveAll();
}


void CGWorldOpenGL::AddTriangleGroup(CGWorldTriangleGroup * pTriangleGroup)
{
	m_TriangleGroupArray.Add(pTriangleGroup);
}

void CGWorldOpenGL::AddLineSegment(CPosMatrix & From,CPosMatrix & To,COLORREF crColor,LINE_TYPE lineType)
{
	ASSERT(From.GetX()<50000.0f && From.GetY()<50000.0f && From.GetZ()<50000.0f);
	ASSERT(To.GetX()<50000.0f && To.GetY()<50000.0f && To.GetZ()<50000.0f);
	ASSERT(From.GetX()>-50000.0f && From.GetY()>-50000.0f && From.GetZ()>-50000.0f);
	ASSERT(To.GetX()>-50000.0f && To.GetY()>-50000.0f && To.GetZ()>-50000.0f);

	CGWorldOpenGLLine line;
	line.crColor=crColor;
	line.lineType = lineType;

	memcpy(&line.pt1,From.GetDirectPointer(),sizeof(float)*3);
	memcpy(&line.pt2,To.GetDirectPointer(),sizeof(float)*3);

	if (lineType != LT_WIREFRAME)
	{
		m_LineArray.Add(line);
	}
	else
	{
		m_WireframeLineArray.Add(line);
	}
}

void CGWorldOpenGL::AddClosureError(CPosMatrix & From,CPosMatrix & To)
{
	AddLineSegment(From,To,CLOSURE_RGB,LT_CLOSUREERROR);
}


void CGWorldOpenGL::AddConstraint(CPosMatrix & Tag,COLORREF crColor,CNode * pNode,BOOL bMatchedSearch)
{
	CGWorldOpenGLConstraint constraint;
	memcpy(&constraint.position,Tag.GetDirectPointer(),sizeof(float)*3);
	constraint.crColor=crColor;
	constraint.node=pNode;
	constraint.bSearchResultTag=bMatchedSearch;
	m_ConstraintArray.Add(constraint);
}

void CGWorldOpenGL::AddLabel(CNode *node,CPosMatrix & Tag,LPCSTR szName,BOOL bJunction,COLORREF crColor)
{
	CGWorldOpenGLLabel label;
	label.node=node;
	label.crColor=crColor;
	lstrcpyn(label.szName,szName,MAX_STATION_NAME_LENGTH-1);
	memcpy(&label.position,Tag.GetDirectPointer(),sizeof(float)*3);
	label.bJunction=bJunction;
	m_LabelArray.Add(label);
}





void CGWorldOpenGL::WriteGWorldToVRML(CArchive& file,STATIONSHOW SS)
{
	char szBuffer[128];
	file.WriteString("#VRML V1.0 ascii\n");
	//We should probably just use the defaults and make things run faster instead of setting
	//a light source.
	file.WriteString("Separator {\n\tDEF SceneInfo Info {\n\t\tstring ");
	file<<'"';
	file.WriteString("Modeled using On Station 3.1");
	file<<'"';
	file.WriteString("\n\t}\n}\n");

	int iTriangleGroupCount=m_TriangleGroupArray.GetSize();
	for (int i=0;i<iTriangleGroupCount;i++)
	{
		sprintf(szBuffer,"DEF TriangleStrip%i Separator {\n",i+1);
		file.WriteString(szBuffer);
		CGWorldTriangleGroup const * pGroup=m_TriangleGroupArray.GetAt(i);
		pGroup->DumpToVRML(file);
		file.WriteString("}\n");
	}
	
	//Next, do the fixed points if appropriate
	int iFixedPointCount=m_ConstraintArray.GetSize();
	CPosMatrix ptPos;
	CPosMatrix transformedPos;
	for (i=0;i<iFixedPointCount;i++)
	{
		sprintf(szBuffer,"DEF FixedPoint%i Separator {\n",i+1);
		file.WriteString(szBuffer);
		
		const CGWorldOpenGLConstraint & fixedPoint=m_ConstraintArray.GetAt(i);
		file.WriteString("\tMaterial {\n\t\tdiffuseColor 1.0 0.0 0.0\n\t}\n");
		sprintf(szBuffer,"\tTranslation {\n\t\ttranslation %f %f %f\n\t}\n",fixedPoint.position[0],fixedPoint.position[1],fixedPoint.position[2]);
		file.WriteString(szBuffer);
		file.WriteString("\tSphere {\n\t\tradius 3\n\t}\n");
		file.WriteString("}\n");
	}
}


void CGWorldOpenGL::WriteGWorldToDXF(CStdioFile *pFile, BOOL bShowClosureError, STATIONSHOW bShowStationNames, BOOL bBoxFixedPoints)
{
	// blocks
	CString stationBlockName = "STATION";
	CString fixedPointBlockName = "FIXEDPOINT";
	// layers
	CString stationLayerName1 = "S_STATION100";
	CString stationLayerName2 = "S_STATION500";
	CString shotLayerName = "S_SHOT";
	CString wallsLayerName = "S_PASSAGEWIDTH";
	CString ceilingLayerName = "S_PASSAGEHEIGHT";
	CString errorLayerName = "S_ERRORS";
	CString heightLayerName = "S_STATIONHEIGHT";
	//CString heightLayerName2 = "S_STATIONHEIGHT500";
	CString namesLayerName1 = "S_STATIONNAMES100";
	CString namesLayerName2 = "S_STATIONNAMES500";
	CString fixedPointLayerName = "S_FIXEDPOINT";
	CString wireFrameLayerName = "S_SURFACEDATA";
	CString crossSectionLayerName = "S_CROSSSECTION";
	CString surfaceShotLayerName = "S_SURFACESHOT";

	// show hourglass mouse pointer
	CWinApp *pApp = AfxGetApp();
	pApp -> BeginWaitCursor();

	// write DXF header
	///////////////////
	pFile-> WriteString("0\nSECTION\n2\nHEADER\n");	// (note: a header section must exist (even if empty) to support LAYER tables)

	// read value off one item to set drawing limits (otherwise the cave may end-up in one corner of the autocad screen)
	if(m_ConstraintArray.GetSize())	// test just in case the user tries to export an empty file
	{
		const CGWorldOpenGLConstraint & fixedPoint=m_ConstraintArray.GetAt(0);
		pFile-> WriteString("9\n$LIMMIN\n10\n"+FloatToString(fixedPoint.position[0])+ "\n20\n"+FloatToString(fixedPoint.position[1])+
							"\n9\n$LIMMAX\n10\n"+FloatToString(fixedPoint.position[0])+"\n20\n"+FloatToString(fixedPoint.position[1])+"\n");
	}

	pFile-> WriteString("0\nENDSEC\n");

	// write tables
	///////////////
	pFile-> WriteString("0\nSECTION\n2\nTABLES\n");
	// write layers
	const int countLayers = 14; //15;
	CString layerNames[countLayers] = {"0", stationLayerName1, stationLayerName2, shotLayerName, wallsLayerName, ceilingLayerName, 
										errorLayerName, heightLayerName, /*heightLayerName2,*/ namesLayerName1, namesLayerName2, 
										fixedPointLayerName, wireFrameLayerName, crossSectionLayerName, surfaceShotLayerName};
	int layerColor[countLayers] = {7, 1, 1, 3, 5, 5, 1, 6, /*6,*/ 4, 4, 2, 8, 5, 9};

	pFile-> WriteString("0\nTABLE\n2\nLAYER\n70\n"+IntToString(countLayers)+"\n");	// change last item (after 70) to max number of layers

	for (int i=0; i<countLayers; ++i)		// write each layer
		pFile-> WriteString("0\nLAYER\n2\n"+layerNames[i]+"\n70\n0\n62\n"+IntToString(layerColor[i])+"\n6\nCONTINUOUS\n");

	pFile-> WriteString("0\nENDTAB\n");
	pFile-> WriteString("0\nENDSEC\n");

	// define blocks (to be included later in the drawing)
	////////////////
	pFile-> WriteString("0\nSECTION\n2\nBLOCKS\n");

	// define station block
	pFile-> WriteString("0\nBLOCK\n8\n"+ stationLayerName1 + "\n2\n" + stationBlockName + "\n70\n0\n10\n0\n20\n0\n30\n0\n");
	// small triangle
	pFile-> WriteString("0\nSOLID\n8\n" + stationLayerName1 + "\n10\n0\n20\n0.15\n30\n0\n11\n-0.1299\n21\n-0.075\n31\n0\n12\n0.1299\n22\n-0.075\n32\n0\n13\n0.1299\n23\n-0.075\n33\n0\n");
	// large triangle
	pFile-> WriteString("0\nPOLYLINE\n8\n" + stationLayerName2 + "\n66\n1\n10\n0.0\n20\n0.0\n30\n0.0\n70\n1\n");
	pFile-> WriteString("0\nVERTEX\n8\n" + stationLayerName2 + "\n10\n0\n20\n0.5\n30\n0\n");
	pFile-> WriteString("0\nVERTEX\n8\n" + stationLayerName2 + "\n10\n-0.433\n20\n-0.25\n30\n0\n");
	pFile-> WriteString("0\nVERTEX\n8\n" + stationLayerName2 + "\n10\n0.433\n20\n-0.25\n30\n0\n");
	pFile-> WriteString("0\nSEQEND\n8\n" + stationLayerName2 + "\n");

	pFile-> WriteString("0\nENDBLK\n8\n" + stationLayerName1+"\n");

	// define fixedPoint block
	pFile-> WriteString("0\nBLOCK\n8\n"+ fixedPointLayerName + "\n2\n" + fixedPointBlockName + "\n70\n0\n10\n0\n20\n0\n30\n0\n");
	pFile-> WriteString("0\nSOLID\n8\n" + fixedPointLayerName + "\n10\n0.5\n20\n0.5\n30\n0\n11\n-0.5\n21\n0.5\n31\n0\n12\n0.5\n22\n-0.5\n32\n0\n13\n-0.5\n23\n-0.5\n33\n0\n");
	pFile-> WriteString("0\nENDBLK\n8\n" + fixedPointLayerName+"\n");

	pFile-> WriteString("0\nENDSEC\n");

	// write DXF entities
	/////////////////////
	pFile-> WriteString("0\nSECTION\n2\nENTITIES\n");

	// write lines
	int iCount=m_LineArray.GetSize();
	for (i=0;i<iCount;i++)
	{
		const CGWorldOpenGLLine& Line=m_LineArray.GetAt(i);

		BOOL bShowThisLine=TRUE;
		CString layer = "0";	// lines of undefined types go to layer 0

		switch (Line.lineType)
		{	// diferent types of lines are output to different layers
		case LT_SHOT:
			layer = shotLayerName;
			break;
		case LT_SURFACESHOT:
			layer = surfaceShotLayerName;
			break;
		case LT_CLOSUREERROR:
			layer = errorLayerName;
			bShowThisLine=bShowClosureError;
			break;
		case LT_PASSAGEWIDTH:
			layer = wallsLayerName;
			break;
		case LT_PASSAGEHEIGHT:
			layer = ceilingLayerName;
			break;
		case LT_SURFACE:
			layer = wireFrameLayerName;
			break;
		case LT_CROSSSECTION:
			layer = crossSectionLayerName;
			break;
//		case LT_WIREFRAME:	// those should not be seen since they only appear on screen when rotating
//		default:	// (there should be no other types)
//			break;
		}

		if (bShowThisLine)
			pFile-> WriteString("0\nLINE\n8\n" + layer + 
						"\n10\n" + FloatToString(Line.pt1[0]) + "\n20\n" + FloatToString(Line.pt1[1]) + "\n30\n" + FloatToString(Line.pt1[2]) + 
						"\n11\n" + FloatToString(Line.pt2[0]) + "\n21\n" + FloatToString(Line.pt2[1]) + "\n31\n" + FloatToString(Line.pt2[2]) + "\n");
	}

	// write stations and station's name and height
	int iLabelCount=m_LabelArray.GetSize();
	for (i=0;i<iLabelCount;i++)
	{
		const CGWorldOpenGLLabel & Label=m_LabelArray.GetAt(i);

		// write station name
		if (bShowStationNames==SHOW_ALL || (Label.bJunction && bShowStationNames==SHOW_INTERSECT))
		{
			// small text
			pFile-> WriteString("0\nTEXT\n8\n" + namesLayerName1 + "\n72\n2\n" +
						"10\n" + FloatToString(Label.position[0]) + "\n20\n" + FloatToString(Label.position[1]) + "\n30\n" + FloatToString(Label.position[2]) + 
						"\n11\n" + FloatToString(Label.position[0] - (float)0.1299) + "\n21\n" + FloatToString(Label.position[1]) + "\n31\n" + FloatToString(Label.position[2]) + 
						"\n40\n0.4\n1\n" + Label.szName + "\n");
			// large text
			pFile-> WriteString("0\nTEXT\n8\n" + namesLayerName2 + "\n72\n2\n" +
						"10\n" + FloatToString(Label.position[0]) + "\n20\n" + FloatToString(Label.position[1]) + "\n30\n" + FloatToString(Label.position[2]) + 
						"\n11\n" + FloatToString(Label.position[0] - (float)0.433) + "\n21\n" + FloatToString(Label.position[1]) + "\n31\n" + FloatToString(Label.position[2]) + 
						"\n40\n1.0\n1\n" + Label.szName + "\n");
		}
		// write station height
		// small text
		pFile-> WriteString("0\nTEXT\n8\n" + heightLayerName + "\n" + //"73\n3\n" +	// tag 73 is not recognized by AutoCAD v.10: we substract the font size from the y value
					"10\n" + FloatToString(Label.position[0] + (float)0.1299) + "\n20\n" + FloatToString(Label.position[1]-(float)0.4) + "\n30\n" + FloatToString(Label.position[2]) + 
					"\n11\n" + FloatToString(Label.position[0] + (float)0.1299) + "\n21\n" + FloatToString(Label.position[1]-(float)0.4) + "\n31\n" + FloatToString(Label.position[2]) + 
					"\n40\n0.4\n1\n" + FloatToString(Label.position[2]) + "\n");
/*		// large text
		pFile-> WriteString("0\nTEXT\n8\n" + heightLayerName2 + "\n73\n3\n" +
					"10\n" + FloatToString(Label.position[0]) + "\n20\n" + FloatToString(Label.position[1]) + "\n30\n" + FloatToString(Label.position[2]) + 
					"\n11\n" + FloatToString(Label.position[0] + (float)0.433) + "\n21\n" + FloatToString(Label.position[1]) + "\n31\n" + FloatToString(Label.position[2]) + 
					"\n40\n1.0\n1\n" + FloatToString(Label.position[2]) + "\n");
*/
		// draw station
		pFile-> WriteString("0\nINSERT\n2\n" + stationBlockName + "\n8\n" + stationLayerName1 + 
					"\n10\n" + FloatToString(Label.position[0])+ "\n20\n" + FloatToString(Label.position[1]) + "\n30\n" + FloatToString(Label.position[2]) + "\n");
	}

	// write fixed points and fixed points' text
	int iFixedPointCount=m_ConstraintArray.GetSize();
	for (i=0;i<iFixedPointCount;i++)
	{
		const CGWorldOpenGLConstraint & fixedPoint=m_ConstraintArray.GetAt(i);
		
		if (!fixedPoint.bSearchResultTag)	// only show fixed points: not search results '?' tags
		{
			// write text
			pFile-> WriteString("0\nTEXT\n8\n" + fixedPointLayerName +
						"\n10\n" + FloatToString(fixedPoint.position[0] + (float)0.6) + "\n20\n" + FloatToString(fixedPoint.position[1]) + "\n30\n" + FloatToString(fixedPoint.position[2]) +
						"\n40\n1.0\n1\n" + fixedPoint.node->GetName() + "\n");
					
			// draw fixed point
			pFile-> WriteString("0\nINSERT\n2\n" + fixedPointBlockName + "\n8\n" + fixedPointLayerName + 
						"\n10\n" + FloatToString(fixedPoint.position[0])+ "\n20\n" + FloatToString(fixedPoint.position[1]) + "\n30\n" + FloatToString(fixedPoint.position[2]) + "\n");

		}
	}

	// end of 'entities' section and end of file
	pFile-> WriteString("0\nENDSEC\n0\nEOF\n");

	// hide hourglass mouse pointer
	pApp -> EndWaitCursor();
}

void CGWorldOpenGL::FindGeometryLimits(CViewMatrix * pViewMatrix,CRect * pRect,BOOL bStereoscopic,BOOL bShowFixedPoints,BOOL bShowClosureErrors)
{
	CPosMatrix ptPos;
	CPosMatrix transformedPos;

	int iLineCount=m_LineArray.GetSize();
	for (int i=0;i<iLineCount;i++)
	{
		const CGWorldOpenGLLine& line=m_LineArray.GetAt(i);
		if (line.crColor!=CLOSURE_RGB)
		{
			line.ExpandRectangle(pViewMatrix,pRect);
		}
	}
	int iTriangleGroupCount=m_TriangleGroupArray.GetSize();
	for (i=0;i<iTriangleGroupCount;i++)
	{
		CGWorldTriangleGroup const * pGroup=m_TriangleGroupArray.GetAt(i);
		pGroup->ExpandRectangle(pViewMatrix,pRect);
	}

	iLineCount=m_LineArray.GetSize();
	for (i=0;i<iLineCount;i++)
	{
		const CGWorldOpenGLLine& line=m_LineArray.GetAt(i);
		line.ExpandRectangle(pViewMatrix,pRect);
	}
	if (bShowClosureErrors)
	{
		for (i=0;i<iLineCount;i++)
		{
			const CGWorldOpenGLLine& line=m_LineArray.GetAt(i);
			line.ExpandRectangle(pViewMatrix,pRect);
		}
	}
}

// **************************************
// PRODUCE THE THING FUNCTIONS
void CGWorldOpenGL::GenerateGeometry(BOOL bFast,BOOL bStereoscopic,BOOL bShowFixedPoints,BOOL bShowClosureErrors)
{
	if (bFast)
	{
		glCallList(MYLIST_WIREFRAMEMODE);
	}
	else
	{
		glCallList(MYLIST_CAVEGEOMETRY);
	}
	if (bShowClosureErrors)
	{
		glCallList(MYLIST_CLOSUREERRORS);
	}
}

CPtrArray * CGWorldOpenGL::GetHitList(CViewMatrix* pViewMatrix,CPoint point,STATIONSHOW show,BOOL bFixedPoints,CRect rViewPort,float fLeft,float fRight,float fTop,float fBottom)
{
	CPtrArray * pPtrArray=new CPtrArray;

	if (bFixedPoints)
	{
		int iFixedPointCount=m_ConstraintArray.GetSize();
		CPosMatrix ptPos;
		CPosMatrix transformedPos;
		for (int i=0;i<iFixedPointCount;i++)
		{
			const CGWorldOpenGLConstraint & fixedPoint=m_ConstraintArray.GetAt(i);
			ptPos.Set(fixedPoint.position[0],fixedPoint.position[1],fixedPoint.position[2]);
			ptPos.Multiply(&transformedPos,*pViewMatrix);
			transformedPos.Normalize();
			CRect rect;
			float fTextLeft=((transformedPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
			float fTextTop=(float)rViewPort.bottom-((transformedPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
			rect.left=(int)fTextLeft-18;
			rect.top=(int)fTextTop;
			rect.right=rect.left+16;
			rect.bottom=rect.top+16;
			if (rect.PtInRect(point))
			{
				pPtrArray->Add((LPVOID)fixedPoint.node);
			}
		}
	}

	if (show==SHOW_NONE)
	{
		return pPtrArray;
	}
	int iLabelCount=m_LabelArray.GetSize();
	CPosMatrix labelPos;
	CPosMatrix transformedPos;
	for (int i=0;i<iLabelCount;i++)
	{
		const CGWorldOpenGLLabel & Label=m_LabelArray.GetAt(i);
		if (show==SHOW_ALL || Label.bJunction)
		{
			labelPos.Set(Label.position[0],Label.position[1],Label.position[2]);
			labelPos.Multiply(&transformedPos,*pViewMatrix);
			CRect rect;
			transformedPos.Normalize();
			float fTextLeft=((transformedPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
			float fTextTop=(float)rViewPort.bottom-((transformedPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
			rect.left=(int)fTextLeft;
			rect.top=(int)fTextTop;
			rect.right=rect.left+50;
			rect.bottom=rect.top+15;
			if (rect.PtInRect(point))
			{
				pPtrArray->Add((LPVOID)Label.node);
			}
		}
	}
	return pPtrArray;
}

void CGWorldOpenGL::DrawGDILineSegments(CDC * dc,CViewMatrix * pViewMatrix, CRect rViewPort,float fLeft, float fRight,float fTop,float fBottom,int iLineSize,BOOL bShowClosureErrors)
{
	CPosMatrix fromPos;
	CPosMatrix transformedFromPos;
	CPosMatrix toPos;
	CPosMatrix transformedToPos;
	int iCount=m_WireframeLineArray.GetSize();
	for (int i=0;i<iCount;i++)
	{
		const CGWorldOpenGLLine& Line=m_WireframeLineArray.GetAt(i);
		fromPos.Set(Line.pt1[0],Line.pt1[1],Line.pt1[2]);
		fromPos.Multiply(&transformedFromPos,*pViewMatrix);
		toPos.Set(Line.pt2[0],Line.pt2[1],Line.pt2[2]);
		toPos.Multiply(&transformedToPos,*pViewMatrix);

		//Reverse black and white when printing
		COLORREF crColor=Line.crColor;
		if (dc->IsPrinting() && crColor==RGB(255,255,255))
		{
			crColor=RGB(0,0,0);
		}
		toPos.Normalize();
		fromPos.Normalize();
		CRect rect;
		transformedFromPos.Normalize();
		float fFromLeft=((transformedFromPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
		float fFromTop=(float)rViewPort.bottom-((transformedFromPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
		float fToLeft=((transformedToPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
		float fToTop=(float)rViewPort.bottom-((transformedToPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
		CPen Pen(PS_SOLID,iLineSize,crColor);
		CPen * pPenOld=dc->SelectObject(&Pen);
		dc->MoveTo((int)fFromLeft,(int)fFromTop);
		dc->LineTo((int)fToLeft,(int)fToTop);
		dc->SelectObject(pPenOld);
	}

	iCount=m_LineArray.GetSize();
	for (i=0;i<iCount;i++)
	{
		const CGWorldOpenGLLine& Line=m_LineArray.GetAt(i);
		//crColor==CLOSURE_RGB indicates loop closure.
		BOOL bShowThisLine=TRUE;
		if (Line.crColor==CLOSURE_RGB)
		{
			bShowThisLine=bShowClosureErrors;
		}

		if (bShowThisLine)
		{
			fromPos.Set(Line.pt1[0],Line.pt1[1],Line.pt1[2]);
			fromPos.Multiply(&transformedFromPos,*pViewMatrix);
			toPos.Set(Line.pt2[0],Line.pt2[1],Line.pt2[2]);
			toPos.Multiply(&transformedToPos,*pViewMatrix);
			//Reverse black and white when printing
			COLORREF crColor=Line.crColor;
			if (dc->IsPrinting() && crColor==RGB(255,255,255))
			{
				crColor=RGB(0,0,0);
			}
			toPos.Normalize();
			fromPos.Normalize();
			CRect rect;
			transformedFromPos.Normalize();
			float fFromLeft=((transformedFromPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
			float fFromTop=(float)rViewPort.bottom-((transformedFromPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
			float fToLeft=((transformedToPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
			float fToTop=(float)rViewPort.bottom-((transformedToPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
			
			CPen Pen(crColor==CLOSURE_RGB?PS_DASH:PS_SOLID,iLineSize,crColor==CLOSURE_RGB?RGB(255,0,0):crColor);
			CPen * pPenOld=dc->SelectObject(&Pen);
			dc->MoveTo((int)fFromLeft,(int)fFromTop);
			dc->LineTo((int)fToLeft,(int)fToTop);
			dc->SelectObject(pPenOld);
		}
	}
	int iTriangleGroupCount=m_TriangleGroupArray.GetSize();
	for (i=0;i<iTriangleGroupCount;i++)
	{
		CGWorldTriangleGroup const * pGroup=m_TriangleGroupArray.GetAt(i);
		pGroup->DumpToGL();
	}
	glEndList();

}

void CGWorldOpenGL::DrawTaggedPoints(CDC *dc,CViewMatrix * pViewMatrix,CRect rViewPort,float fLeft,float fRight,float fTop,float fBottom,BOOL bEliminateFixed)
{
	int iFixedPointCount=m_ConstraintArray.GetSize();
	if (iFixedPointCount==0)
		return;

	CDC pDC2;
	pDC2.CreateCompatibleDC(dc);
	CBitmap pFixedPointBitmap;
	pFixedPointBitmap.LoadBitmap(IDB_BITMAPPINONSCREEN);
	CBitmap pSearchFoundBitmap;
	pSearchFoundBitmap.LoadBitmap(IDB_BITMAPSEARCHFOUND);
	CPosMatrix ptPos;
	CPosMatrix transformedPos;
	for (int i=0;i<iFixedPointCount;i++)
	{
		const CGWorldOpenGLConstraint & fixedPoint=m_ConstraintArray.GetAt(i);
		ptPos.Set(fixedPoint.position[0],fixedPoint.position[1],fixedPoint.position[2]);
		ptPos.Multiply(&transformedPos,*pViewMatrix);
		transformedPos.Normalize();
		float fTextLeft=((transformedPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
		float fTextTop=(float)rViewPort.bottom-((transformedPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
		CBitmap * pBm=NULL;
		if (fixedPoint.bSearchResultTag || !bEliminateFixed)
		{
			CBitmap * pOldBitmap=pDC2.SelectObject(fixedPoint.bSearchResultTag?&pSearchFoundBitmap:&pFixedPointBitmap);
			if (!dc->IsPrinting())
			{
				dc->BitBlt((int)fTextLeft-18,(int)fTextTop,16,16,&pDC2,0,0,SRCCOPY);
			}
			else
			{
				dc->FillSolidRect((int)fTextLeft-10,(int)fTextTop-10,20,20,RGB(0,0,0));
			}
			pDC2.SelectObject(pOldBitmap);
		}
	}
}

void CGWorldOpenGL::DrawText(CDC * dc,CViewMatrix * pViewMatrix,STATIONSHOW show,CRect rViewPort,float fLeft,float fRight,float fTop,float fBottom)
{
	if (show==SHOW_NONE)
		return;
	dc->SetBkMode(TRANSPARENT);
	int iLabelCount=m_LabelArray.GetSize();
	CPosMatrix labelPos;
	CPosMatrix transformedPos;
	for (int i=0;i<iLabelCount;i++)
	{
		const CGWorldOpenGLLabel & Label=m_LabelArray.GetAt(i);
		if (show==SHOW_ALL || Label.bJunction)
		{
			labelPos.Set(Label.position[0],Label.position[1],Label.position[2]);
			labelPos.Multiply(&transformedPos,*pViewMatrix);
			//Reverse black and white when printing
			if (dc->IsPrinting() && Label.crColor==RGB(255,255,255))
			{
				dc->SetTextColor(RGB(0,0,0));
			}
			else
			{
				dc->SetTextColor(Label.crColor);
			}
			CRect rect;
			transformedPos.Normalize();
			float fTextLeft=((transformedPos.GetX()-fLeft)/(fRight-fLeft))*((float)rViewPort.Width())+(float)rViewPort.left;
			float fTextTop=(float)rViewPort.bottom-((transformedPos.GetY()-fTop)/(fBottom-fTop))*((float)rViewPort.Height());
			rect.left=(int)fTextLeft;
			rect.top=(int)fTextTop;
			rect.right=rect.left+50;
			rect.bottom=rect.top+15;
			dc->DrawText(Label.szName,rect,DT_NOCLIP);
//			TRACE("GworldOpenGL:: Outputting %s at %i,%i %i,%i\n",Label.szName,rect.left,rect.top,rect.right,rect.bottom);
		}
	}
}



void CGWorldOpenGL::CompileLists()
{
	//Our compile strategy is to split the geometry into different
	//pieces so that we can turn on and off sections by simply rendering
	//the pieces that we want.

	/*TEST GEOMETRY
	glRGB(255,0,0);
	glBegin(GL_LINES);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,100.0f,-100.0f);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0.0f,100.0f,-100.0f);
	glVertex3f(50.0f,150.0f,-200.0f);
	glEnd();
	glRGB(0,0,255);
	glBegin(GL_TRIANGLES);
	glVertex3f(30.f,30.f,300.f);
	glVertex3f(30.f,40.f,300.f);
	glVertex3f(40.f,40.f,300.f);
	glEnd();
	glRGB(0,0,255);
	glBegin(GL_TRIANGLES);
	glVertex3f(60.f,60.f,-1000.f);
	glVertex3f(60.f,70.f,-1000.f);
	glVertex3f(70.f,70.f,-1000.f);
	glEnd();
	glEndList();
*/

	//Start with the lines and rectangles which define the underground stuff
	glNewList(MYLIST_CAVEGEOMETRY,GL_COMPILE);
	int iLineCount=m_LineArray.GetSize();
	for (int i=0;i<iLineCount;i++)
	{
		const CGWorldOpenGLLine& line=m_LineArray.GetAt(i);
		if (line.crColor!=CLOSURE_RGB)
		{
			line.DumpToGL();
		}
	}
	int iTriangleGroupCount=m_TriangleGroupArray.GetSize();
	for (i=0;i<iTriangleGroupCount;i++)
	{
		CGWorldTriangleGroup const * pGroup=m_TriangleGroupArray.GetAt(i);
		pGroup->DumpToGL();
	}
	glEndList();
	
	glNewList(MYLIST_CLOSUREERRORS,GL_COMPILE);
	for (i=0;i<iLineCount;i++)
	{
		const CGWorldOpenGLLine& line=m_LineArray.GetAt(i);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(5,0x5555);	//dashed lines 5 pixels long
		glRGB(255,0,0);
		if (line.crColor==CLOSURE_RGB)
		{
			line.DumpToGL();
		}
		glDisable(GL_LINE_STIPPLE);
	}
	glEndList();

	glNewList(MYLIST_WIREFRAMEMODE,GL_COMPILE);
	iLineCount=m_WireframeLineArray.GetSize();
	for (i=0;i<iLineCount;i++)
	{
		const CGWorldOpenGLLine& line=m_WireframeLineArray.GetAt(i);
		line.DumpToGL();
	}
	glEndList();

}

#ifdef _DEBUG
void glDumpError()
{
	switch (glGetError())
	{
	case GL_NO_ERROR:
		break;
	case GL_INVALID_ENUM:
		AfxMessageBox("Invalid Enum");	//internationalization OK
		ASSERT(FALSE);
		break;
	case GL_INVALID_VALUE:
		AfxMessageBox("Invalid Value");//internationalization OK
		ASSERT(FALSE);
		break;
	case GL_INVALID_OPERATION:
		AfxMessageBox("Invalid Operation");//internationalization OK
		ASSERT(FALSE);
		break;
	case GL_STACK_OVERFLOW:
		AfxMessageBox("Stack overflow");//internationalization OK
		ASSERT(FALSE);
		break;
	case GL_STACK_UNDERFLOW:
		AfxMessageBox("Stack underflow");//internationalization OK
		ASSERT(FALSE);
		break;
	case GL_OUT_OF_MEMORY:
		AfxMessageBox("Out of Memory");//internationalization OK
		ASSERT(FALSE);
		break;
	default:
		ASSERT(FALSE);
	}
}


#endif




void CGWorldOpenGLLine::DumpToGL() const
{
	if (crColor!=CLOSURE_RGB)
	{
		Colors_G.MakeGLColor(crColor);
	}
 	glBegin(GL_LINES);
	glVertex3f(pt1[0],pt1[1],pt1[2]);
	glVertex3f(pt2[0],pt2[1],pt2[2]);
	glEnd();
}

void CGWorldOpenGLLine::ExpandRectangle(CViewMatrix * pView,CRect * pRect) const
{
	CPosMatrix ptPos;
	CPosMatrix transformedPos1;
	CPosMatrix transformedPos2;
	ptPos.Set(pt1[0],pt1[1],pt1[2]);
	ptPos.Multiply(&transformedPos1,*pView);
	transformedPos1.Normalize();

	ptPos.Set(pt2[0],pt2[1],pt2[2]);
	ptPos.Multiply(&transformedPos2,*pView);
	transformedPos2.Normalize();

	float fLeft=min(transformedPos1.GetX(),transformedPos2.GetX());
	float fRight=max(transformedPos1.GetX(),transformedPos2.GetX());
	float fTop=min(transformedPos1.GetY(),transformedPos2.GetY());
	float fBottom=max(transformedPos1.GetY(),transformedPos2.GetY());

	pRect->left=min((int)fLeft,pRect->left);
	pRect->right=max((int)fRight,pRect->right);
	pRect->top=min((int)fTop,pRect->top);
	pRect->bottom=max((int)fBottom,pRect->bottom);
}

CGWorldTriangleStrip::CGWorldTriangleStrip(int iNumVertices,COLORREF cr):CGWorldTriangleGroup(iNumVertices,cr)
{
}

CGWorldTriangleStrip::~CGWorldTriangleStrip()
{
}

void CGWorldTriangleStrip::DumpToGL() const
{
	glBegin(GL_TRIANGLE_STRIP);
	glRGB(GetRValue(m_crColor),GetGValue(m_crColor),GetBValue(m_crColor));
	for (int i=0;i<m_iNumVertices;i++)
	{
		glNormal3f(m_pVertices[i].m_fNormal[0],m_pVertices[i].m_fNormal[1],m_pVertices[i].m_fNormal[2]);
		glVertex3f(m_pVertices[i].m_fPos[0],m_pVertices[i].m_fPos[1],m_pVertices[i].m_fPos[2]);
	}
	glEnd();
};
void CGWorldTriangleStrip::DumpToVRML(CArchive& file) const
{
	char szBuffer[256];
	//write material properties
	sprintf(szBuffer,"\tMaterial {\n\t\tdiffuseColor %f %f %f\n\t\tambientColor %f %f %f\n\t}\n",	((float)GetRValue(m_crColor))/255.0f,((float)GetGValue(m_crColor))/255.0f,((float)GetBValue(m_crColor))/255.0f,	((float)GetRValue(m_crColor))/255.0f,((float)GetGValue(m_crColor))/255.0f,((float)GetBValue(m_crColor))/255.0f);
	file.WriteString(szBuffer);

	file.WriteString("\tCoordinate3	{\n\t\tpoint [\n");

	for (int i=0;i<m_iNumVertices;i++)
	{
		sprintf(szBuffer,"\t\t\t%f %f %f",m_pVertices[i].m_fPos[0],m_pVertices[i].m_fPos[1],m_pVertices[i].m_fPos[2]);
		file.WriteString(szBuffer);
		if (i!=m_iNumVertices-1)
		{
			file.WriteString(",\n");
		}
		else
		{
			file.WriteString("\n");
		}
	}
	file.WriteString("\t\t]\n\t}\n");
	file.WriteString("\tIndexedFaceSet {\n\t\tcoordIndex [\n");
	for (i=0;i<m_iNumVertices;i++)
	{
		if ((i%2)==0)
		{
			sprintf(szBuffer,"\t\t\t%i,%i,%i,-1",i,(i+1)%m_iNumVertices,(i+2)%m_iNumVertices);
		}
		else
		{
			sprintf(szBuffer,"\t\t\t%i,%i,%i,-1",i,(i+2)%m_iNumVertices,(i+1)%m_iNumVertices);
		}
		file.WriteString(szBuffer);
		if (i!=m_iNumVertices-1)
		{
			file.WriteString(",\n");
		}
		else
		{
			file.WriteString("\n");
		}
	}
	file.WriteString("\t\t]\n\t}\n");
}

void CGWorldTriangleStrip::SetRectNormals()
{
	//Sets normal vectors for all the triangles assuming that
	//the represent a flat rectangle
	ASSERT(m_iNumVertices==4);	//if not,this is some other shape

	float fNormal[3];
	CalcNormal(m_pVertices[0].m_fPos,m_pVertices[1].m_fPos,m_pVertices[2].m_fPos,fNormal);
	for (int i=0;i<m_iNumVertices;i++)
	{
		m_pVertices[i].m_fNormal[0]=fNormal[0];
		m_pVertices[i].m_fNormal[1]=fNormal[1];
		m_pVertices[i].m_fNormal[2]=fNormal[2];
	}
}

void CGWorldTriangleStrip::SetRotationStripNormals()
{
	//Sets normal vectors for all the triangles assuming that
	//the represent a rectangular solid between two stations.
	ASSERT(m_iNumVertices==10);	//if not,this is some other shape
	//Could go to 11, but if the user fucked up the call into here
	//at least this won't crash

	//The pattern we used is aimed at given normals to the
	//vertices which are between the two flat surfaces.  Best is to
	//simply draw the pattern and figure out how this works for
	//yourself.
	for (int i=0;i<m_iNumVertices;i++)
	{
		int iOne,iThree;
		int iTwo=i;
		if (i%2==0)	//even
		{
			iOne=i-3;
			iThree=i+1;
		}
		else
		{
			iOne=i-1;
			iThree=i+3;
		}
		if (iOne<0)
			iOne+=10;
		if (iThree>=10)
			iThree-=10;
		CalcNormal(m_pVertices[iOne].m_fPos,m_pVertices[iTwo].m_fPos,m_pVertices[iThree].m_fPos,m_pVertices[i].m_fNormal);
	}
}


CGWorldTriangleFan::CGWorldTriangleFan(int iNumVertices,COLORREF cr):CGWorldTriangleGroup(iNumVertices,cr)
{
}

CGWorldTriangleFan::~CGWorldTriangleFan()
{
}

void CGWorldTriangleFan::DumpToGL() const
{
	glBegin(GL_TRIANGLE_FAN);
	glRGB(GetRValue(m_crColor),GetGValue(m_crColor),GetBValue(m_crColor));
	for (int i=0;i<m_iNumVertices;i++)
	{
		glNormal3f(m_pVertices[i].m_fNormal[0],m_pVertices[i].m_fNormal[1],m_pVertices[i].m_fNormal[2]);
		glVertex3f(m_pVertices[i].m_fPos[0],m_pVertices[i].m_fPos[1],m_pVertices[i].m_fPos[2]);
	}
	glEnd();
}

void CGWorldTriangleGroup::ExpandRectangle(CViewMatrix * pViewPort,CRect * pRect) const
{
	CPosMatrix ptPos;
	CPosMatrix transformedPos;
	for (int i=0;i<m_iNumVertices;i++)
	{
		ptPos.Set(m_pVertices[i].m_fPos[0],m_pVertices[i].m_fPos[1],m_pVertices[i].m_fPos[2]);
		ptPos.Multiply(&transformedPos,*pViewPort);
		transformedPos.Normalize();
		pRect->left=min(pRect->left,(int)transformedPos.GetX());
		pRect->right=max(pRect->right,(int)transformedPos.GetX());
		pRect->top=min(pRect->top,(int)transformedPos.GetY());
		pRect->bottom=max(pRect->bottom,(int)transformedPos.GetY());
	}
}

void CGWorldTriangleFan::DumpToVRML(CArchive& file) const
{
	char szBuffer[256];
	//write material properties
	sprintf(szBuffer,"\tMaterial {\n\t\tdiffuseColor %f %f %f\n\t\tambientColor %f %f %f\n\t}\n",	((float)GetRValue(m_crColor))/255.0f,((float)GetGValue(m_crColor))/255.0f,((float)GetBValue(m_crColor))/255.0f,	((float)GetRValue(m_crColor))/255.0f,((float)GetGValue(m_crColor))/255.0f,((float)GetBValue(m_crColor))/255.0f);
	file.WriteString(szBuffer);

	file.WriteString("\tCoordinate3	{\n\t\tpoint [\n");

	for (int i=0;i<m_iNumVertices;i++)
	{
		sprintf(szBuffer,"\t\t\t%f %f %f",m_pVertices[i].m_fPos[0],m_pVertices[i].m_fPos[1],m_pVertices[i].m_fPos[2]);
		file.WriteString(szBuffer);
		if (i!=m_iNumVertices-1)
		{
			file.WriteString(",\n");
		}
		else
		{
			file.WriteString("\n");
		}
	}
	file.WriteString("\t\t]\n\t}\n");
	file.WriteString("\tIndexedFaceSet {\n\t\tcoordIndex [\n");
	for (i=0;i<m_iNumVertices;i++)
	{
		sprintf(szBuffer,"\t\t\t%i,%i,%i,-1",0,(i+1)%m_iNumVertices,(i+2)%m_iNumVertices);
		file.WriteString(szBuffer);
		if (i!=m_iNumVertices-1)
		{
			file.WriteString(",\n");
		}
		else
		{
			file.WriteString("\n");
		}
	}
	file.WriteString("\t\t]\n\t}\n");
}

