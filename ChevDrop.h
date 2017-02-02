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

#ifndef __CHEV_DROP_H__
#define __CHEV_DROP_H__

#include "XResponsor.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUTTONSPERROW	1

class CChevToolBarCtrl : public CCommandBarCtrlImpl<CChevToolBarCtrl, CCommandBarCtrlBase>
{
public:
	CChevToolBarCtrl()
	{
	}

	~CChevToolBarCtrl()
	{
	}

public:
	BEGIN_MSG_MAP_EX( XFreeToolBar )
		REFLECTED_COMMAND_RANGE_HANDLER( ALL_COMMAND_BEGIN, ALL_COMMAND_END, OnCommand )
		REFLECTED_NOTIFY_CODE_HANDLER( TBN_DROPDOWN, OnButtonDropDown )
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnButtonDropDown(INT32, LPNMHDR pNmHdr, BOOL& bHandled)
	{
		return 0;
	}
	virtual LRESULT OnCommand(UINT uNotifyCode, int nID, HWND hWndCtl, BOOL &bHandled)
	{

		return 0;
	}
};

class CChevDrop : public CWindowImpl<CChevDrop>,
					public CCustomDraw<CChevDrop>
{

	typedef CWindowImpl<CChevDrop> _CChevDropBase;

	CChevToolBarCtrl m_tb;

	// Imagelist for the toolbar	
	CImageList m_ImageList;
	CImageList m_HotImageList;

	// This window receives the notifications for the toolbar !
	HWND m_hMsgReceiver;
	HWND m_hMainToolBar;

	// Helper to create a toolbar
	HWND CreateToolBar( HWND hwndParent, HWND hToolToReplicate );

	// Cleans all toolbar 
	BOOL CleanToolBar( );

// Construction
public:
	CChevDrop();

	BOOL CreatePopup( CWindow pParent );

	BOOL ShowPopup(	
						CWindow	MsgReceiver,
						CToolBarCtrl ToolBar,
						CRect	rectDisplayed,
						CPoint	ptScreen );

#define XFREE_SINGLE_ROW 0x00000001

	BOOL ShowPopup(	CWindow MsgReceiver,
					CToolBarCtrl ToolBar,
					DWORD dwItemWidth,
					DWORD dwItemHeight,
					INT32 nChevIndex,
					INT32 nSavedBtnCount,
					BOOL *pButtonsHide,
					CPoint ptScreen,
					UINT uFlags = XFREE_SINGLE_ROW );

	BOOL ShowPopup(	CWindow MsgReceiver,
					TBBUTTON *pTBButtons,
					INT32 nBtnCount,
					CPoint ptScreen,
					UINT uFlags);

	BOOL ShowPopup(	
					CWindow MsgReceiver,
					CToolBarCtrl ToolBar,
					INT32 nChevIndex,
					CPoint ptScreen );

	// Attributes
public:

// Operations
public:
	protected:
	virtual void OnNcDestroy();
	virtual LRESULT OnCommandRangeHandler(UINT uNotifyCode, int nID, HWND hWndCtl, BOOL &bHandled);
	virtual LRESULT OnNotifyRangeHandler(UINT uNotifyCode, LPNMHDR lParam, BOOL &bHandled);

// Implementation
public:
	virtual ~CChevDrop();

	// Generated message map functions
protected:
	void OnKillFocus(CWindow NewWnd);
	void OnKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags);
	INT32 AfterCommandHandled( WORD wID );

	BEGIN_MSG_MAP_EX(CChevDrop)
		COMMAND_RANGE_HANDLER( ALL_COMMAND_BEGIN, ALL_COMMAND_END, OnCommandRangeHandler)
		//NOTIFY_RANGE_CODE_HANDLER(0, ALL_COMMAND_END, TBN_DROPDOWN, OnNotifyRangeHandler)
		//CHAIN_MSG_MAP( CCustomDraw<CChevDrop> )
		//NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnToolTip)

		if(uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->idFrom >= 0 && ((LPNMHDR)lParam)->idFrom <= 0xffffffff) 
		{ 
			bHandled = TRUE; 
			ATLTRACE( _T( "Currnet notified code is %u, Tooltip notified code is %u\r\n" ), 
				((LPNMHDR)lParam)->code, 
				TTN_GETDISPINFO );

				lResult = OnNotifyRangeHandler((int)wParam, (LPNMHDR)lParam, bHandled); 
			if(bHandled) 
				return TRUE; 
		}
		//REFLECT_NOTIFICATIONS()

		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_NCDESTROY(OnNcDestroy)
	END_MSG_MAP()

	VOID SetChevResponsor( XResponsor *pResp )
		{
			m_pChevResponsor = pResp;
		}

	CChevToolBarCtrl *GetToolBar()
	{
		return &m_tb;
	}

protected:
	XResponsor *m_pChevResponsor;
};

struct TBInfoGetter
{
	TBBUTTONINFO *m_pButtonInfo;
	CToolBarCtrl* m_pToolbar;
	INT32 m_nID;
	BOOL m_bAbsID;
	TBInfoGetter( CToolBarCtrl* pToolbar, INT32 nID, TBBUTTONINFO *pButtonInfo, BOOL bAbsID = FALSE, CHAR *pszTextBuf = NULL, INT32 nTextBufLen = 0) : 
																				m_pToolbar( pToolbar ),
																				m_nID( nID ), 
																				m_pButtonInfo( pButtonInfo ), 
																				m_bAbsID( bAbsID ) 
	{
		m_pButtonInfo->cbSize = sizeof TBBUTTONINFO;
		m_pButtonInfo->dwMask = 0;
		m_pButtonInfo->pszText = pszTextBuf;
		m_pButtonInfo->cchText = nTextBufLen;
	}

	TBInfoGetter& image()
	{
		ATLASSERT( m_pButtonInfo != NULL );
		m_pButtonInfo->dwMask |= TBIF_IMAGE;
		return *this;
	}

	TBInfoGetter& state()
	{
		ATLASSERT( m_pButtonInfo != NULL );
		m_pButtonInfo->dwMask |= TBIF_STATE; 
		return *this;
	}

	TBInfoGetter& style()
	{
		ATLASSERT( m_pButtonInfo != NULL );
		m_pButtonInfo->dwMask |= TBIF_STYLE;
		return *this;
	}

	TBInfoGetter& text()
	{
		ATLASSERT(m_pButtonInfo != NULL);
		ATLASSERT(m_pButtonInfo->pszText != NULL);
		m_pButtonInfo->dwMask |= TBIF_TEXT;
		return *this;
	}

	TBInfoGetter& size()
	{
		ATLASSERT(m_pButtonInfo != NULL);
		m_pButtonInfo->dwMask|=TBIF_SIZE;
		return *this;
	}

	~TBInfoGetter()
	{
		m_pToolbar->GetButtonInfo( m_bAbsID == TRUE ? m_nID : ButtonIndexToID( m_pToolbar->m_hWnd, m_nID ), 
			m_pButtonInfo );
	}
};

// TBBUTTONINFO  helper
struct TBInfoSetter
{
	TBBUTTONINFO m_ButtonInfo;
	CToolBarCtrl* m_pToolbar;
	INT32 m_nID;
	BOOL m_bAbsId;
	TBInfoSetter( CToolBarCtrl* pToolbar,INT32 id, BOOL bAbsId = FALSE) : 
														m_pToolbar(pToolbar),
														m_nID(id),
														m_bAbsId(bAbsId) 
	{
		m_ButtonInfo.cbSize = sizeof( TBBUTTONINFO );
		m_ButtonInfo.dwMask = 0;
	}
	
	TBInfoSetter& image( INT32 nImageIndex )
	{
		m_ButtonInfo.iImage = nImageIndex;
		m_ButtonInfo.dwMask |= TBIF_IMAGE;
		return *this;
	}

	TBInfoSetter& state( INT32 nState )
	{
		m_ButtonInfo.fsState = nState;
		m_ButtonInfo.dwMask |= TBIF_STATE;
		return *this;
	}

	TBInfoSetter& style( INT32 nStyle )
	{
		m_ButtonInfo.fsStyle = nStyle;
		m_ButtonInfo.dwMask |= TBIF_STYLE;
		return *this;
	}

	TBInfoSetter& text( TCHAR* lpszText )
	{
		m_ButtonInfo.pszText = lpszText; 
		m_ButtonInfo.dwMask |= TBIF_TEXT;
		return *this;
	}

	TBInfoSetter& size( INT32 nSize )
	{
		m_ButtonInfo.cx = nSize;
		m_ButtonInfo.dwMask |= TBIF_SIZE;
		return *this;
	}

	~TBInfoSetter()
	{
		m_pToolbar->SetButtonInfo( m_bAbsId ? m_nID : ButtonIndexToID( m_pToolbar->m_hWnd, m_nID ), &m_ButtonInfo );
	}
};

#endif //__CHEV_DROP_H__
