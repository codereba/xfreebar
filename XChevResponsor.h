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

#ifndef __XCHEVRESPONSOR_H__
#define __XCHEVRESPONSOR_H__
#include "XFreeToolBar.h"
#include "ChevDrop.h"
#include "XResponsor.h"

class XChevResponsor : public XResponsor
{
public:
	XChevResponsor(LPVOID pSource, LPVOID pDest )
	{
		ATLASSERT( NULL != pSource && NULL != pDest );
		m_pXToolBar = (XFreeToolBar*)pSource;
		m_pChevDrop = (CChevDrop*)pDest;
	}

	~XChevResponsor(void)
	{
	}


	INT32 CommandResponse( WORD wID )
	{
		BOOL bRet;
		ATLASSERT( NULL != m_pXToolBar && NULL != m_pChevDrop );
		BOOL bHandled = TRUE;	

		if( wID >= BUTTON_COMMAND_ID_BEGIN && wID <= BUTTON_COMMAND_ID_END )
		{
		m_pXToolBar->OnButton( 0, wID, m_pXToolBar->m_hWnd, bHandled );
		}
		else if( wID >= MENU_COMMAND_ID_BEGIN && wID <= MENU_COMMAND_ID_END )
		{
			m_pXToolBar->OnCommand( 0, wID, m_pXToolBar->m_hWnd, bHandled );
		}

		bRet = ( BOOL )::SendMessage(m_pXToolBar->m_hWnd, TB_ISBUTTONCHECKED, wID, 0L);

		::SendMessage(m_pChevDrop->GetToolBar()->m_hWnd, TB_CHECKBUTTON, wID, MAKELPARAM(bRet, 0));

		if( FALSE == bRet )
			return XERROR_COMMON;
		return 0;
	}
protected:
	XFreeToolBar *m_pXToolBar;
	CChevDrop *m_pChevDrop;
};

#endif