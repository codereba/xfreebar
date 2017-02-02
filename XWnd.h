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

#define XFREE_WND_CLASS_NAME _T("XFREEWND")

BOOL RegisterXFreeWnd();
LRESULT CALLBACK XFreeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CreateXFreeWnd(HINSTANCE hInstance, LONG lLeft, LONG lTop, LONG lWidth, LONG lHeight, HWND hParent = NULL, LPCTSTR lpszTitle = NULL, DWORD dwStyle = NULL, DWORD dwExStyle = NULL);