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

struct XTicker : public Button
{
	XTicker() 
	{
		m_pos_last=0; 
		m_last=QueryCounter();
	}
	virtual void OnOptionChange(XFreeToolBar* ctrl)
	{
		ctrl->HideButtonIfNeeded( m_nID, m_command, FALSE );
	}

	CFont m_Font;
	virtual void XChange( XFreeToolBar* pToolbar )
	{
		Button::XChange( pToolbar );

		TBBUTTONINFO ButtonInfo;
		ButtonInfo.cbSize = sizeof TBBUTTONINFO;
		ButtonInfo.dwMask = TBIF_SIZE | TBIF_STYLE;
		ButtonInfo.fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT;
		tButtonInfobi.cx = m_style.m_dwWidth;
		pToolbar->SetButtonInfo( pToolbar->MapBtnIDFromIndex( m_uID ), &tbi);
		//if(!m_ItemImgs.IsNull())
		//{
		//	m_ItemImgs.Destroy();
		//}
		//m_ItemImgs.Create(100, 100, ILC_COLORDDB, 5, 1);
		UINT uFontWeight = FW_NORMAL;
		if(m_style.m_IsBold)
		{
			uFontWeight = FW_BOLD;
		}

		BYTE uUnderLine = 0;
		if(m_style.m_bUnderLine)
		{
			uUnderLine = 1;
		}

		m_Font.CreateFont(m_style.m_fontsize,0, 0, 0, uFontWeight, 0, uUnderLine, 0,DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS, m_style.m_font.c_str());

		CDC dc = ::GetDC(NULL);
		dc.SelectFont(m_Font.m_hFont);
		for(INT32 i=0;i<m_items.size();i++)
		{
			GetTextExtentPoint(dc,m_items[i]->m_value.c_str(),m_items[i]->m_value.size(),&m_items[i]->m_size);
			if((HBITMAP)m_items[i]->m_bmp != 0)
				m_items[i]->m_bmp.DeleteObject();
			m_items[i]->m_bmp = LoadImage(m_items[i]->m_background);
		}
	}

	virtual void XDetermine(XmlParserProxy& p)
	{
		XToolbarItem::XDetermine(p);
		XML::determineMember<TickerItem>(m_items,_T("tickeritem"),p);
		XML::determineMember<>(m_style,_T("tickerstyle"),p);
		m_strType = _T("XTICKER");
	}

private:
	virtual void Invalidate(XFreeToolBar* ctrl)
	{
		CRect rc;
		ctrl->GetItemRect(m_uID, &rc);
		rc.right = rc.left + m_style.m_dwWidth;
		rc.bottom = TB_HEIGHT;
		if(ctrl->IsWindow())
			ctrl->InvalidateRect(rc,FALSE);
	}

	virtual void OnXFreeBarCommand(XFreeToolBar* ctrl)
	{
		float now = QueryCounter();
		float delta = now - m_last;
		m_last = now;
		m_pos_last += delta*m_style.m_speed;

		while(m_pos_last>=m_items.size())
			m_pos_last -= m_items.size();

		INT32 cur = (INT32)m_pos_last;
		if(Commands::Command* c = ctrl->m_pXFreeToolbar->FindCommandConf(m_items[cur]->m_command))
			c->apply(ctrl);
	}

	virtual DWORD OnItemPrePaint(XFreeToolBar* ctrl,LPNMTBCUSTOMDRAW lpTBCustomDraw)
	{
		float now = QueryCounter();
		float delta = now - m_last;
		m_last = now;
		m_pos_last += delta*m_style.m_speed;

		while(m_pos_last>=m_items.size())
			m_pos_last -= m_items.size();

		INT32 cur = (INT32)m_pos_last;
		float percent = (m_pos_last - cur);

		percent *= (1+m_items[cur]->m_delay);

		if(percent>1)
			percent = 0;

		CDCHandle dc = lpTBCustomDraw->nmcd.hdc;

		dc.SetTextColor(m_style.m_textcolor);
		dc.SetBkMode(TRANSPARENT);
		dc.SelectFont(m_Font.m_hFont);

		// make height from XML
		INT32 cur_height = lpTBCustomDraw->nmcd.rc.bottom - lpTBCustomDraw->nmcd.rc.top;
		INT32 middle = (lpTBCustomDraw->nmcd.rc.bottom + lpTBCustomDraw->nmcd.rc.top)/2;
		lpTBCustomDraw->nmcd.rc.bottom = middle + m_style.m_height/2;
		lpTBCustomDraw->nmcd.rc.top    = middle - m_style.m_height/2;

		// draw border
		CPen pen(CreatePen(0, m_style.m_border, m_style.m_bordercolor));
		lpTBCustomDraw->nmcd.rc.right -= 2;
		CRect r = lpTBCustomDraw->nmcd.rc;
		HPEN old = dc.SelectPen(pen);
		dc.MoveTo(r.left,r.bottom);
		POINT lpPoints[] = { {r.left,r.top},{r.right,r.top},{r.right,r.bottom},{r.left,r.bottom} };
		dc.PolylineTo(lpPoints,4);
		dc.SelectPen(old);


		// correct due to border rect
		lpTBCustomDraw->nmcd.rc.left  += m_style.m_border;
		lpTBCustomDraw->nmcd.rc.right -= m_style.m_border;

		// clip region
		CRgn hrgn;
		hrgn.CreateRectRgnIndirect(&lpTBCustomDraw->nmcd.rc);
		SelectClipRgn(lpTBCustomDraw->nmcd.hdc,hrgn);

		lpTBCustomDraw->nmcd.rc.top   += m_style.m_border;
		lpTBCustomDraw->nmcd.rc.bottom-= m_style.m_border;

		// text height
		INT32 height = m_items[cur]->m_size.cy;
		INT32 h = lpTBCustomDraw->nmcd.rc.bottom - lpTBCustomDraw->nmcd.rc.top;
		if(m_items[cur]->m_bmp)
		{
			CRect BmpRc =  lpTBCustomDraw->nmcd.rc;
			SIZE BmpSz;
			GetBitmpSize(m_items[cur]->m_bmp, BmpSz);
			CDC MemDC;
			MemDC.CreateCompatibleDC(dc);
			CBitmapHandle OldBmp = MemDC.SelectBitmap(m_items[cur]->m_bmp);
			dc.StretchBlt(BmpRc.left, BmpRc.top, BmpRc.Width(), BmpRc.Height(), MemDC, 0, 0,  BmpSz.cx, BmpSz.cy,SRCCOPY);
			MemDC.SelectBitmap(OldBmp);
		}

		lpTBCustomDraw->nmcd.rc.top    += (h-height)/2;
		lpTBCustomDraw->nmcd.rc.bottom -= (h-height)/2;

		CRect rc = lpTBCustomDraw->nmcd.rc;

		INT32 width = m_items[cur]->m_size.cx;
		INT32 w = (rc.right-(rc.left - width));
		lpTBCustomDraw->nmcd.rc.left += w*(1-percent) - width;

		lpTBCustomDraw->nmcd.rc.right = lpTBCustomDraw->nmcd.rc.left+width;
		DrawText(lpTBCustomDraw->nmcd.hdc,m_items[cur]->m_value.c_str(), -1, &lpTBCustomDraw->nmcd.rc, DT_VCENTER | DT_SINGLELINE);

		DeleteObject(hrgn);

		SelectClipRgn(lpTBCustomDraw->nmcd.hdc,0);
		return CDRF_SKIPDEFAULT;
	}

	float m_last;
	float m_pos_last;

	struct TickerStyle : public XmlObject
	{
		TickerStyle() : m_fontsize(10) {}

		INT32     m_border;
		INT32     m_bordercolor;
		//INT32     m_background;
		XString m_font;
		INT32     m_fontsize;
		INT32     m_textcolor;
		INT32     m_dwWidth;
		INT32     m_height;
		INT32		m_IsBold;
		INT32		m_bUnderLine;
		float   m_speed;
		virtual void XDetermine(XmlParserProxy& p)
		{
			XML::determineMember<>(m_font,_T("font"),p);
			XML::determineMember<>(m_fontsize,_T("fontsize"),p);
			XML::determineMember<>(m_textcolor,_T("textcolor"),p);
			XML::determineMember<>(m_border,_T("border"),p);
			XML::determineMember<>(m_IsBold, _T("bold"), p);
			XML::determineMember<>(m_bordercolor,_T("bordercolor"),p);
			//XML::determineMember<>(m_background,_T("background"),p);
			XML::determineMember<>(m_bUnderLine, _T("underline"), p);
			XML::determineMember<>(m_dwWidth,_T("width"),p);
			XML::determineMember<>(m_height,_T("height"),p);
			XML::determineMember<>(m_speed,_T("speed"),p);
		}
	}m_style;

	struct TickerItem : public XmlObject
	{
		TickerItem() : m_delay(0){}
		XString m_command;
		XString m_value;
		XString m_background;
		CBitmap m_bmp;
		float   m_delay;
		SIZE    m_size; // width of string in pixels (precalculated)
		virtual void XDetermine(XmlParserProxy& p)
		{
			XML::determineMember<>(m_delay,_T("delay"),p);
			XML::determineMember<>(m_command,_T("command"),p);
			XML::determineMember<>(m_background,_T("background"),p);
			m_value = p.load();
		}
	};

	XML::vectorDel<TickerItem*> m_items;
	//CImageList m_ItemImgs;
};
