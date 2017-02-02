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

#define CHEVRON_ARROW_LEN 15
#define CHEVRON_ARROW_X_END 2
#define CHEVRON_ARROW_Y_BEGIN 2
#define CHEVRON_ARROW_Y_END 2

class XChevBtn : public CWindow
{
public:
	XChevBtn(void);
	~XChevBtn(void);

	BEGIN_MSG_MAP_EX(XChevBtn)
		MESSAGE_HANDLER( WM_PAINT, OnPaint )
	END_MSG_MAP()

protected:
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CDC DrawDC;
		DrawDC.Attach(GetDC());

		bHandled = FALSE;

		INT32 nArrowPointY;
		INT32 nArrowBeginX;
		INT32 nArrowEndX;
		INT32 nArrowBeginY;
		INT32 nArrowEndY;

		CRect rcChevronBtn;
		GetWindowRect(&rcChevronBtn);

		nArrowPointY = rcChevronBtn.top + rcChevronBtn.Height() / 2;
		nArrowBeginX = rcChevronBtn.right - CHEVRON_ARROW_LEN - CHEVRON_ARROW_X_END;
		nArrowEndX = nArrowBeginX + CHEVRON_ARROW_LEN;
		nArrowBeginY = rcChevronBtn.top + CHEVRON_ARROW_Y_BEGIN;
		nArrowEndY = rcChevronBtn.bottom - CHEVRON_ARROW_Y_END;

		CBrush BrBk, OldBr;
		BrBk.CreateSolidBrush(::GetSysColor(CTLCOLOR_BTN));
		CPen PenBrd, OldPen;
		PenBrd.CreatePen(PS_SOLID, 1, RGB(180, 120, 210));

		OldBr = DrawDC.SelectBrush(BrBk);
		OldPen = DrawDC.SelectPen(PenBrd);

		DrawDC.FillRect(rcChevronBtn, BrBk);
		DrawDC.MoveTo(CPoint(nArrowBeginX, nArrowBeginY));
		DrawDC.LineTo( CPoint(nArrowEndX, nArrowPointY));
		DrawDC.LineTo(CPoint(nArrowBeginX, nArrowEndY));
		DrawDC.SelectBrush(OldBr);
		DrawDC.SelectPen(OldPen);
		ReleaseDC(DrawDC.m_hDC);
		DrawDC.Detach();

		bHandled = TRUE;
		return 0;
	}
};
