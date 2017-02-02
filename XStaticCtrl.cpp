// ============================================================================
// Sample by Rashid Thadha Aug 2001
// ============================================================================

#include "stdafx.h"
#include "BandStaticCtrl.h"

#include "XFreeBar.h"
#include "XToolBarHost.h"
#include "regoper.h"

#include <string>

// CBandStaticCtrl
CBandStaticCtrl::CBandStaticCtrl() : m_cRef(0), m_pWebBrowser(NULL)
{
}

CBandStaticCtrl::~CBandStaticCtrl()
{
}

// Because the host owns the message processing, it will get all keyboard messages
// first.
// 1. We want to use the ENTER key to signal that the user is done in the edit control,
//    so we need to watch for VK_RETURN on WM_KEYUP
// 2. We also want the BACKSPACE, DELETE, END, ARROW keys to go to the edit control and
//	  not to the standard IE interfaces. We translate/dispatch these messages ourselves.
STDMETHODIMP CBandStaticCtrl::TranslateAcceleratorIO(LPMSG pMsg)
{
	int nVirtKey = (int)(pMsg->wParam);
	if (WM_KEYUP == pMsg->message && VK_RETURN == nVirtKey)
	{
		LPCTSTR strNewItem;
		TCHAR cValue[1024];
		GetWindowText(cValue, sizeof(cValue));
		LPCTSTR strEdit(cValue);

	    _variant_t varURL = _bstr_t(strEdit);
	    _variant_t varEmpty;
	    m_pWebBrowser->Navigate2(&varURL, &varEmpty, &varEmpty, &varEmpty, &varEmpty);

		return S_OK;
	}
	else if ((WM_KEYUP == pMsg->message || WM_KEYDOWN == pMsg->message) &&
				VK_BACK == nVirtKey || (VK_END <= nVirtKey && VK_DOWN >= nVirtKey) ||
				VK_DELETE == nVirtKey)
	{

		TranslateMessage(pMsg);
		DispatchMessage(pMsg);

		return S_OK;
	}
	else
		return S_FALSE;
}

LRESULT CBandStaticCtrl::OnSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pBand->FocusChange(true);
	
    return 0;
}

LRESULT CBandStaticCtrl::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// stop the dinging sound when you press the return key !!!!
    bHandled = (wParam == VK_RETURN);

	return 0;
}
        
STDMETHODIMP CBandStaticCtrl::QueryInterface(REFIID riid, VOID** ppv)   
{     
	*ppv=NULL;      
	if (IID_IUnknown==riid || IID_IDropTarget==riid)         
		*ppv=this;      
	if (NULL!=*ppv) 
    {         
		((LPUNKNOWN)*ppv)->AddRef();         
		return NOERROR;         
	}      
	
    return E_NOINTERFACE;
}   

STDMETHODIMP_(ULONG) CBandStaticCtrl::AddRef(void)     
{     
	return ++m_cRef;     
}  

STDMETHODIMP_(ULONG) CBandStaticCtrl::Release(void)   
{      
	if (0!=--m_cRef)         
		return m_cRef;      	
    delete this;     
	return 0;     
}      


STDMETHODIMP CBandStaticCtrl::DragEnter(LPDATAOBJECT pDataObj , DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)    
{     
	m_pIDataObject=NULL;
	LPCTSTR sText;
	
    FORMATETC formatDetails;     
	STGMEDIUM stmg;     

	// Let's get the text from the "clipboard"
 	formatDetails.cfFormat = CF_TEXT;
	formatDetails.dwAspect = DVASPECT_CONTENT;
	formatDetails.lindex = -1;
	formatDetails.ptd = NULL;
	formatDetails.tymed = TYMED_HGLOBAL;

	if (SUCCEEDED(pDataObj->GetData(&formatDetails, &stmg)))
    {
	    sText = (LPSTR)GlobalLock(stmg.hGlobal);
	    GlobalUnlock(stmg.hGlobal);

	    if (stmg.hGlobal != NULL) 
		    ReleaseStgMedium(&stmg);             
    }

	*pdwEffect=DROPEFFECT_COPY;

	m_pIDataObject=pDataObj;     
	m_pIDataObject->AddRef();      
	
	return NOERROR;     
}       
        
STDMETHODIMP CBandStaticCtrl::DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)   
{     
	if (m_pIDataObject == NULL) 
		*pdwEffect=DROPEFFECT_NONE;         
	else 
		*pdwEffect=DROPEFFECT_COPY;

	return NOERROR;     
}       

STDMETHODIMP CBandStaticCtrl::DragLeave(void)     
{     
	m_pIDataObject->Release();
	return NOERROR;     
}      

STDMETHODIMP CBandStaticCtrl::Drop(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)     
{     
	*pdwEffect=DROPEFFECT_NONE;      
	if (m_pIDataObject == NULL)         
		return E_FAIL;

	DragLeave();      
	
    FORMATETC formatDetails;     
	STGMEDIUM stmg;     

	// Let's get the text from the "clipboard"
 	formatDetails.cfFormat = CF_TEXT;
	formatDetails.dwAspect = DVASPECT_CONTENT;
	formatDetails.lindex = -1;
	formatDetails.ptd = NULL;
	formatDetails.tymed = TYMED_HGLOBAL;

	if (SUCCEEDED(pDataObj->GetData(&formatDetails, &stmg)))
    {	
	    LPTSTR sClipboard = (LPTSTR)GlobalLock(stmg.hGlobal);
	    SetWindowText(sClipboard);
	    GlobalUnlock(stmg.hGlobal);

	    if (stmg.hGlobal != NULL) 
		    ReleaseStgMedium(&stmg);         
    }
    
	// Navigate
	TCHAR cValue[1024];
	GetWindowText(cValue, sizeof(cValue));
	LPCTSTR strEdit(cValue);

	_variant_t varURL = _bstr_t(strEdit);
	_variant_t varEmpty;
	m_pWebBrowser->Navigate2(&varURL, &varEmpty, &varEmpty, &varEmpty, &varEmpty);

	*pdwEffect=DROPEFFECT_COPY;      
	return NOERROR;     
} 

LRESULT CBandStaticCtrl::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pBand && m_pBand->GetToolBarCtrl())
    {
        int nState = m_pBand->GetToolBarCtrl()->GetState(ID_BUTTON_GREEN);
	    if (nState != -1)
        {
		    TCHAR cText[2048];
            GetWindowText(cText, 2048);
            CString sText(cText);
            m_pBand->GetToolBarCtrl()->SetState(ID_BUTTON_GREEN, sText.GetLength() ? nState | TBSTATE_ENABLED : nState & ~TBSTATE_ENABLED);
            m_pBand->GetToolBarCtrl()->SetState(ID_BUTTON_RED, sText.GetLength() ? nState | TBSTATE_ENABLED : nState & ~TBSTATE_ENABLED);
        }
    }

	return 0;
}

//// =========================================================================================================
//// CBandComboBoxCtrl - Written by Mike Melnikov
//
//CBandComboBoxCtrl::CBandComboBoxCtrl() : m_cRef(0), m_pBand(NULL), m_bAuto(true)
//{
//}
//
//CBandComboBoxCtrl::~CBandComboBoxCtrl()
//{
//}
//
//
//// Because the host owns the message processing, it will get all keyboard messages
//// first.
//// 1. We want to use the ENTER key to signal that the user is done in the edit control,
////    so we need to watch for VK_RETURN on WM_KEYUP
//// 2. We also want the BACKSPACE, DELETE, END, ARROW keys to go to the edit control and
////	  not to the standard IE interfaces. We translate/dispatch these messages ourselves.
//STDMETHODIMP CBandComboBoxCtrl::TranslateAcceleratorIO(LPMSG pMsg)
//{
//	int nVirtKey = (int)(pMsg->wParam);
//	if ((WM_KEYUP == pMsg->message || WM_KEYDOWN == pMsg->message) && VK_RETURN == nVirtKey)
//	{
//        TCHAR cValue[1024];
//        GetWindowText(cValue, sizeof(cValue));
//        Process(_bstr_t(cValue));
//
//		LPCTSTR strEdit(cValue);
//	    _variant_t varURL = _bstr_t(strEdit);
//	    _variant_t varEmpty;
//	    m_pWebBrowser->Navigate2(&varURL, &varEmpty, &varEmpty, &varEmpty, &varEmpty);
//		return S_OK;
//	}
//	else if ((WM_KEYUP == pMsg->message || WM_KEYDOWN == pMsg->message) &&
//			VK_BACK == nVirtKey || (VK_END <= nVirtKey && VK_DOWN >= nVirtKey) ||
//			VK_DELETE == nVirtKey || ( (GetKeyState(VK_CONTROL)&0x80) && (nVirtKey=='C' || nVirtKey=='V' || nVirtKey=='X')))
//	{        m_bAuto = false;
//        TranslateMessage(pMsg);
//        DispatchMessage(pMsg);
//        m_bAuto = true;
//        if(WM_KEYDOWN == pMsg->message && VK_BACK == nVirtKey) 
//            m_bAuto = false;
//		return S_OK;
//	}
//	else
//		return S_FALSE;
//}
//
//
//STDMETHODIMP CBandComboBoxCtrl::QueryInterface(REFIID riid, VOID** ppv)   
//{     
//    *ppv=NULL;      
//    if (IID_IUnknown==riid || IID_IDropTarget==riid)         
//	    *ppv=this;      
//    if (NULL!=*ppv) 
//    {         
//	    ((LPUNKNOWN)*ppv)->AddRef();         
//	    return NOERROR;         
//    }      
//
//    return E_NOINTERFACE;
//}   
//
//STDMETHODIMP_(ULONG) CBandComboBoxCtrl::AddRef(void)     
//{     
//	return ++m_cRef;     
//}  
//
//STDMETHODIMP_(ULONG) CBandComboBoxCtrl::Release(void)   
//{      
//	if (0!=--m_cRef)         
//		return m_cRef;      	
//    delete this;     
//	return 0;     
//}      
//
//
//STDMETHODIMP CBandComboBoxCtrl::DragEnter(LPDATAOBJECT pDataObj , DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)    
//{     
//	m_pIDataObject=NULL;
//	LPCTSTR sText;
//	
//    FORMATETC formatDetails;     
//	STGMEDIUM stmg;     
//
//	// Let's get the text from the "clipboard"
// 	formatDetails.cfFormat = CF_TEXT;
//	formatDetails.dwAspect = DVASPECT_CONTENT;
//	formatDetails.lindex = -1;
//	formatDetails.ptd = NULL;
//	formatDetails.tymed = TYMED_HGLOBAL;
//
//	if (SUCCEEDED(pDataObj->GetData(&formatDetails, &stmg)))
//    {
//	    sText = (LPSTR)GlobalLock(stmg.hGlobal);
//	    GlobalUnlock(stmg.hGlobal);
//
//	    if (stmg.hGlobal != NULL) 
//		    ReleaseStgMedium(&stmg);             
//    }
//
//	*pdwEffect=DROPEFFECT_COPY;
//
//	m_pIDataObject=pDataObj;     
//	m_pIDataObject->AddRef();      
//	
//	return NOERROR;     
//}       
//        
//STDMETHODIMP CBandComboBoxCtrl::DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)   
//{     
//	if (m_pIDataObject == NULL) 
//		*pdwEffect=DROPEFFECT_NONE;         
//	else 
//		*pdwEffect=DROPEFFECT_COPY;
//
//	return NOERROR;     
//}       
//
//STDMETHODIMP CBandComboBoxCtrl::DragLeave(void)     
//{     
//	m_pIDataObject->Release();
//	return NOERROR;     
//}      
//
//STDMETHODIMP CBandComboBoxCtrl::Drop(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)     
//{     
//	*pdwEffect=DROPEFFECT_NONE;      
//	if (m_pIDataObject == NULL)         
//		return E_FAIL;
//
//	DragLeave();      
//	
//    FORMATETC formatDetails;     
//	STGMEDIUM stmg;     
//
//	// Let's get the text from the "clipboard"
// 	formatDetails.cfFormat = CF_TEXT;
//	formatDetails.dwAspect = DVASPECT_CONTENT;
//	formatDetails.lindex = -1;
//	formatDetails.ptd = NULL;
//	formatDetails.tymed = TYMED_HGLOBAL;
//
//    if (SUCCEEDED(pDataObj->GetData(&formatDetails, &stmg)))
//    {	
//        char* sClipboard = (char*)GlobalLock(stmg.hGlobal);
//        ::SetWindowTextA(m_hWnd,sClipboard);
//
//        Process(sClipboard);
//
//	    GlobalUnlock(stmg.hGlobal);
//
//	    if (stmg.hGlobal != NULL) 
//		    ReleaseStgMedium(&stmg);         
//    }
//
//	*pdwEffect=DROPEFFECT_COPY;      
//	return NOERROR;     
//} 
//
//LRESULT CBandComboBoxCtrl::OnTVSetFocus(WORD, WORD pnmh, HWND,BOOL& )
//{
//    m_pBand->FocusChange(true);
//    return 0;
//}
//
//
//LRESULT CBandComboBoxCtrl::OnTVKillFocus(WORD, WORD pnmh, HWND,BOOL& )
//{
//    m_pBand->FocusChange(false);
//    return 0;
//}
//
//
//bool CBandComboBoxCtrl::InsertHistory()
//{
//    ResetContent();
//
//    for (RegIterator it; it; it++)
//        InsertString(-1, it);
//
//    //@@SetDropDownSize(7);
//
//    return true;
//}
//
//// This will add the history of data used
//LRESULT CBandComboBoxCtrl::Process(_bstr_t bstrURL) 
//{
//    CRegKey keyAppID;
//
//    if(std::basic_string<TCHAR>(bstrURL).find_first_not_of(_T(" "))!=std::string::npos)
//    {
//        int h = GetCount();
//        int pos = FindStringExact(-1,bstrURL);
//        if(pos == CB_ERR) 
//        {
//            // not found
//            if(h<50)
//            {
//                // normal
//                InsertString(0,bstrURL);
//            }
//            else
//            {
//                // replace last
//                DeleteString(h-1);
//                InsertString(0,bstrURL);
//            }
//        }
//        else
//        {
//            // move to last
//            DeleteString(pos);
//            InsertString(0,bstrURL);
//        }
//
//        {
//        CRegKey keyAppID;
//		keyAppID.Attach(GetAppRegistryKey());
//        keyAppID.RecurseDeleteKey(_T("History"));
//        }
//
//        {
//        CRegKey keyAppID;
//		keyAppID.Create(GetAppRegistryKey(),_T("History"));
//        for(int i=0;i<GetCount();i++)
//        {
//          TCHAR cValue[1024];             
//          GetLBText(i, cValue);
//          keyAppID.SetValue(1,cValue);
//        }
//        }
//
//        SetCurSel(0);
//    }
//
//    return 0;
//}
//
//
//LRESULT CBandComboBoxCtrl::OnTVSelChanged(WORD, WORD pnmh, HWND,BOOL& handled) 
//{
//    int iItem = GetCurSel();
//    if(iItem>=0)
//    {
//        TCHAR cValue[1024];             
//        GetLBText(iItem, cValue);
//        //m_edit2.SetWindowText(cValue);
//
//        Process(_bstr_t(cValue));
//
//    }
//
//    return 0;
//}
//
//LRESULT CBandComboBoxCtrl::OnTVSelDate(WORD, WORD pnmh, HWND,BOOL&) 
//{
//    // Get the text in the edit box
//    TCHAR str[200];
//    GetWindowText(str, 200);
//    int nLength = GetWindowTextLength();
//
//    if (!m_bAuto)
//    {
//        return 0;
//    }
//
//    // Currently selected range
//    DWORD dwCurSel = GetEditSel();
//    WORD dStart = LOWORD(dwCurSel);
//    WORD dEnd = HIWORD(dwCurSel);
//
//    // Search for, and select in, and string in the combo box that is prefixed
//    // by the text in the edit box
//    if (SelectString(-1, _bstr_t(str)) == CB_ERR)
//    {
//        SetWindowText(str);		// No text selected, so restore what was there before
//        if (dwCurSel != CB_ERR)
//            SetEditSel(dStart, dEnd);	// restore cursor postion
//    }
//
//    // Set the text selection as the additional text that we have added
//    if (dEnd < nLength && dwCurSel != CB_ERR)
//        SetEditSel(dStart, dEnd);
//    else
//        SetEditSel(nLength, -1);
//
//    return 0;
//}
//
//LRESULT CBandComboBoxCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//    return 0;
//}
//
//void CBandComboBoxCtrl::SetDropDownSize(UINT LinesToDisplay)
//{
//    CRect cbSize;			// current size of combo box
//    int Height;            // new height for drop-down portion of combo box
//
//    GetClientRect(cbSize);
//    Height = GetItemHeight(-1);      // start with size of the edit-box portion
//    Height += GetItemHeight(0) * LinesToDisplay;	// add height of lines of text
//
//    // Note: The use of SM_CYEDGE assumes that we're using Windows '95
//    // Now add on the height of the border of the edit box
//    Height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges
//
//    // The height of the border of the drop-down box
//    Height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges
//
//    // now set the size of the window
//    SetWindowPos(NULL,            // not relative to any other windows
//        0, 0,                         // TopLeft corner doesn't change
//        cbSize.right, Height,         // existing width, new height
//        SWP_NOMOVE | SWP_NOZORDER     // don't move box or change z-ordering.
//        );
//}

