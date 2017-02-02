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

#include "StdAfx.h"
#include "XWnd.h"


LRESULT OnXFreeBarMouseMove(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

BOOL RegisterXFreeWnd() 
{ 
	WNDCLASSEX wcx; 

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW | 
		CS_VREDRAW;                    // redraw if size changes 
	wcx.lpfnWndProc = ::XFreeWndProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = _AtlBaseModule.GetModuleInstance();         // handle to instance 
	wcx.hIcon = LoadIcon(NULL, 
		IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, 
		IDC_ARROW);                    // predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject( 
		WHITE_BRUSH);                  // white background brush 
	wcx.lpszMenuName =  NULL;    // name of menu resource 
	wcx.lpszClassName = XFREE_WND_CLASS_NAME;  // name of window class 
	wcx.hIconSm =  NULL; //LoadIcon(NULL, 
	//IDI_APPLICATION);

	// Register the window class. 

	return RegisterClassEx(&wcx); 
} 

LRESULT CALLBACK XFreeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes;
	switch(uMsg)
	{
	case WM_CREATE:
		//lRes = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		//lRes = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		lRes = OnXFreeBarMouseMove(wParam, lParam);
		break;
	default:
		break;
	}
	lRes = DefWindowProc(hWnd, uMsg, wParam, lParam);
	return lRes;
}

BOOL CreateXFreeWnd(HINSTANCE hInstance, LONG lLeft, LONG lTop, LONG lWidth, LONG lHeight, HWND hParent, LPCTSTR lpszTitle, DWORD dwStyle, DWORD dwExStyle)
{ 
	HWND hWnd; 
	// Create the main window. 

	hWnd = ::CreateWindow( 
		XFREE_WND_CLASS_NAME,        // name of window class 
		lpszTitle,            // title-bar string 
		WS_POPUPWINDOW | /*WS_POPUP | WS_BORDER |*/ dwStyle, // top-level window 
		lLeft, //CW_USEDEFAULT,       // default horizontal position 
		lTop, //CW_USEDEFAULT,       // default vertical position 
		lWidth, //CW_USEDEFAULT,       // default width 
		lHeight, //CW_USEDEFAULT,       // default height 
		(HWND) hParent,         // no owner window 
		(HMENU) NULL,        // use class menu 
		_AtlBaseModule.GetModuleInstance(),           // handle to application instance 
		(LPVOID) NULL);      // no window-creation data 

	if(hWnd == NULL)
	{
		DWORD dwError = ::GetLastError();
		ATLTRACE("Create the xfree windows failed, error code is %d\n", dwError);
		return FALSE; 
	}

	// Show the window and send a WM_PAINT message to the window 
	// procedure. 

	::ShowWindow( hWnd, TRUE ); 
	::UpdateWindow( hWnd ); 
	return TRUE; 
}