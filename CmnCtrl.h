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
#include "CmnUtil.h"

class XFreeToolBar;

struct CPugiObjWrap
{
	CPugiObjWrap() {}
	~CPugiObjWrap()
	{
		m_HotList.Destroy();
		m_DisbList.Destroy();
	}

	CImageList m_NormList,m_HotList, m_DisbList;
	virtual XFreeToolBar* ParentToolBar() = 0;
	LRESULT OnMenuItem(WPARAM wParam);
};

class XFreeSubToolBar : public CCommandBarCtrlImpl<XFreeSubToolBar, CCommandBarCtrlBase>,
	public CPugiObjWrap
{
	BEGIN_MSG_MAP(XFreeSubToolBar)
		REFLECTED_NOTIFY_CODE_HANDLER(TBN_DROPDOWN,   OnButtonDropDown)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		REFLECTED_COMMAND_RANGE_HANDLER( ALL_COMMAND_BEGIN, ALL_COMMAND_END, OnButton )
		COMMAND_ID_HANDLER(MENU_COMMAND_ID_BEGIN,OnCommand)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuItem)
		MESSAGE_HANDLER(WM_PAINT, OnHookPaint)

		MESSAGE_HANDLER(WM_MOUSEMOVE, OnHookMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnHookMouseLeave )

		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnHookMouseLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnHookMouseLButtonUp)
		//MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnHookMouseMove)

	END_MSG_MAP()

	LRESULT OnHookMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	XFreeSubToolBar()
	{
	}

	~XFreeSubToolBar()
	{
	}

	LRESULT OnCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnButtonDropDown(INT32 /*idCtrl*/, LPNMHDR pNmHdr, BOOL& bHandled);
	LRESULT OnMenuItem(UINT , WPARAM wParam, LPARAM lParam, BOOL& ) {return CPugiObjWrap::OnMenuItem(wParam);}  
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHookMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		WhenLButtonDown();
		return 0;
	}

	LRESULT OnHookMouseLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		WhenLButtonUp();
		return 0;
	}

	INT32 WhenLButtonDown()
	{
		m_bLButtonDown = TRUE;
		SetCapture();
		return 0;
	}

	INT32 WhenLButtonUp()
	{
		m_bLButtonDown = FALSE;
		ReleaseCapture();
		return 0;
	}

	virtual XFreeToolBar* ParentToolBar() {return m_pParentToolBar;}
	void setroot(XFreeToolBar* ctrl) {m_pParentToolBar=ctrl;}
public:
	SIZE               m_size;
	INT32                m_uID;
private:
	XFreeToolBar*  m_pParentToolBar;
	BOOL m_bLButtonDown;
	BOOL m_bMouseLeaveHooked;
};