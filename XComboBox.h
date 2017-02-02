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

#define COMBO_LEFT_ICON_WIDTH	16
#define COMBO_LEFT_ICON_HEIGHT	18
#define EDIT_XOFFSET ( INT32 )( COMBO_LEFT_ICON_WIDTH + 1 )

#define LISTBOX_HIGHLIGHT_CLR	RGB(100,100,220)
#define LISTBOX_NORMAL_CLR		RGB(255,255,255)
#define LISTBOX_TEXT_HIGHLIGHT_CLR RGB(80,80,80)
#define DEFAULT_TIP_COLOR			RGB(130,130,130)
#define LISTBOX_TEXT_NORMAL_CLR		RGB(255,255,255)
#define LISTBOX_LEFT		2
#define EDIT_TIP_STRING				_T("Input Key Word")
#define DEFAULT_COMBOBOX_WIDTH 300
#define MAX_COMBOBOX_WIDTH 600

#define IDB_XICON 206
class CODEditImpl : public CWindowImpl<CODEditImpl, CEdit>
{
public:
	CODEditImpl()
	{
		isFocused = false;
		m_TipFont.CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,_T("ms sans serif"));
		CDCHandle dc = ::GetDC(NULL);
		CFontHandle OldFont = dc.SelectFont(m_TipFont);
		SIZE TipSize;
		m_strTip = EDIT_TIP_STRING;
		GetTextExtentPoint( dc, m_strTip.c_str(), ( INT32 )m_strTip.size(), &TipSize);
		m_uTipWidth = TipSize.cx;
		dc.SelectFont(OldFont);
		::ReleaseDC(NULL, dc);
	};

	BEGIN_MSG_MAP(CODEditImpl)
		//CHAIN_MSG_MAP(CWindowImpl<CODEditImpl, CEdit>)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CHAR, OnChar);
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
			MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)	
			DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			if(wParam != VK_RETURN)
				bHandled = FALSE;
			return 0;
		}
#define RECT_MOVE_0(rect) rect.right = rect.right - rect.left; rect.bottom = rect.bottom - rect.top; rect.left = rect.top = 0; 

		LRESULT OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			//CPaintDC hDC(m_hWnd);
			bHandled = FALSE;
			DefWindowProc( uMsg, wParam, lParam );
			CDCHandle hDC = GetDC();

			RECT rcEdit;
			//RECT rcCombobox;
			GetClientRect( &rcEdit );
			//ScreenToClient(&rcEdit);
			//GetParent().GetClientRect(&rcCombobox);
			//GetParent().ScreenToClient(&rcCombobox);
			RECT_MOVE_0(rcEdit);
			//CBrush brWhite;
			////CBrush OldBr;
			//brWhite.CreateSolidBrush(RGB(255,255,255));
			//hDC.FillRect(&rcEdit, brWhite);

			//ATLTRACE("Edit left %d top %d right %d bottom %d Combobox extend %d\n", rcEdit.left, rcEdit.top, rcEdit.right, rcEdit.bottom, rcCombobox.left - rcEdit.left);

			//rcEdit.bottom -= 3;
			CString sz;
			HFONT hOldFont;
			hDC.SetBkMode(TRANSPARENT);
			GetWindowText(sz.GetBufferSetLength(500), 500);
			sz.ReleaseBuffer();
			//hOldFont = hDC.SelectFont(GetFont());
			//DrawText(hDC, sz.GetBuffer(0), sz.GetLength(), &rcEdit,DT_LEFT | DT_VCENTER);
			//hDC.SelectFont(hOldFont);

			if(sz.IsEmpty() && !isFocused && (rcEdit.right - rcEdit.left) > ( LONG )m_uTipWidth)
			{
				DWORD dwOld = hDC.SetTextColor(DEFAULT_TIP_COLOR);
				hOldFont = hDC.SelectFont(m_TipFont);
				//ATLTRACE( "Draw the edit text %s left %d, right %d, top %d, bottom %d \n", m_strTip.c_str(), rcEdit.left, rcEdit.right, rcEdit.top, rcEdit.bottom);
				//SIZE TipSize;
				//m_strTip = EDIT_TIP_STRING;
				//GetTextExtentPoint(hDC,m_strTip.c_str(),m_strTip.size(),&TipSize);
				//m_uTipWidth = TipSize.cx;
				DrawText(hDC, m_strTip.c_str(), ( INT32 )m_strTip.size(),&rcEdit,DT_RIGHT | DT_VCENTER);
				//::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcEdit,  m_strTip.c_str(),m_strTip.size(), NULL);
				//TextOut(hDC, rcEdit.right - 200, 0,  m_strTip.c_str(),m_strTip.size());
				hDC.SelectFont(hOldFont);
				hDC.SetTextColor(dwOld);
			}
			::ReleaseDC(m_hWnd, hDC);
			bHandled = TRUE;
			return 0;
		}

		LRESULT OnKillFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			isFocused = false;
			//UpdateWindow();
			Invalidate(0);
			return 0;
		}

		LRESULT OnSetFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			isFocused = true;
			//UpdateWindow();
			Invalidate(0);
			return 0;
		}

protected:
	bool isFocused;
	CFont m_TipFont;
	UINT m_uTipWidth;
	XString m_strTip;
};

class CODListBoxImpl : public CWindowImpl< CODListBoxImpl, CListBox >,
	public COwnerDraw< CODListBoxImpl >
{
public:
	CODListBoxImpl()
	{
	};

	~CODListBoxImpl()
	{
	};

	BEGIN_MSG_MAP(CODListBoxImpl)
		//MSG_OCM_DRAWITEM(OnDrawItem)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		//CHAIN_MSG_MAP_ALT(COwnerDraw<CODListBoxImpl>, 1)
		//DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	//LRESULT OnPaint (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	//{
	//	RECT rc;
	//	//CDCHandle pDC = GetDC();
	//	CPaintDC pDC(m_hWnd);
	//	GetClientRect(&rc);

	//	CBrush pBrush;
	//	pBrush.CreateSolidBrush( LISTBOX_NORMAL_CLR);

	//	pDC.FillRect(&rc,pBrush);
	//	pBrush.DeleteObject();

	//	pDC.SetTextColor(LISTBOX_TEXT_HIGHLIGHT_CLR);
	//	//CBrush brItemData;
	//	RECT rcItemData;
	//	CString szItemString;
	//	//brItemData.CreateSolidBrush(LISTBOX_HIGHLIGHT_CLR);
	//	pDC.SelectFont(GetFont());
	//	pDC.SetBkMode(TRANSPARENT);

	//	for(INT32 i =0;i<GetCount();i++)
	//	{
	//		GetText(i,szItemString.GetBufferSetLength(500));
	//		szItemString.ReleaseBuffer();
	//		GetItemRect(i,&rcItemData);
	//		rcItemData.left += LISTBOX_LEFT;

	//		pDC.DrawIconEx( rcItemData.left, rcItemData.top, m_MainIcon,16,16);
	//		rcItemData.left += EDIT_XOFFSET;
	//		pDC.DrawText(szItemString.GetBuffer(0),szItemString.GetLength(), &rcItemData, DT_VCENTER | DT_SINGLELINE);

	//	}
	//	//brItemData.DeleteObject();
	//	//::ReleaseDC(m_hWnd, pDC);
	//	return 0;
	//}


	//void OnDrawItem(UINT wParam, LPDRAWITEMSTRUCT lpDrawItemStruct)
	//{
	//	if( lpDrawItemStruct->itemID == -1 )
	//		return;

	//	CDCHandle			pDC;	
	//	pDC.Attach(lpDrawItemStruct->hDC);
	//	CBrush pBrush;
	//	CString		sItem;

	//	//ATLTRACE("%d\n",lpDrawItemStruct->itemState);
	//	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	//	{
	//		pBrush.CreateSolidBrush(LISTBOX_HIGHLIGHT_CLR);
	//		pDC.FillRect(&lpDrawItemStruct->rcItem, pBrush);
	//		pDC.SetTextColor(LISTBOX_TEXT_NORMAL_CLR);
	//	}
	//	else
	//	{
	//		pBrush.CreateSolidBrush(LISTBOX_NORMAL_CLR);
	//		pDC.FillRect(&lpDrawItemStruct->rcItem, pBrush);
	//		pDC.SetTextColor(LISTBOX_TEXT_HIGHLIGHT_CLR);
	//	}

	//	// Copy the text of the item to a string
	//	this->GetText(lpDrawItemStruct->itemID, sItem.GetBufferSetLength(500));
	//	sItem.ReleaseBuffer();

	//	pDC.SetBkMode(TRANSPARENT);

	//	// Draw the text after the images left postion
	//	RECT rcItem = lpDrawItemStruct->rcItem;
	//	rcItem.left += LISTBOX_LEFT;
	//	//pDC.DrawIconEx( rcItem.left, rcItem.top, m_MainIcon,16,16);
	//	rcItem.left += EDIT_XOFFSET;

	//	pDC.DrawText(sItem, sItem.GetLength(),  &rcItem, DT_VCENTER | DT_SINGLELINE);
	//}
	
	public:
		//void SetMainIcon(HICON hIcon)
		//{
		//	m_MainIcon = hIcon;
		//}
	
	protected:
		//HICON m_MainIcon;
		//HBITMAP m_MainIcon;
};

struct IXEditCtrl
{
public:
	virtual ~IXEditCtrl() {}
	virtual void Create( HWND hParentWnd, CToolTipCtrl ToolTip ) = 0;
	virtual bool IsFocus( HWND hwnd ) = 0;
	virtual void SetCurSel(INT32 idx) {}
	virtual void InsertString( INT32 idx, const TCHAR* str ) {}
	virtual void SetItemDataPtr( INT32 idx, void * ) {}
	virtual void ResetContent() {}
	virtual void ClearHistory() {}
	virtual void DestroyWindow() = 0;
	virtual void ShowWindow( bool ) = 0;
	virtual void SetFont( HFONT ) = 0;
	virtual _bstr_t GetText() = 0;
	virtual void SetFocus2() = 0;
	virtual void SetFocus() = 0;
	virtual INT32 FindStringExact( const TCHAR* str ) { return 0; }
	virtual void SetWindowText( const TCHAR* str ) = 0;
	virtual void SetEditSel( INT32 s, INT32 e ) = 0;
	virtual void AddinHistoryAndUpdateWords( _bstr_t URL ) {}
	virtual void MoveWindow( RECT* rc ) = 0;
	STDMETHOD(TranslateAcceleratorIO)( LPMSG pMsg ) = 0;
	XString GetBtnConfCaption() { return m_pComboHost->m_strName; }
	IComboHost* m_pConfigHost;
	Combo* m_pComboHost;
};

//class XEditCtrl : public CWindowImpl<XEditCtrl, CComboBox>,
//class XEditCtrl : public CWindowImpl<XEditCtrl, CEdit>,

//template <class T>
class XEditCtrl : public CWindowImpl<XEditCtrl, WTL::CComboBox>,
	public IDropTarget ,
	public IXEditCtrl
{
public:	
	XEditCtrl() : m_cRef(0), /*m_Edit(NULL, this, 1),*/m_auto(true){m_pConfigHost = NULL;}
	virtual ~XEditCtrl() 
	{
		if(m_Edit.m_hWnd)
		{
			m_Edit.UnsubclassWindow();
			//m_Edit.DestroyWindow();
		}

		if(m_ListBox.m_hWnd)
		{
			m_ListBox.UnsubclassWindow();
			//m_ListBox.DestroyWindow();
		}
	}
	typedef CWindowImpl<XEditCtrl, WTL::CComboBox> baseClass;
	DECLARE_WND_SUPERCLASS(_T("XFree_ComboBox"), TEXT("COMBOBOX"))
protected:
	CODEditImpl m_Edit;
	CODListBoxImpl m_ListBox;

	//private:
	/*The hwnd get by GetFocus()*/
	bool IsFocus(HWND hwnd)
	{
		return hwnd==m_Edit || hwnd==m_hWnd;
	}

	void MoveWindow( RECT* rcComboEdit )
	{
		//CRect rcComboBox;
		//ATLTRACE("Enter combobox move window left %d top %d right %d bottom %d\n",
		//	rc->left,
		//	rc->top,
		//	rc->right,
		//	rc->bottom
		//	);
		RECT rcCombo;
		rcCombo = *rcComboEdit;

		if( rcCombo.bottom - rcCombo.top < COMBOBOX_DROPLINT_HEIGHT )
		{
			rcCombo.top = ( ( rcCombo.bottom - rcCombo.top ) - COMBOBOX_HEIGHT ) / 2;  /*like get system metrix combo height*/
			rcCombo.bottom += COMBOBOX_DROPLINT_HEIGHT;
		}
		baseClass::MoveWindow(&rcCombo);
		//GetClientRect(&rcComboBox);
		XLOG( ( 0, "Combobox moved window left %d top %d right %d bottom %d\n",
			rcCombo.left,
			rcCombo.top,
			rcCombo.right,
			rcCombo.bottom
			) );
		
		INT32 nWidth;
		INT32 nHeight;
		nWidth = rcComboEdit->right - rcComboEdit->left - ( EDIT_XOFFSET + SM_CYVTHUMB + 10);
		nHeight = rcComboEdit->bottom - rcComboEdit->top - 5;
		if(nWidth < 0)
		{
			nWidth = 0;
		}

		if(nHeight < 0)
		{
			nHeight = 0;
		}

		if(m_Edit.m_hWnd)
			if( FALSE == ::MoveWindow(m_Edit.m_hWnd,EDIT_XOFFSET, rcComboEdit->top + 5, nWidth, nHeight, TRUE))
				ATLASSERT(FALSE);

		//baseClass::MoveWindow( rcComboEdit );
		XLOG( ( 0, "The combobox edit move window by the rect(left %d width %d top %d height %d\n", 
			EDIT_XOFFSET, 
			nWidth, 
			rcComboEdit->top + 3, 
			nHeight ) );
		//ATLTRACE("Leave combobox move window left %d top %d right %d bottom %d\n",
		//	rcComboEdit->left,
		//	rcComboEdit->top,
		//	rcComboEdit->right,
		//	rcComboEdit->bottom
		//	);

	}

	void SetEditSel(INT32 s, INT32 e)
	{
		baseClass::SetEditSel(s,e);
	}

	void SetWindowText(const TCHAR* str)
	{
		baseClass::SetWindowText(str);
	}

	void SetFocus()
	{
		baseClass::SetFocus();
	}

	void SetFocus2()
	{
		baseClass::SetFocus();
		baseClass::SetActiveWindow();
		baseClass::SetFocus();
	}

	void Create(/* IComboHost* ctrl */HWND hWnd, CToolTipCtrl ToolTip )
	{
		m_pComboHost->m_style = WS_VSCROLL | WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS |
			CBS_AUTOHSCROLL | WS_TABSTOP | CBS_OWNERDRAWVARIABLE;

		//m_pComboHost->m_rc.bottom = m_pComboHost->m_rc.top + 10;
#ifdef _BIBLE
		if(ctrl->m_pConfigHost->m_vecCombos.size()<2)
#endif
			m_pComboHost->m_style |= (m_pComboHost->m_values.empty() ? CBS_DROPDOWN : CBS_DROPDOWNLIST);

		//ATLTRACE("The combo box creating left %d top %d right %d bottom %d\n",
		//	m_pComboHost->m_rc.left,
		//	m_pComboHost->m_rc.top,
		//	m_pComboHost->m_rc.right,
		//	m_pComboHost->m_rc.bottom
		//	);
		if ( !baseClass::Create( hWnd, CRect(0, 0, 0, 0), NULL, m_pComboHost->m_style ) )//, WS_EX_CLIENTEDGE))
			return;

		HWND wnd;
		COMBOBOXINFO pcbi;
		pcbi.cbSize = sizeof COMBOBOXINFO;
		::GetComboBoxInfo(*this,&pcbi);
		wnd = pcbi.hwndItem;

		//wnd = GetEditCtrl();
		//HWND hEdit = m_wndCombo.GetEditCtrl();
		//wnd = ::GetTopWindow(m_pEdit);

		//wnd = m_pEdit;

		if(wnd)
		{
			m_Edit.SubclassWindow( wnd );
			//::MoveWindow(m_Edit.m_hWnd, m_pComboHost->m_rc.left + EDIT_XOFFSET, m_pComboHost->m_rc.top + 3,  m_pComboHost->m_rc.left + m_pComboHost->m_rc.right - m_pComboHost->m_rc.left - EDIT_XOFFSET - 18, m_pComboHost->m_rc.bottom - m_pComboHost->m_rc.top - 6, TRUE);
		}
		wnd = pcbi.hwndList;

		if( wnd )
		{
			m_ListBox.SubclassWindow(wnd);
		}

		LimitText(m_pComboHost->m_nLimit);

		CToolTipCtrl toolTip = ToolTip; //ctrl->GetToolTips(); //Note: if use chinese or other two byte encode characters then the tooptip function would conflict with it.

		//if(m_Edit.m_hWnd)
		//{
		CToolInfo ti0(TTF_SUBCLASS, m_hWnd, 0, 0, const_cast<TCHAR*>(m_pComboHost->GetHintMsg().c_str()));
		toolTip.AddTool((LPTOOLINFO) &ti0);
		//}

		RegisterDragDrop(m_hWnd, (LPDROPTARGET)(this));

	}

	void ShowWindow(bool s)
	{
		baseClass::ShowWindow(s);
	}

	void SetFont(HFONT hFont)
	{
		baseClass::SetFont( hFont );
		if(m_Edit.m_hWnd)
			m_Edit.SetFont( hFont );
		if(m_ListBox.m_hWnd)
			m_ListBox.SetFont( hFont );
	}

	void DestroyWindow()
	{
		if(m_hWnd) baseClass::DestroyWindow();
	}

	// Operations
	STDMETHOD(TranslateAcceleratorIO)(LPMSG pMsg)
	{
		INT32 nVirtKey = (INT32)(pMsg->wParam);
		if(nVirtKey==VK_TAB)
			return S_OK;
		if (WM_KEYUP == pMsg->message && VK_RETURN == nVirtKey && pMsg->hwnd==m_Edit.m_hWnd)
		{
			Process();
			return S_OK; 
		}
		else if (
			/*(WM_KEYUP == pMsg->message || WM_KEYDOWN == pMsg->message) && */	
			VK_BACK == nVirtKey ||
			(VK_END <= nVirtKey && VK_LEFT >= nVirtKey) || VK_RIGHT == nVirtKey ||
			VK_DELETE == nVirtKey || ( (GetKeyState(VK_CONTROL)&0x80) && (nVirtKey=='C' || nVirtKey=='V' || nVirtKey=='X')))
		{
			m_auto = false;
			TranslateMessage(pMsg);
			DispatchMessage(pMsg);
			m_auto = true;
			if(WM_KEYDOWN == pMsg->message && VK_BACK == nVirtKey) 
			{
				m_auto = false;
			}
			return S_OK;
		}
		else
			return S_FALSE;
	}

	// Support Drag and Drop
	STDMETHODIMP QueryInterface(REFIID riid, VOID** ppv)
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

	STDMETHODIMP_(ULONG) AddRef(void) { return ++m_cRef;}  
	STDMETHODIMP_(ULONG) Release(void) {if (0!=--m_cRef)return m_cRef;delete this;return 0;}      

	STDMETHODIMP DragEnter(LPDATAOBJECT pDataObj , DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
	{     
		m_pIDataObject=NULL;
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
			*pdwEffect=DROPEFFECT_COPY;
			m_pIDataObject=pDataObj;     
			m_pIDataObject->AddRef();      
		}
		return NOERROR;     
	}

	STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD pdwEffect)
	{     
		if (m_pIDataObject == NULL) 
			*pdwEffect=DROPEFFECT_NONE;         
		else 
			*pdwEffect=DROPEFFECT_COPY;

		return NOERROR;     
	}       

	STDMETHODIMP DragLeave()
	{ 
		if(m_pIDataObject)
			m_pIDataObject->Release();
		return NOERROR;     
	}      

	STDMETHODIMP Drop(LPDATAOBJECT pDataObj, DWORD, POINTL, LPDWORD pdwEffect)
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
			char* sClipboard = (char*)GlobalLock(stmg.hGlobal);
			::SetWindowTextA(m_hWnd,sClipboard);

			if(isGetValue(_T("RunSearchDragAutomatically")))
				Process();

			GlobalUnlock(stmg.hGlobal);

			if (stmg.hGlobal != NULL) 
				ReleaseStgMedium(&stmg);         
		}

		*pdwEffect=DROPEFFECT_COPY;      
		return NOERROR;     
	} 

	BEGIN_MSG_MAP_EX(XEditCtrl)
		REFLECTED_COMMAND_CODE_HANDLER(CBN_SETFOCUS, OnTVSetFocus)
		REFLECTED_COMMAND_CODE_HANDLER(CBN_KILLFOCUS, OnTVKillFocus)
		MESSAGE_HANDLER_EX(WM_MOUSEWHEEL, OnMouseWheel)
		//CHAIN_MSG_MAP(CWindowImpl<XEditCtrl, T>)
	END_MSG_MAP()

	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM){return 0;}


	LRESULT Process()
	{
#ifdef _BIBLE
		setValue(_T("SearchUsing"),m_pComboHost->m_strCmd=="Search" ? _T("2") : _T("0"));
#endif
		//Temp remove
		m_pConfigHost->OnComboProcess();
		AddinHistoryAndUpdateWords(GetText());
		return 0;
	}

	LPDATAOBJECT      m_pIDataObject;

	_bstr_t GetText()
	{
		vector<TCHAR> cvalue(1024);
		GetWindowText(&cvalue.front(), ( INT32 )cvalue.size() );
		return &cvalue.front();
	}

	_bstr_t GetLBText(INT32 idx)
	{
		vector<TCHAR> cvalue(1024);
		CComboBox::GetLBText(idx,&cvalue.front());
		return &cvalue.front();
	}

	LRESULT OnTVSetFocus(WORD, WORD pNmHdr, HWND,BOOL& )
	{
		//Temp remove
		if( m_pConfigHost )
			m_pConfigHost->OnComboSetFocus();
		
		isFocused = true;
		return 0;
	}

	LRESULT OnTVKillFocus(WORD, WORD pNmHdr, HWND,BOOL& hand)
	{
		if( m_pConfigHost )
			m_pConfigHost->OnComboKillFocus();
		
		hand = FALSE;
		isFocused = false;

		return 0;
	}

	LRESULT OnDropDown(WORD, WORD pNmHdr, HWND,BOOL& hand)
	{
		isDroped = true;
		Invalidate(FALSE);
		return 0;
	}

	LRESULT OnCloseUp(WORD, WORD pNmHdr, HWND,BOOL& hand)
	{
		isDroped = false;
		Invalidate(FALSE);
		return 0;
	}

protected:
	ULONG m_cRef;
	bool m_auto;
	bool isFocused;
	bool isDroped;
};

class XComboBoxCtrl : public XEditCtrl,
							public COwnerDraw<XComboBoxCtrl>
{
public:
	BEGIN_MSG_MAP_EX(XComboBoxCtrl)
		REFLECTED_COMMAND_CODE_HANDLER(CBN_EDITUPDATE, OnTVSelDate)
		REFLECTED_COMMAND_CODE_HANDLER(CBN_SELENDOK, OnTVSelChanged)
		//REFLECTED_COMMAND_CODE_HANDLER(CBN_SETFOCUS, OnTVSetFocus)
		//REFLECTED_COMMAND_CODE_HANDLER(CBN_KILLFOCUS, OnTVKillFocus)
		REFLECTED_COMMAND_CODE_HANDLER(CBN_DROPDOWN, OnDropDown)
		REFLECTED_COMMAND_CODE_HANDLER(CBN_CLOSEUP,OnCloseUp)

		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		//MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER_EX(WM_MOUSEWHEEL, OnMouseWheel)
		//MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		//MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		CHAIN_MSG_MAP(XEditCtrl)
		CHAIN_MSG_MAP_ALT(COwnerDraw<XComboBoxCtrl>, 1)

		//ALT_MSG_MAP(1)
		//MESSAGE_HANDLER(WM_CHAR, OnChar)
		END_MSG_MAP()
protected:
	HICON m_MainIcon;

public:
	void SetDroped(bool bDrop)
	{
		isDroped = bDrop;
	}

	LRESULT OnCtlColorEdit(HDC wParam, HWND lParam)
	{
		//static bool isFirst = true;
		//if(m_Edit.m_hWnd && isFirst)
		//{
		//	COMBOBOXINFO pcbi;
		//	pcbi.cbSize = sizeof COMBOBOXINFO;
		//	::GetComboBoxInfo(*this,&pcbi);
		//	RECT rcEdit = pcbi.rcItem;
		//	rcEdit.left += EDIT_XOFFSET;
		//	rcEdit.bottom -= 6;
		//	m_Edit.MoveWindow(&rcEdit);
		//	isFirst = false;
		//}
		return 0;
	}

	// 绘制渐变矩形
	VOID WINAPI GradientRectXP(HDC hDC, LPRECT pRect,COLORREF crColor[4])
	{
		INT32 i;
		TRIVERTEX Tve[4]; 
		GRADIENT_RECT GRect;
		GRADIENT_TRIANGLE GTrg;

		for (i = 0; i < 4; i++)
		{
			Tve[i].Red = ((COLOR16) GetRValue(crColor[i])) << 8;
			Tve[i].Green = ((COLOR16) GetGValue(crColor[i])) << 8;
			Tve[i].Blue = ((COLOR16) GetBValue(crColor[i])) << 8;
			Tve[i].Alpha = ((COLOR16) (crColor[i] >> 24)) << 8;
		}

		Tve[0].x = pRect->left;
		Tve[0].y = pRect->top;
		Tve[1].x = pRect->right;
		Tve[1].y = pRect->top;
		Tve[2].x = pRect->left;
		Tve[2].y = pRect->bottom;
		Tve[3].x = pRect->right;
		Tve[3].y = pRect->bottom;

		if ((crColor[0] == crColor[2]) &&
			(crColor[1] == crColor[3]))
			i = GRADIENT_FILL_RECT_H;
		if ((crColor[0] == crColor[1]) &&
			(crColor[2] == crColor[3]))
			i = GRADIENT_FILL_RECT_V;
		else
			i = GRADIENT_FILL_TRIANGLE;

		if (i == GRADIENT_FILL_TRIANGLE)
		{
			GTrg.Vertex1 = 0;
			GTrg.Vertex2 = 1;
			GTrg.Vertex3 = 2;
		}
		else
		{
			GRect.UpperLeft = 0;
			GRect.LowerRight = 3;
		}
		GradientFill(hDC, Tve, 4,
			((i == GRADIENT_FILL_TRIANGLE) ? ((PVOID) &GTrg) : ((PVOID) &GRect)), 1, i);

		if (i == GRADIENT_FILL_TRIANGLE)
		{
			GTrg.Vertex1 = 3;
			GradientFill(hDC,Tve, 4, &GTrg, 1, GRADIENT_FILL_TRIANGLE);
		}
	}

	XComboBoxCtrl()
	{
		isDroped = false;
		//m_MainIcon = ::LoadBitmap(_Module.GetResourceInstance(), MAKEINTRESOURCE( IDB_XICON ) );
		m_MainIcon = ::LoadIcon( _Module.GetResourceInstance(), MAKEINTRESOURCE( IDI_EDIT_ICON ) );
		//m_ListBox.SetMainIcon(m_MainIcon);
	}

	~XComboBoxCtrl()
	{
		if( m_MainIcon )
		{
			::DeleteObject( m_MainIcon );
		}
	}

	INT32 DrawBitmap( HDC hDC, LONG x, LONG y, ULONG lWidth, ULONG lHeight )
	{
		HBITMAP hBmp;
		HBITMAP hOldBmp;
		HDC hMemDC;
		//bitmap = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,with,height,LR_LOADFROMFILE);
		hBmp = LoadBitmap( _Module.get_m_hInstResource(), MAKEINTRESOURCE( IDB_XICON ) ); 
		hMemDC = CreateCompatibleDC( hDC );
		hOldBmp = ( HBITMAP )SelectObject( hMemDC, hBmp );
		BitBlt( hDC, 0, 0, lWidth, lHeight, hMemDC, x, y, SRCCOPY );
		SelectObject( hMemDC, hOldBmp );
		DeleteDC( hMemDC );
		DeleteObject( hBmp );
		return 0;
	}

	LRESULT DrawItem(LPDRAWITEMSTRUCT lParam)
	{
		LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;
		if( lpDrawItemStruct->itemID == -1 /*&& lpDrawItemStruct->itemID == 0*/)
			return 0;

		CDCHandle			pDC;	
		pDC.Attach(lpDrawItemStruct->hDC);
		CBrush pBrush;
		CString		sItem;
		CRect rcDraw = lpDrawItemStruct->rcItem;
		//ATLTRACE("The Combobox draw item left %d top %d right %d bottom %d\n", rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom);
		//rcDraw.bottom -= 10;

		//ATLTRACE("%d\n",lpDrawItemStruct->itemState);
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			pBrush.CreateSolidBrush(LISTBOX_HIGHLIGHT_CLR);
			pDC.FillRect(&rcDraw, pBrush);
			pDC.SetTextColor(LISTBOX_TEXT_NORMAL_CLR);
		}
		else
		{
			pBrush.CreateSolidBrush(LISTBOX_NORMAL_CLR);
			pDC.FillRect(&rcDraw, pBrush);
			pDC.SetTextColor(LISTBOX_TEXT_HIGHLIGHT_CLR);
		}

		// Copy the text of the item to a string
		CComboBox::GetLBText(lpDrawItemStruct->itemID, sItem.GetBufferSetLength(500));
		sItem.ReleaseBuffer();

		pDC.SetBkMode(TRANSPARENT);

		// Draw the text after the images left postion
		RECT rcItem = rcDraw;
		rcItem.left += LISTBOX_LEFT;
		pDC.DrawIconEx( rcItem.left, rcItem.top, m_MainIcon, COMBO_LEFT_ICON_WIDTH, COMBO_LEFT_ICON_HEIGHT );
		//DrawBitmap( pDC, rcItem.left, rcItem.top, COMBO_LEFT_ICON_WIDTH, COMBO_LEFT_ICON_HEIGHT );
		rcItem.left += EDIT_XOFFSET;

		pDC.DrawText(sItem, sItem.GetLength(),  &rcItem, DT_VCENTER | DT_SINGLELINE);
		return 0;
	}

	//LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	//{
	//	return 0;
	//}

	LRESULT OnPaint (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		static COLORREF s_crGradientXP[][4] =
		{
			{0x00EFF3F7, 0x00DEE7E7, 0x00DEE3E7, 0x00DEE3E7},
			{0x00DEAEA5, 0x00F7CBBD, 0x00DE8273, 0x00F7C7B5},
			{0x00EFC7B5, 0x00E7AE94, 0x00DEA284, 0x00DEA68C},
			{0x00FFE3D6, 0x00F7CBBD, 0x00F7C3AD, 0x00F7C7B5},

			{0x00F7F7F7, 0x00EFF3F7, 0x00EFF3F7, 0x00EFF3F7},
			{0x00DEC3BD, 0x00DEB6AD, 0x00FFE3DE, 0x00F7E3DE},
			{0x00EFDBCE, 0x00EFCFC6, 0x00E7CFC6, 0x00E7CBBD},
			{0x00FFEFE7, 0x00FFE7DE, 0x00FFE3DE, 0x00F7E3DE},

			{0x00F7F7F7, 0x00E7EFEF, 0x00E7EBEF, 0x00DEE7E7},
			{0x00F78E6B, 0x00F79684, 0x00EF9E8C, 0x00EFDFD6},
			{0x00FFFFFF, 0x00FFE3CE, 0x00FFDFC6, 0x00FFDBBD},
			{0x00FFEBE7, 0x00FFCFBD, 0x00FFCBB5, 0x00F7CBAD}
		};

		CPaintDC dc(this->m_hWnd); // device context for painting
		RECT rc;
		CDC memDC;

		memDC.CreateCompatibleDC(dc);
		INT32 i;

		// 获取窗口大小
		CBitmap bkBmp, oldBmp;

		GetWindowRect(&rc);
		//GetClientRect(&rc);

		ATLTRACE("ComboBox left %d right %d top %d bottom %d\n", rc.left, rc.right, rc.top, rc.bottom);
		//::MoveWindow(m_hWnd, rc.left, rc.top, 20, rc.right - rc.left, FALSE);
		//rc.bottom -= 2;
		rc.right -= rc.left; //Draw the edit in combobox border.
		rc.bottom -= rc.top;
		rc.top = rc.left = 0;
		bkBmp.CreateCompatibleBitmap(dc, rc.right,rc.bottom);
		oldBmp = memDC.SelectBitmap(bkBmp);

		// 绘制外框
		LONG style = ::GetWindowLong(m_hWnd, GWL_STYLE);
		BOOL isDisable = !IsWindowEnabled();

		CBrush commonBrh;
		commonBrh.CreateSolidBrush((isDisable) ? (GetSysColor(COLOR_BTNFACE) - 0x00202020) : 0x00BD9E7B);
		memDC.FrameRect(&rc, commonBrh);
		DeleteObject(commonBrh);

		// 绘制内框
		InflateRect(&rc, -1, -1);
		commonBrh.Attach(GetSysColorBrush((isDisable) ? COLOR_BTNFACE : COLOR_WINDOW));
		memDC.FrameRect(&rc, commonBrh);

		InflateRect(&rc, -1, -1);
		rc.left = rc.right - GetSystemMetrics(SM_CYVTHUMB);
		memDC.FrameRect(&rc, commonBrh);
		commonBrh.DeleteObject();

		rc.left++;
		if (isDisable)
			i = 0;
		else if ( isDroped)
			i = 1;
		else if (0)
			i = 2;
		else
			i = 3;

		// 绘制下拉外框
		GradientRectXP(memDC, &rc, s_crGradientXP[i]);
		//commonBrh.CreateSolidBrush(s_crGradientXP[i][1]);
		//memDC.FillRect(&rc, commonBrh);
		//commonBrh.DeleteObject();

		// 绘制下拉外框拐角像素
		memDC.SetPixel(rc.left, rc.top, s_crGradientXP[i + 4][0]);
		memDC.SetPixel(rc.right - 1, rc.top, s_crGradientXP[i + 4][1]);
		memDC.SetPixel(rc.left, rc.bottom - 1, s_crGradientXP[i + 4][2]);
		memDC.SetPixel(rc.right - 1, rc.bottom - 1, s_crGradientXP[i + 4][3]);		

		// 绘制下拉内框
		InflateRect(&rc, -1, -1);
		GradientRectXP(memDC, &rc, s_crGradientXP[i + 8]);
		//commonBrh.CreateSolidBrush(s_crGradientXP[i + 8][1]);
		//memDC.FillRect(&rc, commonBrh);
		//commonBrh.DeleteObject();

		// 绘制下拉标志
		rc.left += (rc.right - rc.left) / 2;
		rc.top += (rc.bottom - rc.top) / 2;

		CPen commonPen, oldPen;
		commonPen.CreatePen(PS_SOLID, 1, (isDisable) ? 0x00C6CBCE : 0x0084614A);
		oldPen = memDC.SelectPen(commonPen);

		memDC.MoveTo(rc.left - 4, rc.top - 2);
		memDC.LineTo(rc.left, rc.top + 2);
		memDC.LineTo(rc.left + 5, rc.top - 3);
		memDC.MoveTo(rc.left - 3, rc.top - 2);
		memDC.LineTo(rc.left, rc.top + 1);
		memDC.LineTo(rc.left + 4, rc.top - 3);
		memDC.MoveTo(rc.left - 3, rc.top - 3);
		memDC.LineTo(rc.left, rc.top);
		memDC.LineTo(rc.left + 4, rc.top - 4);

		memDC.SelectPen(oldPen);
		DeleteObject(commonPen);

		GetClientRect(&rc);
		//rc.bottom -= 2;
		rc.right -= rc.left;
		rc.bottom -= rc.top;
		rc.top = rc.left = 0;

		rc.right = rc.right - rc.left;
		rc.bottom = rc.bottom - rc.top;
		rc.top = rc.left = 0;
		rc.left +=2;
		rc.right -= 18;
		rc.top += 2;
		rc.bottom -= 2;

		memDC.FillSolidRect(&rc,LISTBOX_NORMAL_CLR);
		//ATLTRACE("Draw icon left %d, top %d\n", rc.left, rc.top);
		memDC.DrawIconEx( rc.left, rc.top, m_MainIcon, COMBO_LEFT_ICON_WIDTH, COMBO_LEFT_ICON_HEIGHT );
		//DrawBitmap( memDC, rc.left + 1, rc.top, COMBO_LEFT_ICON_WIDTH, COMBO_LEFT_ICON_HEIGHT );

		// 还原并释放内存设备场景
		GetWindowRect(&rc);
		dc.BitBlt(0, 0,
			rc.right - rc.left, rc.bottom - rc.top, memDC, 0, 0, SRCCOPY);

		memDC.SelectBitmap(oldBmp);
		bkBmp.DeleteObject();
		memDC.DeleteDC();
		bHandled = TRUE;
		return 0;
	}

private:

	void SetItemDataPtr(INT32 idx,void *v)
	{
		baseClass::SetItemDataPtr(idx,v);
	}

	void InsertString(INT32 idx, const TCHAR* str)
	{
		baseClass::InsertString(idx,str);
	}

	void ResetContent() 
	{
		baseClass::ResetContent();
	}

	void SetCurSel(INT32 idx)
	{
		baseClass::SetCurSel(idx);
	}

	void AddinHistoryAndUpdateWords(_bstr_t URL)
	{
		if(std::basic_string<TCHAR>(URL).find_first_not_of(_T(" "))!=std::string::npos)
		{
			if(isGetValue(_T("KeepHistory")) && m_pComboHost->m_values.empty())
			{
				INT32 h = GetCount();
				INT32 pos = CComboBox::FindStringExact(-1,URL);
				if(pos== CB_ERR) 
				{
					// not found
					if(h<m_pComboHost->m_histmax)
					{
						// normal
						InsertString(0,URL);
					}
					else
					{
						// replace last
						DeleteString(h-1);
						InsertString(0,URL);
					}
				}
				else
				{
					// move to last
					DeleteString(pos);
					InsertString(0,URL);
				}

				{
					CRegKey keyAppID;
					keyAppID.Attach(GetAppRegistryKey());
					keyAppID.RecurseDeleteKey(getHistName(m_pComboHost->m_strName));
				}
				{
					CRegKey keyAppID;
					keyAppID.Create(GetAppRegistryKey(),getHistName(m_pComboHost->m_strName));
					for(INT32 i=0;i<GetCount();i++)
					{
						keyAppID.SetValue( ( DWORD )1,GetLBText(i));
					}
				}
				SetCurSel(0);
			}
		}
		m_pConfigHost->UpdateSearchedWords();
	}

	void ClearHistory()
	{
		if (m_pComboHost->m_style != CBS_DROPDOWNLIST) 
		{
			CRegKey keyAppID;
			keyAppID.Attach(GetAppRegistryKey());
			keyAppID.RecurseDeleteKey(getHistName(GetBtnConfCaption()));
			ResetContent();
		}
	}

	INT32 FindStringExact(const TCHAR* str)
	{
		return CComboBox::FindStringExact(-1,str);
	}

	_bstr_t GetText()
	{
		vector<TCHAR> cvalue(1024);
		GetWindowText( &cvalue.front(), ( INT32 )cvalue.size() );
		if( m_pComboHost->m_style == CBS_DROPDOWNLIST )
		{
			XString* s = (XString*)GetItemDataPtr(CComboBox::FindStringExact(-1,&cvalue.front()));
			return s ? s->c_str() : &cvalue.front();
		}

		return &cvalue.front();
	}

	LRESULT OnTVSelDate(WORD, WORD pNmHdr, HWND,BOOL& )
	{
		// Get the text in the edit box
		_bstr_t str = GetText();
		INT32 nLength = GetWindowTextLength();

		m_pConfigHost->UpdateSearchedWords();

		// if we are not to auto update the text, get outta here
		if(!isGetValue(_T("AutoComplete")))
			return 0;

		if(!m_auto)
		{
			return 0;
		}

		// Currently selected range
		DWORD dwCurSel = GetEditSel();
		WORD dStart = LOWORD(dwCurSel);
		WORD dEnd   = HIWORD(dwCurSel);

		// Search for, and select in, and string in the combo box that is prefixed
		// by the text in the edit box
		if (SelectString(-1, str) == CB_ERR)
		{
			SetWindowText(str);		// No text selected, so restore what was there before
			if (dwCurSel != CB_ERR)
				SetEditSel(dStart, dEnd);	//restore cursor postion
		}

		// Set the text selection as the additional text that we have added
		if (dEnd < nLength && dwCurSel != CB_ERR)
			SetEditSel(dStart, dEnd);
		else
			SetEditSel(nLength, -1);
		return 0;
	}

	LRESULT OnTVSelChanged(WORD, WORD pNmHdr, HWND,BOOL& )
	{
		INT32 iItem = GetCurSel();
		if(iItem>=0)
		{
			if(m_pComboHost->m_values.empty())
			{
				m_Edit.SetWindowText(GetLBText(iItem));
				if(isGetValue(_T("RunSearchAutomatically")))
					Process();
			}
			else
			{
				if(onCombo(m_pConfigHost,iItem))
				{
					XString s;
					s += char(iItem + '0');
					setValue(_T("SearchUsing"),s);
					SetCurSel(iItem);
				}
			}
			m_pConfigHost->UpdateSearchedWords();
		}
		return 0;
	}
};
