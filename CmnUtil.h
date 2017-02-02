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

#pragma once
#include <vector>
using namespace std;

#define REG_COMPANY_NAME			_T("coderebasoft")
#define XBAR_LOG_PATH				_T("\\XBar.log")
#define ADD_TO_DROP_MENU 20
#define BMP_LOADFLAG LR_LOADFROMFILE
#define CLIENT_OFFSET(rect) rect.right = rect.right - rect.left; \
							rect.bottom = rect.bottom - rect.top; \
							rect.left = rect.top = 0;


#define ICON_IMGAE 2
#define BITMAP_IMAGE 1

enum {
	url_none = 0,
	url_normal = 1,
	url_unsafe = 2,
	url_reserved = 4
};

// type of index image
#if defined(_ODBC_DIRECT)
typedef XString image_index_t;
std::set<XString> gImages;
#else
typedef XString image_index_t;
#endif

// some toolbar specific includes
#if defined (_ODBC_DIRECT) || defined(_MASTER_BAR)
#include "Toolband7\CPage.cpp"
#endif
#ifdef _PROXY_CHANGE_TOOLBAR
#include "Toolband26\Proxy.cpp"
#endif
#ifdef _ODBC_DIRECT
#include "PostInc.cpp"
#endif

class IComboHost
{
public:
	virtual INT32 OnComboProcess() = 0;
	virtual INT32 OnComboCommand() = 0;
	virtual INT32 OnComboSetFocus() = 0;
	virtual INT32 OnComboKillFocus() = 0;
	virtual INT32 UpdateSearchedWords() = 0;
};

#define MAX_URL_LENGTH ( INT32 )( 2048 )
INT32 GetWebBrowserCurUrl( IWebBrowserPtr pWebBrowser, TCHAR *lpszURL, UINT uBufSize );
INT32 DownloadWebSiteFavIcon( LPCTSTR lpszUrl, LPCTSTR lpszFileName );
INT32 GetXmlConfDir( LPTSTR lpszConfFileName, DWORD dwBufLen );
BOOL IsNumberStr(LPCTSTR lpszInput);
BOOL XmlEncode(LPCTSTR lpszIn, LPCTSTR lpszOut, UINT uLen = 512);
BOOL XmlDecode(LPCTSTR lpszIn, LPCTSTR lpszOut, UINT uLen = 512);
BOOL GetBitmpSize(CBitmap &Bmp, SIZE& sz);
HBITMAP LoadImage(const XString& str);
HICON LoadIcon(const XString& str IN);
void LoadImageDirect(LPTSTR str IN, HANDLE &hImg IN OUT, UINT &ImageType);
SIZE GetIconSize(HICON hIcon);
_bstr_t getprogpath(const _bstr_t& str);
_bstr_t getprogpathfromreg( XString &strFile);
_bstr_t IntToStr(INT32 nVal, INT32 type = 10);
bool isGetValue(const XString& s);
bool isGetValue2(const XString& s);
_bstr_t getHistName(const XString& id);
_bstr_t getdllpath();
HKEY GetAppRegistryKey();
HKEY GetCompanyRegistryKey();
INT32 GetToolBarItemBmp(XString s);
INT32 GetToolBarItemBmp(INT32 s);
bool CheckUpdate(XString& strFile, bool update = false);
void throwException(long, _bstr_t mess,TCHAR const* file, INT32);
void ShowBand();
IWebBrowser2* CreateNewIEInstance( bool show =true);
_bstr_t replaceUrl(const XString& ss);
void UnRegisterServer();
void addKey(bool a);
void __cdecl ReEnable(void* lpParam);
bool checkTime(const XString& s,INT32 delta, bool isHour = true);
bool replace(XString& str,const XString& from,const XString& to);
bool replace(XString& str,const XString& from,const _bstr_t& to);
void ShowBandObject(IWebBrowser2 *pIE,const _bstr_t& pszBarCLSID,bool bShow);
// in: CLSID-string of a IE-Bar element (toolbar or Pugi), and IWebBrowser interface
void ShowBandObject(IWebBrowser2 *pIE,const _bstr_t& pszBarCLSID, const _bstr_t url, bool bShow = true);
void CloseAllRuningIE();

void setValue(const XString& s,const XString& val);
void setValue(const XString& s,long val);
XString getValue(const XString& s,const XString& def =_T("1"));
long getValue(const XString& s,const long& def);
const XString urlencode(const XString& _s, INT32 _flags = url_normal);
DWORD GetCRC(_bstr_t& strFilePath);
// process some custom procedures during combo line changes
bool onCombo(IComboHost* ctrl,INT32& iItem);
XString replaceSpace(XString str0,std::vector<XString>& v);
BOOL MyQueryPresizeFrq(float& frq);
float QueryCounter();
_variant_t getPostData(const XString& post);
BOOL IsHotkeyDown();
INT32 ButtonIndexToID(HWND hToolbar, INT32 nIndex);
