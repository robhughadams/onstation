// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "gridctrl.h"

// Dispatch interfaces referenced by this interface
#include "Picture.h"
#include "Font.h"

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl

IMPLEMENT_DYNCREATE(CGridCtrl, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl properties

CString CGridCtrl::Get_Text()
{
	CString result;
	GetProperty(0x0, VT_BSTR, (void*)&result);
	return result;
}

void CGridCtrl::Set_Text(LPCTSTR propVal)
{
	SetProperty(0x0, VT_BSTR, propVal);
}

CString CGridCtrl::GetText()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

void CGridCtrl::SetText(LPCTSTR propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}

CString CGridCtrl::GetFontName()
{
	CString result;
	GetProperty(0x2, VT_BSTR, (void*)&result);
	return result;
}

void CGridCtrl::SetFontName(LPCTSTR propVal)
{
	SetProperty(0x2, VT_BSTR, propVal);
}

float CGridCtrl::GetFontSize()
{
	float result;
	GetProperty(0x3, VT_R4, (void*)&result);
	return result;
}

void CGridCtrl::SetFontSize(float propVal)
{
	SetProperty(0x3, VT_R4, propVal);
}

BOOL CGridCtrl::GetFontBold()
{
	BOOL result;
	GetProperty(0x4, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetFontBold(BOOL propVal)
{
	SetProperty(0x4, VT_BOOL, propVal);
}

BOOL CGridCtrl::GetFontItalic()
{
	BOOL result;
	GetProperty(0x5, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetFontItalic(BOOL propVal)
{
	SetProperty(0x5, VT_BOOL, propVal);
}

BOOL CGridCtrl::GetFontStrikethru()
{
	BOOL result;
	GetProperty(0x6, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetFontStrikethru(BOOL propVal)
{
	SetProperty(0x6, VT_BOOL, propVal);
}

BOOL CGridCtrl::GetFontUnderline()
{
	BOOL result;
	GetProperty(0x7, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetFontUnderline(BOOL propVal)
{
	SetProperty(0x7, VT_BOOL, propVal);
}

short CGridCtrl::GetRows()
{
	short result;
	GetProperty(0x8, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetRows(short propVal)
{
	SetProperty(0x8, VT_I2, propVal);
}

short CGridCtrl::GetCols()
{
	short result;
	GetProperty(0x9, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetCols(short propVal)
{
	SetProperty(0x9, VT_I2, propVal);
}

short CGridCtrl::GetFixedRows()
{
	short result;
	GetProperty(0xa, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetFixedRows(short propVal)
{
	SetProperty(0xa, VT_I2, propVal);
}

short CGridCtrl::GetFixedCols()
{
	short result;
	GetProperty(0xb, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetFixedCols(short propVal)
{
	SetProperty(0xb, VT_I2, propVal);
}

short CGridCtrl::GetRow()
{
	short result;
	GetProperty(0xc, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetRow(short propVal)
{
	SetProperty(0xc, VT_I2, propVal);
}

short CGridCtrl::GetCol()
{
	short result;
	GetProperty(0xd, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetCol(short propVal)
{
	SetProperty(0xd, VT_I2, propVal);
}

long CGridCtrl::GetScrollBars()
{
	long result;
	GetProperty(0xe, VT_I4, (void*)&result);
	return result;
}

void CGridCtrl::SetScrollBars(long propVal)
{
	SetProperty(0xe, VT_I4, propVal);
}

BOOL CGridCtrl::GetCellSelected()
{
	BOOL result;
	GetProperty(0xf, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetCellSelected(BOOL propVal)
{
	SetProperty(0xf, VT_BOOL, propVal);
}

CString CGridCtrl::GetClip()
{
	CString result;
	GetProperty(0x10, VT_BSTR, (void*)&result);
	return result;
}

void CGridCtrl::SetClip(LPCTSTR propVal)
{
	SetProperty(0x10, VT_BSTR, propVal);
}

short CGridCtrl::GetSelStartRow()
{
	short result;
	GetProperty(0x11, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetSelStartRow(short propVal)
{
	SetProperty(0x11, VT_I2, propVal);
}

short CGridCtrl::GetSelEndRow()
{
	short result;
	GetProperty(0x12, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetSelEndRow(short propVal)
{
	SetProperty(0x12, VT_I2, propVal);
}

short CGridCtrl::GetSelStartCol()
{
	short result;
	GetProperty(0x13, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetSelStartCol(short propVal)
{
	SetProperty(0x13, VT_I2, propVal);
}

short CGridCtrl::GetSelEndCol()
{
	short result;
	GetProperty(0x14, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetSelEndCol(short propVal)
{
	SetProperty(0x14, VT_I2, propVal);
}

CPicture CGridCtrl::GetPicture()
{
	LPDISPATCH pDispatch;
	GetProperty(0x15, VT_DISPATCH, (void*)&pDispatch);
	return CPicture(pDispatch);
}

void CGridCtrl::SetPicture(LPDISPATCH propVal)
{
	SetProperty(0x15, VT_DISPATCH, propVal);
}

BOOL CGridCtrl::GetGridLines()
{
	BOOL result;
	GetProperty(0x16, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetGridLines(BOOL propVal)
{
	SetProperty(0x16, VT_BOOL, propVal);
}

short CGridCtrl::GetTopRow()
{
	short result;
	GetProperty(0x17, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetTopRow(short propVal)
{
	SetProperty(0x17, VT_I2, propVal);
}

short CGridCtrl::GetLeftCol()
{
	short result;
	GetProperty(0x18, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetLeftCol(short propVal)
{
	SetProperty(0x18, VT_I2, propVal);
}

BOOL CGridCtrl::GetHighLight()
{
	BOOL result;
	GetProperty(0x19, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetHighLight(BOOL propVal)
{
	SetProperty(0x19, VT_BOOL, propVal);
}

short CGridCtrl::GetGridLineWidth()
{
	short result;
	GetProperty(0x1a, VT_I2, (void*)&result);
	return result;
}

void CGridCtrl::SetGridLineWidth(short propVal)
{
	SetProperty(0x1a, VT_I2, propVal);
}

OLE_COLOR CGridCtrl::GetBackColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_BACKCOLOR, VT_I4, (void*)&result);
	return result;
}

void CGridCtrl::SetBackColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_BACKCOLOR, VT_I4, propVal);
}

OLE_COLOR CGridCtrl::GetForeColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_FORECOLOR, VT_I4, (void*)&result);
	return result;
}

void CGridCtrl::SetForeColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_FORECOLOR, VT_I4, propVal);
}

BOOL CGridCtrl::GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}

void CGridCtrl::SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}

OLE_HANDLE CGridCtrl::GetHWnd()
{
	OLE_HANDLE result;
	GetProperty(DISPID_HWND, VT_I4, (void*)&result);
	return result;
}

void CGridCtrl::SetHWnd(OLE_HANDLE propVal)
{
	SetProperty(DISPID_HWND, VT_I4, propVal);
}

COleFont CGridCtrl::GetFont()
{
	LPDISPATCH pDispatch;
	GetProperty(DISPID_FONT, VT_DISPATCH, (void*)&pDispatch);
	return COleFont(pDispatch);
}

void CGridCtrl::SetFont(LPDISPATCH propVal)
{
	SetProperty(DISPID_FONT, VT_DISPATCH, propVal);
}

long CGridCtrl::GetFillStyle()
{
	long result;
	GetProperty(0x1b, VT_I4, (void*)&result);
	return result;
}

void CGridCtrl::SetFillStyle(long propVal)
{
	SetProperty(0x1b, VT_I4, propVal);
}

long CGridCtrl::GetBorderStyle()
{
	long result;
	GetProperty(0x1c, VT_I4, (void*)&result);
	return result;
}

void CGridCtrl::SetBorderStyle(long propVal)
{
	SetProperty(0x1c, VT_I4, propVal);
}

long CGridCtrl::GetMousePointer()
{
	long result;
	GetProperty(0x27, VT_I4, (void*)&result);
	return result;
}

void CGridCtrl::SetMousePointer(long propVal)
{
	SetProperty(0x27, VT_I4, propVal);
}

CPicture CGridCtrl::GetMouseIcon()
{
	LPDISPATCH pDispatch;
	GetProperty(0x28, VT_DISPATCH, (void*)&pDispatch);
	return CPicture(pDispatch);
}

void CGridCtrl::SetMouseIcon(LPDISPATCH propVal)
{
	SetProperty(0x28, VT_DISPATCH, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl operations

void CGridCtrl::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

long CGridCtrl::GetRowHeight(short Index)
{
	long result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x1f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetRowHeight(short Index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x1f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, nNewValue);
}

long CGridCtrl::GetColWidth(short Index)
{
	long result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetColWidth(short Index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x20, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, nNewValue);
}

short CGridCtrl::GetColAlignment(short Index)
{
	short result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x21, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetColAlignment(short Index, short nNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2;
	InvokeHelper(0x21, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, nNewValue);
}

short CGridCtrl::GetFixedAlignment(short Index)
{
	short result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x22, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetFixedAlignment(short Index, short nNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2;
	InvokeHelper(0x22, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, nNewValue);
}

long CGridCtrl::GetRowPos(short Index)
{
	long result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x23, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetRowPos(short Index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x23, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, nNewValue);
}

long CGridCtrl::GetColPos(short Index)
{
	long result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x24, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetColPos(short Index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x24, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, nNewValue);
}

BOOL CGridCtrl::GetRowIsVisible(short Index)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x25, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetRowIsVisible(short Index, BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_BOOL;
	InvokeHelper(0x25, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, bNewValue);
}

BOOL CGridCtrl::GetColIsVisible(short Index)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x26, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		Index);
	return result;
}

void CGridCtrl::SetColIsVisible(short Index, BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_BOOL;
	InvokeHelper(0x26, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, bNewValue);
}

short CGridCtrl::AddItem(LPCTSTR Item, const VARIANT& Index)
{
	short result;
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		Item, &Index);
	return result;
}

short CGridCtrl::RemoveItem(short RemRow)
{
	short result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		RemRow);
	return result;
}

void CGridCtrl::Refresh()
{
	InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}