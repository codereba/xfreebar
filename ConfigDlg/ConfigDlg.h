/*
 * Copyright 2010 www.codereba.com
 *
 * This file is part of xfreebar.
 *
 * xfreebar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xfreebar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xfreebar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CONFIG_DLG_H__
#define __CONFIG_DLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <atlddx.h>
#include <vector>
#include <atlframe.h>
#include <atldlgs.h>
#include "xml.h"
#include "CmnUtil.h"
#include "ButtonXP.h"

#define DEFAULT_NORM_BMP _T("xicon.ico")
#define DEFAULT_HOT_BMP _T("xicon.ico")
#define BUTTON_CAPTION_ATRRIB _T("caption")
#define BUTTON_COMMAND_ATRRIB _T("command")
#define BUTTON_HINT_ATTRIB _T("hint")
#define BUTTON_IMG_ATTRIB _T("img")
#define BUTTON_HOTIMG_ATTRIB _T("hot_img")
#define COMMAND_URL_ATTRIB _T("href")
#define COMMAND_NAME_ATTRIB _T("name")
#define EXTENT_XFREEBAR_CONFIG _T("xfreebar_config_ex.xml")
#define ADD_BUTTON_COMMAND _T("Add Button")
#define ADD_MENU_COMMAND _T("Add Menu")
#define COMMAND_POSTFIX _T("URL")
#define XML_ROOT _T("TOOLBAR")
#define XML_CHILD _T("BUTTON")
#define XML_URL_NODE _T("WEBJUMP")
#define XML_MENU_NODE _T("MENU")
#define XML_MENUITEM_NODE _T("ITEM")
#define XML_COMMANDS_NODE _T("COMMANDS")

#define WM_SETMAINMENU ( WM_USER + 0x1001 )
#define WM_SETROOTNODE ( WM_USER + 0x1002 )
#define WM_SETXMLDOC ( WM_USER + 0x1003 )
#define WM_GETBTNICON ( WM_USER + 0x1004 )
#define WM_GETFAVICONFILE ( WM_USER + 0x1005 )
#define WM_SAVEFAVICONFILE ( WM_USER + 0x1006 )

class CEncodeMgr
{

#define DEF_STRING_STATE true
public:
	bstr_t strToEnc;
	bool IsEncoded;
	CEncodeMgr() : IsEncoded(DEF_STRING_STATE){}

	bool EncodeString()
	{
		if(IsEncoded)
			return true;

		CString strTmp;
		if(!XmlEncode(strToEnc, strTmp.GetBufferSetLength(512),512))
		{
			strTmp.ReleaseBuffer();
			IsEncoded = false;
			return false;
		}
		else
		{
			strTmp.ReleaseBuffer();
			strToEnc = strTmp;
			IsEncoded = true;
			return true;
		}
	}

	bool DecodeString()
	{
		if(!IsEncoded)
			return true;

		CString strTmp;
		if(!XmlDecode(strToEnc, strTmp.GetBufferSetLength(512),512))
		{
			strTmp.ReleaseBuffer();
			IsEncoded = true;
			return false;
		}
		else
		{
			strTmp.ReleaseBuffer();
			strToEnc = strTmp;
			IsEncoded = false;
			return true;
		}
	}

	bstr_t& GetString()
	{
		return strToEnc;
	}

	CEncodeMgr& operator = (LPCTSTR lpszVal)
	{
		IsEncoded = DEF_STRING_STATE;
		strToEnc = lpszVal;
		return *this;
	}

	operator LPCTSTR () throw()
	{
		return (LPCTSTR)strToEnc;
	}

	operator bstr_t () throw()
	{
		return strToEnc;
	}

	CEncodeMgr& Init(LPCTSTR lpszVal)
	{
		IsEncoded = DEF_STRING_STATE;
		strToEnc = lpszVal;
		return *this;
	}
	
	inline BSTR& GetBSTR()  
	{
		return strToEnc.GetBSTR();
	}

};

LPCTSTR GetPathFormFileName(LPCTSTR strPath)
{
	LPTSTR lpszPath = (LPTSTR)strPath;
	UINT uCount = _tcslen(lpszPath);
	TCHAR *cTmp;
	for( cTmp = lpszPath + uCount - 1; (TCHAR)*cTmp != '\\' && (TCHAR)*cTmp != '/' && uCount > 0 ; cTmp --)
	{
		*cTmp = 0;
		uCount--;
	}
	if((TCHAR)*cTmp == '\\' || (TCHAR)*cTmp == '/')
		*cTmp = 0;
	return lpszPath;
}

BOOL DrawImage(HDC hDC, CBitmap& tmpBmp)
{
	ATLASSERT(tmpBmp != (HBITMAP)NULL);

	CDC memDC = CreateCompatibleDC(hDC);
	//memDC.SaveDC();
	BITMAP BmpInfo;
	tmpBmp.GetBitmap(&BmpInfo);
	memDC.SelectBitmap(tmpBmp);
	::BitBlt(hDC, 0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,memDC, 0,0,SRCCOPY);
	//memDC.RestoreDC(1);
	return TRUE;
}

_bstr_t RandString(_bstr_t strOrg)
{
	UINT uCalc = 0;
	char *lpszOrg = strOrg;
	UINT uValue;
	for(ULONG i = 0; i < strOrg.length(); i++)
	{
		uValue = (uCalc) % 1024;
		uCalc += *(lpszOrg + i) / (uValue ? uValue : 1);
	}
	char szBuff[4];
	itoa(uCalc, szBuff, 10);
	return szBuff;
}

BOOL CheckSameDir(LPCTSTR strSrc, LPCTSTR strDest)
{
	CString srcPath = strSrc;
	srcPath = GetPathFormFileName(srcPath);
	if(srcPath == "")
		return TRUE;

	CString destPath = strDest;
	destPath = GetPathFormFileName(destPath);
	if(srcPath.CompareNoCase(destPath) == 0)
		return TRUE;
	return FALSE;
}

#define MAX_BUFF_SIZE 1024
BOOL CopyResFile(LPCTSTR strSrc, LPCTSTR strDest)
{
	BOOL bRet = TRUE;
	HANDLE hDestFile = INVALID_HANDLE_VALUE;
	HANDLE hSrcFile = INVALID_HANDLE_VALUE;

	do
	{
		hSrcFile = CreateFile( strSrc
			, GENERIC_READ
			, FILE_SHARE_READ
			, NULL
			, OPEN_EXISTING
			, 0
			, 0
			);

		if(hSrcFile == INVALID_HANDLE_VALUE)
		{
			bRet = FALSE;
			break;
		}

		hDestFile = CreateFile( strDest
			, GENERIC_READ | GENERIC_WRITE
			, FILE_SHARE_READ
			, NULL
			, CREATE_ALWAYS
			, 0
			, 0
			);

		if(hDestFile == INVALID_HANDLE_VALUE)
		{
			bRet = FALSE;
			break;
		}

		//LARGE_INTEGER liOff;
		//liOff.QuadPart = 0;
		//liOff.LowPart = ::SetFilePointer(hDestFile, liOff.LowPart, &liOff.HighPart,
		//	(DWORD)0x0);
		//if (liOff.LowPart  == (DWORD)-1)
		//{
		//	if (::GetLastError() != NO_ERROR)
		//	{
		//		bRet = FALSE;
		//		break;
		//	}
		//}

		//if (!::SetEndOfFile(hDestFile))
		//{
		//	bRet = FALSE;
		//	break;
		//}

		ULARGE_INTEGER liSize;
		liSize.LowPart = ::GetFileSize(hSrcFile, &liSize.HighPart);
		if (liSize.LowPart == (DWORD)-1)
		{
			if (::GetLastError() != NO_ERROR)
			{
				bRet = FALSE;
				break;
			}
		}

		DWORD nFileSize = liSize.QuadPart;
		DWORD nReadSize;
		DWORD dwRead;
		DWORD nWritten;
		BYTE *pBuff = new BYTE[MAX_BUFF_SIZE];
		while(nFileSize > 8)
		{
			if(nFileSize > MAX_BUFF_SIZE)
			{
				nReadSize = MAX_BUFF_SIZE;
			}
			else
			{
				nReadSize = nFileSize;
			}

			if (!::ReadFile(hSrcFile, pBuff, nReadSize, &dwRead, NULL))
			{
				bRet = FALSE;
				break;
			}

			if(dwRead != nReadSize)
			{
				bRet = FALSE;
				break;
			}

			if (!::WriteFile(hDestFile, pBuff, nReadSize, &nWritten, NULL))
			{
				bRet = FALSE;
				break;
			}

			// Win95 will not return an error all the time (usually DISK_FULL)
			if (nWritten != nReadSize)
			{
				bRet = FALSE;
				break;
			}

			nFileSize -= nReadSize;
		}
		delete[] pBuff;
	}while(0);

	if(hDestFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hDestFile);
	}
	if(hSrcFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hSrcFile);
	}
	return bRet;
}

BOOL OpenFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
					LPCTSTR lpszDefExt = NULL,
					LPCTSTR lpszFileName = NULL,
					LPCTSTR lpszFileTitle = NULL,
					DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					LPCTSTR lpszFilter = NULL,
					HWND hWndParent = NULL)
{
	OPENFILENAME m_ofn;
	memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
	m_ofn.lStructSize = sizeof(m_ofn);
#if (_WIN32_WINNT >= 0x0500)
	// adjust struct size if running on older version of Windows
	if(AtlIsOldWindows())
	{
		ATLASSERT(sizeof(m_ofn) > OPENFILENAME_SIZE_VERSION_400);   // must be
		m_ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
	}
#endif //(_WIN32_WINNT >= 0x0500)
	m_ofn.lpstrFile = (LPTSTR)lpszFileName;
	m_ofn.nMaxFile = _MAX_PATH;
	m_ofn.lpstrDefExt = lpszDefExt;
	m_ofn.lpstrFileTitle = (LPTSTR)lpszFileTitle;
	m_ofn.nMaxFileTitle = _MAX_FNAME;
#ifndef _WIN32_WCE
	m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLESIZING;
#else // CE specific
	m_ofn.Flags = dwFlags | OFN_EXPLORER;
#endif //!_WIN32_WCE
	m_ofn.lpstrFilter = lpszFilter;
#if (_ATL_VER >= 0x0700)
	m_ofn.hInstance = ATL::_AtlBaseModule.GetResourceInstance();
#else //!(_ATL_VER >= 0x0700)
	m_ofn.hInstance = _Module.GetResourceInstance();
#endif //!(_ATL_VER >= 0x0700)
	HWND hTmpWnd = ::GetActiveWindow();
	m_ofn.hwndOwner = hWndParent ? hWndParent : hTmpWnd;

	BOOL bRet;
	if(bOpenFileDialog)
		bRet = ::GetOpenFileName(&m_ofn);
	else
		bRet = ::GetSaveFileName(&m_ofn);
	return bRet;
}

struct ImgListItem
{
	LPCTSTR lpszString;
	HBITMAP hBmp;
};

class CImgListBoxImpl : public CWindowImpl<CImgListBoxImpl, CListBox>,
								COwnerDraw<CImgListBoxImpl>
{
public:

	BEGIN_MSG_MAP(CImgListBoxImpl)
		MESSAGE_HANDLER(LB_ADDSTRING, OnAddString)
		MESSAGE_HANDLER(LB_INSERTSTRING, OnAddString)
		MESSAGE_HANDLER(LB_DELETESTRING, OnDeleteString)
		MESSAGE_HANDLER(LB_RESETCONTENT, OnResetContent)
	END_MSG_MAP()

	CImgListBoxImpl()
	{
		m_nMaxWidth = 0;
	}

	~CImgListBoxImpl()
	{

	}

	//BOOL AddItem(HBITMAP hBmp, LPCTSTR lpszString)
	//{
	//	return TRUE;
	//}

	
	LRESULT OnResetContent (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_nMaxWidth = 0;
		SetHorizontalExtent(0);
		return 0;
	}

	LRESULT OnDeleteString (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT lRect;
		GetWindowRect(&lRect);

		DefWindowProc(uMsg, wParam,lParam);

		INT32 nBoxWidth = lRect.right - lRect.left;
		m_nMaxWidth = 0;

		SIZE sSize;
		CClientDC myDC(m_hWnd);

		INT32 i;
		TCHAR szEntry[512];

		//SCROLLINFO scrollInfo;
		//memset(&scrollInfo, 0, sizeof(SCROLLINFO));
		//scrollInfo.cbSize = sizeof(SCROLLINFO);
		//scrollInfo.fMask  = SIF_ALL;
		//GetScrollInfo(SB_VERT, &scrollInfo, SIF_ALL);

		//INT32 nScrollWidth = 0;
		//if(GetCount() > 1 && ((INT32)scrollInfo.nMax 
		//> = (INT32)scrollInfo.nPage))
		//{
		//	nScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
		//}

		CFontHandle pListBoxFont = GetFont();
		if(pListBoxFont != NULL)
		{
			CFontHandle pOldFont = 
				myDC.SelectFont(pListBoxFont);
			for (i = 0; i < GetCount(); i++)
			{
				GetText(i, szEntry);
				GetTextExtentPoint32(myDC.m_hDC, 
					szEntry, strlen(szEntry), &sSize);
				m_nMaxWidth = max(m_nMaxWidth, (INT32)sSize.cx);
			}
			myDC.SelectFont(pOldFont);
		}

		if(m_nMaxWidth > nBoxWidth) // 显示水平滚动条
		{
			ShowScrollBar(SB_HORZ, TRUE);
			SetHorizontalExtent(m_nMaxWidth + 3);
			return 0;
		}
		else
		{
			ShowScrollBar(SB_HORZ, FALSE);
		}
		return 0;
	}

	LRESULT OnAddString (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DefWindowProc(uMsg, wParam,lParam);

		LPCTSTR lpszItem = (LPCTSTR)lParam;
		SCROLLINFO scrollInfo;
		memset(&scrollInfo, 0, sizeof(SCROLLINFO));
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask  = SIF_ALL;
		GetScrollInfo(SB_VERT, &scrollInfo);

		INT32 nScrollWidth = 0;
		if(GetCount() > 1 && ((INT32)scrollInfo.nMax 
		>= (INT32)scrollInfo.nPage))
		{
			nScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
		}

		SIZE      sSize;
		CClientDC myDC(m_hWnd);

		CFontHandle pListBoxFont = GetFont();
		if(pListBoxFont != NULL)
		{
			CFontHandle pOldFont = 
				myDC.SelectFont(pListBoxFont);

			GetTextExtentPoint32(myDC, 
				lpszItem, _tcslen(lpszItem), &sSize);
			m_nMaxWidth = max(m_nMaxWidth, (INT32)sSize.cx);
			SetHorizontalExtent(m_nMaxWidth + nScrollWidth + 3);
			myDC.SelectFont(pOldFont);
		}
		return 0;
	}

protected:
	std::vector<ImgListItem> ItemList;
	INT32 m_nMaxWidth;
};

class CBmpStaticImpl : public CWindowImpl<CBmpStaticImpl, CStatic>
                      //public COwnerDraw<CBmpStaticImpl>
{
public:
    CBmpStaticImpl()
	{
		m_hIcon = NULL;
	}

    BEGIN_MSG_MAP(CBmpStaticImpl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
        //CHAIN_MSG_MAP_ALT(COwnerDraw<CBmpStaticImpl>, 1)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

#define ICON_MAX_LEN 32
#define BITMAP_MAX_LEN 16

	BOOL CheckAndLoadImage(LPCTSTR pBmpPath, BOOL IsCurDir = FALSE, BOOL bIsIcon = TRUE )
	{
		if((HBITMAP)m_bmp)
			m_bmp.DeleteObject();
		if(m_hIcon)
		{
			DestroyIcon(m_hIcon);
			m_hIcon = NULL;
		}

		bstr_t szTmpPath = pBmpPath;
		if(szTmpPath.length() == 0)
		{
			Invalidate(0);
			return TRUE;
		}

		if(IsCurDir)
		{
			szTmpPath = getprogpath(szTmpPath);
		}

		HANDLE hImg;
		UINT uType;
		LoadImageDirect(szTmpPath, hImg, uType);
		if(!hImg)
		{
			MessageBox(_T("The image file is not exist."));
			Invalidate(0);
			return FALSE;
		}

		if( FALSE == bIsIcon )
		{
			if(uType == IMAGE_BITMAP)
			{
				m_bmp = (HBITMAP)hImg;
			}
			else if(uType == IMAGE_ICON)
			{
				m_hIcon = (HICON)hImg;
			}
			goto CheckAndLoadImage_Exit;
		}

		if(uType == IMAGE_BITMAP)
		{
			m_bmp = (HBITMAP)hImg;
			BITMAP BmpInfo;
			m_bmp.GetBitmap(&BmpInfo);
			if(BmpInfo.bmWidth > BITMAP_MAX_LEN || BmpInfo.bmHeight > BITMAP_MAX_LEN)
			{
				m_bmp.DeleteObject();
				MessageBox(_T("The size of the image can't great than 16 * 16"), _T("Image error"));
				Invalidate(0);
				return FALSE;
			}
		}
		else if(uType == IMAGE_ICON)
		{
			m_hIcon = (HICON)hImg;
			SIZE IconSize;
			IconSize = GetIconSize(m_hIcon);

			if(IconSize.cx > ICON_MAX_LEN || IconSize.cy > ICON_MAX_LEN)
			{
				DestroyIcon(m_hIcon);
				m_hIcon = NULL;
				MessageBox(_T("The size of the icon can't great than 32 * 32"), _T("Image error"));
				Invalidate(0);
				return FALSE;
			}
		}

CheckAndLoadImage_Exit:
		Invalidate(0);
		return TRUE;
	}
	
	LRESULT OnPaint (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		CPaintDC paintDC(this->m_hWnd);
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		CLIENT_OFFSET(rcWnd);

		CPen BordPen;
		CPenHandle OldPen;
		CBrushHandle OldBrh;

		BordPen.CreatePen(PS_DOT, 1, RGB(10,10,10));
		OldPen = paintDC.SelectPen(BordPen);
		OldBrh = paintDC.SelectBrush((HBRUSH)::GetStockObject(HOLLOW_BRUSH));

		paintDC.Rectangle(&rcWnd);
		paintDC.SelectPen(OldPen);
		paintDC.SelectBrush(OldBrh);

		if(m_bmp == (HBITMAP)NULL)
		{
			paintDC.FillRect(&rcWnd, ::GetSysColorBrush(COLOR_BTNFACE));
			//bHandled = FALSE;
		}
		
		if(m_hIcon != NULL)
		{
			DrawIconEx(paintDC,0, 0, m_hIcon, rcWnd.right- rcWnd.left,rcWnd.bottom - rcWnd.top,0, 0, DI_NORMAL);
		}
		else if(m_bmp != (HBITMAP)NULL)
		{
			DrawImage(paintDC, m_bmp);
		}
		return 0;
	}
    //void DrawItem ( LPDRAWITEMSTRUCT lpdis );

	VOID SetBtnIcon( HICON hBtnIcon )
	{
		m_hIcon = hBtnIcon;
	}

	VOID SetBmp( HBITMAP hBmp )
	{
		m_bmp.Attach( hBmp );
	}

protected:
    CBitmap m_bmp;
	HICON m_hIcon;
};

UINT ConvertFilterString(LPCTSTR pszFilter)
{
	// Translate filter into commdlg format (lots of \0)
	if (pszFilter != NULL)
	{
		LPTSTR pch = (LPTSTR)pszFilter; // modify the buffer in place
		// MFC delimits with '|' not '\0'
		while ((pch = _tcschr(pch, '|')) != NULL)
			*pch++ = '\0';
		// do not call ReleaseBuffer() since the string contains '\0' characters
	}
	return 0;
}

LPCTSTR GetFileNameFromPath(LPCTSTR strPath)
{
	LPTSTR lpszPath = (LPTSTR)strPath;
	UINT uCount = _tcslen(lpszPath);
	TCHAR *cTmp;
	for(cTmp = lpszPath + uCount; (TCHAR)*cTmp != '\\' && (TCHAR)*cTmp != '/' && uCount > 0 ; cTmp --)
		uCount--;
	if((TCHAR)*cTmp == '\\' || (TCHAR)*cTmp == '/')
		return cTmp + 1;
	return cTmp;
}

BOOL CheckSameCommand(CXmlNode* pToolbarNode, bstr_t &strCmp)
{
	CXmlNodePtr pCommandsItem;
	CXmlNodesPtr pWebJumpNodes;
	pCommandsItem = pToolbarNode->GetChild(XML_COMMANDS_NODE);
	if(!pCommandsItem->IsNull())
	{
		pWebJumpNodes = pCommandsItem->GetChildren();
		if(!pWebJumpNodes->IsNull())
		{
			XString strTmp;
			for(INT32 i=0; i < pWebJumpNodes->GetCount(); i++)
			{
				strTmp = (LPTSTR)pWebJumpNodes->operator [](i)->GetAttribute(COMMAND_NAME_ATTRIB);
				if(strTmp == (LPCTSTR)strCmp)
					return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL  GetCurrectCommandName(bstr_t &strCommand, bstr_t &strCaption, CXmlNode* pToolbarNode)
{
	bstr_t strPrefix;
	CString strPostfix;
	bstr_t strTmp;
	if(strCaption.length() == 0)
		strPrefix = _T("COMMAND");
	else
		strPrefix = strCaption;
	if(CheckSameCommand(pToolbarNode, strPrefix))
	{
		strCommand = strPrefix;
		return TRUE;
	}

	for(INT32 i = 0; i < 255; i++)
	{
		itoa(i, strPostfix.GetBufferSetLength(3), 10);
		strPostfix.ReleaseBuffer();
		strTmp = strPrefix + strPostfix.GetBuffer(0);
		if(CheckSameCommand(pToolbarNode, strTmp))
		{
			strCommand = strTmp;
			return TRUE;
		}
	}
	return FALSE;
}

#define DLG_BK_CLR RGB( 255, 255, 255 )
class CUITab : public CWindow
{
public:
	CUITab(HWND hWnd = NULL) throw() :
		CWindow(hWnd)
	{
		m_BkBr.CreateSolidBrush( DLG_BK_CLR );
	}

	CUITab& operator=(HWND hWnd) throw()
	{
		m_hWnd = hWnd;
		return *this;
	}
	
	~CUITab()
	{
		m_BkBr.DeleteObject();
	}

public:
	LRESULT OnCtlColor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		bHandled = TRUE;
		return (LRESULT)(HBRUSH)m_BkBr;
	}
protected:
	CBrush m_BkBr;
};

class CAddMenuTab :
	public CDialogImpl<CAddMenuTab, CUITab>,
	public CWinDataExchange<CAddMenuTab>
{
public:
	enum { IDD = IDD_TAB_ADDMENU };

	CAddMenuTab()
	{
		m_strFilter = _T("ICON image(*.ico)|*.ico|Bitmap image(*.bmp)|*.bmp||"); //|GIF image(*.gif)|*.gif|
		ConvertFilterString(m_strFilter);
		//m_strImage = _T("");
		m_strUrl = _T("");
		m_strDesc = _T("");
		m_strCaption = _T("");
		m_pMainMenu = NULL;
		m_pXmlDoc = NULL;
		m_pRootNode = NULL;
	}

	BEGIN_MSG_MAP(CAddMenuTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		COMMAND_HANDLER(IDC_BUTTON_MENU_ADD_, BN_CLICKED, OnOKBtnClicked)
		COMMAND_HANDLER(IDC_BUTTON_MENU_DELETE_, BN_CLICKED, OnDelBtnClicked)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint);
		//MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor )
		//MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor )
		//COMMAND_HANDLER(IDC_STATIC_IMG_MENU, STN_CLICKED, OnNorImgClicked)
		COMMAND_HANDLER(IDC_LIST_ALL_MENUITEM, LBN_SELCHANGE, OnListSelChange)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CAddMenuTab)
		DDX_TEXT(IDC_EDIT_CMD_MENU, m_strUrl.GetBSTR());
		DDX_TEXT(IDC_EDIT_TIP_MENU, m_strDesc.GetBSTR());
		DDX_TEXT(IDC_EDIT_TITLE_MENU, m_strCaption.GetBSTR());
	END_DDX_MAP()

	void SetMainMenu(CXmlNode* pMenu)
	{
		m_pMainMenu = pMenu;
	}
	void SetRootNode(CXmlNode* pRootNode)
	{
		m_pRootNode = pRootNode;
	}
	void SetXmlDoc(CXml* pXmlDoc)
	{
		m_pXmlDoc = pXmlDoc;
	}

	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_pMainMenu == NULL)
		{
			m_ListBox.ResetContent();
			return 0;
		}
		if(m_pMainMenu->IsNull())
		{
			m_ListBox.ResetContent();
			return 0;
		}

		if(!wParam)
		{
			return 0;
		}

		LoadAllString();
		m_ListBox.SetCurSel(0);
		return 0;
	}

	//HBRUSH hBakBr;
#define XCONFIG_DLG_BK_CLR RGB( 240, 240, 240 )
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//if (1 == GetPropertySheet().GetActiveIndex())
		//	GetPropertySheet().CenterWindow();

		//m_MenuStc.SubclassWindow(GetDlgItem(IDC_STATIC_IMG_MENU));
		m_ListBox.SubclassWindow( GetDlgItem(IDC_LIST_ALL_MENUITEM));
		m_BtnOK.SubclassWindow( GetDlgItem( IDC_BUTTON_MENU_ADD_ ) );
		m_BtnCancel.SubclassWindow( GetDlgItem( IDC_BUTTON_MENU_DELETE_ ) );
		ATLASSERT( NULL != m_BtnOK.m_pfnSuperWindowProc );
		ATLASSERT( NULL != m_BtnCancel.m_pfnSuperWindowProc );
		//hBakBr = ::CreateSolidBrush( XCONFIG_DLG_BK_CLR ); 
		return 1;  // Let the system set the focus
	}

	BOOL LoadAllString()
	{
		if(m_pMainMenu == NULL || m_pMainMenu->IsNull())
			return 0;
		m_ListBox.ResetContent();
		m_ListBox.AddString(ADD_MENU_COMMAND);
		bstr_t strCommand;
		for(CXmlNodePtr pMenuItem = m_pMainMenu->GetFirstChild(); !(pMenuItem->IsNull()); pMenuItem = pMenuItem->GetNextSibling())
		{
			if(pMenuItem->CheckName(XML_MENUITEM_NODE))
			{
				strCommand = pMenuItem->GetAttribute(BUTTON_COMMAND_ATRRIB);
				m_ListBox.AddString(strCommand);
			}
		}
		return TRUE;
	}

	//LRESULT OnNorImgClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	//{
	//	// TODO: 在此添加控件通知处理程序代码
	//	CString strTmp;
	//	BOOL Ret = OpenFileDialog(TRUE, 0, strTmp.GetBufferSetLength(_MAX_PATH), 0,  OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, m_strFilter);
	//	strTmp.ReleaseBuffer();
	//	if (Ret)
	//	{
	//		m_strImage = strTmp;
	//		if(!m_MenuStc.CheckAndLoadImage(m_strImage))
	//			m_strImage = "";
	//	}
	//	return 0;
	//}

	LRESULT OnListSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if(m_pMainMenu == NULL || m_pRootNode == NULL || m_pXmlDoc == NULL)
			return 0;
		INT32 nIndex = m_ListBox.GetCurSel();

		bstr_t strTmp;
		m_ListBox.GetTextBSTR(nIndex, strTmp.GetBSTR());
		bstr_t strTmp2(ADD_MENU_COMMAND);
		if(strTmp == strTmp2)
		{
			//if(!m_MenuStc.CheckAndLoadImage(m_strImage, TRUE))
			//	m_strImage = "";
			return 0;
		}

		CXmlNodePtr pTmpNode = m_pMainMenu->GetChild(BUTTON_COMMAND_ATRRIB, strTmp);
		ATLASSERT(!pTmpNode->IsNull());

		CXmlNodePtr pCommandsNode = m_pRootNode->GetChild(XML_COMMANDS_NODE);
		ATLASSERT(!pCommandsNode->IsNull());

		CXmlNodePtr pWebJumpNode = pCommandsNode->GetChild(COMMAND_NAME_ATTRIB, strTmp);
		ATLASSERT(!pWebJumpNode->IsNull());

		
		m_strUrl = pWebJumpNode->GetAttribute(COMMAND_URL_ATTRIB);

		m_strDesc = pTmpNode->GetAttribute(BUTTON_HINT_ATTRIB);
		m_strCaption = pTmpNode->GetAttribute(BUTTON_CAPTION_ATRRIB);
		//m_strImage = pTmpNode->GetAttribute(BUTTON_IMG_ATTRIB);

		//if(!m_MenuStc.CheckAndLoadImage(m_strImage, TRUE))
		//	m_strImage = "";
		DoDataExchange(FALSE);
		return 0;
	}

	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		//bHandled = TRUE;
		return 0;
	}

	LRESULT OnDelBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if(m_pMainMenu == NULL || m_pRootNode == NULL || m_pXmlDoc == NULL)
			return 0;
		DoDataExchange(TRUE);

		BOOL bRet = TRUE;
		bstr_t strCulSel;
		m_ListBox.GetTextBSTR(m_ListBox.GetCurSel(), strCulSel.GetBSTR());
		bstr_t strCmp(ADD_MENU_COMMAND);
		if(strCulSel != strCmp)
		{
			do
			{
				CXmlNodePtr pItemNode = m_pMainMenu->GetChild(BUTTON_COMMAND_ATRRIB, strCulSel);
				if(pItemNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				if(!m_pMainMenu->RemoveChild(pItemNode))
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pCommandsNode = m_pRootNode->GetChild(_T("COMMANDS"));

				if(pCommandsNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pWebJumpNode = pCommandsNode->GetChild(COMMAND_NAME_ATTRIB, strCulSel);

				if(pWebJumpNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				if(!pCommandsNode->RemoveChild(pWebJumpNode))
				{
					bRet = FALSE;
					break;
				}

				if(!m_pXmlDoc->Save())
				{
					bRet = FALSE;
					break;
				}
				LoadAllString();
			}while(0);
			if(!bRet)
			{
				MessageBox("Delete menu item failed.");
			}
			else
			{
				MessageBox("Delete menu itme success.");
			}
			if(m_ListBox.GetCount() > 1)
			{
				m_ListBox.SetCurSel(1);
			}
		}
		return 0;
	}

	LRESULT OnOKBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		// TODO: 在此添加控件通知处理程序代码
		if(m_pMainMenu == NULL || m_pRootNode == NULL || m_pXmlDoc == NULL)
			return 0;
		DoDataExchange(TRUE);

		BOOL bRet = TRUE;
		bstr_t strCulSel;
		m_ListBox.GetTextBSTR(m_ListBox.GetCurSel(), strCulSel.GetBSTR());
		bstr_t strCmp(ADD_MENU_COMMAND);
		if(strCulSel == strCmp)
		{
			do
			{
				CXmlNodePtr pMenuItem = m_pMainMenu->AppendChild(XML_MENUITEM_NODE);
				if(pMenuItem->IsNull())
				{
					bRet = FALSE;
					break;
				}

				bstr_t strCommand;
				if(!GetCurrectCommandName(strCommand, m_strCaption, m_pRootNode))
				{
					pMenuItem->Remove();
					bRet = FALSE;
					break;
				}

				pMenuItem->SetAttribute(_T("caption"), m_strCaption);
				pMenuItem->SetAttribute(_T("hint"), m_strDesc);
				pMenuItem->SetAttribute(_T("command"), strCommand);

				//_bstr_t strTmp;
				//strTmp = GetFileNameFromPath(m_strImage);
				//strTmp  = getprogpath(strTmp);
				//if(!CheckSameDir(m_strImage, strTmp))
				//{
				//	if(!CopyResFile(m_strImage, strTmp))
				//	{
				//		pMenuItem->Remove();
				//		bRet = FALSE;
				//		break;
				//	}
				//}
				//pMenuItem->SetAttribute(_T("img"), GetFileNameFromPath(m_strImage));

				CXmlNodePtr pCommandsNode = m_pRootNode->GetChild(_T("COMMANDS"));

				if(pCommandsNode->IsNull())
				{
					pMenuItem->Remove();
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pWebJumpNode = pCommandsNode->AppendChild(_T("WEBJUMP"));

				if(pWebJumpNode->IsNull())
				{
					pMenuItem->Remove();
					bRet = FALSE;
					break;
				}

				pWebJumpNode->SetAttribute(_T("name"), strCommand);
				pWebJumpNode->SetAttribute(_T("href"), m_strUrl);

				m_pMainMenu->SetAttribute(_T("type"), _T("split"));
				
				if(!m_pXmlDoc->Save())
				{
					pWebJumpNode->Remove();
					pMenuItem->Remove();
					bRet = FALSE;
					break;
				}
				LoadAllString();
			}while(0);
			if(!bRet)
			{
				MessageBox("Add menu item failed.");
			}
			else
			{
				MessageBox("Add menu itme success.");
			}
		}
		else
		{
			do
			{
				CXmlNodePtr pCommandsNode = m_pRootNode->GetChild(_T("COMMANDS"));
				if(pCommandsNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pMenuItem = m_pMainMenu->GetChild(BUTTON_COMMAND_ATRRIB, strCulSel);
				if(pMenuItem->IsNull())
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pWebJumpNode = pCommandsNode->GetChild(COMMAND_NAME_ATTRIB, strCulSel);
				if(pWebJumpNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				bstr_t strCommand = pMenuItem->GetAttribute(_T("command"));
				pMenuItem->SetAttribute(_T("caption"), m_strCaption);
				pMenuItem->SetAttribute(_T("hint"), m_strDesc);

				//_bstr_t strTmp;
				//strTmp = GetFileNameFromPath(m_strImage);
				//strTmp  = getprogpath(strTmp);
				//if(!CheckSameDir(m_strImage, strTmp))
				//{
				//	if(!CopyResFile(m_strImage, strTmp))
				//	{
				//		bRet = FALSE;
				//		break;
				//	}
				//}
				//pMenuItem->SetAttribute(_T("img"), GetFileNameFromPath(m_strImage));

				pWebJumpNode->SetAttribute(_T("name"), strCommand);
				pWebJumpNode->SetAttribute(_T("href"), m_strUrl);
				if(!m_pXmlDoc->Save())
				{
					bRet = FALSE;
					break;
				}
			}while(0);

			if(!bRet)
			{
				MessageBox("Modify menu item failed.");
			}
			else
			{
				MessageBox("Modify menu itme success.");
			}
		}
		return bRet;
	}

protected:
	CXmlNode* m_pMainMenu;
	CXmlNode* m_pRootNode;
	CXml* m_pXmlDoc;
	CImgListBoxImpl m_ListBox;
	//CBmpStaticImpl m_MenuStc;
	CButtonXP m_BtnOK;
	CButtonXP m_BtnCancel;
	_bstr_t m_strUrl;
	_bstr_t m_strDesc;
	_bstr_t m_strCaption;
	//_bstr_t m_strImage;
	_bstr_t m_strFilter;
};

class CAddButtonTab :
	public CDialogImpl<CAddButtonTab, CUITab>,
	public CWinDataExchange<CAddButtonTab>
	{

public:
	_bstr_t m_strUrl;
	_bstr_t m_strDesc;
	_bstr_t m_strCaption;
	_bstr_t m_strImage;
	_bstr_t m_strHotImage;
	_bstr_t m_strFilter;

	CButtonXP m_BtnOK;
	CButtonXP m_BtnCancel;
	CImgListBoxImpl m_ListBox;
	CBmpStaticImpl m_NorStc;
	CBmpStaticImpl m_HotStc;
	CXmlNodePtr m_pToolbarNode;
	CXmlNodePtr m_pButtonMenu;
	CXml m_XmlWriter;

	CAddButtonTab()
	{
		m_strFilter = _T("ICON image(*.ico)|*.ico|Bitmap image(*.bmp)|*.bmp||"); //|GIF image(*.gif)|*.gif|
		ConvertFilterString(m_strFilter);
	}

	~CAddButtonTab()
	{
	}

	enum { IDD = IDD_TAB_ADDBUTTON };

	BEGIN_MSG_MAP(CAddButtonTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
		//MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor )
		COMMAND_HANDLER(IDC_BUTTON_MODIFY, BN_CLICKED, OnModifyBtnClicked)
		COMMAND_HANDLER(IDC_BUTTON_DELETE, BN_CLICKED, OnDelBtnClicked)
		
		COMMAND_HANDLER(IDC_STATIC_HOT_BMP, STN_CLICKED, OnHotImgClicked)
		COMMAND_HANDLER(IDC_STATIC_NORM_BMP, STN_CLICKED, OnNorImgClicked)
		COMMAND_HANDLER(IDC_LIST_ALLBTN, LBN_SELCHANGE, OnListSelChange)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CAddButtonTab)
		DDX_TEXT(IDC_EDIT_URL, m_strUrl.GetBSTR());
		DDX_TEXT(IDC_EDIT_DESC, m_strDesc.GetBSTR());
		DDX_TEXT(IDC_EDIT_CAPTION, m_strCaption.GetBSTR());
	END_DDX_MAP()

	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(INT32 idCtrl, LPNMHDR pNmHdr, BOOL& bHandled);

	// CAddButtonTab 消息处理程序
	VOID LoadInitIcon()
	{
		HICON hBtnIcon;
		LPCTSTR lpszFavIcon;
		TCHAR szIconFile[ MAX_PATH ];
		lpszFavIcon = ( LPCTSTR )GetParent().SendMessage( WM_GETFAVICONFILE, NULL, (LPARAM)NULL );
		hBtnIcon = ( HICON )GetParent().SendMessage( WM_GETBTNICON, NULL, (LPARAM)NULL );
		GetParent().SendMessage(WM_SETMAINMENU , NULL, (LPARAM)NULL);
		m_strImage = _T( "" );
		m_strHotImage = _T( "" );
		if( NULL != lpszFavIcon && _T( '\0' ) != lpszFavIcon[ 0 ] )
		{
			m_strImage = lpszFavIcon;
			m_strHotImage = lpszFavIcon;
		}
		else
		{
			if( NULL != hBtnIcon )
			{
				m_NorStc.SetBtnIcon ( hBtnIcon );
				m_HotStc.SetBtnIcon ( hBtnIcon );
				Invalidate(0);
			}
			else
			{
				if( 0 == GetXmlConfDir( szIconFile, MAX_PATH ) )
				{
					_tcscat( szIconFile, DEFAULT_NORM_BMP );
					m_strImage = szIconFile;
				}

				if( 0 == GetXmlConfDir( szIconFile, MAX_PATH ) )
				{
					_tcscat( szIconFile, DEFAULT_HOT_BMP );
					m_strHotImage = szIconFile;
				}
			}
		}

		if( 0 != m_strImage.length() )
		{
			if(!m_NorStc.CheckAndLoadImage(m_strImage))
			{
				m_strImage = _T( "" );
			}
		}

		if( 0 != m_strHotImage.length() )
		{
			if(!m_HotStc.CheckAndLoadImage(m_strHotImage))
			{
				m_strHotImage = _T( "" );
			}
		}
		return;
	}

	LRESULT OnListSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		INT32 nIndex = m_ListBox.GetCurSel();

		bstr_t strTmp;
		m_ListBox.GetTextBSTR(nIndex, strTmp.GetBSTR());
		bstr_t strTmp2(ADD_BUTTON_COMMAND);

		if(strTmp == strTmp2)
		{
			LoadInitIcon();
			return 0;
		}

		CXmlNodePtr pTmpNode = m_pToolbarNode->GetChild(BUTTON_COMMAND_ATRRIB, strTmp);
		ATLASSERT(!pTmpNode->IsNull());
		CXmlNodePtr pCommandsNode = m_pToolbarNode->GetChild(_T("COMMANDS"));
		ATLASSERT(!pCommandsNode->IsNull());
		CXmlNodePtr pWebJumpNode = pCommandsNode->GetChild(COMMAND_NAME_ATTRIB, strTmp);
		ATLASSERT(!pWebJumpNode->IsNull());

		m_pButtonMenu = pTmpNode->GetChild(XML_MENU_NODE);
		ATLASSERT(!m_pButtonMenu->IsNull());

		CXmlNode *pXmlNode = m_pButtonMenu.get();
		GetParent().SendMessage(WM_SETMAINMENU , NULL, (LPARAM)pXmlNode);

		m_strUrl = pWebJumpNode->GetAttribute(COMMAND_URL_ATTRIB);

		m_strDesc = pTmpNode->GetAttribute(BUTTON_HINT_ATTRIB);
		m_strCaption = pTmpNode->GetAttribute(BUTTON_CAPTION_ATRRIB);
		m_strImage = m_pButtonMenu->GetAttribute(BUTTON_IMG_ATTRIB);
		m_strHotImage = m_pButtonMenu->GetAttribute(BUTTON_HOTIMG_ATTRIB);

		if(!m_NorStc.CheckAndLoadImage(m_strImage, TRUE))
			m_strImage = "";
		if(!m_HotStc.CheckAndLoadImage(m_strHotImage, TRUE))
			m_strHotImage = "";

		DoDataExchange(FALSE);
		return 0;
	}

	//void TestXmlNode(CXmlNodePtr& pXmlNode)
	//{
	//	XString caption;
	//	caption = pXmlNode->GetAttribute("caption");
	//	XString img = pXmlNode->GetAttribute("img");
	//}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//if (0 == GetPropertySheet().GetActiveIndex())
		//	GetPropertySheet().CenterWindow();

		bHandled = FALSE;
		bstr_t strXmlEx = EXTENT_XFREEBAR_CONFIG;
		strXmlEx = getprogpath(strXmlEx);

		m_XmlWriter.Open(strXmlEx);
		GetParent().SendMessage(WM_SETXMLDOC , NULL, (LPARAM)&m_XmlWriter);
		m_pToolbarNode =  m_XmlWriter.GetRoot("TOOLBAR");
		if(m_pToolbarNode->IsNull())
		{
			m_pToolbarNode = m_XmlWriter.AppendRoot("TOOLBAR");
			if(m_pToolbarNode->IsNull())
			{
				MessageBox(_T("Config file is modified, please reinstall."));
				GetParent().SendMessage(WM_CLOSE);
				return 1;
			}
		}
		GetParent().SendMessage(WM_SETROOTNODE , NULL, (LPARAM)m_pToolbarNode.get());
		if(!CheckXmlExFile(m_pToolbarNode))
		{
			MessageBox(_T("Config file is modified, please reinstall."));
			GetParent().SendMessage(WM_CLOSE);
			return 1;
		}

		m_NorStc.SubclassWindow(GetDlgItem(IDC_STATIC_NORM_BMP));
		m_HotStc.SubclassWindow(GetDlgItem(IDC_STATIC_HOT_BMP));
		m_BtnOK.SubclassWindow( GetDlgItem( IDC_BUTTON_MODIFY ) );
		m_BtnCancel.SubclassWindow( GetDlgItem( IDC_BUTTON_DELETE ) );
		ATLASSERT( NULL != m_BtnOK.m_pfnSuperWindowProc );
		ATLASSERT( NULL != m_BtnCancel.m_pfnSuperWindowProc );

		LoadInitIcon();

		m_ListBox.SubclassWindow( GetDlgItem(IDC_LIST_ALLBTN));

		LoadAllString();

		m_ListBox.SetCurSel(0);
		return TRUE;  // Let the system set the focus
	}

	BOOL LoadAllString()
	{
		m_ListBox.ResetContent();
		m_ListBox.AddString(ADD_BUTTON_COMMAND);
		bstr_t strCommand;
		for(CXmlNodePtr pMenuItem = m_pToolbarNode->GetFirstChild(); !(pMenuItem->IsNull()); pMenuItem = pMenuItem->GetNextSibling())
		{
			if(pMenuItem->CheckName(XML_CHILD))
			{
				strCommand = pMenuItem->GetAttribute(BUTTON_COMMAND_ATRRIB);
				m_ListBox.AddString(strCommand);
			}
		}
		return TRUE;
	}

	BOOL CheckSameName(vector<XString> &CmdNames )
	{
		XString strSource;
		XString strDest;
		for(ULONG i = 0; i < CmdNames.size(); i++)
		{
			strSource = CmdNames[i];
			for(ULONG j = i + 1; j <CmdNames.size(); j++)
			{
				strDest = CmdNames[j];
				if(strSource == strDest)
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	BOOL CheckXmlExFile(CXmlNodePtr &pToolbarNode)
	{
		if(!pToolbarNode->CheckName(XML_ROOT))
		{
			if(!pToolbarNode->ModifyName(XML_ROOT))
				return FALSE;;
		}

		CXmlNodesPtr pXmlNodeList;
		CXmlNodePtr pTmpItem;
		CXmlNodePtr pCommandsItem;
		CXmlNodesPtr pWebJumpNodes;
		CXmlNodesPtr pAllMenuItem;
		CXmlNodePtr pCmdTmpItem;
		CXmlNodePtr pMenuItem;
		CXmlNodePtr pButtonMenu;

		pCommandsItem = pToolbarNode->GetChild(XML_COMMANDS_NODE);
		if(pCommandsItem->IsNull())
		{
			pCommandsItem = pToolbarNode->AppendChild(XML_COMMANDS_NODE);
			if(pCommandsItem->IsNull())
			{
				return FALSE;
			}
		}

		pWebJumpNodes = pCommandsItem->GetChildren();
		if(!pCommandsItem->IsNull())
		{
			vector<XString> AllCmd;
			XString strTmp;
			for(INT32 i=0; i < pWebJumpNodes->GetCount(); i++)
			{
				strTmp = (LPTSTR)pWebJumpNodes->operator [](i)->GetAttribute(COMMAND_NAME_ATTRIB);
				AllCmd.push_back(strTmp);
			}
			if(CheckSameName(AllCmd))
			{
				return FALSE;
			}
		}

		pXmlNodeList = pToolbarNode->GetChildren();
		bstr_t strCommand;
		if(!pXmlNodeList->IsNull())
		{
			for(INT32 i=0; i < pXmlNodeList->GetCount(); i++)
			{
				pTmpItem = pXmlNodeList->GetItem(i);
				ATLASSERT(!pTmpItem->IsNull());

				if(pTmpItem->CheckName(XML_CHILD))
				{
					strCommand = pTmpItem->GetAttribute(BUTTON_COMMAND_ATRRIB);
					if(!strCommand.length())
						return FALSE;
					pCmdTmpItem = pCommandsItem->GetChild(COMMAND_NAME_ATTRIB, strCommand);
					if(pCmdTmpItem->IsNull())
					{
						return FALSE;
					}

					pButtonMenu = pTmpItem->GetChild(XML_MENU_NODE);
					if(pButtonMenu->IsNull())
					{
						return FALSE;
					}

					pAllMenuItem = pButtonMenu->GetChildren();
					if(!pAllMenuItem->IsNull())
					{
						for(INT32 j = 0; j < pAllMenuItem->GetCount(); j++)
						{
							pMenuItem = pAllMenuItem->GetItem(j);
							ATLASSERT(!pMenuItem->IsNull());

							strCommand = pMenuItem->GetAttribute(BUTTON_COMMAND_ATRRIB);
							pCmdTmpItem = pCommandsItem->GetChild(COMMAND_NAME_ATTRIB, strCommand);
							if(pCmdTmpItem->IsNull())
							{
								return FALSE;
							}
						}
					}
				}
				else if(pTmpItem->CheckName(XML_COMMANDS_NODE))
				{
					;
				}
				else
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	LRESULT OnNorImgClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		// TODO: 在此添加控件通知处理程序代码
		CString strTmp;
		BOOL Ret = OpenFileDialog(TRUE, 0, strTmp.GetBufferSetLength(_MAX_PATH), 0,  OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, m_strFilter);
		strTmp.ReleaseBuffer();
		if (Ret)
		{
			m_strImage = strTmp;
			if(!m_NorStc.CheckAndLoadImage(strTmp))
				m_strImage = "";
		}
		return 0;
	}

	LRESULT OnHotImgClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		// TODO: 在此添加控件通知处理程序代码
		CString strTmp;
		BOOL Ret = OpenFileDialog(TRUE, 0, strTmp.GetBufferSetLength(_MAX_PATH),0,  OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, m_strFilter);
		strTmp.ReleaseBuffer();
		if (Ret)
		{
			m_strHotImage = strTmp;
			if(!m_HotStc.CheckAndLoadImage(m_strHotImage))
			{
				m_strHotImage = "";
			}
		}
		return 0;
	}

	LRESULT OnDelBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		DoDataExchange(TRUE);

		BOOL bRet = TRUE;
		bstr_t strCulSel;
		m_ListBox.GetTextBSTR(m_ListBox.GetCurSel(), strCulSel.GetBSTR());
		bstr_t strCmp(ADD_MENU_COMMAND);
		if(strCulSel != strCmp)
		{
			do
			{	
				CXmlNodePtr pButtonNode = m_pToolbarNode->GetChild(BUTTON_COMMAND_ATRRIB, strCulSel);
				if(pButtonNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				if(!m_pToolbarNode->RemoveChild(pButtonNode))
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pCommandsNode = m_pToolbarNode->GetChild(_T("COMMANDS"));

				if(pCommandsNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pWebJumpNode = pCommandsNode->GetChild(COMMAND_NAME_ATTRIB, strCulSel);

				if(pWebJumpNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				if(!pCommandsNode->RemoveChild(pWebJumpNode))
				{
					bRet = FALSE;
					break;
				}

				if(!m_XmlWriter.Save())
				{
					bRet = FALSE;
					break;
				}
				LoadAllString();
			}while(0);
			if(!bRet)
			{
				MessageBox("Delete button failed.");
			}
			else
			{
				MessageBox("Delete button success.");
			}
			if(m_ListBox.GetCount() > 1)
			{
				m_ListBox.SetCurSel(1);
			}
		}
		return 0;
	}


	LRESULT OnModifyBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		// TODO: 在此添加控件通知处理程序代码
		DoDataExchange(TRUE);
		bstr_t strSel;
		m_ListBox.GetTextBSTR(m_ListBox.GetCurSel(), strSel.GetBSTR());
		bstr_t strCmp(ADD_BUTTON_COMMAND);
		BOOL bRet = TRUE;
		if(strSel == strCmp)
		{
			do
			{
				CXmlNodePtr pButtonNode = m_pToolbarNode->AppendChild(XML_CHILD);
				if(pButtonNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				bstr_t strCommand;
				if(!GetCurrectCommandName(strCommand, m_strCaption, m_pToolbarNode.get()))
				{
					pButtonNode->Remove();
					bRet = FALSE;
					break;
				}

				pButtonNode->SetAttribute(_T("caption"), m_strCaption);
				pButtonNode->SetAttribute(_T("hint"), m_strDesc);
				pButtonNode->SetAttribute(_T("command"), strCommand);
				CXmlNodePtr pMenuNode = pButtonNode->AppendChild(_T("MENU"));

				if(pMenuNode->IsNull())
				{
					pButtonNode->Remove();
					bRet = FALSE;
					break;
				}
				pMenuNode->SetAttribute(_T("type"), _T("None"));

				_bstr_t strTmp;
				strTmp = GetFileNameFromPath(m_strImage);
				strTmp  = getprogpath(strTmp);
				if(!CheckSameDir(m_strImage, strTmp))
				{
					if(!CopyResFile(m_strImage, strTmp))
					{
						pButtonNode->Remove();
						bRet = FALSE;
						break;
					}
				}
				pButtonNode->SetAttribute(_T("img"), GetFileNameFromPath(m_strImage));

				strTmp = GetFileNameFromPath(m_strHotImage);
				strTmp  = getprogpath(strTmp);
				if(!CheckSameDir(m_strHotImage, strTmp))
				{
					if(!CopyResFile(m_strHotImage, strTmp))
					{
						pButtonNode->Remove();
						bRet = FALSE;
						break;
					}
				}
				pButtonNode->SetAttribute( _T( "hot_img" ), GetFileNameFromPath( m_strHotImage ) );

				CXmlNodePtr pCommandsNode = m_pToolbarNode->GetChild(_T("COMMANDS"));

				if(pCommandsNode->IsNull())
				{
					pButtonNode->Remove();
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pWebJumpNode = pCommandsNode->AppendChild(_T("WEBJUMP"));

				if(pWebJumpNode->IsNull())
				{
					pButtonNode->Remove();
					bRet = FALSE;
					break;
				}

				pWebJumpNode->SetAttribute(_T("name"), strCommand);
				pWebJumpNode->SetAttribute(_T("href"),m_strUrl);

				if(!m_XmlWriter.Save())
				{
					pWebJumpNode->Remove();
					pButtonNode->Remove();
					bRet = FALSE;
					break;
				}

				_bstr_t strFavIcon;
				strFavIcon = ( LPCTSTR )GetParent().SendMessage( WM_GETFAVICONFILE, NULL, (LPARAM)NULL );
				if( strFavIcon == m_strImage )
				{
					GetParent().SendMessage( WM_SAVEFAVICONFILE, NULL, (LPARAM)NULL );
				}
				LoadAllString();
			}while(0);
			if(!bRet)
			{
				MessageBox("Add button procedure failed.");
			}
			else
			{
				MessageBox("Add button procedure success.");
			}
		}
		else
		{
			do
			{
				CXmlNodePtr pButtonNode = m_pToolbarNode->GetChild(BUTTON_COMMAND_ATRRIB, strSel);
				if(pButtonNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pCommandsNode = m_pToolbarNode->GetChild(_T("COMMANDS"));
				if(pCommandsNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pWebJumpNode = pCommandsNode->GetChild(COMMAND_NAME_ATTRIB, strSel);
				if(pWebJumpNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				CXmlNodePtr pMenuNode = pButtonNode->GetChild(_T("MENU"));
				if(pMenuNode->IsNull())
				{
					bRet = FALSE;
					break;
				}

				pButtonNode->SetAttribute(_T("caption"), m_strCaption);
				pButtonNode->SetAttribute(_T("hint"), m_strDesc);

				_bstr_t strTmp;
				strTmp = GetFileNameFromPath(m_strImage);
				strTmp  = getprogpath(strTmp);
				if(!CheckSameDir(m_strImage, strTmp))
				{
					if(!CopyResFile(m_strImage, strTmp))
					{
						bRet = FALSE;
						break;
					}
				}
				pButtonNode->SetAttribute(_T("img"), GetFileNameFromPath(m_strImage));

				strTmp = GetFileNameFromPath(m_strHotImage);
				strTmp  = getprogpath(strTmp);
				if(!CheckSameDir(m_strHotImage, strTmp))
				{
					if(!CopyResFile(m_strHotImage, strTmp))
					{
						bRet = FALSE;
						break;
					}
				}
				pButtonNode->SetAttribute(_T("hot_img"), GetFileNameFromPath(m_strHotImage));

				pWebJumpNode->SetAttribute(_T("href"), m_strUrl);
				if(!m_XmlWriter.Save())
				{
					bRet = FALSE;
					break;
				}
				
			}while(0);
			if(!bRet)
			{
				MessageBox(_T("Modifing is fail."));
			}
			else
			{
				MessageBox("Modifing is success.");
			}
		}
		return 0;
	}

	BOOL OnSetActive()
	{
		//SetModified(TRUE);
		return DoDataExchange(FALSE);
	}

	BOOL OnKillActive()
	{
		return DoDataExchange(TRUE);
	}
};

#define WM_CTLCOLOR                             0x0019
class XTabCtrl : public CWindowImpl<XTabCtrl, CTabCtrlT< CUITab > >
{
public:
	XTabCtrl()
	{
	}

	~XTabCtrl()
	{
	}

	BEGIN_MSG_MAP(XTabCtrl)
		//XLOG( ( 0,  _T( "Current XTabCtrl recved msg is %u\r\n" ), uMsg ) );
		//MESSAGE_HANDLER(WM_CTLCOLOR, OnCtlColor)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		CPaintDC DrawDC( m_hWnd );
		DrawDC.FillRect( &CRect( 0, 0, 100, 100 ), RGB( 100, 100, 100 ) );
		DWORD dwStyle;
		dwStyle = GetWindowLong( GWL_STYLE );
		ATLTRACE( _T( "XTabCtrl style is %u now in self paint\r\n" ), dwStyle );
		return 0;
	}

	LRESULT OnEraseBkGnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		RECT rcTab;
		CBrush BkBr;
		CDC DC;
		BkBr.CreateSolidBrush( XCONFIG_DLG_BK_CLR );
		GetWindowRect( &rcTab );
		DC = GetDC();
		DC.FillRect( &rcTab, BkBr );
		bHandled = TRUE;
		return 0;
	}
};

class CXToolBarOptionDlg : public CDialogImpl<CXToolBarOptionDlg, CWindow>,
	//public COwnerDraw< CXToolBarOptionDlg >, 
							public CWinDataExchange<CXToolBarOptionDlg>
{
public:
	CXToolBarOptionDlg() : hDispWnd( NULL ),
							m_hBtnIcon( NULL ),
							m_bSaveFavIcon( FALSE )
	{
		*m_szFavIcon = _T( '\0' );
	}

	virtual ~CXToolBarOptionDlg()
	{
	}

// 对话框数据
	enum { IDD = IDD_XTOOLBAR_OPTION_DLG };

	BEGIN_MSG_MAP(CXToolBarOptionDlg)
		//XLOG( ( 0, _T( "Current CXToolBarOptionDlg recved msg is %u\r\n" ), uMsg ) );
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER( WM_GETBTNICON, OnGetBtnIcon )
		MESSAGE_HANDLER( WM_GETFAVICONFILE, OnGetFavIconFile )
		MESSAGE_HANDLER( WM_SAVEFAVICONFILE, OnSaveIconFile )
		MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
		//MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnUpdateToolBar)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)
		NOTIFY_HANDLER(IDC_OPTION_TAB, TCN_SELCHANGE, OnTabSelChanged)
		if( uMsg == WM_DRAWITEM )
		{
			HANDLE hWndChild;
			if(wParam)	// not from a menu
				hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
			XLOG( ( 0, _T( "draw item window handle is %u\r\n" ), hWndChild ) );
		}
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CXToolBarOptionDlg)
	END_DDX_MAP()

	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		DWORD dwStyle;
		dwStyle = GetWindowLong( GWL_STYLE );
		//XLOG( ( 0, _T( "Option dlg style is %u\r\n" ), dwStyle ) );

		dwStyle = m_Tabs.GetWindowLong( GWL_STYLE );
		//XLOG( ( 0, _T( "XTabCtrl style is %u\r\n" ), dwStyle ) );

		CPaintDC DrawDC( m_hWnd );
		DrawDC.FillRect( &CRect( 0, 0, 100, 100 ), RGB( 100, 100, 100 ) );
		return 0;
	}

	LRESULT  OnGetBtnIcon( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = TRUE;
		return ( LRESULT )m_hBtnIcon;
	}

	LRESULT  OnGetFavIconFile( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = TRUE;
		return ( LRESULT )m_szFavIcon;
	}

	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		TCITEM TcItem;
		m_Tabs.SubclassWindow( GetDlgItem( IDC_OPTION_TAB ) );
		ATLASSERT( NULL != m_Tabs.m_pfnSuperWindowProc  );
		TcItem.mask = TCIF_TEXT | TCIF_IMAGE;  
		TcItem.iImage = -1; 
		TcItem.pszText = "Add Button"; 
		m_Tabs.AddItem( &TcItem );
		TcItem.pszText = "Add Menu"; 
		m_Tabs.AddItem( &TcItem );
		
		m_Tabs.GetWindowRect( &rcSubTab );
		ScreenToClient( &rcSubTab );
		rcSubTab.top += SM_CYVSCROLL + SM_CYBORDER;
		rcSubTab.left += SM_CXBORDER;
		rcSubTab.right -= SM_CXBORDER;
		rcSubTab.bottom -= SM_CYBORDER;

		//BOOL bHandled;
		OnTabSelChanged( 0, 0, bHandled );
		Invalidate( 0 );

		m_BtnOK.SubclassWindow( GetDlgItem( IDOK ) );
		m_BtnCancel.SubclassWindow( GetDlgItem( IDCANCEL ) );

		XLOG( ( 0, _T( "Button OK window handle is %u\r\n" ), m_BtnOK.m_hWnd ) ); 
		ATLASSERT( NULL != m_BtnOK.m_pfnSuperWindowProc );
		ATLASSERT( NULL != m_BtnCancel.m_pfnSuperWindowProc );

		bHandled = FALSE;
		return TRUE;
	}

	LRESULT OnTabSelChanged(INT32 wParam, LPNMHDR lpNMHdr, BOOL &bHandled)
	{ 
		bHandled = FALSE;
		int nSel = m_Tabs.GetCurSel();
		if( NULL != hDispWnd )
		{
			::ShowWindow( hDispWnd, SW_HIDE );
			::EnableWindow( hDispWnd, FALSE );
		}

		// Create the new child dialog box. 
		switch( nSel )
		{
		case 0:
			if( NULL == m_BtnTab.m_hWnd )
			{
				m_BtnTab.Create( m_hWnd );
				m_BtnTab.MoveWindow( rcSubTab, FALSE );
				m_BtnTab.ShowWindow( SW_SHOW );
			}
			else
			{
				m_BtnTab.ShowWindow( SW_SHOW );
				m_BtnTab.EnableWindow( TRUE );
			}
			hDispWnd = m_BtnTab.m_hWnd;
			break;
		case 1:
			if( NULL == m_MenuTab.m_hWnd )
			{
				m_MenuTab.Create( m_hWnd );
				m_MenuTab.MoveWindow( rcSubTab, FALSE );
				m_MenuTab.ShowWindow( SW_SHOW );
			}
			else
			{
				m_MenuTab.ShowWindow( SW_SHOW );
				m_MenuTab.EnableWindow( TRUE );
			}
			hDispWnd = m_MenuTab.m_hWnd;
			break;
		default:
			break;
		}
		bHandled = TRUE;
		return TRUE;
	} 

	//LRESULT OnCtlColor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	//{
	//	bHandled = FALSE;
	//	//bHandled = TRUE;
	//	return 0;
	//}
	
	LRESULT OnUpdateToolBar( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
	{
		bHandled = FALSE;
		EndDialog( TRUE );
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
	{
		bHandled = FALSE;
		EndDialog( FALSE );
		bHandled = TRUE;
		return 0;
	}

public:
	VOID SetTitleIcon( HICON hTitleIcon )
	{
		m_hBtnIcon = hTitleIcon;
	}

	VOID SetCurSiteFavIcon( LPCTSTR lpszFavIcon )
	{
		if( _tcslen( lpszFavIcon ) >= MAX_PATH )
		{
			m_szFavIcon[0] = _T('\0' );
			return;
		}
		
		_tcscpy( m_szFavIcon, lpszFavIcon );
	}

	LRESULT  OnSaveIconFile( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = TRUE;
		m_bSaveFavIcon = TRUE;
		return 0;
	}

	LRESULT  OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		if( m_bSaveFavIcon == FALSE )
		{
			if( *m_szFavIcon != _T( '\0' ) )
			{
				DeleteFile( m_szFavIcon );
			}
		}
		return 0;
	}


protected:
	CAddMenuTab m_MenuTab;
	CAddButtonTab m_BtnTab;
	XTabCtrl m_Tabs;
	CRect rcSubTab;
	CButtonXP m_BtnOK;
	CButtonXP m_BtnCancel;
	HWND hDispWnd;
	HICON m_hBtnIcon;
	TCHAR m_szFavIcon[ MAX_PATH ];
	BOOL m_bSaveFavIcon;
};

class XAboutDlg :
	public CDialogImpl<XAboutDlg, CUITab>,
	public CWinDataExchange<XAboutDlg>
{
public:
	enum { IDD = IDD_ABOUT };

	XAboutDlg()
	{
	}

	~XAboutDlg()
	{
	}

	BEGIN_MSG_MAP(CAddMenuTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint);
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor )
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor )
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOKBtnClicked)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancelBtnClicked)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CAddMenuTab)
	END_DDX_MAP()

#define XFREEBAR_LOGO_FILE "xfreelogo.bmp"
	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
	{
		bHandled = FALSE;
		m_StcMainTitle.SubclassWindow( GetDlgItem( IDC_STATIC_ABOUT ) );
		ATLASSERT( NULL != m_StcMainTitle.m_hWnd );
		m_BtnOK.SubclassWindow( GetDlgItem( IDOK ) );
		ATLASSERT( NULL != m_BtnOK.m_hWnd );
		//m_BtnCancel.SubclassWindow( GetDlgItem( IDCANCEL ) );
		//ATLASSERT( NULL != m_BtnCancel.m_hWnd );
		
		m_strImage = XFREEBAR_LOGO_FILE;
		if(!m_StcMainTitle.CheckAndLoadImage(m_strImage, TRUE, FALSE ))
		{
			m_strImage = "";
		}
		return 0;
	}

	LRESULT OnOKBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		bHandled = FALSE;
		EndDialog( IDOK );
		return 0;
	}

	LRESULT OnCancelBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		bHandled = FALSE;
		EndDialog( IDCANCEL );
		return 0;
	}
	
protected:
	CBmpStaticImpl m_StcMainTitle;
	CButtonXP m_BtnOK;
	CButtonXP m_BtnCancel;
	_bstr_t m_strImage;
};

#endif //__CONFIG_DLG_H__
