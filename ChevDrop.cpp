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


#include "stdafx.h"
#include "ChevDrop.h"

/////////////////////////////////////////////////////////////////////////////
// CChevDrop

CChevDrop::CChevDrop()
{
	m_hMsgReceiver= NULL;

}

CChevDrop::~CChevDrop()
{
	CleanToolBar();
}

BOOL CChevDrop::CreatePopup( CWindow WndParent )
{
	if ( !m_hWnd )
	{
		// Register for our popup window
		return (Create(	WndParent, 
								CRect(0,0,0,0),
								"",
								WS_POPUP|WS_BORDER,
								WS_EX_TOOLWINDOW,
								(HMENU)0, 
								_Module.GetResourceInstance() ) != (HWND)-1);
	}

	return TRUE;
}

// helper to create a new toolbar
HWND CChevDrop::CreateToolBar( HWND hwndParent, HWND hToolToReplicate ) 
{ 
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
		CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | 
		TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

	HWND hwndTB;
	hwndTB = m_tb.Create(hwndParent,CRect(0,0,0,0), NULL, WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE | 
							CCS_NODIVIDER, NULL, 0);

	// Doc says, required for backward compatibility
	::SendMessage( hwndTB, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

	// Our toolbar may have dropdown buttons, so got to set the extended style
	::SendMessage( hwndTB, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS );

	// Get ImageList from the toolbar
	HIMAGELIST hOrgImageList = (HIMAGELIST)::SendMessage( hToolToReplicate, TB_GETIMAGELIST, 0, 0 );
	HIMAGELIST hOrgHotImageList = (HIMAGELIST)::SendMessage( hToolToReplicate, TB_GETHOTIMAGELIST, 0, 0);

	// Create a duplicate of the imagelist
	HIMAGELIST hImageList = ImageList_Duplicate( hOrgImageList );
	HIMAGELIST hHotImageList = ImageList_Duplicate( hOrgHotImageList);

	// Set the imagelist for our new toolbar
	::SendMessage( hwndTB, TB_SETIMAGELIST, 0, (LPARAM)hImageList );
	::SendMessage( hwndTB, TB_SETHOTIMAGELIST, 0, (LPARAM)hHotImageList);

	// Attach it to an MFC object so that it automagically deletes the handle 
	// when it goes out of scope, toooooooo lazy to delete it 
	m_ImageList.Attach ( hImageList );
	m_HotImageList.Attach (hHotImageList );

	return hwndTB; 
} 

BOOL CChevDrop::CleanToolBar( )
{

	BOOL	bReturn = TRUE;

	if ( (HWND)m_tb )
	{
		HWND hTool = m_tb.Detach ( );
		bReturn = ::DestroyWindow ( hTool ) || bReturn ;
	}

	if ( (HIMAGELIST)m_ImageList )
	{
		bReturn = m_ImageList.Destroy() || bReturn ;
	}

	if ( (HIMAGELIST)m_HotImageList)
	{
		bReturn = m_HotImageList.Destroy() || bReturn;
	}

	return bReturn;
}

#define TBCAPTION_MAX_LEN 128

BOOL CChevDrop::ShowPopup(	CWindow MsgReceiver,
							CToolBarCtrl ToolBar,
							DWORD dwItemWidth,
							DWORD dwItemHeight,
							INT32 nChevIndex,
							INT32 nSavedBtnCount,
							BOOL *pButtonsHide,
							CPoint ptScreen,
							UINT uFlags )
{
	// Somebody should be there to receive the message from toolbar
	// The receiving handle should not be empty
	ATLASSERT((HWND)MsgReceiver!=NULL);
	// The source toolbar should exist
	ATLASSERT((HWND)ToolBar != NULL);
	// It is really a toolbar ??

	ATLASSERT( pButtonsHide != NULL );

	INT32 nButtonCount;
	TBBUTTON TBBtn;
	CRect rectButton;
	// Clean if any previous objects have been attached
	CleanToolBar( );

	// Create a tool bar with the popup as parent
	m_hMainToolBar = (HWND)ToolBar;
	CreateToolBar( m_hWnd, (HWND)ToolBar );

	// Store the window which receives notifications from the toolbar
	// We have to redirect a few later
	m_hMsgReceiver = (HWND)MsgReceiver;
	
	// This flag indicates if atleast one has been added to the menu
	// POPUP Menu is shown only if atleast one item has to be shown
	// Run along all the buttons, find hidden ones and add them to new toolbar
	int iCount;
	nButtonCount  = ToolBar.GetButtonCount();

	CSize szBtnMax(0,0);

	ATLTRACE( "The button count is %d, saved button count is %d\r\n", nButtonCount, nSavedBtnCount );
	TCHAR TBCaption[TBCAPTION_MAX_LEN];
	for ( iCount = nChevIndex; iCount < nButtonCount - nSavedBtnCount ; iCount++ )
	{
		ToolBar.GetButton ( iCount, &TBBtn );
		//int nRet = m_tb.AddStrings(TBCaption);
		
		// If the button is a separator then we can also add a separator to the
		// popup menu
		if (  TBBtn.fsStyle & TBSTYLE_SEP )
		{
		}
		else
		{
			if( FALSE == pButtonsHide[ iCount - nChevIndex ] )
			{
				TBBtn.fsState &= ~TBSTATE_HIDDEN;
				*TBCaption = 0;

				ToolBar.GetButtonText( TBBtn.idCommand, TBCaption );
				m_tb.AddButtons ( 1, &TBBtn );
				TBInfoSetter(&m_tb, TBBtn.idCommand, TRUE).text( TBCaption );
			}
		}
	}

	m_tb.AutoSize();

	szBtnMax.cx += 10;
	// Better call this after modifying buttons.. dunno what will go wrong if i dont
	// use this
	ShowWindow( SW_HIDE );
	nButtonCount = m_tb.GetButtonCount();
	// Show the window only if atleast one item has been added
	if ( 0 == nButtonCount )
	{
		return FALSE;
	}

	for(int i = 0; i < nButtonCount; i++)
	{
		if( FALSE == m_tb.GetItemRect(i, &rectButton) )
		{
			ATLASSERT( FALSE );
			continue;
		}
		
		if(rectButton.Width() > szBtnMax.cx && rectButton.Width() < 150)
		{
			szBtnMax.cx = rectButton.Width();
		}

		if(rectButton.Height() > szBtnMax.cy)
		{
			szBtnMax.cy = rectButton.Height();
		}
	}

	szBtnMax.cx = dwItemWidth;
	for(int i = 0; i < nButtonCount; i++)
	{
		TBInfoSetter( &m_tb, i ).size(szBtnMax.cx);
	}

	// Calculate the number of approx rows required
	int iRows = max( (nButtonCount + BUTTONSPERROW - 1) / BUTTONSPERROW, 1 );

	CRect recWindow;
	// Set rows calculated. recWindow is used in resizing the parent popup window
	m_tb.SetRows ( iRows, TRUE, &recWindow );

	// NOTE: the number of rows need not be the same as iRows... Please check
	// doc for CToolBarCtrl::SetRows( )

	// BUGBUG: When the toolbar had one button, recWindow had 0 width, so have to 
	// get width from GetMaxSize( ). This may be wrong when the button
	// had TBSTYLE_DROPDOWN style...

	// Get the top and bottom spacing for the toolbar

	DWORD dwPad = (DWORD)::SendMessage( (HWND)m_tb, TB_GETPADDING, 0, 0 );
	CRect rectWindow(	ptScreen.x, 
						ptScreen.y, 
						ptScreen.x + szBtnMax.cx + 2,//LOWORD(dwPad),
						ptScreen.y + szBtnMax.cy * iRows + 2//HIWORD(dwPad)
						);

	// Have to adjust to screen pos
	int cxScreen = ::GetSystemMetrics ( SM_CXSCREEN );
	int cyScreen = ::GetSystemMetrics ( SM_CYSCREEN );

	// Move little left to show the complete toolbar
	if ( rectWindow.right > cxScreen )
	{
		int diff = rectWindow.right - cxScreen;
		rectWindow.left -= diff;
		rectWindow.right -= diff;
	}
	// Move little up to show the complete toolbar
	if ( rectWindow.bottom > cyScreen )
	{
		int diff = rectWindow.bottom - cyScreen;
		rectWindow.top -= diff;
		rectWindow.bottom -= diff;
	}

	// Should we check for top and left positions ????
	// Move the parent popup window and show
	MoveWindow( &rectWindow, TRUE );

	// Move toolbar to the top corner and show
	m_tb.MoveWindow (	//hWndToolbar, 
					rectWindow.left, 
					rectWindow.top,
					rectWindow.Width(),
					rectWindow.Height(),
					TRUE );

	ShowWindow( SW_SHOW );
	m_tb.ShowWindow ( SW_SHOW );

	return TRUE;
}

BOOL CChevDrop::ShowPopup(	
							CWindow MsgReceiver,
							CToolBarCtrl ToolBar,
							INT32 nChevIndex,
							CPoint	ptScreen )
{
	// Somebody should be there to receive the message from toolbar
	// The receiving handle should not be empty
	ATLASSERT((HWND)MsgReceiver!=NULL);
	// The source toolbar should exist
	ATLASSERT((HWND)ToolBar != NULL);
	// It is really a toolbar ??

	INT32 nButtonCount;
	TBBUTTON TBBtn;
	CRect rectButton;
	// Clean if any previous objects have been attached
	CleanToolBar( );

	// Create a tool bar with the popup as parent
	m_hMainToolBar = (HWND)ToolBar;
	CreateToolBar( m_hWnd, (HWND)ToolBar );

	// Store the window which receives notifications from the toolbar
	// We have to redirect a few later
	m_hMsgReceiver = (HWND)MsgReceiver;
	
	// This flag indicates if atleast one has been added to the menu
	// POPUP Menu is shown only if atleast one item has to be shown
	// Run along all the buttons, find hidden ones and add them to new toolbar
	int iCount;
	nButtonCount  = ToolBar.GetButtonCount();

	CSize szBtnMax(0,0);

	TCHAR TBCaption[TBCAPTION_MAX_LEN];
	for ( iCount = nChevIndex ; iCount < nButtonCount  ; iCount++ )
	{
		ToolBar.GetButton ( iCount, &TBBtn );
		
		// If the button is a separator then we can also add a separator to the
		// popup menu
		if (  TBBtn.fsStyle & TBSTYLE_SEP )
		{
		}
		else
		{
			ToolBar.GetString( iCount, TBCaption, TBCAPTION_MAX_LEN);
			TBCaption[strlen(TBCaption) + 2] = 0;
			TBCaption[strlen(TBCaption) + 1] = 0;
			TBCaption[strlen(TBCaption) + 0] = ' ';

			TBBtn.fsState |= TBSTATE_ENABLED;
			m_tb.AddButtons ( 1, &TBBtn );
			TBInfoSetter(&m_tb, TBBtn.idCommand, TRUE).text( TBCaption );
		}
	}

	m_tb.AutoSize();
	INT32 nCount = m_tb.GetButtonCount();
	for(INT32 i = 0; i < nCount; i++ )
	{
		//Get the button rectangle
		if(FALSE == m_tb.GetItemRect ( i, &rectButton ))
		{
			continue;
		}
		if(rectButton.Width() > szBtnMax.cx && rectButton.Width() < 150)
		{
			szBtnMax.cx = rectButton.Width();
		}

		if(rectButton.Height() > szBtnMax.cy)
		{
			szBtnMax.cy = rectButton.Height();
		}
	}

	szBtnMax.cx += 10;
	SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE);
	// Better call this after modifying buttons.. dunno what will go wrong if i dont
	// use this

	nButtonCount = m_tb.GetButtonCount();
	// Show the window only if atleast one item has been added
	if ( 0 == nButtonCount )
	{
		return FALSE;
	}

	for(int i = 0; i < nButtonCount; i++)
	{
		m_tb.GetButton ( i, &TBBtn );
		TBInfoSetter(&m_tb, TBBtn.idCommand, TRUE).size(szBtnMax.cx);
	}

	// Calculate the number of approx rows required
	int iRows = max( (nButtonCount + BUTTONSPERROW - 1) / BUTTONSPERROW, 1 );

	CRect recWindow;
	// Set rows calculated. recWindow is used in resizing the parent popup window
	m_tb.SetRows ( iRows, TRUE, &recWindow );

	// NOTE: the number of rows need not be the same as iRows... Please check

	// BUGBUG: When the toolbar had one button, recWindow had 0 width, so have to 
	// get width from GetMaxSize( ). This may be wrong when the button
	// had TBSTYLE_DROPDOWN style...

	// Get the top and bottom spacing for the toolbar
	DWORD dwPad = (DWORD)::SendMessage( (HWND)m_tb, TB_GETPADDING, 0, 0 );
	CRect rectWindow(	ptScreen.x, 
						ptScreen.y, 
						ptScreen.x + szBtnMax.cx + 2,//LOWORD(dwPad),
						ptScreen.y + szBtnMax.cy * iRows + 2//HIWORD(dwPad)
						);

	// Have to adjust to screen pos
	int cxScreen = ::GetSystemMetrics ( SM_CXSCREEN );
	int cyScreen = ::GetSystemMetrics ( SM_CYSCREEN );

	// Move little left to show the complete toolbar
	if ( rectWindow.right > cxScreen )
	{
		int diff = rectWindow.right - cxScreen;
		rectWindow.left -= diff;
		rectWindow.right -= diff;
	}
	// Move little up to show the complete toolbar
	if ( rectWindow.bottom > cyScreen )
	{
		int diff = rectWindow.bottom - cyScreen;
		rectWindow.top -= diff;
		rectWindow.bottom -= diff;
	}

	// Should we check for top and left positions ????
	// Move the parent popup window and show
	MoveWindow( &rectWindow, TRUE );

	// Move toolbar to the top corner and show
	m_tb.MoveWindow (	//hWndToolbar, 
					rectWindow.left, 
					rectWindow.top,
					rectWindow.Width(),
					rectWindow.Height(),
					TRUE );

	ShowWindow( SW_SHOW );
	m_tb.ShowWindow ( SW_SHOW );
	return TRUE;
}

BOOL CChevDrop::ShowPopup(	CWindow MsgReceiver,
							TBBUTTON *pTBButtons,
							INT32	nBtnCount,
							CPoint	ptScreen,
							UINT	uFlags)
{
	// Somebody should be there to receive the message from toolbar
	// The receiving handle should not be empty
	ATLASSERT((HWND)MsgReceiver!=NULL);

	ATLASSERT( pTBButtons != NULL && nBtnCount != 0 );
	// It is really a toolbar ??
#define TBCAPTION_MAX_LEN 128

	// Clean if any previous objects have been attached
	CleanToolBar( );

	// Create a tool bar with the popup as parent
	CreateToolBar( m_hWnd, (HWND)MsgReceiver );

	// Store the window which receives notifications from the toolbar
	// We have to redirect a few later
	m_hMsgReceiver = (HWND)MsgReceiver;

	m_tb.AddButtons ( nBtnCount, pTBButtons );

	// Better call this after modifying buttons.. dunno what will go wrong if i dont
	// use this

	INT32 nToolbarRows = 1;
	DWORD dwPad = (DWORD)::SendMessage( (HWND)m_tb, TB_GETPADDING, 0, 0 );
	CRect rcToolbar;
	CRect rcButton;
	CSize szBtnMax;
	CSize szToolbar;
	
	if( uFlags & XFREE_SINGLE_ROW)
	{
		szBtnMax.cx = 0;
		szBtnMax.cy = 0;
		for(int i = 0; i < nBtnCount; i++)
		{
			if(FALSE == m_tb.GetItemRect ( i, &rcButton ))
			{
				ATLASSERT(FALSE);
				continue;
			}

			if(rcButton.Width() > szBtnMax.cx && rcButton.Width() < 150)
			{
				szBtnMax.cx = rcButton.Width();
			}

			if(rcButton.Height() > szBtnMax.cy)
			{
				szBtnMax.cy = rcButton.Height();
			}
		}

		for(int i = 0; i < nBtnCount; i++)
		{
			TBInfoSetter(&m_tb, pTBButtons->idCommand, TRUE).size(szBtnMax.cx);
		}
		// Calculate the number of approx rows required
		nToolbarRows = max( (nBtnCount + BUTTONSPERROW - 1) / BUTTONSPERROW, 1 );
		m_tb.SetRows ( nToolbarRows, TRUE, &rcToolbar );
		rcToolbar.left = ptScreen.x;
		rcToolbar.top = ptScreen.y;
		rcToolbar.right = ptScreen.x + szBtnMax.cx + 2;//LOWORD(dwPad)
		rcToolbar.bottom = ptScreen.y + szBtnMax.cy * nToolbarRows + 2;//HIWORD(dwPad)
	}
	else
	{
		m_tb.AutoSize();
		m_tb.GetWindowRect(&rcToolbar);
		m_tb.GetMaxSize ( &szToolbar );
		INT32 nToolbarWidth = rcToolbar.Width() == 0 ? szToolbar.cx : rcToolbar.Width();
	}

	// Set rows calculated. recWindow is used in resizing the parent popup window

	// NOTE: the number of rows need not be the same as iRows... Please check
	// doc for CToolBarCtrl::SetRows( )

	// BUGBUG: When the toolbar had one button, recWindow had 0 width, so have to 
	// get width from GetMaxSize( ). This may be wrong when the button
	// had TBSTYLE_DROPDOWN style...

	// Get the top and bottom spacing for the toolbar

	// Have to adjust to screen pos
	int cxScreen = ::GetSystemMetrics ( SM_CXSCREEN );
	int cyScreen = ::GetSystemMetrics ( SM_CYSCREEN );

	// Move little left to show the complete toolbar
	if ( rcToolbar.right > cxScreen )
	{
		int diff = rcToolbar.right - cxScreen;
		rcToolbar.left -= diff;
		rcToolbar.right -= diff;
	}
	// Move little up to show the complete toolbar
	if ( rcToolbar.bottom > cyScreen )
	{
		int diff = rcToolbar.bottom - cyScreen;
		rcToolbar.top -= diff;
		rcToolbar.bottom -= diff;
	}

	// Should we check for top and left positions ????
	// Move the parent popup window and show
	MoveWindow( &rcToolbar, TRUE );

	// Move toolbar to the top corner and show
	m_tb.MoveWindow (	//hWndToolbar, 
					rcToolbar.left, 
					rcToolbar.top,
					rcToolbar.Width(),
					rcToolbar.Height(),
					TRUE );

	ShowWindow( SW_SHOW );
	m_tb.ShowWindow ( SW_SHOW );
	return TRUE;
}

void CChevDrop::OnKillFocus(CWindow NewWnd) 
{
	SetWindowPos( NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOACTIVATE );
}

void CChevDrop::OnNcDestroy() 
{

	CleanToolBar( );
}

INT32 CChevDrop::AfterCommandHandled( WORD wID )
{
	BOOL bRet;
	TBBUTTONINFO BtnInfo;
	memset( &BtnInfo, 0, sizeof( BtnInfo ) );
	BtnInfo.cbSize = sizeof( TBBUTTONINFO );
	BtnInfo.dwMask |= TBIF_STATE | TBIF_STYLE; 

	bRet = ( BOOL )::SendMessage(m_hMainToolBar, TB_GETBUTTONINFO, wID, (LPARAM)&BtnInfo);
	if( FALSE == bRet )
		return XERROR_COMMON;

	bRet = ( BOOL )::SendMessage(m_hMainToolBar, TB_ISBUTTONCHECKED, wID, 0L);

	ATLTRACE( "Btn %d is checked %d\r\n", wID, bRet );
	::SendMessage(m_hWnd, TB_CHECKBUTTON, wID, MAKELPARAM(bRet, 0));


	bRet = ( BOOL )::SendMessage(m_hWnd, TB_SETBUTTONINFO, wID, (LPARAM)&BtnInfo);
	Invalidate( 0 );
	if( FALSE == bRet )
		return XERROR_COMMON;
	return 0;
}

LRESULT CChevDrop::OnCommandRangeHandler(UINT uNotifyCode, int nID, HWND hWndCtl, BOOL &bHandled)
{
	if( NULL != m_pChevResponsor )
		m_pChevResponsor->CommandResponse( nID );
	bHandled = TRUE;
	return 0;
}

LRESULT CChevDrop::OnNotifyRangeHandler(UINT uNotifyCode, LPNMHDR lpNMHdr, BOOL &bHandled)
{
	LRESULT lReturn;
	NMTOOLBAR NMTB = *(LPNMTOOLBAR)lpNMHdr;

	if(lpNMHdr->code == TTN_GETDISPINFO)
	{
		lReturn = ::SendMessage( m_hMsgReceiver, WM_NOTIFY, (WPARAM)uNotifyCode, (LPARAM)lpNMHdr );
		DWORD dwError = ::GetLastError();
		bHandled = TRUE;
		return lReturn;
	}

	ClientToScreen(&((LPNMTOOLBAR)lpNMHdr)->rcButton);
	::ScreenToClient(m_hMainToolBar, (LPPOINT)&((LPNMTOOLBAR)lpNMHdr)->rcButton);
	::ScreenToClient(m_hMainToolBar, ((LPPOINT)&((LPNMTOOLBAR)lpNMHdr)->rcButton) + 1);
	lpNMHdr->hwndFrom = m_hMainToolBar;
	lReturn = ::SendMessage( m_hMsgReceiver, WM_NOTIFY, (WPARAM)uNotifyCode, (LPARAM)lpNMHdr );

	*(LPNMTOOLBAR)lpNMHdr = NMTB;
	bHandled = TRUE;
	return lReturn;
}

void CChevDrop::OnKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( nChar == VK_ESCAPE )
	{
		// If Escape key was pressed, close popup window
		SetWindowPos( NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOACTIVATE );
	}
}

