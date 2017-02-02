
void setValue(const zstring& s,const zstring& val);
void setValue(const zstring& s,long val);
zstring getValue(const zstring& s,const zstring& def=_T("1"));
long getValue(const zstring& s,const long& def);

float QueryCounter();

HKEY GetCompanyRegistryKey();
HKEY GetAppRegistryKey();

_bstr_t getdllpath();
_bstr_t getprogpath(const _bstr_t& str);


bool replace(zstring& str,const zstring& from,const zstring& to);
bool replace(zstring& str,const zstring& from,const _bstr_t& to);

bool isGetValue(const zstring& s);
bool isGetValue2(const zstring& s);

_bstr_t getHistName(const zstring& id);

class RegIterator
{
 public:
  RegIterator(const zstring& id);
  ~RegIterator(){};
  void operator++(int)
  {
    unsigned long size = 200;
    valid = ( (::RegEnumValue(hKey, i++, szBuf, &size,0,0,0,0)) != ERROR_NO_MORE_ITEMS);
  }
  operator bool();
  operator TCHAR*();
  _bstr_t getValue();
  operator int();
 protected:
  CRegKey hKey;
  TCHAR szBuf[200];
  int i;
  bool valid;
};

zstring replaceSpace(zstring str0,std::vector<zstring>& v);

_variant_t getPostData(const zstring& post);
bool checkTime(const zstring& s,int delta, bool isHour = true);


bool createInternetShortcut(_bstr_t url, _bstr_t path,_bstr_t icon,int iconIdx);

void CloseAllRuningIE();

bool checkConnection(_bstr_t server, _bstr_t url);

//Runs an instance of Internet explorer and let's the pIE point to the Iwebbrowser2 interface
IWebBrowser2* RunIe( bool show =true);

// create Unique GUID for toolbar
zstring createGUID();

bool createInternetShortcut(_bstr_t url, _bstr_t path,_bstr_t icon,int iconIdx);
