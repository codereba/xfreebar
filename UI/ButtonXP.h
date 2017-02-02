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

#include "Tools.h"
///////////////////////////////////////////////////////////////////////////////
class CButtonXP : public CWindowImpl<CButtonXP, CButton >,
				public COwnerDraw< CButtonXP >
{
public:
	CButtonXP();

public:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	BOOL SubclassWindow(HWND hWnd)
	{
		BOOL bRet;
		bRet = CWindowImpl<CButtonXP, CButton >::SubclassWindow( hWnd );
		PreSubclassWindow( );
		return bRet;
	}

BEGIN_MSG_MAP_EX( CButtonXP )
		CHAIN_MSG_MAP_ALT( COwnerDraw<CButtonXP>, 1)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseOut )
		DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()

// Implementation
public:
    virtual ~CButtonXP();

protected:
    CMouseMgr m_MouseMgr;

    // Generated message map functions
protected:
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &Handled);
	LRESULT OnMouseOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &Handled);
};

