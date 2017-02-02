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
#include "CmnCtrl.h"
#include "ConfigHost.h"
#include "XmlParserImpl.h"
using namespace XML;

#define XTYPE_BUTTON _T("XButton")
#define XTYPE_RADIO _T("XRadio")
#define XTYPE_SETTINGS _T("XSettings")
#define XTYPE_COMMAND _T("XCommand")
#define XTYPE_SPECIAL _T("XSpecial")
#define XTYPE_SCRIPT _T("XScript")
#define XTYPE_POPBLOCK _T("XPopBlock")
#define XTYPE_WEBJUMP _T("XWebJump")
#define XTYPE_SHELLEXECUTE _T("XShellExecute")
#define XTYPE_COMMANDS _T("XCommands")
#define XTYPE_FONT _T("XFont")
#define XTYPE_HIGHLIGHT _T("XHighlight")
#define XTYPE_PAGE _T("XPage")
#define XTYPE_COMBO _T("XCombo")
#define XTYPE_SEPARATOR _T("XSeparator")
#define XTYPE_SPLITTER _T( "XSplitter" )

struct XToolBarConfig;

// fonst used in toolbar
struct _Font : public XmlObject
{
	_Font()
	{
	}

	INT32 m_FontNum;
	XString m_strName;

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XML::determineMember<>( m_FontNum, _T( "size" ), parser );
		XML::determineMember<>( m_strName, _T( "name" ), parser );
	}
};

// colors for HightLighting
struct Highlight : public XmlObject
{
	Highlight()
	{
	}

	vector< XString > m_colors;

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XML::determineMember< XString >( m_colors, _T("COLOR"), parser );
	}
};

//store setting to show popup messages
struct Popup : public XmlObject
{
	Popup() : m_time(0) 
	{
	}

	XString m_show;
	XString m_strUrl;
	INT32 m_dwWidth;
	INT32 m_height;
	INT32 m_time;

	virtual void XDetermine(XmlParserProxy& parser)
	{
		XML::determineMember<XString>(m_strUrl, _T("url"), parser);
		XML::determineMember<XString>(m_height, _T("height"), parser);
		XML::determineMember<XString>(m_dwWidth, _T("width"), parser);
		XML::determineMember<XString>(m_show, _T("show"), parser);
		XML::determineMember<XString>(m_time, _T("time"), parser);
	}
};

// Setting class - parent for all objects that can be used in Options page
struct Setting : public XmlObject
{
	XString  m_strID;
	XString  m_strDefault;

	void Init()
	{
		setValue( m_strID, GetSetting() ); 
	}

	XString GetSetting()
	{
		return getValue( m_strID, m_strDefault.size() ? m_strDefault : _T( "1" ) );
	}

	virtual void OnOpen( IConfigHost* pConfigHost, IHTMLDocument3* pHtmlDoc3 ) = 0;
	virtual void OnSelChange( IHTMLElementPtr parser, XString id ) { };
	virtual void OnClick( IConfigHost* pConfigHost, XString id ) { };

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XML::determineMember<>( m_strID, _T("id"), parser );
		XML::determineMember<>( m_strDefault, _T("default"), parser );
	}
};


// check box in options page, also can set some default parameters for toolbar 
struct CheckBox : public Setting
{
	virtual void OnOpen( IConfigHost* pConfigHost, IHTMLDocument3* pHtmlDoc3 )
	{
		pConfigHost->OnOptionCheckBoxOpen( pHtmlDoc3, GetSetting(), m_strID );
	}

	virtual void OnClick(IConfigHost* pConfigHost, XString strID )
	{
		pConfigHost->OnOptionCheckBoxClick( strID , m_strID, GetSetting() );
	}
};

#ifndef NO_OPTIONS_PAGE
// we can put some buttons in HTML page and assign some command to it

struct Button : public Setting
{
	XString m_strCmd;
	virtual void OnOpen( IConfigHost* pConfigHost, IHTMLDocument3* pHtmlDoc3 ) {}
	
	virtual void OnClick( IConfigHost* pConfigHost, XString strID )
	{
		pConfigHost->OnOptionButtonClick( strID );
	}

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XML::determineMember<>( m_strCmd, _T("command"), parser );
		Setting::XDetermine( parser );
	}
};

// radio control on option page
struct Radio : public Setting
{
	XString m_strName;
	vector< XString >  m_vecIDS;
	virtual void OnOpen( IConfigHost *pConfigHost, IHTMLDocument3* pHtmlDoc3 )
	{
		IHTMLElementPtr pelement;
		for( UINT32 i = 0; i < m_vecIDS.size(); i++ )
		{
			pHtmlDoc3->getElementById( _bstr_t( m_vecIDS[ i ].c_str() ), &pelement );
			if( pelement != NULL && getValue( m_strName, m_strDefault ) == XString( IntToStr( i ) ) )
				pelement->click();
		}
	}

	virtual void OnClick( IConfigHost* pConfigHost, XString strID )
	{
		for( UINT32 i = 0; i < m_vecIDS.size(); i++ )
		{
			if( m_vecIDS[ i ] == strID )
			{
				setValue( m_strName, XString( IntToStr(i) ) );
			}
		}
	}

	virtual void XDetermine(XmlParserProxy& parser)
	{
		XML::determineMember<XString>( m_vecIDS, _T("id"), parser );
		XML::determineMember<>( m_strName, _T("name"), parser );
		Setting::XDetermine( parser );
	}
};

// combo box control in options page - used only to seleect default search engine
struct Select : public Setting
{
	virtual void OnOpen( IConfigHost* pConfigHost, IHTMLDocument3* pHtmlDoc3 )
	{
		pConfigHost->OnOptionSelectOpen( pHtmlDoc3, m_strID, GetSetting() );
	}

	virtual void OnSelChange( IConfigHost* pConfigHost, IHTMLElementPtr pelement, XString strID )
	{
		LONG lIndex;

		IHTMLSelectElementPtr pselelement = pelement;
		pselelement->get_selectedIndex( &lIndex );

		TCHAR szTitle[] = _T("0");
		szTitle[ 0 ] += static_cast< TCHAR >( lIndex );
		setValue( m_strID, szTitle );
	}
};
#endif

// Settings class - collection of settings that appear in Options page
struct Settings : public XmlObject
{
	Settings() {}
	XString  m_updateVer;
	XString  m_wordfindHint;

	XML::vectorDel<_Font*>  m_fonts;

	Highlight m_highlight;

	XString m_popuponload;

	Popup m_popup;

	void Init()
	{
		for( INT32 i = 0; ( UINT32 )i < m_Settings.size(); i++ )
			m_Settings[ i ]->Init();
	}

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XML::determineMember<>( m_updateVer, _T("updateVer"), parser );
		XML::determineMember<>( m_wordfindHint, _T("WordFindHint"), parser );
		XML::determineMember<_Font>( m_fonts, _T("FONT"), parser );
		XML::determineMember<>( m_highlight, _T("HIGHLIGHT"), parser );
		XML::determineMember<>( m_popuponload, _T("POPUPONLOAD"), parser );
		XML::determineMember<>( m_popup, _T("POPUP"), parser );
		XML::determineMember<CheckBox>( m_Settings, _T("CHECKBOX"), parser );
#ifndef NO_OPTIONS_PAGE  
		XML::determineMember<Radio>( m_Settings, _T("RADIO"), parser );
		XML::determineMember<Select>( m_Settings, _T("SELECT"), parser );
		XML::determineMember<Button>( m_Settings, _T("BUTTON"), parser );
#endif
	}
	XML::vectorDel< Setting* > m_Settings;
};

// parent for all menu items in toolbar's menus
struct IMenuItem  : public XmlObject
{
	XString m_strHint;
	XString m_strCmd;
	XString m_strVisId; // id of visiable "group"
	virtual void AppendMenuItem( IConfigHost* pConfigHost, CMenuHandle, INT32&, INT32 ) = 0;   // all this popup menu (if any)
	virtual IMenuItem* GetMenuItem(INT32& items,INT32 c) = 0;     // get child menu item by position
	bool IsShow()
	{
		return m_strVisId.empty() || isGetValue( m_strVisId );
	}

	XString GetHintMsg()
	{
		return m_strHint;
	}
};

// parent of all command classes
struct Command : public XmlObject
{
	XString m_strName;
	virtual bool check() {return true;}              // load (and check) additional files (if any)
	// virtual void setHost(ToobarItem *pItem){};
	virtual bool canAddToSelect() {return false;}    // is it search "engine command"?
	virtual void apply(IConfigHost* ) = 0;      // execute this command
	virtual void applyPresed(IConfigHost* pConfigHost) {} // execute this command after button (hightlight) was pressed
	virtual void XDetermine(XmlParserProxy& parser)
	{
		XML::determineMember<>(m_strName,_T("name"),parser);
	}
};

// all "speacial" types of Commans (uninstall, highlight, etc.)
struct Special : public Command
{
	XString m_strType;
	XString m_strDisplay;

	virtual void XDetermine(XmlParserProxy& parser)
	{
		Command::XDetermine(parser);
		XML::determineMember<>(m_strType,_T("type"),parser);
		XML::determineMember<>(m_strDisplay,_T("display"),parser);
	}

	virtual void apply(IConfigHost* pConfigHost)
	{
		pConfigHost->OnCommandSpecialApply( m_strType , m_strName, m_strDisplay );
	}
};

// execute java script on current page
struct Script : public Command
{
	XString m_strBody;
	virtual void apply(IConfigHost* pConfigHost)
	{
		pConfigHost->OnCommandScriptApply( m_strBody.c_str() );
	}
	virtual void XDetermine(XmlParserProxy& parser)
	{
		Command::XDetermine(parser);
		XML::determineMember<>(m_strBody,_T("body"),parser);
	}
};

struct PopBlock : public Command
{
	XString m_strBlockType;

	PopBlock(){};

	virtual void apply(IConfigHost* pConfigHost)
	{
		pConfigHost->OnCommandPopClockApply( m_strBlockType );
	}

	virtual void applyPresed(IConfigHost* pConfigHost)
	{
		pConfigHost->OnCommandPopClockPressed();
	}
	virtual void XDetermine(XmlParserProxy& parser)
	{
		Command::XDetermine(parser);
		//XML::determineMember<>(m_bBlock,_T("IsBlock"),parser);
		//XML::determineMember<>(m_bSound,_T("IsSound"),parser);
		//XML::determineMember<>(m_bClean,_T("IsClean"),parser);

		XML::determineMember<>(m_strBlockType,_T("BlockOption"),parser);
	}
};

// open web URL in browser
struct WebJump : public Command
{
	XString m_strHttpRef;
	XString m_strNewBrowserWindow;
	XString m_strPost;
	XString m_strTarget;
	XString m_strOption;
	XString m_strAddToSel;

	WebJump() : m_strTarget( _T( "" ) )  //"_TOP"
	{
	}

	// I don't know better solution now
	virtual bool canAddToSelect()
	{
		return m_strAddToSel == _T("yes");
	}

	virtual void apply(IConfigHost* pConfigHost)
	{ 
		do_apply( pConfigHost, m_strHttpRef , m_strNewBrowserWindow);
	}

	void do_apply( IConfigHost* pConfigHost, XString strHttpRef, XString strNewWindow )
	{
		pConfigHost->OnCommandWebJumpApply( strHttpRef, m_strPost, strNewWindow, m_strOption, m_strTarget );
	}

	virtual void XDetermine( XmlParserProxy& parser )
	{
		Command::XDetermine( parser );
		XML::determineMember<>( m_strHttpRef, _T("href"), parser);
		XML::determineMember<>( m_strPost, _T("post"), parser );
		XML::determineMember<>( m_strTarget ,_T("target"), parser );
		XML::determineMember<>( m_strOption, _T("options"), parser );
		XML::determineMember<>( m_strNewBrowserWindow, _T("newbrowserwindow"), parser );
		XML::determineMember<>( m_strAddToSel, _T("addtoselect"), parser );
	}
};


//INT32 OnWebJumpLocalHTMLCheck()
//{		
//
//}
// open previous loaded HTML page
struct LocalHTML : public WebJump
{
	XString m_ref_path;

	virtual bool check()
	{
		m_ref_path = m_strHttpRef;
		if( m_strNewBrowserWindow == _T( "toolbarwindow" ) )
			return true;
		return CheckUpdate( m_ref_path ) != 0;
	}
	virtual void apply( IConfigHost* pConfigHost )
	{
		if( check() )
			do_apply( pConfigHost, m_ref_path , m_strNewBrowserWindow);
	}
};

#define CMD_SUPER 1
#define CMD_COMMON 0

// execute some local executable or some shell command (create email)
struct _ShellExecute : public Command
{
	XString m_strCmd;
	XString m_strCantFound;
	INT32 m_nType;

	virtual void apply( IConfigHost* pConfigHost )
	{
		pConfigHost->OnCommandShellApply( m_nType, m_strCmd, m_strCantFound );
	}

	virtual void XDetermine( XmlParserProxy& parser )
	{
		Command::XDetermine( parser );
		XML::determineMember<>( m_strCmd, _T( "command" ), parser);
		XML::determineMember<>( m_strCantFound, _T( "not_found" ), parser );
		XML::determineMember< INT32 >( m_nType, _T("type"), parser );
	}
};

#ifndef NO_HIGHTLIGHT
// process ala Google Highlight
struct _Highlight : public Command
{
	virtual void applyPresed(IConfigHost* pConfigHost)
	{
		pConfigHost->OnCommandHilightPressed();
	}
	virtual void apply(IConfigHost* pConfigHost)
	{
		pConfigHost->OnCommandHilightApply();
	}

	_bstr_t getColor( IConfigHost* pConfigHost, INT32 nIndex )
	{
		pConfigHost->GetCommandHighlightColor( nIndex );
	}
};
#endif

// vector of all commands that can our toolbar execute (may be make it a map?)
struct Commands : public XmlObject
{
	virtual void XDetermine(XmlParserProxy& parser)
	{
		XML::determineMember<WebJump>(m_commands,_T("WEBJUMP"),parser);
		XML::determineMember<PopBlock>(m_commands,_T("POPBLOCK"),parser);
		XML::determineMember<Script>(m_commands,_T("SCRIPT"),parser);
		XML::determineMember<Special>(m_commands,_T("SPECIAL"),parser);
		XML::determineMember<LocalHTML>(m_commands,_T("LOCALHTML"),parser);
		XML::determineMember<_ShellExecute>(m_commands,_T("SHELLEXECUTE"),parser);
#if !defined(NO_HIGHTLIGHT)
		XML::determineMember<_Highlight>(m_commands,_T("HIGHLIGHT"),parser);
#endif
	}
	XML::vectorDel<Command*> m_commands;
};

// parent of all Toolbar's controls
struct XToolbarItem : public XmlObject
{
	XToolbarItem() : m_nID( -1 ), 
					m_nConfIndex( -1 ),
					m_Command( 0 ), 
					m_dwStyle( 0 ), 
					m_strType( _T("") ), 
					m_strHintMsg( _T("") )
	{
	}

	INT32 m_nID;
	INT32 m_nConfIndex;
	Command* m_Command;
	XString m_strHint;
	XString m_strHintMsg;
	XString m_strCaption;
	XString m_strCmd;
	XString m_strVisId; // id of visiable "group"
	XString m_strType;
	INT32	m_dwStyle;
	XString& GetBtnConfCaption()
	{
		return m_strCaption;
	}

	XString& GetHintMsg()
	{
		return m_strHintMsg;
	}

	virtual void OnXFreeBarCommand( IConfigHost* ) {}   // button pressed
	virtual void Invalidate(IConfigHost* ) {}  // invalidate owner-drawn controls

	XString GetToolbarItemType()
	{
		return m_strType;
	}

	INT32 GetBtnCmdId()
	{
		return m_nID;
	}

	VOID SetBtnCmdId( INT32 nID )
	{
		m_nID = nID;
	}

	VOID SetBtnConfIndex( INT32 nIndex )
	{
		m_nConfIndex = nIndex;
	}

	INT32 GetBtnConfIndex()
	{
		return m_nConfIndex;
	}

	virtual DWORD OnItemPrePaint( IConfigHost* pConfigHost, LPNMTBCUSTOMDRAW lpTBCustomDraw )
	{
		pConfigHost->OnToolbarItemPrePaint( lpTBCustomDraw );
		return 0;
	}

	virtual bool isShownAlways() { return false; }  // used to calculate min width of toolbar

	virtual IMenuItem* GetMenuItem( INT32 nIndex ) { return 0; } // get menu item (if any) by position

	virtual BOOL StopMouseEvents( IConfigHost * pConfigHost = NULL )
	{
		return FALSE;
	} // used to disalbe mouse input for TEXT control

	image_index_t GetConfImageIndex(){ return m_strImage; }         // working with icons
	void SetConfImageIndex( image_index_t strImg ){ m_strImage = strImg; }

	virtual bool check() {return true;}           // load (and check) additional files (if any)
	virtual void OnOptionChange(IConfigHost* pConfigHost) {}; // called after option page changed
	// show Popup menu
	virtual void ShowMenu(IConfigHost* pConfigHost, HWND wnd,INT32 x, INT32 y) {ATLASSERT(false);} 
	// called during building toolbar to apply some custom control's attributes
	virtual void XChange(IConfigHost* pConfigHost) 
	{
		pConfigHost->OnToolbarItemChange( m_nConfIndex );
	}

	VOID UpdateHintMsg( LPCTSTR lpszAppend = NULL )
	{
		if( NULL == lpszAppend )
			return;

		//XString strHinMsg;
		m_strHintMsg = m_strHint;
		m_strHintMsg += lpszAppend;
		return;
	}

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XML::determineMember<>( m_strHint, _T("hint"), parser );
		XML::determineMember<>( m_strCaption, _T( "caption" ), parser );
		XML::determineMember<>( m_strImage, _T( "img" ), parser );
		XML::determineMember<>( m_strCmd, _T( "command" ), parser );
		XML::determineMember<>( m_strVisId, _T("vis_id"), parser );
		XML::determineMember< INT32 >( m_dwStyle, _T("style"), parser );
		m_strHintMsg = m_strHint;
	}

protected:
	image_index_t  m_strImage;
};

// Separator control, need to say more? :)
struct Separator  : public XToolbarItem
{
	Separator()
	{
		m_strType = XTYPE_SEPARATOR;
	}

	virtual void XChange(IConfigHost* pConfigHost) 
	{
		XToolbarItem::XChange( pConfigHost );
		pConfigHost->OnItemSeparatorChange( m_nConfIndex );
	}

	virtual void OnOptionChange(IConfigHost* pConfigHost)
	{
		pConfigHost->OnItemSeparatorOptionChange( m_nConfIndex, m_strVisId );
	}
};

struct MenuItem  : public IMenuItem
{
	XString m_strCheckKey;
	virtual IMenuItem* GetMenuItem( INT32& items, INT32 c ) { return 0; }
	virtual void XDetermine( XmlParserProxy& parser )
	{
		XML::determineMember<>( m_strHint, _T("hint"), parser );
		XML::determineMember<>( m_strCmd, _T("command"), parser );
		XML::determineMember<>( m_strVisId, _T("vis_id"), parser );
		XML::determineMember<>( m_strCheckKey, _T("checkkey"), parser );
	}
};

// struct to contain child items
struct IMenu
{
	XML::vectorDel< MenuItem* >   m_vecMenuItems;
};

// separator in menu - very usefull thing :)
struct _Separator  : public MenuItem
{
	virtual void XDetermine(XmlParserProxy& parser)  {}
	virtual void AppendMenuItem( IConfigHost* pConfigHost, CMenuHandle HandleMenu, INT32&, INT32 )
	{
		pConfigHost->OnMenuSeparatorAppendMenu( HandleMenu );
	}
};

// "normal" menu item = icon + text + command to execute
struct _Command  : public MenuItem
{
	image_index_t GetConfImageIndex(){return m_strImage;}
	void SetConfImageIndex(image_index_t img){m_strImage = img;}

	XString m_strCaption;


	virtual void AppendMenuItem(IConfigHost* pConfigHost, CMenuHandle HanldeMenu, INT32 &nMenuItemIndex, INT32 nMenuIndex )
	{
		pConfigHost->OnMenuCommandAppendMenu( HanldeMenu, m_strCaption, atoi(m_strImage.c_str()), m_strCheckKey, m_strCmd, nMenuIndex, nMenuItemIndex);
	}

	virtual void XDetermine(XmlParserProxy& parser)
	{
		MenuItem::XDetermine(parser);
		XML::determineMember<>(m_strImage,_T("img"),parser);
		XML::determineMember<>(m_strCaption,_T("caption"),parser);
	}
private:
	image_index_t  m_strImage;
	DWORD		m_dwStyle;
};

// we need more levels, more menus
struct PopupMenu  : public MenuItem, public IMenu
{
	XString m_strCaption;
	virtual void XDetermine(XmlParserProxy& parser)
	{
		MenuItem::XDetermine(parser);
		XML::determineMember<>(m_strCaption,_T("caption"),parser);
		XML::determineMember<_Command>(m_vecMenuItems,_T("ITEM"),parser);
		XML::determineMember<_Separator>(m_vecMenuItems,_T("SEPARATOR"),parser);
		XML::determineMember<PopupMenu>(m_vecMenuItems,_T("MENU"),parser);
	}

	virtual void AppendMenuItem( IConfigHost* pConfigHost, CMenuHandle HandleMenu, INT32& items, INT32 id )
	{
		CMenuHandle MenuPopup;
		MenuPopup.CreatePopupMenu();

		for( UINT32 i = 0; i < m_vecMenuItems.size(); i++ )
		{
			if( m_vecMenuItems[ i ]->IsShow() )
			{
				m_vecMenuItems[ i ]->AppendMenuItem( pConfigHost, MenuPopup, items, id ); /// pass right values
				items++;
			}
		}

#ifdef USE_SIMPLE_MENU
		handle.AppendMenu(MF_POPUP,(UINT32)(HMENU)MenuPopup,m_strCaption.c_str());
#else
		pConfigHost->OnPopupMenuAppenMenu( HandleMenu, m_strCaption.c_str(), MenuPopup );
#endif
	}

	IMenuItem* GetMenuItem(INT32& items, INT32 c)
	{
		for(UINT32 j=0;j<m_vecMenuItems.size();j++)
		{
			if(IMenuItem* s = m_vecMenuItems[j]->GetMenuItem( items, c ) )
				return s;
			if(items==c)
				return m_vecMenuItems[j];
			items++;
		}
		return 0;
	}
};

// Button's menu
struct Menu : public XmlObject, public IMenu
{
	~Menu()
	{
		if(m_SubToolBar.m_hWnd)
			m_SubToolBar.DestroyWindow();
		m_SubToolBar.m_hWnd = NULL;
	}

	IMenuItem* GetMenuItem( INT32 c )
	{
		INT32 items = 0;
		for( UINT32 j = 0; j < m_vecMenuItems.size(); j++ )
		{
			if( IMenuItem* s = m_vecMenuItems[ j ]->GetMenuItem( items, c ) )
				return s;
			if( items == c )
				return m_vecMenuItems[ j ];
			items++;
		}
		return 0;
	}

	virtual void XDetermine(XmlParserProxy& parser)
	{
		XML::determineMember<_Command>(m_vecMenuItems,_T("ITEM"),parser);
		XML::determineMember<_Separator>(m_vecMenuItems,_T("SEPARATOR"),parser);
		XML::determineMember<PopupMenu>(m_vecMenuItems,_T("MENU"),parser);
		XML::determineMember<>(m_strType,_T("type"),parser);
		XML::determineMember<>(m_strImage,_T("img"),parser);
		XML::determineMember<>(m_strHotImage,_T("hot_img"),parser);
	}

	XString getHotBitmap()
	{
		return m_strHotImage;
	}

	XFreeSubToolBar m_SubToolBar;
	XString m_strType;
	XString m_strImage;
	XString m_strHotImage;
};

//  Button control
struct _Button : public XToolbarItem
{
	_Button()
	{
		m_strHotImage = _T("");
		m_strType = XTYPE_BUTTON;
	}

	~_Button() {}

	virtual IMenuItem* GetMenuItem( INT32 nIndex )
	{
		return m_DropMenu.GetMenuItem( nIndex );
	}

	Menu m_DropMenu;

	virtual void ShowMenu( IConfigHost* pConfigHost, HWND hWnd, INT32 x, INT32 y)
	{
		CMenu MenuPopup;
		MenuPopup.CreatePopupMenu();
		INT32 nMenuItemCount = 0;
		for( UINT32 i = 0; i < m_DropMenu.m_vecMenuItems.size(); i++ )
		{
			if( m_DropMenu.m_vecMenuItems[ i ]->IsShow() )
			{
				m_DropMenu.m_vecMenuItems[ i ]->AppendMenuItem( pConfigHost, ( CMenuHandle )MenuPopup, nMenuItemCount, m_nConfIndex );
				nMenuItemCount++;
			}
		}

		//ATLTRACE( "Menu Item Count Is: %d\r\n", nMenuItemCount );
		//INT32 nRet;
		//for( INT32 i = 0; i < 5; i ++ )
		//{
		//	//_XFreeMenuItemData* pMI = new _XFreeMenuItemData();
		//	//pMI->fType = MF_STRING;
		//	//pMI->lpstrText = "HELLO";
		//	//pMI->iButton = GetToolBarItemBmp( i );//GetImageId(GetConfImageIndex());
		//	//if( pMI->iButton >= 0 )
		//	//{
		//	//	pMI->iButton += TOOLBAR_IMAGE_INDEX_BEGIN;
		//	//}

		//	//nRet = MenuPopup.AppendMenu( MF_STRING, MENU_COMMAND_ID_BEGIN + MAX_MENU_SIZE  + i, (LPCTSTR)"HELLO" );

		//}
		::TrackPopupMenu( MenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hWnd, NULL );
		pConfigHost->ClearTempMenuItems();
	}

	virtual void OnOptionChange( IConfigHost* pConfigHost )
	{
		pConfigHost->OnItemButtonOptionChange( ( VOID* ) this );
	}

	virtual bool check()
	{
		if( m_DropMenu.m_strImage.empty() )
			return true;

		XString menuimg = m_DropMenu.m_strImage;
		XString hotimage = m_DropMenu.m_strHotImage;
#ifdef _ODBC_DIRECT
		return true;
#endif
		if( m_strHotImage.size() != 0 && !CheckUpdate( m_strHotImage ) )
			return false;

		return CheckUpdate( menuimg ) && (hotimage.size() == 0 || CheckUpdate( hotimage ) );
	}

	virtual void XChange(IConfigHost* pConfigHost)
	{
		XToolbarItem::XChange( pConfigHost );
		pConfigHost->OnItemButtonChange( this );
	}

	XString m_strCheckKey;
	image_index_t m_strHotImage;
	
	image_index_t getHotBitmap()
	{
		return m_strHotImage;
	}

	virtual void XDetermine(XmlParserProxy& parser)
	{
		XToolbarItem::XDetermine(parser);
		XML::determineMember<>(m_DropMenu,_T("MENU"),parser);
		XML::determineMember<>(m_strCheckKey, _T("checkkey"),parser);
		XML::determineMember<>(m_strHotImage, _T("hot_img"),parser);
	}
}; // end of Button class

// Text = Button - mouse events
struct Text : public _Button
{
	virtual BOOL StopMouseEvents() {return TRUE;}
};

// WebBrowser control wrapper
class CPage :
	public IDispEventImpl<1, CPage, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>,
	public CWindowImpl<CPage,CAxWindow>

{
	DWORD & m_dwWidth;
public:
	CPage( DWORD & dwWidth ) : m_dwWidth( dwWidth ),m_wasComplete(false) {}

	DECLARE_WND_CLASS(_T("MSHTML_Toolbar_Page"))

	DECLARE_EMPTY_MSG_MAP()
#define DISPID_DOCUMENTCOMPLETE     259   // new document goes ReadyState_Complete
#define DISPID_BEFORENAVIGATE2      250   // hyperlink clicked on
	BEGIN_SINK_MAP(CPage)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE , OnDocumentComplete) //, VTS_DISPATCH VTS_PVARIANT)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2 , OnBeforeNavigate2) //, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	END_SINK_MAP()

	STDMETHOD( OnBeforeNavigate2 )(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel)
	{
		_bstr_t url = URL->bstrVal;
		if(m_wasComplete)
		{
			*Cancel = 1;
		}
		return S_OK;
	}

	STDMETHOD( OnDocumentComplete ) ( IDispatch *pDisp, VARIANT *URL)
	{
		IWebBrowserPtr web = pDisp;
		LPDISPATCH pDispatch;
		web->get_Document(&pDispatch);
		if(pDispatch)
		{
			IHTMLDocument2Ptr lpHtmlDocument = pDispatch;
			IHTMLElement *lpBodyElm = 0;
			lpHtmlDocument->get_body(&lpBodyElm);
			if(lpBodyElm)
			{
				IHTMLElement2Ptr lpBody = lpBodyElm;
				lpBody->get_scrollWidth((LONG*)&m_dwWidth);
				m_wasComplete = true;
				//pConfigHost->OnOptionChange();
			}
		}
		return S_OK;
	}
	IWebBrowser2Ptr         m_browser;
	IConfigHost*       pConfigHost;
	bool                    m_wasComplete;
};

// we would like to insert some IE's into our toolbar
struct Page : public XToolbarItem
{
	Page() : m_dwWidth( 90 ),
		m_Page( m_dwWidth )
	{
		m_strType = XTYPE_PAGE;
	}

	~Page(){if(m_Page.m_hWnd)m_Page.DestroyWindow();}

	virtual void OnOptionChange(IConfigHost* pConfigHost)
	{
		pConfigHost->OnItemPageOptionChange( (VOID*)this );
	}

	virtual void XChange(IConfigHost* pConfigHost)
	{
		XToolbarItem::XChange( pConfigHost );
		pConfigHost->OnItemPageChange( (VOID*)this );
	}

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XToolbarItem::XDetermine( parser );
		XML::determineMember<>( m_strUrl, _T("url"), parser );
	}

	DWORD m_dwWidth;
	XString m_strUrl;
	CPage m_Page;
}; // end of Page class

struct IXEditCtrl;
// Combo (Edit) box
struct Combo : public XToolbarItem
{
	Combo() : m_pEdit( NULL )
	{
		m_strType = XTYPE_COMBO;
	}

	~Combo();

	virtual bool isShownAlways() {return true;}
	virtual void OnOptionChange(IConfigHost* pConfigHost)
	{
		pConfigHost->OnItemComboOptinoChange( (VOID*)this );
	}

	virtual void XChange( IConfigHost* pConfigHost )
	{
		XToolbarItem::XChange( pConfigHost ); 
		pConfigHost->OnItemComboChange( (VOID*)this );
	}

	virtual void XDetermine(XmlParserProxy& parser)
	{
		XToolbarItem::XDetermine(parser);
		XML::determineMember<>(m_strName,_T("name"),parser);
		XML::determineMember<>(m_nLimit,_T("limit"),parser);
		XML::determineMember<>(m_histmax,_T("histmax"),parser);
		XML::determineMember<INT32>(m_widths,_T("WIDTH"),parser);
		XML::determineMember<Value>(m_values,_T("VALUE"),parser);
	}

public:
	IXEditCtrl* m_pEdit;

	//RECT            m_rc;
	INT32             m_nLimit;
	vector<INT32>     m_widths;
	XString         m_strName;
	INT32             m_style;
	INT32             m_histmax;

	struct Value : public XmlObject
	{
		XString m_strDisplay;
		XString m_value;
		virtual void XDetermine(XmlParserProxy& parser)
		{
			XML::determineMember<>(m_strDisplay,_T("display"),parser);
			m_value = parser.load();
		}
	};
	XML::vectorDel<Value*> m_values;
};

#include "XComboBox.h" // here is CComboBox (CEdit) hidden

struct Splitter : public Separator
{
	XString strEditName;
	//DWORD m_oldx;
	BOOL m_bFirstChange;

#define SPLITTER_WIDTH 11

	IXEditCtrl* pDraggingEdit;

	Splitter() :pDraggingEdit(0), m_bFirstChange(true)//, m_oldx(0)
	{
		m_strType = XTYPE_SPLITTER;
	}

	BOOL GetSplitterRegion( IConfigHost *pConfigHost, LPRECT lpRect )
	{
		return pConfigHost->OnSplitterGetRegion( m_nConfIndex, lpRect );
	}

	virtual void OnOptionChange( IConfigHost *pConfigHost )
	{
		Separator::OnOptionChange( pConfigHost );
		pConfigHost->OnDragHandleOptionChange( ( VOID* )this );
	}

#define BLANK_WIDTH SPLITTER_WIDTH
#define MIN_COMBO_WIDTH		30
	virtual BOOL StopMouseEvents( IConfigHost *pConfigHost ) 
	{
		static bool bFirst = true;
		if( bFirst )
		{
			bFirst = false;
			return TRUE;
		}

		if(!pDraggingEdit)
			return FALSE;

		return pConfigHost->OnDragHandleDragStop( (VOID*)this );
	}

	VOID SetDraggingEdit( IXEditCtrl *pEdit )
	{
		pDraggingEdit = pEdit;
	}

	IXEditCtrl *GetDraggingEdit()
	{
		return pDraggingEdit;
	}

	virtual void XDetermine( XmlParserProxy& parser )
	{
		XToolbarItem::XDetermine( parser );
		XML::determineMember<>( strEditName, _T( "editname" ), parser );
	}
};

typedef vector< IXEditCtrl* > BandEdits;
struct IBandEdits // iterator helper
{
	IBandEdits(INT32 nEditNum, BandEdits::const_iterator Iter ) : m_nEditNum( nEditNum ),
												m_Iter( Iter ),
												m_nIndex(0)
	{
	}

	operator bool()
	{
		return m_nIndex < m_nEditNum && m_nIndex >= 0;
	}
	
	void operator ++(INT32) 
	{ 
		m_nIndex ++; 
		m_Iter ++;
	}
	
	void operator --(INT32)
	{
		m_nIndex--;
		m_Iter--;
	}
	operator INT32()
	{ 
		return m_nIndex + 1;
	}

	operator IXEditCtrl*() 
	{ 
		return ( *m_Iter );
	}

	void operator = ( IXEditCtrl* pEdit ) 
	{ 
		pEdit = *m_Iter;
	}

	IXEditCtrl* operator ->() 
	{ 
		return ( *m_Iter );
	}

private:
	BandEdits::const_iterator m_Iter;
	INT32                       m_nEditNum;
	INT32                       m_nIndex;
};

struct XToolBarConfig : public XmlObject
{
	~XToolBarConfig() {}
	Settings m_Settings;
	Commands m_commands;

	// here is XTicker struct - colorfull XTicker to show good news 
//#ifdef USE_TICKER
//#include "TickerControl.inl"
//#endif

	XString                      m_bmpFileName;
	XString                      m_bmpHotFileName;
	XString							m_bmpDisbFileName;
	vector<XString>              m_include_xmls;
	XML::vectorDel<XToolbarItem*> m_BtnConfs;

	// sometimes to want to access some buttons "directly" so here are 2 find helpers
	INT32 MapBtnConfIndexFromCmd(const XString& strCmd )
	{
		for( INT32 i = 0; (size_t)i < m_BtnConfs.size(); i++ )
			if(m_BtnConfs[i]->m_strCmd == strCmd )
				return i;
		return -1;
	}

	XToolbarItem* MapBtnConfFromCmd(const XString& str)
	{
		INT32 nIndex = MapBtnConfIndexFromCmd( str );
		if( nIndex >= 0 )
			return m_BtnConfs[ nIndex ];
		return 0;
	}

	XToolbarItem* MapBtnConfFromID( INT32 nID )
	{
		for( INT32 i = 0; (size_t)i< m_BtnConfs.size(); i++ )
			if(m_BtnConfs[i]->GetBtnCmdId() == nID )
				return m_BtnConfs[i];
		return NULL;
	}

	IMenuItem* GetMenuItemOnButton( INT32 nID )
	{
		UINT32 code = nID - MENU_COMMAND_ID_BEGIN;
		UINT32 id = code / MAX_MENU_SIZE;
		if( id >= m_BtnConfs.size() )
			return 0;
		return m_BtnConfs[ id ]->GetMenuItem( code - id * MAX_MENU_SIZE );
	}

	// do all files loaded (bmp, html)?
	bool check()
	{
		if(!m_bmpFileName.empty() && !CheckUpdate(XString(m_bmpFileName)))
			return false;
		if(!m_bmpHotFileName.empty() && !CheckUpdate(XString(m_bmpHotFileName)))
			return false;

		if( FALSE == m_bmpDisbFileName.empty() )
		{
			CheckUpdate( XString( m_bmpDisbFileName ) );
		}

		for(UINT32 i = 0; i < m_BtnConfs.size(); i++)
			if(!m_BtnConfs[i]->check())
				return false;
		for(UINT32 j = 0; j < m_commands.m_commands.size(); j++)
			if(!m_commands.m_commands[j]->check())
				return false;
		return true;
	}

	virtual void XDetermine(XmlParserProxy& parser)
	{
		XML::determineMember< _Button >( m_BtnConfs ,_T("BUTTON"), parser );
		XML::determineMember< Text >( m_BtnConfs, _T("TEXT"), parser );
		XML::determineMember< Splitter >( m_BtnConfs, _T("SPLITTER"), parser );
		XML::determineMember< Combo >( m_BtnConfs, _T("COMBO"), parser );
#ifdef USE_TICKER
		//XML::determineMember<XTicker>(m_BtnConfs,_T("TICKER"),parser);
#endif
		XML::determineMember< Page >( m_BtnConfs, _T("PAGE"), parser );
		XML::determineMember< Separator >( m_BtnConfs, _T("SEPARATOR"), parser );
		XML::determineMember< Commands >( m_commands, _T("COMMANDS"), parser );
		XML::determineMember<>( m_Settings, _T("SETTINGS"), parser );
		XML::determineMember<>( m_bmpFileName, _T("img"), parser );
		XML::determineMember<>( m_bmpHotFileName, _T("hot_img"), parser );
		XML::determineMember<>(m_bmpDisbFileName,_T("disb_img"),parser);
		XML::determineMember<XString>( m_include_xmls, _T("INCLUDEXML"), parser );
	}

	// COMBOs support - we can have lot of combo controls now
	BandEdits m_vecCombos;
	std::vector< Splitter* > m_splitters;

	// find comand to execute by name
	Command* FindCommandConf(const XString& name)
	{
		for( UINT32 c = 0; c < m_commands.m_commands.size(); c ++ )
			if( m_commands.m_commands[c]->m_strName == name )
				return m_commands.m_commands[c];
		return 0;
	}

	//Update all configure at option changed;
	void OnOptionChange( IConfigHost *pConfigHost )
	{
		//std::for_each( m_pXConfig->m_BtnConfs.begin(),m_pXConfig->m_BtnConfs.end(),
		//std::bind2nd(std::mem_fun1(&\XToolbarItem::OnOptionChange),this));

		for(UINT32 n = 0; n < m_BtnConfs.size(); n++)
			m_BtnConfs[n]->OnOptionChange( pConfigHost );
	}
};

