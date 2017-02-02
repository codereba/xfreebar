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

#ifndef __CONFIG_HOST_H__
#define __CONFIG_HOST_H__
#define ERR_NOT_IMPL -11

#define COMBOBOX_MIN_WIDTH 30
#define TOOLBAR_RESERVE_SPACE 8
#define COMBOBOX_DROPLINT_HEIGHT 220
#define TOOLBAR_IMAGE_INDEX_BEGIN 6
#define DEFLATE_RECT(rc, dx, dy) rc.left += dx; rc.right -= dx; rc.top += dy; rc.bottom -= dy;
#define HOVED_BTN_BK_CLR RGB( 30,180, 196)
#define PRESSED_BTN_BK_CLR RGB( 90, 20, 185)
#define NORMAL_BTN_BK_CLR RGB(190,180, 195)
#define NORMAL_BTN_FR_CLR RGB(0,0,0)
#define CHEVRON_IMAGE_INDEX 6
#define XFREEBAR_MAX_BTN_COUNT 60
#define FIXED_WIDTH_BTN 0x01
#define FIXED_RIGHT_SIDE_BTN 0x02
#define FIXED_LEFT_SIDE_BTN 0x04
#define XFREEBAR_HILIGHT_BUTTON 0x20;
#define XFREEBAR_CHEVRON_BTN 0x40
#define CHEVRON_BUTTON_COUNT 2
#define XFREEBAR_FILLSPACE_BTN 0x80
#define XFREEBAR_GROUP_FREE 0x01
#define XFREEBAR_GROUP_SIZE 0x02
#define XFREEBAR_GROUP_FIXED_LEFT 0x03
#define XFREEBAR_GROUP_FIXED_RIGHT 0x04
#define XFREEBAR_NO_CHEVRON_LEFT_BTNS 4
#define DRAG_IMAGE_BEGIN_INDEX 3
#define DRAG_IMAGE_END_INDEX 6
#define FIXED_BUTTON_COUNT 5
#define FIXED_BUTTON_BEGIN_INDEX (INT32)( XFREEBAR_MAX_BTN_COUNT - HILIGHT_BUTTON_COUNT - CHEVRON_BUTTON_COUNT)
#define FIXED_BUTTON_INDEX(index) (INT32)(  FIXED_BUTTON_BEGIN_INDEX + index )
#define HILIGHT_BUTTON_COUNT 20
#define HILIGHT_BUTTON_BEGIN_INDEX (INT32)( XFREEBAR_MAX_BTN_COUNT - HILIGHT_BUTTON_COUNT - CHEVRON_BUTTON_COUNT - FIXED_BUTTON_COUNT)
#define HILIGHT_BUTTON_INDEX(index) (INT32)( HILIGHT_BUTTON_BEGIN_INDEX + index )
#define MAP_HILIGHT_BUTTON_INFO_INDEX( index ) (INT32)(HILIGHT_BUTTON_BEGIN_INDEX + index)
#define FILLED_BUTTON_INDEX (INT32)(XFREEBAR_MAX_BTN_COUNT - CHEVRON_BUTTON_COUNT )
#define CHEVRON_BUTTON_INDEX (INT32)(XFREEBAR_MAX_BTN_COUNT - CHEVRON_BUTTON_COUNT + 1)
#define CHEVRON_BUTTON_BEGIN_INDEX FILLED_BUTTON_INDEX

class IConfigHost //__declspec( novtable )
{
public:
	virtual INT32 OnOptionCheckBoxOpen( IHTMLDocument3* pHtmlDoc3 ,XString strSetting, XString strID ) = 0;
	virtual INT32 OnOptionCheckBoxClick( XString strCurID, XString strOrgID, XString strSetting ) = 0;
	virtual INT32 OnOptionRadioOpen( IHTMLDocument3* pHtmlDoc3 ) = 0;
	virtual INT32 OnOptionRadioClick( XString strID ) = 0;
	virtual INT32 OnOptionSelectOpen( IHTMLDocument3 *pHtmlDoc3, XString strID, XString strSetting ) = 0;
	virtual INT32 OnOptionSelectSelChange( XString strID ) = 0;
	virtual INT32 OnOptionButtonClick( XString strID ) = 0;
	virtual INT32 OnCommandSpecialApply( XString strType, XString strName, XString strDisplay ) = 0;
	virtual INT32 OnCommandScriptApply( LPCTSTR lpszBody ) = 0;
	virtual INT32 OnCommandPopClockApply( XString strBlockType ) = 0;
	virtual INT32 OnCommandPopClockPressed() = 0;
	virtual INT32 OnCommandWebJumpApply( XString strURL, XString strPost, XString strNewWindow, XString strOption, XString strTarget ) = 0;
	virtual INT32 OnWebJumpLocalHTMLApply() = 0;
	virtual INT32 OnCommandShellApply( INT32 nType, XString strCmd, XString strCantFound) = 0;
	virtual INT32 OnCommandHilightApply() = 0;
	virtual INT32 OnCommandHilightPressed() = 0;
	virtual INT32 OnToolbarItemPrePaint( LPNMTBCUSTOMDRAW lpTBCustomDraw ) = 0;
	virtual INT32 OnToolbarItemChange( INT32 nIndex ) = 0;
	virtual INT32 OnItemSeparatorChange( INT32 nIndex ) = 0;
	virtual INT32 OnItemSeparatorOptionChange( INT32 nIndex, XString strVisId) = 0;
	virtual INT32 OnDragHandleDragStop( VOID *pParam ) = 0;
	virtual INT32 OnMenuCommandAppendMenu( CMenuHandle &HanldeMenu, XString &strCaption, INT32 nImgIndex, XString strCheckKey, XString strCmd, INT32& nMenuIndex, INT32 nMenuItemIndex ) = 0;
	virtual INT32 OnItemButtonOptionChange( VOID *Param ) = 0;
	virtual INT32 OnItemButtonChange( VOID *pParam ) = 0;
	virtual INT32 OnItemPageOptionChange( VOID *pParam ) = 0;
	virtual INT32 OnItemPageChange( VOID *pParam ) = 0;
	virtual INT32 OnItemComboChange( VOID *pParam ) = 0;
	virtual INT32 OnItemComboOptinoChange( VOID *pParam ) = 0;
	virtual INT32 OnDragHandleOptionChange( VOID *pParam ) = 0;
	virtual INT32 OnPopupMenuAppenMenu( CMenuHandle &HandleMenu, LPCTSTR lpszTitle, CMenuHandle &MenuPopup ) = 0;
	virtual XString GetCommandHighlightColor( INT32 nIndex ) = 0;
	virtual INT32 OnMenuSeparatorAppendMenu( CMenuHandle &HandleMenu ) = 0;
	virtual INT32 OnConfigWebBrowserBeforeNavigate( XString strURL ) = 0;
	virtual INT32 OnSplitterGetRegion( INT32 nID, LPRECT pRect ) = 0;
	virtual INT32 ClearTempMenuItems() = 0;
};

#endif //__CONFIG_HOST_H__