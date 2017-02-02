#if !defined(AFX_TABCTRLSSL_H__75BE48A7_864C_11D5_9F04_000102FB9990__INCLUDED_)
#define AFX_TABCTRLSSL_H__75BE48A7_864C_11D5_9F04_000102FB9990__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabCtrlEx.h : header file
//

#include "TabPageSSL.h"
#include <atlframe.h>
#include <vector>

#ifdef _DEBUG
#pragma pack (push, 1)

typedef struct {
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
} DLGTEMPLATEEX;

#pragma pack (pop)
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlSSL window
template <class T, class TBase = CTabCtrl, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CTabCtrlWithDisableImpl : public CWindowImpl< T, TBase, TWinTraits >,
	public COwnerDraw< CTabCtrlWithDisableImpl >
{
public:
	CTabCtrlWithDisableImpl()
	{
	}

	virtual ~CTabCtrlWithDisableImpl() 
	{
	}


	BEGIN_MSG_MAP(CTabCtrlWithDisableImpl)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		MESSAGE_HANDLER(WM_COMMAND, OnCmdMsg)
		REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGING, OnSelChanging)
		REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnSelChange)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CTabCtrlWithDisableImpl>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	// My thanks to Tomasz Sowinski for all his help coming up with a workable
	// solution to the stack versus heap object destruction
	LRESULT OnDestroy (UINT, WPARAM, LPARAM, BOOL& bHandled) {
		bHandled = FALSE;

		int nCount = m_tabs.size ();

		// Destroy dialogs and delete CTabCtrlSSL objects.
		if (nCount > 0) {
			for (int i=nCount - 1; i>=0; i--) {
				OnDestroyPage (i, m_nPageIDs[i]);
				TabDelete tabDelete = m_tabs[i];
				CTabPageSSL* pDialog = tabDelete.pTabPage;
				if (pDialog != NULL) {
					pDialog->DestroyWindow ();
					if (TRUE == tabDelete.bDelete) {
						delete pDialog;
					}
				}
			}
		}

		// Clean up the internal arrays.
		m_tabs.clear ();
		m_hFocusWnd.clear ();
		m_nPageIDs.clear ();
	}

	LRESULT OnSetFocus(UINT, WPARAM, LPARAM hOldWnd, BOOL& bHandled) {
		bHandled = FALSE;
		// Set the focus to a control on the current page.
		int nIndex = GetCurSel ();
		if (nIndex != -1)
			::SetFocus (m_hFocusWnd[nIndex]);	
	}

	LRESULT OnKillFocus (UINT, WPARAM, LPARAM, BOOL& bHandled) {
		bHandled = FALSE;

		// Save the HWND of the control that holds the input focus.
		int nIndex = GetCurSel ();
		if (nIndex != -1)
			m_hFocusWnd[nIndex] = ::GetFocus ();	
	}

	LRESULT OnSelChanging (int idCtrl, LPNMHDR pnmh, BOOL& bHandled) {
		// Notify derived classes that the selection is changing.

		// Figure out index of new tab we are about to go to, as opposed
		// to the current one we're at. Believe it or not, Windows doesn't
		// pass this info
		//
		//TCHITTESTINFO htinfo;
		//GetCursorPos(&htinfo.pt);
		//ScreenToClient(&htinfo.pt);
		//htinfo.flags = TCHT_ONITEM;
		//int nNewTab = HitTest(&htinfo);

		int nIndex = GetCurSel ();
		if (nIndex == -1)
			return 0;

		if (nIndex >= 0 && !IsTabEnabled(nIndex))
			return TRUE;// tab disabled: prevent selection

		OnDeactivatePage (nIndex, m_nPageIDs[nIndex]);

		// Save the input focus and hide the old page.
		TabDelete tabDelete = m_tabs[nIndex];
		CTabPageSSL* pDialog = tabDelete.pTabPage;

		if (pDialog != NULL) {
			m_hFocusWnd[nIndex] = ::GetFocus ();
			pDialog->ShowWindow (SW_HIDE);
		}
		return 0;
	};

	LRESULT OnSelChange (int idCtrl, LPNMHDR pnmh, BOOL& bHandled) {
		int nIndex = GetCurSel ();
		if (nIndex == -1)
			return;

		// Show the new page.
		TabDelete tabDelete = m_tabs[nIndex];
		CTabPageSSL* pDialog = tabDelete.pTabPage;

		if (pDialog != NULL) {
			::SetFocus (m_hFocusWnd[nIndex]);
			CRect rect;
			GetClientRect (&rect);
			ResizeDialog (nIndex, rect.Width (), rect.Height ());
			pDialog->ShowWindow (SW_SHOW);
		}

		// Notify derived classes that the selection has changed.
		OnActivatePage (nIndex, m_nPageIDs[nIndex]);
	}

	virtual	BOOL IsTabEnabled(int nTab) = 0;  // you must override

	void Attach(HWND hWndNew)
	{
		ATLASSERT(::IsWindow(hWndNew));
		CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWndNew);
		_Init();
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		//@@ needs subclassing here, I suspect - Not tested when creating control dynamically
		_Init();
		return lRes;
	}

	void _Init()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
		InitTabStatus();
	}

	BOOL SubclassDlgItem(UINT nID, HWND pParent)
	{
		if (!CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(::GetDlgItem(pParent, nID)))
			return FALSE;
		_Init();
		return TRUE;                
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		short nShift = GetKeyState(VK_SHIFT);
		short nControl = GetKeyState(VK_CONTROL);

		bHandled = FALSE;
		switch (LOWORD(wParam))
		{                
		case VK_TAB :
		case VK_PRIOR :
		case VK_NEXT :
			{
				if (nControl < 0)
				{
					int nNewTab = (LOWORD(wParam) == VK_PRIOR || nShift < 0) ?
						PrevEnabledTab(GetCurSel(), TRUE) : NextEnabledTab(GetCurSel(), TRUE);
					if (nNewTab >= 0)
						SetActiveTab(nNewTab);
					bHandled = TRUE;
				}
				else
				{
					// Normal Tab Mode
					::SetFocus(::GetNextDlgTabItem(GetParent(), m_hWnd, (nShift < 0)));

				}
			}
			break;
		case VK_LEFT :
		case VK_RIGHT :
			{
				int nNewTab = (LOWORD(wParam) == VK_LEFT) ?
					PrevEnabledTab(GetCurSel(), TRUE) : NextEnabledTab(GetCurSel(), TRUE);
				if (nNewTab >= 0)
					SetActiveTab(nNewTab);
				// No where to go lets set the focus back here
				if (nNewTab == GetItemCount()-1)
					::SetFocus(m_hWnd);
				bHandled = TRUE;
			}
			break;
		default :
			lRes = DefWindowProc(uMsg, wParam, lParam); 
			break;
		};

		return lRes;
	}

	int	NextEnabledTab(int nCurrentTab, BOOL bWrap)
	{
		int nTabs = GetItemCount();
		if(nTabs)
		{
			for (int nTab = nCurrentTab+1; nTab != nCurrentTab; nTab++) 
			{
				if (nTab >= nTabs) 
				{
					if (!bWrap)
						return -1;
					nTab = 0;
				}
				if (IsTabEnabled(nTab)) 
				{
					return nTab;
				}
			}
		}
		return -1;    
	}

	int PrevEnabledTab(int nCurrentTab, BOOL bWrap)
	{
		for (int nTab = nCurrentTab-1; nTab != nCurrentTab; nTab--) 
		{
			if (nTab < 0) 
			{
				if (!bWrap)
					return -1;
				nTab = GetItemCount() - 1;
			}
			if (IsTabEnabled(nTab)) 
			{
				return nTab;
			}
			else
			{
				if (nTab == 0)
					return GetItemCount() - 1;
			}
		}
		return -1;
	}

	BOOL SetActiveTab(UINT nNewTab)
	{
		// send the parent TCN_SELCHANGING
		NMHDR nmh;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = GetDlgCtrlID();
		nmh.code = TCN_SELCHANGING;

		if (::SendMessage(GetParent(), WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh) >=0) {
			// OK to change: set the new tab
			SetCurSel(nNewTab);

			// send parent TCN_SELCHANGE
			nmh.code = TCN_SELCHANGE;
			::SendMessage(GetParent(), WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
			return TRUE;
		}
		return FALSE;
	}

	void OnDrawText(CDCHandle& dc, CRect rc, CString sText, BOOL bDisabled)
	{
		dc.SetTextColor(GetSysColor(bDisabled ? COLOR_3DHILIGHT : COLOR_BTNTEXT));
		dc.DrawText(sText, -1, &rc, DT_CENTER|DT_VCENTER);

		if (bDisabled) {
			// disabled: draw again shifted northwest for shadow effect
			rc += CPoint(-1,-1);
			dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			dc.DrawText(sText, -1, &rc, DT_CENTER|DT_VCENTER);
		}
	}
	// Owner draw methods

	void DeleteItem(LPDELETEITEMSTRUCT lpdis)
	{
	}
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
	}

	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
		if (lpdis->itemID==-1)
			return; // If there are no list box items, skip this message. 

		// use draw item DC
		CDCHandle dc(lpdis->hDC);

		DRAWITEMSTRUCT& ds = *lpdis;

		int iItem = ds.itemID;

		// Get tab item info
		TCHAR text[512];
		TCITEM tci;
		tci.mask = TCIF_TEXT;
		tci.pszText = text;
		tci.cchTextMax = sizeof(text);
		GetItem(iItem, &tci);

		// calculate text rectangle and color
		CRect rc = ds.rcItem;
		rc += CPoint(1,4);						 // ?? by trial and error

		// draw the text
		OnDrawText(dc, rc, text, !IsTabEnabled(iItem));
	}	

	LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return DefWindowProc(uMsg, wParam, lParam) | DLGC_WANTALLKEYS;
	}

	void InitTabStatus()
	{
		// If first tab is disabled, go to next enabled tab
		if (!IsTabEnabled(0)) 
		{
			int nTab = NextEnabledTab(0, TRUE);
			SetActiveTab(nTab);
		}        
	}

	// Page Functions
	int AddSSLPage (LPCTSTR pszTitle, int nPageID, CTabPageSSL* pTabPage) {
		// Add a page to the tab control.
		TabDelete tabDelete;
		tabDelete.pTabPage = pTabPage;
		tabDelete.bDelete = FALSE;

		return AddPage (pszTitle, nPageID, tabDelete);
	}

	int AddSSLPage (LPCTSTR pszTitle, int nPageID, LPCTSTR pszTemplateName) {
		// Verify that the dialog template is compatible with CTabCtrlSSL
		// (debug builds only). If your app asserts here, make sure the dialog
		// resource you're adding to the view is a borderless child window and
		// is not marked visible.
#ifdef _DEBUG
		if (pszTemplateName != NULL) {
			BOOL bResult = CheckDialogTemplate (pszTemplateName);
			ASSERT (bResult);
		}
#endif // _DEBUG

		// Add a page to the tab control.
		// Create a modeless dialog box.
		CTabPageSSL* pDialog = new CTabPageSSL;

		if (pDialog == NULL) {
			return -1;
		}

		if (!pDialog->Create (pszTemplateName, this)) {
			pDialog->DestroyWindow ();
			delete pDialog;
			return -1;
		}

		TabDelete tabDelete;
		tabDelete.pTabPage = pDialog;
		tabDelete.bDelete = TRUE;

		return AddPage (pszTitle, nPageID, tabDelete);
	}

	int AddSSLPage (LPCTSTR pszTitle, int nPageID, int nTemplateID) {
		return AddSSLPage (pszTitle, nPageID, MAKEINTRESOURCE (nTemplateID));
	}

	BOOL RemoveSSLPage (int nIndex) {
		if (nIndex >= GetItemCount ())
			return FALSE;

		// Notify derived classes that the page is being destroyed.
		OnDestroyPage (nIndex, m_nPageIDs[nIndex]);

		// Switch pages if the page being deleted is the current page and it's
		// not the only remaining page.
		int nCount = GetItemCount ();
		if (nCount > 1 && nIndex == GetCurSel ()) {
			int nPage = nIndex + 1;
			if (nPage >= nCount)
				nPage = nCount - 2;
			ActivateSSLPage (nPage);
		}

		// Remove the page from the tab control.
		DeleteItem (nIndex);

		// Destroy the dialog (if any) that represents the page.
		TabDelete tabDelete = m_tabs[nIndex];
		CTabPageSSL* pDialog = tabDelete.pTabPage;
		if (pDialog != NULL) {
			pDialog->DestroyWindow ();	
			delete pDialog;
		}

		// Clean up, repaint, and return.
		m_tabs.RemoveAt (nIndex);
		m_hFocusWnd.RemoveAt (nIndex);
		m_nPageIDs.RemoveAt (nIndex);
		Invalidate ();
		return TRUE;
	}

	int GetSSLPageCount (void) {
		return GetItemCount ();
	}

	BOOL GetSSLPageTitle (int nIndex, CString &strTitle) {
		if (nIndex >= GetItemCount ())
			return FALSE;

		TCHAR szTitle[1024];

		TC_ITEM item;
		item.mask = TCIF_TEXT;
		item.pszText = szTitle;
		item.cchTextMax = sizeof szTitle / sizeof (TCHAR);

		if (!GetItem (nIndex, &item))
			return FALSE;

		strTitle = item.pszText;
		return TRUE;
	}
	BOOL SetSSLPageTitle (int nIndex, LPCTSTR pszTitle) {
		if (nIndex >= GetItemCount ())
			return FALSE;

		TC_ITEM item;
		item.mask = TCIF_TEXT;
		item.pszText = (LPTSTR) pszTitle;

		BOOL bResult = SetItem (nIndex, &item);
		if (bResult)
			Invalidate ();
		return bResult;
	}
	int GetSSLPageID (int nIndex) {
		if (nIndex >= GetItemCount ())
			return -1;

		return m_nPageIDs[nIndex];
	}
	int SetSSLPageID (int nIndex, int nPageID) {
		if (nIndex >= GetItemCount ())
			return -1;

		int nOldPageID = m_nPageIDs[nIndex];
		m_nPageIDs[nIndex] = nPageID;
		return nOldPageID;
	}
	BOOL ActivateSSLPage (int nIndex) {
		if (nIndex >= GetItemCount ())
			return FALSE;

		// Do nothing if the specified page is already active.
		if (nIndex == GetCurSel ())
			return TRUE;

		// Deactivate the current page.
		int nOldIndex = GetCurSel ();

		if (nIndex != -1) {
			TabDelete tabDelete = m_tabs[nOldIndex];
			CTabPageSSL* pDialog = tabDelete.pTabPage;
			if (pDialog != NULL) {
				m_hFocusWnd[nOldIndex] = ::GetFocus ();
				pDialog->ShowWindow (SW_HIDE);
			}
		}

		// Activate the new one.
		SetCurSel (nIndex);
		TabDelete tabDelete = m_tabs[nIndex];
		CTabPageSSL* pDialog = tabDelete.pTabPage;

		if (pDialog != NULL) {
			::SetFocus (m_hFocusWnd[nIndex]);
			CRect rect;
			GetClientRect (&rect);
			ResizeDialog (nIndex, rect.Width (), rect.Height ());
			pDialog->ShowWindow (SW_SHOW);
		}
		return TRUE;
	}

	int GetSSLActivePage (void) {
		return GetCurSel ();
	}

	CWindow* GetSSLPage (int nIndex) {
		if (nIndex >= GetItemCount ())
			return NULL;

		TabDelete tabDelete = m_tabs[nIndex];
		return (CWindow*) tabDelete.pTabPage;
	}

	int GetSSLPageIndex (int nPageID) {
		int nCount = GetItemCount ();
		if (nCount == 0)
			return -1;

		for (int i=0; i<nCount; i++) {
			if (m_nPageIDs[i] == nPageID)
				return i;
		}

		return -1;
	}
#ifdef _DEBUG
	BOOL CheckDialogTemplate (LPCTSTR pszTemplateName) {
		// Verify that the dialog resource exists.
		ASSERT (pszTemplateName != NULL);
		HINSTANCE hInstance = AfxFindResourceHandle (pszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource (hInstance, pszTemplateName, RT_DIALOG);

		if (hResource == NULL)
			return FALSE; // Resource doesn't exist

		HGLOBAL hTemplate = LoadResource (hInstance, hResource);
		ASSERT (hTemplate != NULL);

		// Get the dialog's style bits.
		DLGTEMPLATEEX* pTemplate = (DLGTEMPLATEEX*) LockResource (hTemplate);

		DWORD dwStyle;
		if (pTemplate->signature == 0xFFFF)
			dwStyle = pTemplate->style;
		else
			dwStyle = ((DLGTEMPLATE*) pTemplate)->style;

		UnlockResource (hTemplate);
		FreeResource (hTemplate);

		// Verify that the dialog is an invisible child window.
		if (dwStyle & WS_VISIBLE)
			return FALSE; // WS_VISIBLE flag is set

		if (!(dwStyle & WS_CHILD))
			return FALSE; // WS_CHILD flag isn't set

		// Verify that the dialog has no border and no title bar.
		if (dwStyle & (WS_BORDER | WS_THICKFRAME | DS_MODALFRAME))
			return FALSE; // One or more border flags are set

		if (dwStyle & WS_CAPTION)
			return FALSE; // WS_CAPTION flag is set

		return TRUE;
	}
#endif // _DEBUG

	void ResizeDialog (int nIndex, int cx, int cy) {
		if (nIndex != -1) {
			TabDelete tabDelete = m_tabs[nIndex];
			CTabPageSSL* pDialog = tabDelete.pTabPage;

			if (pDialog != NULL) {
				CRect rect;
				GetItemRect (nIndex, &rect);

				int x, y, nWidth, nHeight;
				DWORD dwStyle = GetStyle ();

				if (dwStyle & TCS_VERTICAL) { // Vertical tabs
					int nTabWidth =
						rect.Width () * GetRowCount ();
					x = (dwStyle & TCS_RIGHT) ? 4 : nTabWidth + 4;
					y = 4;
					nWidth = cx - nTabWidth - 8;
					nHeight = cy - 8;
				}
				else { // Horizontal tabs
					int nTabHeight =
						rect.Height () * GetRowCount ();
					x = 4;
					y = (dwStyle & TCS_BOTTOM) ? 4 : nTabHeight + 4;
					nWidth = cx - 8;
					nHeight = cy - nTabHeight - 8;
				}
				pDialog->SetWindowPos (NULL, x, y, nWidth, nHeight, SWP_NOZORDER);
			}
		}
	}

protected:
	struct TabDelete {
		CTabPageSSL*   pTabPage;
		BOOL        bDelete;
	};
	std::vector<TabDelete> m_tabs;
	std::vector<HWND> m_hFocusWnd;
	std::vector<int> m_nPageIDs;

	int AddPage (LPCTSTR pszTitle, int nPageID, TabDelete tabDelete) {
		// Add a page to the tab control.
		TC_ITEM item;
		item.mask = TCIF_TEXT;
		item.pszText = (LPTSTR) pszTitle;
		int nIndex = GetItemCount ();

		if (InsertItem (nIndex, &item) == -1)
			return -1;

		if (NULL == tabDelete.pTabPage) {
			// Fail - no point calling the function with a NULL pointer!
			DeleteItem (nIndex);
			return -1;
		}
		else {
			// Record the address of the dialog object and the page ID.
			int nArrayIndex = m_tabs.push_back (tabDelete);
			ASSERT (nIndex == nArrayIndex);

			nArrayIndex = m_nPageIDs.push_back (nPageID);
			ASSERT (nIndex == nArrayIndex);

			// Size and position the dialog box within the view.
			tabDelete.pTabPage->SetParent (this); // Just to be sure

			CRect rect;
			GetClientRect (&rect);

			if (rect.Width () > 0 && rect.Height () > 0)
				ResizeDialog (nIndex, rect.Width (), rect.Height ());

			// Initialize the page.
			if (OnInitPage (nIndex, nPageID)) {
				// Make sure the first control in the dialog is the one that
				// receives the input focus when the page is displayed.
				HWND hwndFocus = tabDelete.pTabPage->GetTopWindow ()->m_hWnd;
				nArrayIndex = m_hFocusWnd.push_back (hwndFocus);
				ASSERT (nIndex == nArrayIndex);
			}
			else {
				// Make the control that currently has the input focus is the one
				// that receives the input focus when the page is displayed.
				m_hFocusWnd.push_back (::GetFocus ());
			}

			// If this is the first page added to the view, make it visible.
			if (nIndex == 0)
				tabDelete.pTabPage->ShowWindow (SW_SHOW);
		}
		return nIndex;
	}

	BOOL OnInitPage (int nIndex, int nPageID) {
		// TODO: Override in derived class to initialise pages.
		return TRUE;
	}

	void OnActivatePage (int nIndex, int nPageID) {
		// TODO: Override in derived class to respond to page activations.
	}

	void OnDeactivatePage (int nIndex, int nPageID) {
		// TODO: Override in derived class to respond to page deactivations.
	}

	void OnDestroyPage (int nIndex, int nPageID) {
		// TODO: Override in derived class to free resources.
	}
	BOOL OnCommand (WPARAM wParam, LPARAM lParam) {
		// Forward WM_COMMAND messages to the dialog's parent.
		return GetParent ()->SendMessage (WM_COMMAND, wParam, lParam);
	}
	BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
		// Forward WM_NOTIFY messages to the dialog's parent.
		return GetParent ()->SendMessage (WM_NOTIFY, wParam, lParam);
	}


	BOOL OnCmdMsg (UINT, WPARAM nCode, LPARAM, BOOL& bHandled) {
			// Forward ActiveX control events to the dialog's parent.
#ifndef _AFX_NO_OCC_SUPPORT
			if (nCode == CN_EVENT)
				return GetParent ()->OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
#endif // !_AFX_NO_OCC_SUPPORT

			return CTabCtrl::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
		}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCTRLSSL_H__75BE48A7_864C_11D5_9F04_000102FB9990__INCLUDED_)
