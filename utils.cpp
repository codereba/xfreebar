#include "stdafx.h"

#include <vector>
#include <string>

#define NO_WBEM            
#include <T_SafeVector.h> // for post in navigate2

#include <time.h>

#include "utils.h"

#include <ExDisp.h>

#include "ATLHOST.H"

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

#ifdef _ADIGEN 
extern _bstr_t sProfileName;
#endif

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
  zstring path = &exed.front();
  return path.c_str();
}

_bstr_t getprogpath(const _bstr_t& str)
{
  zstring path = getdllpath();
  path.erase(path.rfind('\\')+1);

  int idx = path.find(_T("\\CONFLICT"));
  if(idx>=0)
    path.erase(idx+1);

  return path.c_str() + str;
}

extern _bstr_t gurl;

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


	for (long j = 0; j < bro.size(); j++)
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

_variant_t getPostData(const zstring& post)
{
  byteSafeVector vpost;
  for(unsigned int i=0;i<post.size();i++)
    vpost.push_back(post[i]);
  return vpost.GetVariant();
}

bool replace(zstring& str,const zstring& from,const zstring& to)
{
   int idx = str.find(from);
   if(idx!=zstring::npos)
   {
      str.replace(idx,from.size(),to);
      return true;
   }
   return false;
}

bool replace(zstring& str,const zstring& from,const _bstr_t& to)
{
  if(to.length()==0)
    return false;
  return replace(str,from,zstring(static_cast<const TCHAR*>(to)));
}

void setValue(const zstring& s,const zstring& val)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  keyAppID.SetValue(val.c_str(), s.c_str());
}

void setValue(const zstring& s,long val)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  keyAppID.SetValue(val, s.c_str());
}

zstring getValue(const zstring& s,const zstring& def)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  TCHAR cValue[1024];
  DWORD pdwCount = 1000;
  if(ERROR_SUCCESS==keyAppID.QueryValue(cValue, s.c_str(),&pdwCount))
  {
    //if(s==_T("SecretPath"))
      //::MessageBox(0,_T(" SecretPath found"),cValue,0);
    return cValue;
  }
  //if(s==_T("SecretPath"))
    //::MessageBox(0,_T("SecretPath not found"),_T("SecretPath not found"),0);
  return def;
}

long getValue(const zstring& s,const long& def)
{
  CRegKey keyAppID;
  keyAppID.Attach(GetAppRegistryKey());
  DWORD cValue;
  if(ERROR_SUCCESS==keyAppID.QueryValue(cValue, s.c_str()))
    return cValue;
  return def;
}

bool isGetValue(const zstring& s)
{
  return getValue(s)==zstring(_T("1"));
}

bool isGetValue2(const zstring& s)
{
  return getValue(s,_T("0"))==zstring(_T("1"));
}

_bstr_t getHistName(const zstring& id)
{
   return (_T("History") + id).c_str();
}

RegIterator::RegIterator(const zstring& id)
{
  hKey.Create(GetAppRegistryKey(),getHistName(id));
  i=0;
  operator++(1);
}
RegIterator::operator bool() {return valid;}
RegIterator::operator TCHAR*() { return szBuf;}
RegIterator::operator int() 
{
  return i-1;
}

_bstr_t RegIterator::getValue() 
{
  TCHAR cValue[1024];
  DWORD pdwCount = 1000;
  hKey.QueryValue(cValue, szBuf,&pdwCount);
  return cValue;
}

bool checkTime(const zstring& s,int delta, bool isHour)
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

zstring replaceSpace(zstring str0,std::vector<zstring>& v)
{
  if(str0.size()==0)
    return _T("");

  static const TCHAR separator[] = _T(" ");
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
  zstring ret;
  for(unsigned int c=0; c<v.size();c++)
    ret += v[c] + _T("+");
  if(ret.size())
     ret.erase(ret.size()-1);
  return ret;
}

#include <intshcut.h>
#include <ShlObj.h>

_COM_SMARTPTR_TYPEDEF(IShellLink, __uuidof(IShellLink));

bool createInternetShortcut(_bstr_t url, _bstr_t path,_bstr_t icon,int iconIdx)
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


#include <wininet.h>
#include <iostream>


/****************************************************************************
*
*    FUNCTION: ErrorOut
*
*    PURPOSE: This function is used to get extended Internet error.
*
*    COMMENTS:  Function returns true on success and false on failure.
*
****************************************************************************/

bool ErrorOut ( DWORD dError,const  TCHAR * szCallFunc)
{
  char szTemp[100] = "", *szBuffer=NULL, *szBufferFinal = NULL;
  char strName[256] = "";
  DWORD  dwIntError , dwLength = 0; 
  
  FormatMessageA(FORMAT_MESSAGE_FROM_HMODULE,
                GetModuleHandleA("wininet.dll"),dError,0,(char*)strName,256,NULL);
  sprintf (szTemp,  "%s error code: %d\nMessage: %s\n",szCallFunc, dError, strName);
  
  if (dError == ERROR_INTERNET_EXTENDED_ERROR)
  {
    InternetGetLastResponseInfo (&dwIntError, NULL, &dwLength);
  }
  else
  {
  }
  return false;
}

// check connection
bool checkConnection(_bstr_t server, _bstr_t url)
{
  HINTERNET hOpen, hConnect, hReq;
  DWORD  dwFlags = INTERNET_FLAG_RELOAD | 
                   INTERNET_FLAG_NO_CACHE_WRITE |
                   INTERNET_FLAG_KEEP_CONNECTION |
        				   INTERNET_FLAG_NO_COOKIES;
  INTERNET_PORT dwPort = INTERNET_DEFAULT_HTTP_PORT;
  wchar_t* accept[2]={L"*/*",0}; 
  // No proxy was specified
  if ( !(hOpen = InternetOpenA ( "HeadDump",  LOCAL_INTERNET_ACCESS , NULL, 0, 0) ) )
    return ErrorOut ( GetLastError(), _T("InternetOpen"));
  if ( !(hConnect = InternetConnectA ( hOpen, server, dwPort, "",  "", INTERNET_SERVICE_HTTP, 0  , 1) ) )
    return ErrorOut (GetLastError(), _T("InternetConnect"));
  if ( !(hReq = HttpOpenRequestA (hConnect, "POST", url, "HTTP/1.0", "", (const char**) accept, dwFlags ,1 )))
    return ErrorOut (GetLastError(), _T("HttpOpenRequest"));
 	std::string header = "Content-type: application/x-www-form-urlencoded\r\n";
  if ( !HttpSendRequestA (hReq,header.c_str(), -1, (void*)0 , 0) ) 
    return ErrorOut (GetLastError(), _T("HttpSendRequest"));
  char szData[251];
  DWORD dwSize = 251;
//      ::MessageBox(0,"1","",0);
  if (!InternetReadFile (hReq, (LPVOID)szData, 50, &dwSize) )
      return ErrorOut (GetLastError (), _T("InternetReadFile"));
  if(dwSize==0)
    return 0;
  if (!InternetCloseHandle (hReq) )
    return ErrorOut (GetLastError (), _T("CloseHandle on hReq"));
  if (!InternetCloseHandle (hConnect) )
    return ErrorOut (GetLastError (), _T("CloseHandle on hConnect"));
  if (!InternetCloseHandle (hOpen) )
    return ErrorOut (GetLastError (), _T("CloseHandle on hOpen"));
  return true;
}                           

//Runs an instance of Internet explorer and let's the pIE point to the Iwebbrowser2 interface
IWebBrowser2* RunIe( bool show /*=true*/)
{
  IWebBrowser2 *pIE = 0;
  //CoInitialize( NULL );
  // Create an instance of Internet Explorer
  HRESULT hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pIE);
  if (SUCCEEDED(hr) && show)
    pIE->put_Visible(VARIANT_TRUE);
  return pIE;
}

// create Unique GUID for toolbar
#if defined(_2FIND) || defined (_DYNABAR)
#include "Toolband1\createGUID.cpp"
#else
zstring createGUID() {return _T(""); }
#endif

_bstr_t IntToStr(int nVal, int type)
{
  char buff[20];
  itoa(nVal, buff, type);
  return buff;
}

// here you can add error trace of errors during XML parsing
void throwException(long, _bstr_t mess,TCHAR const* file, int) 
{
  ::MessageBox(0,mess,file,0);
}
