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

#ifndef __GLOBAL_DEF_H__
#define __GLOBAL_DEF_H__

#include "GlobalString.h"

#define MAX_BUTTON_COUNT											1024
#define MAX_MENU_SIZE												1450
#define MAX_ALL_COMMAND_COUNT										( INT32 )( 26 * MAX_MENU_SIZE )
#define BUTTON_COMMAND_ID_BEGIN										WM_USER + 0
#define BUTTON_COMMAND_ID_END										WM_USER + MAX_BUTTON_COUNT
#define MENU_COMMAND_ID_BEGIN										( INT32 )( WM_USER + MAX_BUTTON_COUNT )
#define MENU_COMMAND_ID_END											( INT32 )( WM_USER + MAX_ALL_COMMAND_COUNT )
#define ALL_COMMAND_BEGIN											BUTTON_COMMAND_ID_BEGIN
#define ALL_COMMAND_END												BUTTON_COMMAND_ID_BEGIN + MAX_BUTTON_COUNT
#define TB_HEIGHT													24
#define COMBOBOX_HEIGHT												22 

#endif