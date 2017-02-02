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
#include "ButtonXP.h"
#include "Draw.h"

////////////////////////////////////////////////////////////////////////////////
CButtonXP::CButtonXP ()
{
}

////////////////////////////////////////////////////////////////////////////////
CButtonXP::~CButtonXP ()
{
}

////////////////////////////////////////////////////////////////////////////////
void CButtonXP::PreSubclassWindow ()
{
    ModifyStyle (0, BS_OWNERDRAW);
    m_MouseMgr.Init (m_hWnd);
}

////////////////////////////////////////////////////////////////////////////////
void CButtonXP::DrawItem (LPDRAWITEMSTRUCT pDis)
{
    CBufferDC cDC (pDis->hDC);
    CRect rcItem (pDis->rcItem);

    HFONT hOldFont = (HFONT)cDC.SelectFont((HFONT)::GetStockObject (DEFAULT_GUI_FONT));
    CPenDC pen (cDC);
    CBrushDC brush (cDC);

    if ( (pDis->itemState & ODS_FOCUS) ||
         LOWORD(::SendMessage (::GetParent (m_hWnd), DM_GETDEFID, 0, 0)) == GetDlgCtrlID() )
    {
        rcItem.DeflateRect (1, 1);
    }
    if ( (pDis->itemState & ODS_DISABLED) ||
         (!m_MouseMgr.MouseOver() && !(pDis->itemState & ODS_SELECTED)) )
    {
        pen.Color (::GetSysColor (COLOR_3DSHADOW));
        brush.Color (HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -10, 0));
    }
    else
    {
        COLORREF crBorder = ::GetSysColor (COLOR_HIGHLIGHT);

        pen.Color (crBorder);

        if ( (pDis->itemState & ODS_SELECTED) )
        {
            brush.Color (HLS_TRANSFORM (crBorder, +50, -50));
            cDC.SetTextColor (RGB(240,240,240));
        }
        else
        {
            brush.Color (HLS_TRANSFORM (crBorder, +80, -66));
            cDC.SetTextColor (::GetSysColor (COLOR_BTNTEXT));
        }
    }
    if ( (pDis->itemState & ODS_DISABLED) )
    {
        cDC.SetTextColor (::GetSysColor (COLOR_GRAYTEXT));
    }
    else if ( (pDis->itemState & ODS_SELECTED) )
    {
        cDC.SetTextColor (RGB(240,240,240));
    }
    else if ( m_MouseMgr.MouseOver() )
    {
        cDC.SetTextColor (0);
    }
    else
    {
        cDC.SetTextColor (::GetSysColor (COLOR_BTNTEXT));
    }
    cDC.Rectangle (rcItem);
    cDC.SetBkMode (TRANSPARENT);
	CWindowText strWnd(m_hWnd);
    cDC.DrawText (strWnd, strWnd.GetLength(), &rcItem, DT_SINGLELINE|DT_CENTER|DT_VCENTER);

    if ( (pDis->itemState & ODS_FOCUS) )
    {
        rcItem.DeflateRect (3, 3);
        cDC.DrawFocusRect (rcItem);
    }
    cDC.SelectFont (hOldFont);
}

////////////////////////////////////////////////////////////////////////////////
LRESULT CButtonXP::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &Handled)
{
	Handled = FALSE;
    m_MouseMgr.OnMouseMove();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
LRESULT CButtonXP::OnMouseOut (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &Handled)
{
	Handled = FALSE;
    m_MouseMgr.OnMouseOut();
	return 0;
}
