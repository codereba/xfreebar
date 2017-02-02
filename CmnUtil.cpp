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

#include "StdAfx.h"
#include "CmnUtil.h"
#include "RegOper.h"
#include "time.h"
#include "GlobalString.h"
 
INT32 GetWebBrowserCurUrl( IWebBrowserPtr pWebBrowser, TCHAR *lpszURL, UINT uBufSize )
{
	BSTR bstrURL;
	TCHAR *pszURL;
	OLECHAR szTemp[ MAX_URL_LENGTH ];
	bstrURL = SysAllocStringLen( szTemp, MAX_URL_LENGTH );
	if( S_OK != pWebBrowser->get_LocationURL( &bstrURL ) )
	{
		*lpszURL = _T( '\0' );
		return -131;
	}
#ifdef _UNICODE
	pszURL = bstrURL;
#else
	pszURL = _com_util::ConvertBSTRToString(bstrURL);
#endif
	_tcsncpy( lpszURL, pszURL, uBufSize );

	SysFreeString( bstrURL );
	return 0;
}

INT32 DownloadWebSiteFavIcon( LPCTSTR lpszUrl, LPCTSTR lpszFileName )
{ 
	if( S_OK == URLDownloadToFile( 0, lpszUrl, lpszFileName, 0, 0 ) )
		return 0;
	return -1;
}

INT32 GetXmlConfDir( LPTSTR lpszConfFileName, DWORD dwBufLen )
{
	INT32 nIndex;
	INT32 nFileNameLen;
	CHAR szModuleFileName[ MAX_PATH ];
	::GetModuleFileNameA( _Module.GetModuleInstance(), szModuleFileName, MAX_PATH);
	
	nFileNameLen = strlen(szModuleFileName);
	
	for( nIndex = nFileNameLen - 1; nIndex >= 0; nIndex-- )
	{
		if(szModuleFileName[nIndex] == _T( '\\') )
		{
			if( nIndex < nFileNameLen - 1 )
				szModuleFileName[ nIndex + 1 ] = _T( '\0' );
			break;
		}
		
	}

	if( dwBufLen < nIndex )
	{
		return XERROR_BUF_TOOL_SMALL;
	}


	_tcscpy( lpszConfFileName, szModuleFileName );
	return XSUCCESS;
}

BOOL IsNumberStr(LPCTSTR lpszInput)
{
	UINT uCount = _tcslen(lpszInput);
	if(uCount == 0)
		return TRUE;

	for(TCHAR *pCh = (TCHAR*)lpszInput; pCh != _T( '\0' ) && uCount > 0; pCh++)
	{
		if(*pCh < _T( '0' ) || *pCh > _T( '9' ) )
			return FALSE;
		uCount--;
	}
	return TRUE;
}

BOOL XmlEncode(LPCTSTR lpszIn, LPCTSTR lpszOut, UINT uLen )
{
	CString strTmp;
	strTmp = lpszIn;
	strTmp.Replace(_T("&"), _T("&amp;"));
	memset((void*)lpszOut, 0, uLen * sizeof(TCHAR));
	if(strTmp.GetLength() > uLen)
		return FALSE;
	else
	{
		memcpy((void*)lpszOut, strTmp.GetBuffer(0), strTmp.GetLength() * sizeof(TCHAR));
	}
	return TRUE;
}

BOOL XmlDecode(LPCTSTR lpszIn, LPCTSTR lpszOut, UINT uLen )
{
	CString strTmp;
	strTmp = lpszIn;
	strTmp.Replace(_T("&amp;"), _T("&"));
	memset((void*)lpszOut, 0, uLen * sizeof(TCHAR));

	if(strTmp.GetLength() > uLen)
		return FALSE;
	else
	{
		memcpy((void*)lpszOut, strTmp.GetBuffer(0), strTmp.GetLength() * sizeof(TCHAR));
	}
	return TRUE;
}

BOOL GetBitmpSize(CBitmap &Bmp, SIZE& sz)
{
	BITMAP BmpInfo;
	if(!Bmp.GetBitmap(&BmpInfo))
		return FALSE;
	sz.cx = BmpInfo.bmWidth;
	sz.cy = BmpInfo.bmHeight;
	return TRUE;
}

HBITMAP LoadImage(const XString& str IN)
{
#ifndef _ODBC_DIRECT
	XString img = str;
	CheckUpdate(img);
	return AtlLoadBitmapImage(img.c_str(),BMP_LOADFLAG);
#else
	return (HBITMAP)::LoadImage(m_hImageDll, str.c_str(), IMAGE_BITMAP, 0, 0, BMP_LOADFLAG);
#endif
}

HICON LoadIcon(const XString& str IN)
{
#ifndef _ODBC_DIRECT
	XString img = str;
	CheckUpdate(img);
	return AtlLoadIconImage(img.c_str(),LR_LOADFROMFILE);
#else
	return (HICON)::LoadImag(m_hImageDll,str.c_str(),IMAGE_ICON,16,16,LR_LOADFROMFILE);
#endif
}

void LoadImageDirect(LPTSTR str IN, HANDLE &hImg IN OUT, UINT &uImageType)
{
	XString img = str;
	HANDLE hTmp;
	if(img.find(_T(".ico")) != -1 || img.find(_T(".ICO")) != -1)
	{
		 hTmp =  AtlLoadIconImage(img.c_str(),LR_LOADFROMFILE);
		 uImageType = IMAGE_ICON;
	}
	else
	{
		 hTmp =  AtlLoadBitmapImage(img.c_str(),BMP_LOADFLAG);
		 uImageType = IMAGE_BITMAP;
	}
	hImg = hTmp;
	return;
}

SIZE GetIconSize(HICON hIcon)
{
	SIZE IconSize;
	ICONINFO IconInfo;
	::GetIconInfo(hIcon,&IconInfo);
	HBITMAP hbm;
	BITMAP bm;
	hbm = IconInfo.hbmColor;
	::GetObject(hbm,sizeof(bm),&bm);
	IconSize.cx = bm.bmWidth;
	IconSize.cy = bm.bmHeight;
	return IconSize;
}

long getValue(const XString& s,const long& def)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  DWORD cValue;
  if(ERROR_SUCCESS==keyAppID.QueryValue(cValue, s.c_str()))
    return cValue;
  return def;
}

XString getValue(const XString& s,const XString& def)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  TCHAR cValue[1024];
  DWORD pdwCount = 1000;
  if(ERROR_SUCCESS==keyAppID.QueryValue(cValue, s.c_str(),&pdwCount))
  {
    return cValue;
  }
  return def;
}

void setValue(const XString& s,const XString& val)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  keyAppID.SetValue(val.c_str(), s.c_str());
}

void setValue(const XString& s,long val)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  keyAppID.SetValue(val, s.c_str());
}

_bstr_t getHistName(const XString& strId)
{
   return ( _T("History") + strId ).c_str();
}

_bstr_t getdllpath()
{
#ifdef _ADIGEN 
  if(sProfileName.length())
  {
    return "c:\\" + sProfileName + "\\adigen.dll";
  }
#endif

  std::vector<TCHAR> exed(1000);
  ::GetModuleFileName(_Module.GetModuleInstance(),&exed.front(),1000);
  XString path = &exed.front();
  return path.c_str();
}

_bstr_t getprogpath(const _bstr_t& str)
{
  XString path = getdllpath();
  path.erase(path.rfind('\\')+1);

  INT32 idx = path.find(_T("\\CONFLICT"));
  if(idx>=0)
    path.erase(idx+1);

  return path.c_str() + str;
}

_bstr_t getprogpathfromreg( XString &strFile)
{
	_bstr_t ConfigPath;
	_bstr_t XmlFile;

	if(!GetConfigPath(ConfigPath))
	{
		return _T("");
	}

	if(!strFile.length())
	{
		if(!GetXmlPath(XmlFile))
		{
			return _T("");
		}
		strFile = XmlFile;
	}
	else
	{
		XmlFile = strFile.c_str();
	}

	ConfigPath = ConfigPath + XmlFile;

	return ConfigPath;
}

_bstr_t IntToStr(INT32 nVal, INT32 type)
{
  char buff[20];
  itoa(nVal, buff, type);
  return buff;
}

bool isGetValue(const XString& s)
{
  return getValue(s)==XString(_T("1"));
}

bool isGetValue2(const XString& s)
{
  return getValue(s,_T("0"))==XString(_T("1"));
}

HKEY GetAppRegistryKey()
{
    HKEY hAppKey = NULL;
    HKEY hCompanyKey = GetCompanyRegistryKey();
    DWORD dwDisp;
    RegCreateKeyEx( hCompanyKey, PROFILE_FILE_NAME, 0, REG_NONE,
        REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
        &hAppKey, &dwDisp );
    if ( hCompanyKey != NULL )
        RegCloseKey( hCompanyKey );
    return hAppKey;
}

HKEY GetCompanyRegistryKey()
{
    HKEY hSoftKey = NULL;
    HKEY hCompanyKey = NULL;
    if ( RegOpenKeyEx( HKEY_CURRENT_USER, _T("software"), 0, KEY_WRITE | KEY_READ,
        &hSoftKey ) == ERROR_SUCCESS )
    {
        DWORD dw;
        RegCreateKeyEx( hSoftKey, XREG_KEY_NAME, 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
            &hCompanyKey, &dw );
    }
    if ( hSoftKey != NULL )
        RegCloseKey( hSoftKey );
    
    return hCompanyKey;
}

INT32 GetToolBarItemBmp(XString s)
{
#if defined(_ODBC_DIRECT)
  #include "GetBitmap.inl"
#else
	if(IsNumberStr(s.c_str()))
	{
		return atoi(s.c_str());
	}
	else
	{
		return -2;
	}
#endif
}

INT32 GetToolBarItemBmp(INT32 s)
{
#if defined(_ODBC_DIRECT)
  #include "GetBitmap.inl"
#else
	return s;
#endif
}

bool CheckUpdate(XString& strFile, bool update)
{
#ifdef USE_RESOURCE_FILES
  if(strFile.find(_T("bmp"))!=XString::npos)
	  return true;
#endif

  _bstr_t path;

  if(strFile == _T(""))
  {
	  path = getprogpathfromreg(strFile);
	  if(_T("") == path)
	  {
		  return false;
	  }
  }
  else
  {
	  path = getprogpath(strFile.c_str());
  }

  _bstr_t sstrFile = strFile.c_str();

#ifdef _ECOACH
  bool isXML = false;
  if(sstrFile == _bstr_t("ecoach.xml"))
  {
    isXML = true;
    XString updateVer = getValue(XString(_T("updateVer")),_T(""));
    sstrFile = updateVer.empty() ? _T("ecoach.asp") : updateVer.c_str();
    sstrFile += "?URL=" + gurl;
  }
#endif

#ifdef _XFREE
  const _bstr_t serverpath = getValue(_T("SecretPath"),_T("http://www.xfree.co.il/toolbar/")).c_str();
#endif

  _bstr_t pathFromFolder = XUPDATE_SERVER_URL;
  _bstr_t pathFrom = pathFromFolder + sstrFile;
  _bstr_t pathTo = path;

  XString oldcrc = getValue(strFile); //get the crc code for this file by the param, because save it in reg.

  DWORD crc = GetCRC(path); //calc the crc code for the file.

  XString newcrc = IntToStr(crc);

  bool download =
#ifndef DONT_CHECK_LOCAL_FILES
    oldcrc==_T("1") || oldcrc!=newcrc ||
#endif
    crc==0 || update || isGetValue2(_T("gUpdate"));

#ifdef _ECOACH
  if(strFile.find(_T(".bmp"))!=XString::npos)
  {
    if(isGetValue2(_T("BeforeNavigate2")))
      download = false;
  }
#endif

  if(download)
  {
    bool canDown = true;

    HRESULT hr = 0;

    if(canDown || crc==0)
    {
#ifdef _ODBC_DIRECT
      #include "Toolband7\CheckUpdate.cpp"
#else

      hr = URLDownloadToFile( NULL,pathFrom,pathTo,0,0);
      CRegKey keyAppID;
      keyAppID.Create(GetAppRegistryKey(),getHistName(_T("files")));
      keyAppID.SetValue(1,pathTo);
#endif
    }
    else
    {
      if(crc==0)
      {
        hr = -1;
      }
      else
      {
        strFile = path;
        return true; // working offline
      }
    }

    if ( SUCCEEDED(hr) )
    {
    }
    else if(crc==0)
    {
      if(!isGetValue2(_T("gUpdate")))
      {
#ifdef SHOW_DEBUG_MESSAGES
        ::MessageBox(0,_T("Can't load file ")+sstrFile+_T(" from ")+pathFrom,XTOOLBAR_ALERT_TITLE,0);
#endif
        if(isGetValue(_T("showcorrupted")))
        {
          setValue(_T("showcorrupted"),_T("0"));
        }
        return false;
      }
    }
    else
    {
      strFile = path;
      return true; // working offline
    }

  XString oldnewcrc = newcrc;

  newcrc = IntToStr(GetCRC(path));
  setValue(strFile,newcrc);
#ifdef _ECOACH
  if(isXML && isGetValue2(XString(_T("BeforeNavigate2"))) && oldnewcrc==newcrc)
     return false;
#endif
  }

  strFile = path;
  return true;
}

// here you can add error trace of errors during XML parsing
void throwException(long, _bstr_t mess,TCHAR const* file, INT32) 
{
  ::MessageBox(0,mess,file,0);
}

// getting Company and Application keys
// create Unique GUID for toolbar
#if defined(_XFREE) || defined (_DYNABAR)
#include "Toolband1\createGUID.cpp"
#else
XString createGUID() {return _T(""); }
#endif

// called on dll register - process all first time registration
void ShowBand()
{
	addKey(true);

	setValue(_T("gUpdate"),_T("1"));

#if !defined(_ODBC_DIRECT)
	setValue(_T("NID"),getValue(_T("NID"),createGUID()));
#endif

	setValue(_T("toolbar_id"),getValue(_T("toolbar_id"),createGUID()));

#ifdef _PRAIZE
#include "Toolband8\addButtonToStandartToollbar.inl"
#endif
#ifdef _MASTER_BAR
#include "Toolband27\OnShow.inl"
#endif

	// open new IE instance and "select" our toolbar from toolbar's list
	//#ifndef _DEBUG
	bool wasupdate = getValue( XString( _T( "UpdateBegin" ) ),_T( "0" ) ) == XString( _T( "1" ) );

#ifdef UPDATE_DLL_FROM_EXE
	if( wasupdate )
		DeleteFile( exeName );
#endif

	ShowBandObject( CreateNewIEInstance(), XTOOLBAR_UUID, wasupdate ? URL_AFTER_UPDATE : MAIN_SITE_URL, true );

	setValue(_T("UpdateBegin"),_T("0"));
	//#endif

	setValue(_T("gUpdate"),_T("0"));
}

//Runs an instance of Internet explorer and let's the pIE point to the Iwebbrowser2 interface
IWebBrowser2* CreateNewIEInstance( bool show /*=true*/)
{
	IWebBrowser2 *pIE = 0;
	// Create an instance of Internet Explorer
	HRESULT hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pIE);
	if (SUCCEEDED(hr) && show)
		pIE->put_Visible(VARIANT_TRUE);
	return pIE;
}

bool replace(XString& str,const XString& from,const XString& to)
{
   INT32 idx = str.find(from);
   if(idx!=XString::npos)
   {
      str.replace(idx,from.size(),to);
      return true;
   }
   return false;
}

bool replace(XString& str,const XString& from,const _bstr_t& to)
{
  if(to.length()==0)
    return false;
  return replace(str,from,XString(static_cast<const TCHAR*>(to)));
}

extern _bstr_t gurl;
// replace some common templates in urls
_bstr_t replaceUrl(const XString& ss)
{
  XString s = ss;
  replace(s,_T("%url"),gurl);

  replace(s,_T("%id"),getValue(_T("toolbar_id"),_T("0")));
  replace(s,_T("%nid"),getValue(_T("NID"),_T("")));
  replace(s,_T("%pass"),getValue(_T("Pass"),_T("")));
  replace(s,_T("%CustID"),getValue(_T("CustID"),_T("")));
  
  COLORREF color = GetSysColor(COLOR_3DFACE);
  char str[256];
  sprintf(str,"%06X",color);
  replace(s,_T("%bgc"),_bstr_t(str));

  const XString p[] = {_T("10"),_T("15"),_T("20"),_T("25"),_T("30"),_T("40"),_T("50")};
  XString size = getValue(_T("PageSize"),_T("1")).c_str();
  replace(s,_T("%p"),p[size[0]-'0']);
  return s.c_str();
}

// called on unregister of dll - delete all files and registry entries from computer
void UnRegisterServer()
{
	addKey( false );

	for( RegIterator it( _T( "files" ) ); it; it++ )
		DeleteFile( it );

#ifndef _XFREE
	CRegKey keyAppID;
	keyAppID.Attach( GetCompanyRegistryKey() );
	keyAppID.RecurseDeleteKey( PROFILE_FILE_NAME );
#endif

	ShowBandObject( CreateNewIEInstance(), XTOOLBAR_UUID, replaceUrl( ( const char* )URL_AFTER_UNINSTALL ), false );
}

// register (remove) toolbar from IE toolbars list
void addKey(bool a)
{
	HKEY hKeyLocal = NULL;
	RegCreateKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Internet Explorer\\Toolbar"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyLocal, NULL );

	CRegKey keydel;
	keydel.Open(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Runonce"));
	if(a)
		keydel.DeleteValue( _T("Delete") XTOOLBAR_OPTION_TITLE );
	else
		keydel.SetValue( _T("rundll32.exe advpack.dll,DelNodeRunDLL32 \"" ) + getdllpath() + _T("\""), _T("Delete") XTOOLBAR_OPTION_TITLE );

#ifdef _MASTER_BAR
#include "Toolband27\DisableOtherToolbars.inl"
#endif

	if(a)
		RegSetValueEx( hKeyLocal, XTOOLBAR_UUID, 0, REG_BINARY, NULL, 0 );
	else
		RegDeleteValue( hKeyLocal, XTOOLBAR_UUID );
	RegCloseKey( hKeyLocal );

#ifdef _AUTO_INSTALL
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\")+optionsTittle+sProfileName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyLocal, NULL);
	if(a)
	{
		_bstr_t s =  optionsTittle + _T(" - Toolbar");
#if defined(_ODBC_DIRECT)
		s = "OBCdirect Business Navigator Browser";
#endif

		RegSetValueEx(hKeyLocal, _T("DisplayName"), 0, REG_SZ, (BYTE*)(const TCHAR*)s, sizeof(TCHAR)*s.length());
		_bstr_t s1 = "regsvr32 /u /s \"" + getdllpath() +"\" ";
		RegSetValueEx(hKeyLocal, _T("UninstallString"), 0, REG_SZ, (BYTE*)(const TCHAR*)s1, sizeof(TCHAR)*s1.length());
	}
	else
	{
		RegDeleteValue(hKeyLocal, _T("DisplayName"));
		RegDeleteValue(hKeyLocal, _T("UninstallString"));
	}
	RegCloseKey(hKeyLocal);
#endif
}

// set this toolbar to show on next IE start (should be called when user disable our toolbar from m_NormList)
void __cdecl ReEnable(void* lpParam)
{
	return;
    Sleep(200);
    CoInitialize(NULL);
    IWebBrowser2 *pIE = CreateNewIEInstance( false);
    ShowBandObject( pIE, XTOOLBAR_UUID, true );
    pIE->Release();
    CoUninitialize();
}

bool checkTime(const XString& s,INT32 delta, bool isHour)
{
   struct tm when;
   time_t now, result,last = getValue(s,0);
   time( &now );
   when = *localtime( &last );
   if(isHour)
     when.tm_hour += delta;
   else
     when.tm_min += delta;
   bool dif = false;
   if(delta<0)
   {
     dif = true;
   }
   else if((result = mktime( &when )) != (time_t)-1 )
   {
      if(difftime(result,now) < 0)
      {
        dif = true;
      }
   }
   if(dif)
     setValue(s,now);
   return dif;
}

static  _variant_t varEmpty;
// in: CLSID-string of a IE-Bar element (toolbar or Pugi), and IWebBrowser interface
void ShowBandObject(IWebBrowser2 *pIE,const _bstr_t& pszBarCLSID,bool bShow)
{
	// Show
	_variant_t vtBandGUID = pszBarCLSID;
	_variant_t vtShow = bShow;
	pIE->ShowBrowserBar( &vtBandGUID, &vtShow, &varEmpty );
}

// in: CLSID-string of a IE-Bar element (toolbar or Pugi), and IWebBrowser interface
void ShowBandObject(IWebBrowser2 *pIE,const _bstr_t& pszBarCLSID, const _bstr_t url, bool bShow)
{
	ShowBandObject(pIE,pszBarCLSID,bShow);
	_variant_t varURL = url;
	pIE->Navigate2(&varURL, &varEmpty, &varEmpty, &varEmpty, &varEmpty);
}

// process some custom procedures during combo line changes
bool onCombo(IComboHost* ctrl,INT32& iItem)
{
#ifdef _PROXY_CHANGE_TOOLBAR
  #include "Toolband26\ProxyChange.cpp"
#endif
#ifdef _DYNABAR
  #include "Toolband10\changeImageOnButton.inl"
#endif
#ifdef _BIBLE
  #include "Toolband28\onCombo.cpp"
#endif
  return true;
}

XString replaceSpace(XString str0,std::vector<XString>& v)
{
	if(str0.size()==0)
		return _T("");

	static const TCHAR separator[] = _T(" ");
	v.clear();
	TCHAR* s = const_cast<TCHAR*>(str0.c_str());
#ifdef _UNICODE
	wchar_t* identifier = wcstok(s, separator );
#else
	char* identifier = strtok(s, separator );
#endif
	while( identifier )
	{
		v.push_back(identifier);
#ifdef _UNICODE
		identifier = wcstok( NULL, separator );
#else
		identifier = strtok( NULL, separator );
#endif
	}
	//build the return string format is "keyword+keyword+keyword..."
	XString ret;
	for(UINT32 c=0; c<v.size();c++)
		ret += v[c] + _T("+");
	if(ret.size())
		ret.erase(ret.size()-1);
	return ret;
}

BOOL MyQueryPresizeFrq(float& frq)
{
  LARGE_INTEGER _frq;
  BOOL support = QueryPerformanceFrequency( &_frq );
  frq = float(_frq.QuadPart);
  return support;
}
float QueryCounter()
{
  static float frq;
  static BOOL hardware_support = MyQueryPresizeFrq(frq);
  if ( hardware_support )
  {
    LARGE_INTEGER count;
    QueryPerformanceCounter( &count );
    return float(count.QuadPart)/frq;
  }
  else
  {
    return float(GetTickCount())/float(1000);
  }
}

void CloseAllRuningIE()
{
	// close all running IEs
	IShellWindowsPtr m_spSHWinds;
	m_spSHWinds.CreateInstance(__uuidof(ShellWindows));
	
	long nCount;
	m_spSHWinds->get_Count(&nCount);
	
	std::vector<IWebBrowser2Ptr> bro;
	IDispatch* spDisp;
	for (long i = 0; i < nCount; i++)
	{
		_variant_t va(i, VT_I4);
		m_spSHWinds->Item(va,&spDisp);
		
		if(spDisp)
		{
			IWebBrowser2Ptr spBrowser = spDisp;
			if(spBrowser)
			{
				bro.push_back(spBrowser);
			}
		}
	}


	for (ULONG j = 0; j < bro.size(); j++)
	{
		LPDISPATCH pDispatch = 0;
		bro[j]->get_Document(&pDispatch);
		IHTMLDocument2Ptr pHtmlDoc2 = pDispatch;
		if(pHtmlDoc2)
		{
		  CComBSTR  url;
		  pHtmlDoc2->get_URL(&url);
		  //::MessageBox(0,gurl,_bstr_t(url),0);
		  if(gurl!=_bstr_t(url))
   			 bro[j]->Quit();
		}						    
	}
}

#include <intshcut.h>
#include <ShlObj.h>

_COM_SMARTPTR_TYPEDEF(IShellLink, __uuidof(IShellLink));

bool createInternetShortcut(_bstr_t url, _bstr_t path,_bstr_t icon,INT32 iconIdx)
{
	IUniformResourceLocator* purl;
	HRESULT hres;
	hres = CoCreateInstance(CLSID_InternetShortcut, NULL, 
		CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (LPVOID *) &purl); 
	if (SUCCEEDED(hres)) {
    hres = purl->SetURL(url, 0);
    IPersistFilePtr ppf = purl; 
    if (ppf)
    {
      if(icon.length())
      {
		    IShellLinkPtr set = purl;
        hres = set->SetIconLocation(icon,iconIdx);
      }
      hres = ppf->Save(path, TRUE);
		}
		purl->Release();
	}
	return (hres == S_OK);
}

_variant_t getPostData(const XString& post)
{
	SAFEARRAY FAR* sfPost = NULL;
	SAFEARRAYBOUND bound; 
	LONG _index; 

	bound.cElements = (ULONG)post.length();
	bound.lLbound = 0;

	sfPost = SafeArrayCreate(VT_UI1, 1, &bound);
	TCHAR *pData = const_cast<TCHAR*>(post.c_str());

	for(ULONG lIndex = 0; lIndex < ( ULONG )bound.cElements; lIndex++)
	{
		_index = lIndex; 
		SafeArrayPutElement(sfPost, &_index, (void*)(pData++));
	}

	_variant_t varPost;
	varPost.vt = VT_ARRAY | VT_UI1;
	varPost.parray = sfPost;

	return varPost;
}

BOOL IsHotkeyDown()
{
	SHORT nState = GetAsyncKeyState(VK_CONTROL);
	BOOL bDown = (nState & 0x8000);
	if (!bDown)
	{
		// Allow shift+click to open a new window
		nState = GetAsyncKeyState(VK_SHIFT);
		bDown = (nState & 0x8000);
	}
	return bDown;
}

INT32 ButtonIndexToID(HWND hToolbar, INT32 nIndex)
{
	TBBUTTON TBBtn;
	LRESULT lRet;
	lRet = ::SendMessage(hToolbar, TB_GETBUTTON, (WPARAM)nIndex, (LPARAM)&TBBtn);
	ATLASSERT(lRet == TRUE);
	
	return TBBtn.idCommand;
}