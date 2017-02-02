#ifndef __BANDSTATICCTRL_H_
#define __BANDSTATICCTRL_H_


#include <atlctrls.h>
#include <atlmisc.h>
#include <commctrl.h>

class CToolBarObj;

/////////////////////////////////////////////////////////////////////////////
// CBandStaticCtrl
class CBandStaticCtrl : public CWindowImpl<CBandStaticCtrl, CStatic>,
                      public IDropTarget  

{
public:	
	CBandStaticCtrl();
	virtual ~CBandStaticCtrl();
    void SetWebBrowser(IWebBrowser2* pWebBrowser) {m_pWebBrowser = pWebBrowser;}

// Operations
public:
    STDMETHOD(TranslateAcceleratorIO)(LPMSG pMsg);

    // Support Drag and Drop
	STDMETHODIMP QueryInterface(REFIID, VOID**);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

    STDMETHODIMP DragEnter(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
	STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD);
	STDMETHODIMP DragLeave(void);
	STDMETHODIMP Drop(LPDATAOBJECT, DWORD, POINTL, LPDWORD);

    DECLARE_WND_SUPERCLASS(NULL, TEXT("EDIT"))

	BEGIN_MSG_MAP(CBandStaticCtrl)
        OCM_COMMAND_CODE_HANDLER(EN_SETFOCUS, OnSetFocus)
        MESSAGE_HANDLER(WM_CHAR, OnChar)
        MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
	END_MSG_MAP()

	CToolBarObj* m_pBand;
    LPDATAOBJECT m_pIDataObject;

protected:
    LRESULT OnSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	ULONG m_cRef;
    IWebBrowser2* m_pWebBrowser;

};

///////////////////////////////////////////////////////////////////////////////
//// CBandComboBoxCtrl - Written by Mike Melnikov
//class CBandComboBoxCtrl : public CWindowImpl<CBandComboBoxCtrl, CComboBox>,
//                          public IDropTarget  
//{
//public:	
//    CBandComboBoxCtrl();
//	virtual ~CBandComboBoxCtrl();
//    void SetWebBrowser(IWebBrowser2* pWebBrowser) {m_pWebBrowser = pWebBrowser;}
//    //@@CContainedWindow m_edit2;
//
//// Operations
//public:
//	STDMETHOD(TranslateAcceleratorIO)(LPMSG pMsg);
//
//    // Support Drag and Drop
//	STDMETHODIMP QueryInterface(REFIID, VOID**);
//	STDMETHODIMP_(ULONG) AddRef(void);
//	STDMETHODIMP_(ULONG) Release(void);
//
//    STDMETHODIMP DragEnter(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
//	STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD);
//	STDMETHODIMP DragLeave(void);
//	STDMETHODIMP Drop(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
//                             
//    DECLARE_WND_SUPERCLASS(_T("Band_ComboBox"), TEXT("COMBOBOX"))
//
//	BEGIN_MSG_MAP(CBandComboBoxCtrl)
//        REFLECTED_COMMAND_CODE_HANDLER(CBN_EDITUPDATE, OnTVSelDate)
//        REFLECTED_COMMAND_CODE_HANDLER(CBN_SELENDOK, OnTVSelChanged)
//        REFLECTED_COMMAND_CODE_HANDLER(CBN_SETFOCUS, OnTVSetFocus)
//        REFLECTED_COMMAND_CODE_HANDLER(CBN_KILLFOCUS, OnTVKillFocus)
//
//        //ALT_MSG_MAP(1)
//	        //MESSAGE_HANDLER(WM_CHAR, OnChar)
//    END_MSG_MAP()
//
///*    LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//    {
//        if (wParam != VK_RETURN)
//            bHandled = FALSE;
//    
//        return 0;
//    }
//*/
//
//    LRESULT OnTVSelDate(WORD, WORD pnmh, HWND,BOOL& );
//    LRESULT OnTVSelChanged(WORD, WORD pnmh, HWND,BOOL& );
//    LRESULT OnTVSetFocus(WORD, WORD pnmh, HWND,BOOL& );
//    LRESULT OnTVKillFocus(WORD, WORD pnmh, HWND,BOOL& );  
//    LRESULT Process(_bstr_t varURL);
//	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//
//	CToolBarObj* m_pBand;
//    LPDATAOBJECT m_pIDataObject;
//
//    bool InsertHistory();
//    void SetDropDownSize(UINT LinesToDisplay);
//
//protected:
//	ULONG m_cRef;
//    bool m_bAuto;
//    IWebBrowser2* m_pWebBrowser;
//};
//
#endif //__BANDSTATICCTRL_H_