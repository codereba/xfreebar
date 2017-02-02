struct ToolbarConfig : public XmlObject
{
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

	~ToolbarConfig() {}

	// Settings class - collection of settings that appear in Options page
	struct Settings : public XmlObject
	{
		Settings() {}
		XString  m_updateVer;
		XString  m_wordfindHint;

		// fonst used in toolbar
		struct Font : public XmlObject
		{
			Font()
			{
			}

			INT32 m_size;
			XString m_name;
			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_size,_T("size"),p);
				XML::determineMember<>(m_name,_T("name"),p);
			}
		};
		XML::vectorDel<Font*>  m_fonts;

		// colors for HightLighting
		struct Highlight : public XmlObject
		{
			Highlight()
			{
			}
			vector<XString> m_colors;
			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<XString>(m_colors,_T("COLOR"), p);
			}
		}m_highlight;

		XString m_popuponload;

		//store setting to show popup messages
		struct Popup : public XmlObject
		{
			Popup() : m_time(0) 
			{
			}

			XString m_show;
			XString m_url;
			INT32 m_width;
			INT32 m_height;
			INT32 m_time;

			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<XString>(m_url, _T("url"), p);
				XML::determineMember<XString>(m_height, _T("height"), p);
				XML::determineMember<XString>(m_width, _T("width"), p);
				XML::determineMember<XString>(m_show, _T("show"), p);
				XML::determineMember<XString>(m_time, _T("time"), p);
			}
		} m_popup;

		void Init()
		{
			for( INT32 i = 0; i < m_settings.size(); i++ )
				m_settings[ i ]->init();
		}

		// Setting class - parent for all objects that can be used in Options page
		struct Setting : public XmlObject
		{
			XString  m_id;
			XString  m_default;

			void Init()
			{
				setValue( m_id, GetSettingValue() ); 
			}

			XString GetSettingValue()
			{
				return getValue( m_id, m_default.size() ? m_default : _T( "1" ) );
			}

			virtual void OnOpen( CHostToolbar* pToolbar, IHTMLDocument3* pHtmlDoc3 ) = 0;
			virtual void OnSelChange( IHTMLElementPtr p, XString id ) { };
			virtual void OnClick( CHostToolbar* pToolbar, XString id ) { };
			
			virtual void XDetermine( XmlParserProxy& parser )
			{
				XML::determineMember<>( m_nID, _T("id"), parser );
				XML::determineMember<>( m_default, _T("default"), parser );
			}
		};

		// check box in options page, also can set some default parameters for toolbar 
		struct CheckBox : public Setting
		{
			virtual void OnOpen(CHostToolbar* pToolbar,IHTMLDocument3* pHtmlDoc3)
			{
				if( get() == XString( _T( "1" ) ))
				{
					IHTMLElementPtr element;
					pHtmlDoc3->getElementById( _bstr_t( m_nID.c_str() ), &element );
					if(element)
					{
						if( IHTMLInputElementPtr pelement = element )
							pelement->put_checked(VARIANT_TRUE);
					}
				}
			}

			virtual void OnClick(CHostToolbar* pToolbar, XString id )
			{
				if( id == m_id)
					setValue( id, get() == XString(_T("1")) ? _T("0") : _T("1") );
			}
		};

#ifndef NO_OPTIONS_PAGE
		// we can put some buttons in HTML page and assign some command to it
		struct Button : public Setting
		{
			XString m_command;
			virtual void OnOpen(CHostToolbar*pToolbar, IHTMLDocument3* pHtmlDoc3) {}
			virtual void OnClick(CHostToolbar*pToolbar,XString id)
			{
				if( id == m_id)
				{
					if( Commands::Command* command = pToolbar->m_pXFreeToolbar->selectCommand( m_command ) )
						command->apply( pToolbar );
				}
			}
			virtual void XDetermine( XmlParserProxy& parser )
			{
				XML::determineMember<>( m_command, _T("command"), parser );
				Setting::XDetermine( parser );
			}
		};

		// radio control on option page
		struct Radio : public Setting
		{
			XString m_name;
			vector< XString >  m_ids;
			virtual void OnOpen(CHostToolbar *pToolbar,IHTMLDocument3* pHtmlDoc3)
			{
				IHTMLElementPtr pelement;
				for(UINT32 i = 0; i < m_ids.size(); i++)
				{
					pHtmlDoc3->getElementById( _bstr_t( m_ids[i].c_str() ), &pelement);
					if( pelement!=NULL && getValue(m_name,m_default) == XString( IntToStr( i ) ) )
						el->click();
				}
			}

			virtual void OnClick( CHostToolbar* pToolbar, XString id )
			{
				for( UINT32 i = 0; i < m_ids.size(); i++ )
					if( m_ids[ i ] == id )
						setValue( m_name, XString( IntToStr(i) ) );
			}

			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<XString>(m_ids,_T("id"),p);
				XML::determineMember<>(m_name,_T("name"),p);
				Setting::XDetermine(p);
			}
		};

		// combo box control in options page - used only to seleect default search engine
		struct Select : public Setting
		{
			virtual void OnOpen( CHostToolbar* pToolbar, IHTMLDocument3* pHtmlDoc3 )
			{
				IHTMLElementPtr pelement = NULL;
				pHtmlDoc3->getElementById( _bstr_t( m_nID.c_str() ), &pelement );
				
				if( pelement )
				{
					IHTMLDocument2Ptr pHtmlDoc2 = pHtmlDoc3;
					IHTMLSelectElementPtr pselelement = pelement;
					if( pselelement )
					{
						// register handler of the onchange event with the CHostToolbar::OnSelChange method.
						pselelement->put_onchange( _variant_t( CHtmlEventObject<CHostToolbar>::CreateHandler( pToolbar, &CHostToolbar::OnSelChange, 1 ) ) );

						for( UINT32 b=0; b < pToolbar->m_pXFreeToolbar->m_AllToolbarItem.size(); b++ )
						{
							XString command = pToolbar->m_pXFreeToolbar->m_AllToolbarItem[b]->m_command;
							
							if(Commands::Command* command = pToolbar->m_pXFreeToolbar->selectCommand( command ) )
							{
								if( command->canAddToSelect() )
								{
									IHTMLElementPtr element;
									pHtmlDoc2->createElement( _bstr_t( _T( "OPTION" ) ), &element );
									IHTMLOptionElementPtr option = element;
									option->put_text( _bstr_t( pToolbar->m_pXFreeToolbar->m_AllToolbarItem[b]->m_caption.c_str() ) );
									e->add(element,_variant_t( long( -1 ) ) );
								}
							}
						}

						XString sindex = get();
						long index = sindex[0] - '0';
						e->put_selectedIndex( index );
					}
				}
			}

			virtual void onSelChange( IHTMLElementPtr pelement, XString id )
			{
				IHTMLSelectElementPtr pselelement = pelement;
				long index;
				pselelement->get_selectedIndex( &index );
				TCHAR stitle[] = _T("0");
				stitle[0] += static_cast<TCHAR>( index );
				setValue( m_id, stitle );
			}
		};
#endif

		virtual void XDetermine(XmlParserProxy& p)
		{
			XML::determineMember<>(m_updateVer,_T("updateVer"),p);
			XML::determineMember<>(m_wordfindHint,_T("WordFindHint"),p);
			XML::determineMember<Font>(m_fonts,_T("FONT"),p);
			XML::determineMember<>(m_highlight,_T("HIGHLIGHT"),p);
			XML::determineMember<>(m_popuponload,_T("POPUPONLOAD"),p);
			XML::determineMember<>(m_popup,_T("POPUP"),p);
			XML::determineMember<CheckBox>(m_settings,_T("CHECKBOX"),p);
#ifndef NO_OPTIONS_PAGE  
			XML::determineMember<Radio>(m_settings,_T("RADIO"),p);
			XML::determineMember<Select>(m_settings,_T("SELECT"),p);
			XML::determineMember<Button>(m_settings,_T("BUTTON"),p);
#endif
		}

		XML::vectorDel<Setting*> m_settings;

	}m_settings;

	// parent for all menu items in toolbar's menus
	struct IMenuItem  : public XmlObject
	{
		XString m_hint;
		XString m_command;
		XString m_vis_id; // id of visiable "group"
		virtual void appendMenu(CMenuHandle,INT32& ,INT32) = 0;   // all this popup menu (if any)
		virtual IMenuItem* getItem(INT32& items,INT32 c) = 0;     // get child menu item by position
		bool shouldShow() {return m_vis_id.empty() || isGetValue(m_vis_id);}
	};

	// vector of all commands that can our toolbar execute (may be make it a map?)
	struct Commands : public XmlObject
	{
		// parent of all command classes
		struct Command : public XmlObject
		{
			XString m_name;
			virtual bool check() {return true;}              // load (and check) additional files (if any)
			// virtual void setHost(ToobarItem *pItem){};
			virtual bool canAddToSelect() {return false;}    // is it search "engine command"?
			virtual void apply(CHostToolbar* ) = 0;      // execute this command
			virtual void applyPresed(CHostToolbar* pToolbar) {} // execute this command after button (hightlight) was pressed
			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_name,_T("name"),p);
			}
		};

		// all "speacial" types of Commans (uninstall, highlight, etc.)
		struct Special : public Command
		{
			XString m_type;
			XString m_display;

			virtual void XDetermine(XmlParserProxy& p)
			{
				Command::XDetermine(p);
				XML::determineMember<>(m_type,_T("type"),p);
				XML::determineMember<>(m_display,_T("display"),p);
			}

			virtual void apply(CHostToolbar* pToolbar)
			{
				if(m_type==_T("Hide toolbar"))
				{
					ShowBandObject(pToolbar->m_pBand->m_pWebBrowser,bandUUID,false);
				}
				else if(m_type==_T("Message"))
				{
					::MessageBox(0,_T("Searchit IE Toolbar 1.0"),_T("Searchit IE Toolbar 1.0"),MB_OKCANCEL);
				}
				else if(m_type==_T("Search"))
				{
					pToolbar->RunSearch();
				}
				else if(m_type==_T("ClearHistory"))
				{
					CRegKey keyAppID;
					keyAppID.Attach(GetAppRegistryKey());
					keyAppID.RecurseDeleteKey(_T("History1"));
					keyAppID.RecurseDeleteKey(_T("History2"));
					for(ToolbarConfig::IBandEdits it(pToolbar);it;it++)
						it->ClearHistory();
				}
				else if(m_type==_T("Uninstall"))
				{
					if(MessageBox(0,uninstallMsg,alertTitle,MB_YESNO)==IDYES)
					{
						pToolbar->m_pBand->m_pWebBrowser->Quit();
						DllUnregisterServer();
					}
				}
				else if(m_type==_T("Update"))
				{
					setValue(_T("gUpdate"),_T("1"));
					pToolbar->BuildToolbar();
					IWebBrowser2 *pIE = pToolbar->m_pBand->m_pWebBrowser;
					//CloseAllRuningIE();
					//IWebBrowser2 *pIE = 0;
					//if(RunIe(pIE))
					{
						_variant_t varURL = urlAfterUpdate;
#ifdef _XFree
						XString verFile = _T("version.txt");
						XString oldCRC = getValue(verFile);
						if(checkFile(verFile,true))
						{
							XString newCRC = getValue(verFile);
							if(oldCRC==newCRC)
								varURL  = urlAfterUpdate2;
						}
#endif
						pIE->Navigate2(&varURL, &varEmpty, &varEmpty, &varEmpty, &varEmpty);
					}
				}
				else if(m_type==_T("ShowDialog"))
				{
#ifdef _ODBC_DIRECT
#include "Toolband7\ShowDialog.cpp"
#endif
#ifdef _SITE_SAFE
#include "Toolband3\ShowDialog.cpp"
#endif
				}
				else if(m_type==_T("Make Default Toolbar"))
				{
					setValue(_T("Toolbar Extension"),gAppendCommand);
					setValue("Toolbar Extension Display",gAppendCommandDisplay);
				}
#ifdef _MICROSOLUTIONS
#include "Toolband22\popup.cpp"
#endif
				else if(m_type==_T("Switch to"))
				{
					gAppendCommand = m_name;
					for(ToolbarConfig::IBandEdits it(pToolbar);it;it++)
					{
						gLastSearchWords[it->GetXFreeBarItemName()] = it->GetText();
					}
#ifdef _GERMAN
#include "Toolband21\SwitchTo.cpp"
#endif
					pToolbar->OnOptionChange();
					if(XToolbarItem* item = pToolbar->m_pXFreeToolbar->Find2("search"))
					{
						item->m_caption = m_display;
						gAppendCommandDisplay = m_display;
					}
					pToolbar->BuildToolbar();
				}
				else if(m_type==_T("Custom"))
				{
					pToolbar->OnConfig();
				}
#ifdef _ODBC_DIRECT
#include "Toolband7\startAutoLogin.cpp"
#endif
			}
		};
		// execute java script on current page
		struct Script : public Command
		{
			XString m_body;
			virtual void apply(CHostToolbar* pToolbar)
			{
				IHTMLDocument2Ptr pHtmlDoc = pToolbar->GetHtmlDocument();
				if(pHtmlDoc)
				{
					CComPtr<IDispatch> spDisp;
					pHtmlDoc->get_Script( &spDisp );
					CComQIPtr<IHTMLWindow2> spWin = spDisp; //This IHTMLWindow2 interface has the execScript method to execture the script
					VARIANT pvarRet;
					spWin->execScript(_bstr_t(m_body.c_str()),_bstr_t("JavaScript"),&pvarRet);
				}
			}
			virtual void XDetermine(XmlParserProxy& p)
			{
				Command::XDetermine(p);
				XML::determineMember<>(m_body,_T("body"),p);
			}
		};

		struct PopBlock : public Command
		{
			XString strBlockType;

			PopBlock(){};

			virtual void apply(CHostToolbar* pToolbar)
			{

				if(strBlockType == "IsBlock")
				{
					setValue("IsBlockWindow", "1");
					setValue("IsBlockSound", "1");
					pToolbar->CheckButton(WM_USER+pToolbar->m_pXFreeToolbar->Find(_T("ToggleBlock")),TRUE);
				}
				else if(strBlockType == "IsSound")
				{
					setValue("IsBlockSound", isGetValue2("IsBlockSound") ? "0" : "1");
				}
				else if(strBlockType == "IsClean")
				{
					pToolbar->m_pBand->CleanBlockCount();
				}
			}

			virtual void applyPresed(CHostToolbar* pToolbar)
			{
				setValue("IsBlockWindow", "0");
				pToolbar->CheckButton(WM_USER + pToolbar->m_pXFreeToolbar->Find(_T("ToggleBlock")),FALSE);
			}
			virtual void XDetermine(XmlParserProxy& p)
			{
				Command::XDetermine(p);
				//XML::determineMember<>(m_bBlock,_T("IsBlock"),p);
				//XML::determineMember<>(m_bSound,_T("IsSound"),p);
				//XML::determineMember<>(m_bClean,_T("IsClean"),p);

				XML::determineMember<>(strBlockType,_T("BlockOption"),p);
			}
		};

		// open web URL in browser
		struct WebJump : public Command
		{
			XString m_href;
			XString m_newbrowserwindow;
			XString m_post;
			XString m_target;
			XString m_options;
			XString m_addtoselect;

			// I don't know better solution now
			virtual bool canAddToSelect() {return m_addtoselect==_T("yes");}

			virtual void apply(CHostToolbar* pToolbar) { do_apply(pToolbar,m_href); }
			void do_apply(CHostToolbar* pToolbar,XString s)
			{
				IHTMLDocument2Ptr pHtmlDoc = pToolbar->GetHtmlDocument();
				XString post = m_post;

				for(ToolbarConfig::IBandEdits it(pToolbar);it;it++)
				{
					gLastSearchWords[it->GetXFreeBarItemName()] = it->GetText(); // GetText knows about CBS_DROPDOWNLIST style
					XString strEdit = replaceSpace((urlencode(XString(it->GetText()))),vector<XString>());
#ifdef _BIBLE
					if(it->m_combo->m_name==XString("book"))
						strEdit = IntToStr(it->FindStringExact(strEdit.c_str())+1);
#endif
					XString mask = _T("%")+it->GetXFreeBarItemName();

					if((replace(s,mask,strEdit) || replace(post,mask,strEdit)) && it->m_combo->m_style!=CBS_DROPDOWNLIST)
						it->AddinHistoryAndUpdateWords(it->GetText());
				}

				CComBSTR  url;
				CComBSTR  domain;
				CComBSTR  title;
				if(pHtmlDoc)
				{
					if(pHtmlDoc->get_URL(&url)==0)
						gurl = urlencode(XString(_bstr_t(url))).c_str();
					pHtmlDoc->get_domain(&domain);
					pHtmlDoc->get_title(&title);
				}

				replace(s,_T("%d"),_bstr_t(domain));
				replace(s,_T("%title"),_bstr_t(title));

				_variant_t varURL = replaceUrl(s);
				_variant_t postData = getPostData((const TCHAR*)replaceUrl(post));
				//_variant_t varHeaders = _T("");
				_variant_t varHeaders;
				varHeaders.vt = VT_BSTR;
				varHeaders.bstrVal = NULL;

#ifdef _DYNABAR
				varHeaders = "Referer: DynaBar toolbar";
#endif

				if(m_newbrowserwindow==_T("true") ||
					(m_newbrowserwindow.empty() && isGetValue2(_T("OpenNew"))) )
				{
					IWebBrowser2 *pIE = RunIe();
					pIE->Navigate2(&varURL, &varEmpty, &varEmpty, &postData, &varHeaders);
					//long wnd;
					//pIE->get_HWND((long*)&wnd);
					//SetFocus(wnd);
				}
				else
				{
					if(m_newbrowserwindow==_T("modaldialog"))
					{
						IHTMLDocument2Ptr pHtmlDoc = pToolbar->GetHtmlDocument();
						if(pHtmlDoc)
						{
							IHTMLWindow2Ptr win = 0;
							pHtmlDoc->get_parentWindow(&win);
							if(win)
								win->showModalDialog(_bstr_t(varURL),&_variant_t(""),&_variant_t(m_options.c_str()),&varEmpty);
						}
					}
					// show HTMl page that can have a lot of "our" page links
					else if(m_newbrowserwindow==_T("toolbarwindow"))
					{
#if defined(_ALEXA) || defined (_TYCOON) || defined (_ESTELLEREYNA)
						CHtmlPage* dlg = new CHtmlPage;
						dlg->m_parentBrowser = pToolbar->m_pBand->m_pWebBrowser;
						dlg->m_url = varURL;
						dlg->Create(*pToolbar);
						dlg->ShowWindow(SW_SHOW);
						dlg->m_wasShow = true;

						CRect rc,rc2;
						pToolbar->GetItemRect(pToolbar->m_lastPresedButton, rc);
						pToolbar->ClientToScreen(rc);
						dlg->GetWindowRect(&rc2);
						dlg->MoveWindow(rc.left,rc.bottom,rc2.Width(),rc2.Height());
#endif
					}
					else
					{
						_variant_t target = m_target.c_str();
						pToolbar->m_pBand->m_pWebBrowser->Navigate2(&varURL, &varEmpty, &target, &postData, &varHeaders);
					}
				}
			}
			virtual void XDetermine(XmlParserProxy& p)
			{
				Command::XDetermine(p);
				XML::determineMember<>(m_href,_T("href"),p);
				XML::determineMember<>(m_post,_T("post"),p);
				XML::determineMember<>(m_target,_T("target"),p);
				XML::determineMember<>(m_options,_T("options"),p);
				XML::determineMember<>(m_newbrowserwindow,_T("newbrowserwindow"),p);
				XML::determineMember<>(m_addtoselect,_T("addtoselect"),p);
			}
		};

		// open previous loaded HTML page
		struct LocalHTML : public WebJump
		{
			XString m_ref_path;
			virtual bool check()
			{
				m_ref_path = m_href;
				if(m_newbrowserwindow==_T("toolbarwindow"))
					return true;
				return checkFile(m_ref_path)!=0;
			}
			virtual void apply(CHostToolbar* pToolbar)
			{
				if(check())
					do_apply(pToolbar,m_ref_path);
			}
		};

#define CMD_SUPER 1
#define CMD_COMMON 0
		// execute some local executable or some shell command (create email)
		struct ShellExecute : public Command
		{
			XString m_command;
			XString m_not_found;
			INT32 m_type;

			virtual void apply(CHostToolbar* pToolbar)
			{
				XString s = m_command;
				if(m_type == CMD_SUPER)
				{
					for(ToolbarConfig::IBandEdits it(pToolbar);it;it++)
					{
						gLastSearchWords[it->GetXFreeBarItemName()] = it->GetText(); // GetText knows about CBS_DROPDOWNLIST style
						XString strEdit = replaceSpace(XString(it->GetText()),vector<XString>());
#ifdef _BIBLE
						if(it->m_combo->m_name==XString("book"))
							strEdit = IntToStr(it->FindStringExact(strEdit.c_str())+1);
#endif
						XString mask = _T("%")+it->GetXFreeBarItemName();

						if(replace(s,mask,strEdit) && it->m_combo->m_style!=CBS_DROPDOWNLIST)
							it->AddinHistoryAndUpdateWords(it->GetText());
					}
				}
				HINSTANCE ret = ::ShellExecute(NULL,NULL,s.c_str(),NULL,NULL,SW_SHOWNORMAL);
				if(INT32(ret)<32)
					MessageBox(0,m_not_found.c_str(),alertTitle,0);
			}

			virtual void XDetermine(XmlParserProxy& p)
			{
				Command::XDetermine(p);
				XML::determineMember<>(m_command,_T("command"),p);
				XML::determineMember<>(m_not_found,_T("not_found"),p);
				XML::determineMember<INT32>(m_type,_T("type"),p);
			}
		};

#ifndef NO_HIGHTLIGHT
		// process ala Google Highlight
		struct Highlight : public Command
		{
			virtual void applyPresed(CHostToolbar* pToolbar)
			{
				for(UINT32 c=0;c<pToolbar->m_pXFreeToolbar->m_settings.m_highlight.m_colors.size();c++)
				{
					pToolbar->ClearSearchResults();
				}
				pToolbar->CheckButton(WM_USER + pToolbar->m_pXFreeToolbar->Find(_T("Highlight")), FALSE);
			}
			virtual void apply(CHostToolbar* pToolbar)
			{
				INT32 nColors;
				if( (nColors = pToolbar->m_pXFreeToolbar->m_settings.m_highlight.m_colors.size()) <= 0)
					return;

				applyPresed(pToolbar);

				XString strAllKeyWords;
				for(ToolbarConfig::IBandEdits it(pToolbar);it;it++)
					strAllKeyWords += it->GetText() + " ";
				// Get the text in the edit box
				std::vector<XString> vecKeyWords;
				replaceSpace(strAllKeyWords,vecKeyWords);

				for(INT32 id=0;id<vecKeyWords.size();id++)
				{
					XString val = vecKeyWords[id];
					replace(val,_T("\""),XString(_T("")));

					pToolbar->FindText2(val.c_str(),tomMatchWord,getColor(pToolbar,id % nColors));
				}
				pToolbar->MoveFocusToHTML();

				pToolbar->CheckButton(WM_USER + pToolbar->m_pXFreeToolbar->Find(_T("Highlight")), TRUE);
			}

			_bstr_t getColor(CHostToolbar* pToolbar,INT32 c)
			{
				return (_T("color:#000000;background-color:") + pToolbar->m_pXFreeToolbar->m_settings.m_highlight.m_colors[c] + _T(";")).c_str();
			}
		};
#endif

		virtual void XDetermine(XmlParserProxy& p)
		{
			XML::determineMember<WebJump>(m_commands,_T("WEBJUMP"),p);
			XML::determineMember<PopBlock>(m_commands,_T("POPBLOCK"),p);
			XML::determineMember<Script>(m_commands,_T("SCRIPT"),p);
			XML::determineMember<Special>(m_commands,_T("SPECIAL"),p);
			XML::determineMember<LocalHTML>(m_commands,_T("LOCALHTML"),p);
			XML::determineMember<ShellExecute>(m_commands,_T("SHELLEXECUTE"),p);
#if !defined(NO_HIGHTLIGHT)
			XML::determineMember<Highlight>(m_commands,_T("HIGHLIGHT"),p);
#endif
		}
		XML::vectorDel<Command*> m_commands;
	}m_commands;

	// parent of all Toolbar's controls
	struct XToolbarItem : public XmlObject
	{
		XToolbarItem() : m_nID(-2), m_command_id(0),m_dwStyle(0) {}
		INT32 m_nID;
		Commands::Command* m_command_id;
		XString m_hint;
		XString m_caption;
		XString m_command;
		XString m_vis_id; // id of visiable "group"
		XString m_strType;
		INT32	m_dwStyle;
		XString& GetXFreeBarItemName()    {return m_caption;}
		XString& GetXFreeBarItemToolTip() {return m_hint;}

		virtual void OnXFreeBarCommand(CHostToolbar* ) {}   // button pressed
		virtual void Invalidate(CHostToolbar* ) {}  // invalidate owner-drawn controls
		virtual DWORD OnItemPrePaint(CHostToolbar* pToolbar,LPNMTBCUSTOMDRAW lpTBCustomDraw)
		{
			bool hot = (lpTBCustomDraw->nmcd.uItemState & CDIS_HOT) == CDIS_HOT;
			bool selected = (lpTBCustomDraw->nmcd.uItemState & CDIS_SELECTED) == CDIS_SELECTED;
			if(hot||selected)
			{
				lpTBCustomDraw->clrText = 200<<16;
				lpTBCustomDraw->clrBtnHighlight = 234;

				//CBrushHandle back(CreateSolidBrush(255));

				CDCHandle dc = lpTBCustomDraw->nmcd.hdc;
				dc.SetTextColor(lpTBCustomDraw->clrText);
				dc.SetBkColor(255);
				dc.SetBkMode(lpTBCustomDraw->nStringBkMode);
				//dc.SelectBrush(back);

				HFONT hFont = pToolbar->GetFont();
				HFONT hFontOld = NULL;
				if(hFont != NULL)
					hFontOld = dc.SelectFont(hFont);
				//RoundRect(lpTBCustomDraw->nmcd.hdc,lpTBCustomDraw->nmcd.rc.left,lpTBCustomDraw->nmcd.rc.top,lpTBCustomDraw->nmcd.rc.right,lpTBCustomDraw->nmcd.rc.bottom,20,20);
			}
			return CDRF_DODEFAULT;
		}

		virtual bool isShownAlways() {return false;}  // used to calculate min width of toolbar

		virtual IMenuItem* getItem(INT32 c) {return 0;} // get menu item (if any) by position

		virtual BOOL StopMouseEvents(CHostToolbar * pToolbar = NULL) {return FALSE;} // used to disalbe mouse input for TEXT control

		image_index getBitmap(){return m_img;}         // working with icons
		void setBitmap(image_index img){m_img = img;}

		virtual bool check() {return true;}           // load (and check) additional files (if any)
		virtual void OnOptionChange(CHostToolbar* pToolbar) {}; // called after option page changed
		// show Popup menu
		virtual void ShowMenu(HWND wnd,INT32 x, INT32 y) {ATLASSERT(false);} 
		// called during building toolbar to apply some custom control's attributes
		virtual void XChange(CHostToolbar* pToolbar) 
		{
			XString s = GetXFreeBarItemName();
			s += _T('\0');

			pToolbar->AddStrings(s.c_str());
			ATLTRACE("Add strings %s\n", s.c_str());
		};
		virtual void XDetermine(XmlParserProxy& p)
		{
			XML::determineMember<>(m_caption,_T("caption"),p);
			XML::determineMember<>(m_img,_T("img"),p);
			XML::determineMember<>(m_command,_T("command"),p);
			XML::determineMember<>(m_hint,_T("hint"),p);
			XML::determineMember<>(m_vis_id,_T("vis_id"),p);
			XML::determineMember<INT32>(m_dwStyle, _T("style"), p);
		}
	protected:
		image_index  m_img;
	};

	// Separator control, need to say more? :)
	struct Separator  : public XToolbarItem
	{
		Separator()
		{
			m_strType = XTYPE_SEPARATOR;
		}

		virtual void XChange(CHostToolbar* pToolbar) 
		{
			XToolbarItem::XChange(pToolbar);
			TBInfoSetter(pToolbar,m_nID).style(TBSTYLE_SEP);//.image(8); 
		}

		virtual void OnOptionChange(CHostToolbar* pToolbar)
		{
			pToolbar->HideButtonIfNeeded(m_nID,m_vis_id);// to hide last by default
		}
	};

	//  Button control
	struct Button : public XToolbarItem
	{
		Button()
		{
			m_hot_img = _T("");
			m_strType = XTYPE_BUTTON;
		}

		~Button() {}

		struct MenuItem  : public IMenuItem
		{
			XString m_checkKey;
			virtual IMenuItem* getItem(INT32& items,INT32 c) {return 0;}
			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_hint,_T("hint"),p);
				XML::determineMember<>(m_command,_T("command"),p);
				XML::determineMember<>(m_vis_id,_T("vis_id"),p);
				XML::determineMember<>(m_checkKey, _T("checkkey"),p);
			}
		};

		virtual IMenuItem* getItem(INT32 c) {return m_menu.getItem(c);}

		// struct to contain child items
		struct IMenu
		{
			XML::vectorDel<MenuItem*>   m_items;
		};

		// Button's menu
		struct Menu : public XmlObject, public IMenu
		{
			~Menu()
			{
				if(m_ctrlBarFake.m_hWnd)
					m_ctrlBarFake.DestroyWindow();
			}

			// "normal" menu item = icon + text + command to execute
			struct Command  : public MenuItem
			{
				image_index getBitmap(){return m_img;}
				void setBitmap(image_index img){m_img = img;}

				XString m_caption;

				virtual void appendMenu(CMenuHandle handle,INT32& j,INT32 id)
				{
#ifdef USE_SIMPLE_MENU
					handle.AppendMenu(MF_STRING,WM_MENU_USER + MAX_MENU_SIZE*id +j,m_caption.c_str());
#else
					CHostToolbar::_MenuItemData2* pMI = new CHostToolbar::_MenuItemData2();
					pMI->fType = MF_STRING;
					pMI->lpstrText = (LPTSTR)m_caption.c_str();
					pMI->iButton = GetBitmap(getBitmap());//pToolbar->GetImageId(getBitmap());
					if(pMI->iButton >= 0)
					{
						pMI->iButton += TOOLBAR_IMAGE_INDEX_BEGIN;
					}
					handle.AppendMenu(MF_OWNERDRAW,WM_MENU_USER + MAX_MENU_SIZE*id +j,LPCTSTR(pMI));
#endif
					if(m_checkKey != "")
					{
						handle.CheckMenuItem(j, MF_BYPOSITION|(isGetValue2(m_checkKey) ? MF_CHECKED :MF_UNCHECKED));
					}

					if(m_command==gAppendCommand)
						handle.CheckMenuRadioItem(WM_MENU_USER + MAX_MENU_SIZE*id +0,WM_MENU_USER + MAX_MENU_SIZE*id +j,WM_MENU_USER + MAX_MENU_SIZE*id +j,MF_BYCOMMAND);
				}

				virtual void XDetermine(XmlParserProxy& p)
				{
					XML::determineMember<>(m_img,_T("img"),p);
					MenuItem::XDetermine(p);
					XML::determineMember<>(m_caption,_T("caption"),p);
				}
			private:
				image_index  m_img;
				DWORD		m_dwStyle;
			};

			// separator in menu - very usefull thing :)
			struct Separator  : public MenuItem
			{
				virtual void XDetermine(XmlParserProxy& p)  {}
				virtual void appendMenu(CMenuHandle handle,INT32&,INT32)
				{
#ifdef USE_SIMPLE_MENU
					handle.AppendMenu(MF_SEPARATOR);
#else
					CHostToolbar::_MenuItemData2* pMI = new CHostToolbar::_MenuItemData2();
					pMI->fType = MF_SEPARATOR;
					handle.AppendMenu(MF_OWNERDRAW, (UINT_PTR)0, LPCTSTR(pMI));
#endif
				}
			};

			// we need more levels, more menus
			struct PopupMenu  : public MenuItem, public IMenu
			{
				XString m_caption;
				virtual void XDetermine(XmlParserProxy& p)
				{
					MenuItem::XDetermine(p);
					XML::determineMember<>(m_caption,_T("caption"),p);
					XML::determineMember<Command>(m_items,_T("ITEM"),p);
					XML::determineMember<Separator>(m_items,_T("SEPARATOR"),p);
					XML::determineMember<PopupMenu>(m_items,_T("MENU"),p);
				}
				virtual void appendMenu(CMenuHandle handle,INT32& items,INT32 id)
				{
					CMenuHandle MenuPopup;
					MenuPopup.CreatePopupMenu();

					for(UINT32 j=0;j<m_items.size();j++)
					{
						if(m_items[j]->shouldShow())
						{
							m_items[j]->appendMenu(MenuPopup,items,id); /// pass right values
							items++;
						}
					}

#ifdef USE_SIMPLE_MENU
					handle.AppendMenu(MF_POPUP,(UINT32)(HMENU)MenuPopup,m_caption.c_str());
#else
					CHostToolbar::_MenuItemData2* pMI = new CHostToolbar::_MenuItemData2();
					pMI->fType = MF_STRING;
					pMI->lpstrText = (LPTSTR)m_caption.c_str();
					pMI->iButton = -1;//m_img;
					handle.AppendMenu(MF_OWNERDRAW|MF_POPUP,(UINT32)(HMENU)MenuPopup,LPCTSTR(pMI));
#endif
				}
				IMenuItem* getItem(INT32& items, INT32 c)
				{
					for(UINT32 j=0;j<m_items.size();j++)
					{
						if(IMenuItem* s = m_items[j]->getItem(items,c))
							return s;
						if(items==c)
							return m_items[j];
						items++;
					}
					return 0;
				}
			};

			IMenuItem* getItem(INT32 c)
			{
				INT32 items = 0;
				for(UINT32 j=0;j<m_items.size();j++)
				{
					if(IMenuItem* s = m_items[j]->getItem(items,c))
						return s;
					if(items==c)
						return m_items[j];
					items++;
				}
				return 0;
			}

			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<Command>(m_items,_T("ITEM"),p);
				XML::determineMember<Separator>(m_items,_T("SEPARATOR"),p);
				XML::determineMember<PopupMenu>(m_items,_T("MENU"),p);
				XML::determineMember<>(m_type,_T("type"),p);
				XML::determineMember<>(m_img,_T("img"),p);
				XML::determineMember<>(m_hot_img,_T("hot_img"),p);
			}

			XString getHotBitmap() {return m_hot_img;};
			CBandToolBarCtrlFake m_ctrlBarFake;
			XString m_type;
			XString m_img;
			XString m_hot_img;
		}m_menu;


		virtual void ShowMenu(HWND wnd,INT32 x, INT32 y)
		{
			CMenu MenuPopup;
			MenuPopup.CreatePopupMenu();
			INT32 items = 0;
			for(UINT32 j=0;j<m_menu.m_items.size();j++)
			{
				if(m_menu.m_items[j]->shouldShow())
				{
					m_menu.m_items[j]->appendMenu((CMenuHandle)MenuPopup,items,m_nID);
					items++;
				}
			}
			::TrackPopupMenu(MenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, wnd, NULL);
			CHostToolbar::_MenuItemData2::MenuItems.clear();
		}

		virtual void OnOptionChange(CHostToolbar* pToolbar)
		{
			pToolbar->AddCustomImage(m_img,m_hot_img);
			TBBUTTONINFO tbi;
			tbi.cbSize = sizeof TBBUTTONINFO;
			tbi.dwMask = TBIF_TEXT | TBIF_IMAGE;
#ifdef _HUNTBAR
#include "Toolband12\DescriptiveText.cpp"
#else
			if(!isGetValue(_T("DescriptiveText")))
				tbi.pszText = _T("");
			else
				tbi.pszText = const_cast<TCHAR*>(GetXFreeBarItemName().c_str());
			if(!isGetValue(_T("DescriptiveImage")))
				tbi.iImage = -2;
			else
				tbi.iImage = pToolbar->GetImageId(getBitmap());
			pToolbar->SetButtonInfo(WM_USER+m_nID, &tbi);
			pToolbar->HideButtonIfNeeded(m_nID,m_vis_id);
#endif

			if(m_img.empty() && !m_menu.m_img.empty())
			{
				RECT rc;
				pToolbar->GetItemRect(m_nID, &rc);
				m_menu.m_ctrlBarFake.MoveWindow(&rc);
			}

			if(m_checkKey != "")
			{
				pToolbar->CheckButton(WM_USER + m_nID, isGetValue2(m_checkKey));
			}
		}

		virtual bool check()
		{
			if(m_menu.m_img.empty())
				return true;
			XString menuimg = m_menu.m_img;
			XString hotimage = m_menu.m_hot_img;
#ifdef _ODBC_DIRECT
			return true;
#endif
			if(m_hot_img.size()!= 0 && !checkFile(m_hot_img))
				return false;

			return checkFile(menuimg) && (hotimage.size()==0 || checkFile(hotimage));
		}

		virtual void XChange(CHostToolbar* pToolbar)
		{
			XToolbarItem::XChange(pToolbar);
			RECT rc;
			pToolbar->GetItemRect(m_nID, &rc);
			if(!m_menu.m_type.empty())
			{
				TBInfoSetter(pToolbar,m_nID).style(BTNS_SHOWTEXT | TBSTYLE_AUTOSIZE | (m_menu.m_type == _T("Single") ? BTNS_WHOLEDROPDOWN : TBSTYLE_DROPDOWN) ); 
			}


			if(m_img.empty() && !m_menu.m_img.empty())
			{
				if(m_menu.m_img.find(_T(".ico")) != -1 || m_menu.m_img.find(_T(".ICO")) != -1)
				{
					m_menu.m_ctrlBarFake.m_size.cx = 16;//GetIconSize(LoadIcon(m_menu.m_img));
					m_menu.m_ctrlBarFake.m_size.cy = 16;
				}
				else
				{
					CBitmap(LoadImage(m_menu.m_img)).GetSize(m_menu.m_ctrlBarFake.m_size);
				}

#ifdef _DYNABAR
				m_menu.m_ctrlBarFake.m_size.cx /=2;
#endif
				CDC dc = ::GetDC(NULL);
				dc.SelectFont(pToolbar->font.m_hFont);
				SIZE size;
				GetTextExtentPoint(dc,m_caption.c_str(),m_caption.size(),&size);

				{TBInfoSetter(pToolbar,m_nID).style(TBSTYLE_BUTTON).size(size.cx + m_menu.m_ctrlBarFake.m_size.cx + ADD_TO_DROP_MENU);}

				m_menu.m_ctrlBarFake.m_nID = m_nID;
				m_menu.m_ctrlBarFake.setroot(pToolbar);

				rc.bottom = TB_HEIGHT;

				m_menu.m_ctrlBarFake.Create(*pToolbar, rc, NULL,
					WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
					CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN |
					TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST ,0); // | TBSTYLE_TRANSPARENT,0);
			}
		}

		XString m_checkKey;
		image_index m_hot_img;
		image_index getHotBitmap() {return m_hot_img;};
		virtual void XDetermine(XmlParserProxy& p)
		{
			XToolbarItem::XDetermine(p);
			XML::determineMember<>(m_menu,_T("MENU"),p);
			XML::determineMember<>(m_checkKey, _T("checkkey"),p);
			XML::determineMember<>(m_hot_img, _T("hot_img"),p);
		}
	}; // end of Button class

	// Text = Button - mouse events
	struct Text : public Button
	{
		virtual BOOL StopMouseEvents() {return TRUE;}
	};

	// we would like to insert some IE's into our toolbar
	struct Page : public XToolbarItem
	{
		Page() : m_width(90),
			dlg(m_width)
		{
			m_strType = XTYPE_PAGE;
		}

		~Page(){if(dlg.m_hWnd)dlg.DestroyWindow();}

		XString m_url;

    // WebBrowser control wrapper
    class CPage :
       public IDispEventImpl<1, CPage, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>,
       public CWindowImpl<CPage,CAxWindow>

	{
		long& m_width;
	public:
		CPage(long& w) : m_width(w),m_wasComplete(false) {}

		DECLARE_WND_CLASS(_T("MSHTML_Toolbar_Page"))

		DECLARE_EMPTY_MSG_MAP()
#define DISPID_DOCUMENTCOMPLETE     259   // new document goes ReadyState_Complete
#define DISPID_BEFORENAVIGATE2      250   // hyperlink clicked on
		BEGIN_SINK_MAP(CPage)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE , OnDocumentComplete) //, VTS_DISPATCH VTS_PVARIANT)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2 , OnBeforeNavigate2) //, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
		END_SINK_MAP()

		STDMETHOD(OnBeforeNavigate2)(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel)
		{
			_bstr_t url = URL->bstrVal;
			if(m_wasComplete)
			{
				*Cancel = 1;
				pToolbar->m_pBand->m_pWebBrowser->Navigate2(&_variant_t(url), &varEmpty, &varEmpty, &varEmpty, &varEmpty);
			}
			return S_OK;
		}

		STDMETHOD(OnDocumentComplete) (IDispatch *pDisp, VARIANT *URL)
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
					lpBody->get_scrollWidth(&m_width);
					m_wasComplete = true;
					pToolbar->OnOptionChange();
				}
			}
			return S_OK;
		}
		IWebBrowser2Ptr         m_browser;
		CHostToolbar*       pToolbar;
		bool                    m_wasComplete;
	}dlg;

	virtual void OnOptionChange(CHostToolbar* pToolbar)
	{
		static bool old = g_DO_HTML;
		if((dlg.m_browser==NULL) && g_DO_HTML)
		{
			AtlAxCreateControlEx(replaceUrl(m_url),dlg.m_hWnd, NULL, (IUnknown**)&dlg.m_browser,NULL,DIID_DWebBrowserEvents2,(IUnknown*)&dlg);
			::SendMessage(pToolbar->m_hWnd, WM_SIZE, 0, m_width);
		}
		dlg.ShowWindow(SW_SHOW);// : SW_HIDE);
		pToolbar->HideButtonIfNeeded(m_nID,m_vis_id);

		CRect rc;
		pToolbar->GetItemRect(m_nID, &rc);

		TBInfoSetter(pToolbar,m_nID).style(TBSTYLE_BUTTON).state(TBSTATE_INDETERMINATE).size(m_width); 

		rc.right = rc.left + m_width;
		rc.top += 2;
		rc.bottom -= 2;
		rc.bottom += TB_HEIGHT + 8; 
		dlg.MoveWindow(rc);
	}

	virtual void XChange(CHostToolbar* pToolbar)
	{
		XToolbarItem::XChange(pToolbar);

		if(!dlg.m_hWnd)
		{
			CRect rc;
			pToolbar->GetItemRect(m_nID, &rc);
			dlg.pToolbar = pToolbar;
			dlg.Create(*pToolbar, rc, NULL, WS_CHILD|WS_TABSTOP|WS_VISIBLE);
			if(g_DO_HTML)
				AtlAxCreateControlEx(replaceUrl(m_url),dlg.m_hWnd, NULL, (IUnknown**)&dlg.m_browser,NULL,DIID_DWebBrowserEvents2,(IUnknown*)&dlg);
		}
	}
	virtual void XDetermine(XmlParserProxy& p)
	{
		XToolbarItem::XDetermine(p);
		XML::determineMember<>(m_url,_T("url"),p);
	}
	long m_width;
	}; // end of Page class

	// Combo (Edit) box
	struct Combo : public XToolbarItem
	{
		Combo() : m_ctlBandEdit(0)
		{
			m_strType = XTYPE_COMBO;
		}

		~Combo()
		{
			if( m_ctlBandEdit )
				m_ctlBandEdit->DestroyWindow();
		
			delete m_ctlBandEdit;
		}

		virtual bool isShownAlways() {return true;}
		virtual void OnOptionChange(CHostToolbar* pToolbar)
		{
			if(!m_vis_id.empty())
			{
				pToolbar->HideButton(WM_USER+m_nID,!isGetValue(m_vis_id));
				m_ctlBandEdit->ShowWindow(isGetValue(m_vis_id));
			}

			RECT rcEdit;
			pToolbar->GetItemRect(m_nID, &rcEdit);
			//g_XBarLog.Print(0, "The combobox item rect geted for initializing:(left %d right %d top %d bottom %d\n", 
			//	m_rc.left, 
			//	m_rc.right, 
			//	m_rc.top, 
			//	m_rc.bottom);

			if(rcEdit.bottom-rcEdit.top<COMBOBOX_DROPLINT_HEIGHT )
			{
				rcEdit.top = ((rcEdit.bottom-rcEdit.top) - 22 /*like get system metrix combo height*/)/2;
				rcEdit.bottom+=COMBOBOX_DROPLINT_HEIGHT;
			}

			XString p = getValue(_T("EditWidth")+m_name);
			UINT32 ip = p[0]-'0';
			if(ip>=m_widths.size())
				ip = 0;
			INT32 new_width = (pToolbar->m_start_width*m_widths[ip])/(100);
			rcEdit.right = rcEdit.left + new_width;

			//g_XBarLog.Print(0, "The combobox item rect geted that passing to movewindow for initializing:(left %d right %d top %d bottom %d\n", 
			//	rcEdit.left, 
			//	rcEdit.right, 
			//	rcEdit.top, 
			//	rcEdit.bottom);
			m_ctlBandEdit->MoveWindow(&rcEdit);
			TBInfoSetter(pToolbar,m_nID).size(new_width).text(_T("")); 
			//m_ctlBandEdit->MoveWindow(&rcEdit);


			m_ctlBandEdit->SetFont(pToolbar->font);
		}

		virtual void XChange(CHostToolbar* pToolbar)
		{
			XToolbarItem::XChange(pToolbar); //Add the name of the toolbaritem to the toolbar
			//pToolbar->GetItemRect(m_nID, &rcEdit);
			// rcEdit.right += 50;
			//rcEdit.bottom += 100;
			//rcEdit.bottom = rcEdit.top + COMBOBOX_DROPLINT_HEIGHT; //Maintain the combobox height

			//g_XBarLog.Print(0, "The combobox item rect geted from xml:(left %d right %d top %d bottom %d\n", 
			//	rcEdit.left, 
			//	rcEdit.right, 
			//	rcEdit.top, 
			//	rcEdit.bottom);
			
#define XFREE_BAR_COMBO_MAX_INIT_WIDTH 260
			{ 
				TBInfoSetter BtnInfoSetting(pToolbar,m_nID);
				if(m_widths.size() > 0 && m_widths[0] >= 0 && m_widths[0] <= XFREE_BAR_COMBO_MAX_INIT_WIDTH)
					BtnInfoSetting.size(m_widths[0]);
				BtnInfoSetting.style(TBSTYLE_BUTTON | BTNS_SHOWTEXT);
			} 

			if(!m_ctlBandEdit)
			{
				m_ctlBandEdit = new CBandComboBoxCtrl;
				m_ctlBandEdit->m_pXFreeToolbar = pToolbar;
				m_ctlBandEdit->m_combo = this;
				m_ctlBandEdit->Create(pToolbar);
				//m_ctlBandEdit->MoveWindow(&CRect(0, 0, 100, 30));
			}

			m_ctlBandEdit->ResetContent(); //Clean the text in the edit by send a TB_RESET_CONTENT message

			pToolbar->m_pXFreeToolbar->m_ctlBandEdits.push_back(m_ctlBandEdit); //Toolbar has the EditCtrl list too.std::Vector<CBandComboBoxCtrl>

			if(m_values.empty())
			{
				for(RegIterator it(m_name);it;it++)
					m_ctlBandEdit->InsertString(-1,it);

				m_ctlBandEdit->SetWindowText(gLastSearchWords[m_name].c_str());

				//m_ctlBandEdit->SetEditSel(0,0);

				pToolbar->UpdateAutoWords();
			}
			else
			{
				for(UINT32 it=0;it<m_values.size();it++)
				{
					m_ctlBandEdit->InsertString(it,m_values[it]->m_display.empty() ? m_values[it]->m_value.c_str() : m_values[it]->m_display.c_str());
					m_ctlBandEdit->SetItemDataPtr(it,&m_values[it]->m_value);
				}
#ifdef _PROXY_CHANGE_TOOLBAR
#include "Toolband26\initProxyList.inl"
#endif
				INT32 idx = XString(getValue(_T("SearchUsing"),_T("0")))[0] - '0';
				m_ctlBandEdit->SetCurSel(idx);
			}

			AtlInitCommonControls(ICC_WIN95_CLASSES);

			//m_ctlBandEdit.m_edit2.SetFocus();
			pToolbar->EnableButton(WM_USER + m_nID,FALSE);
		}
		virtual void XDetermine(XmlParserProxy& p)
		{
			XToolbarItem::XDetermine(p);
			XML::determineMember<>(m_name,_T("name"),p);
			XML::determineMember<>(m_limit,_T("limit"),p);
			XML::determineMember<>(m_histmax,_T("histmax"),p);
			XML::determineMember<INT32>(m_widths,_T("WIDTH"),p);
			XML::determineMember<Value>(m_values,_T("VALUE"),p);
		}

	public:

#include "BandEditCtrl.inl" // here is CComboBox (CEdit) hidden

		ICBandEditCtrl* m_ctlBandEdit;

		//RECT            m_rc;
		INT32             m_limit;
		vector<INT32>     m_widths;
		XString         m_name;
		INT32             m_style;
		INT32             m_histmax;

		struct Value : public XmlObject
		{
			XString m_display;
			XString m_value;
			virtual void XDetermine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_display,_T("display"),p);
				m_value = p.load();
			}
		};
		XML::vectorDel<Value*> m_values;
	};

	// here is XTicker struct - colorfull XTicker to show good news 
#ifdef USE_TICKER
#include "TickerControl.inl"
#endif

	XString                      m_bmpFileName;
	XString                      m_bmpHotFileName;
	vector<XString>              m_include_xmls;
	XML::vectorDel<XToolbarItem*> m_AllToolbarItem;

	// sometimes to want to access some buttons "directly" so here are 2 find helpers
	INT32 Find(const XString& str)
	{
		for(UINT32 b=0;b<m_AllToolbarItem.size();b++)
			if(m_AllToolbarItem[b]->m_command==str)
				return b;
		return -1;
	}

	XToolbarItem* Find2(const XString& str)
	{
		INT32 b = Find(str);
		if(b>=0)
			return m_AllToolbarItem[b];
		return 0;
	}

	IMenuItem* getMenuItem(INT32 wID)
	{
		UINT32 code = wID-WM_MENU_USER;
		UINT32 id = code/MAX_MENU_SIZE;
		if(id>=m_AllToolbarItem.size())
			return 0;
		return m_AllToolbarItem[id]->getItem(code - id*MAX_MENU_SIZE);
	}

	// do all files loaded (bmp, html)?
	bool check()
	{
		if(!m_bmpFileName.empty() && !checkFile(XString(m_bmpFileName)))
			return false;
		if(!m_bmpHotFileName.empty() && !checkFile(XString(m_bmpHotFileName)))
			return false;

		for(UINT32 i = 0; i < m_AllToolbarItem.size(); i++)
			if(!m_AllToolbarItem[i]->check())
				return false;
		for(UINT32 j = 0; j < m_commands.m_commands.size(); j++)
			if(!m_commands.m_commands[j]->check())
				return false;
		return true;
	}
	struct Splitter : public Separator
	{
		XString strEditName;
		//DWORD m_oldx;
		BOOL m_bFirstChange;

#define SPLITTER_WIDTH 11

		Combo::ICBandEditCtrl* pEdit;
		Splitter() : pEdit(0), m_bFirstChange(true)//, m_oldx(0)
		{
		}

		BOOL getSplitRc(CHostToolbar *pToolbar, LPRECT prc)
		{
			return pToolbar->GetItemRect(m_nID, prc);
		}

		virtual void OnOptionChange(CHostToolbar *pToolbar)
		{
			Separator::OnOptionChange(pToolbar);
			//for(INT32 i = 0; i< m_pXFreeToolbar->m_AllToolbarItem.size(); i++)
			//{
			// if(m_pXFreeToolbar->m_AllToolbarItem[i].m_nID == strEditName)
			// {
			//  if(strEditName == it->GetXFreeBarItemName())
			//  {
			//	  pEdit = (Combo::ICBandEditCtrl*)it;
			//	  pToolbar->m_pXFreeToolbar->m_splitters.push_back(this);
			//	  break;
			//  }
			//}
			//}
			//TBBUTTONINFO tbi;
			//pToolbar->GetButtonInfo(m_nID, &tbi);
			//tbi.fsState |= TBSTATE_ENABLED;
			//tbi.dwMask |= TBIF_STATE;
			//pToolbar->SetButtonInfo(m_nID,&tbi);
			for(ToolbarConfig::IBandEdits it(pToolbar);it;it++)
			{
				if(strEditName == it->GetXFreeBarItemName())
				{
					if(m_bFirstChange)
					{
						CRect rcEdit;
						pEdit = (Combo::ICBandEditCtrl*)it;
						pToolbar->GetItemRect(pEdit->m_combo->m_nID, &rcEdit);
						//rcEdit.left += 80;
						//rcEdit.right += 4;

						if(rcEdit.right - rcEdit.left > 600)
							break;

						pEdit->MoveWindow(rcEdit);
						TBInfoSetter(pToolbar,pEdit->m_combo->m_nID).size(rcEdit.Width()/* - 80*/ ).text(_T("")); 
						pToolbar->CalcXFreeBarWidth();
						m_bFirstChange = false;
					}
					pToolbar->m_pXFreeToolbar->m_splitters.push_back(this);
					TBInfoSetter(pToolbar, m_nID).size(SPLITTER_WIDTH).text(_T(""));
					break;
				}
			}
		}

#define BLANK_WIDTH SPLITTER_WIDTH
#define MIN_COMBO_WIDTH		30
		virtual BOOL StopMouseEvents(CHostToolbar *pToolbar) 
		{
			static bool bFirst = true;
			if(bFirst)
			{
				bFirst = false;
				return TRUE;
			}

			if(!pEdit)
				return FALSE;
			POINT curPoint;
			::GetCursorPos(&curPoint);
			//dwPoint = ::GetMessagePos();
			// POINT curPoint= { GET_X_LPARAM(dwPoint), GET_Y_LPARAM(dwPoint) };

			pToolbar->ScreenToClient(&curPoint);
			CRect rcEdit;
			pToolbar->GetItemRect(pEdit->m_combo->m_nID, &rcEdit);

			g_XBarLog.Print(0, "The combobox item rect geted:(left %d right %d top %d bottom %d\n", 
				rcEdit.left, 
				rcEdit.right, 
				rcEdit.top, 
				rcEdit.bottom);

			CRect rcBand;
			pToolbar->GetWindowRect(&rcBand);
			pToolbar->ScreenToClient(&rcBand);
			rcEdit.right = curPoint.x;
			///rcEdit.bottom = rcEdit.top + COMBOBOX_HEIGHT;

			if(rcEdit.right < rcBand.right - BLANK_WIDTH && rcEdit.Width() > MIN_COMBO_WIDTH  && rcEdit.Width() < 600)
			{
				//g_XBarLog.Print(0, "The combobox item rect geted that passing to movewindow:(left %d right %d top %d bottom %d\n", 
				//	rcEdit.left, 
				//	rcEdit.right, 
				//	rcEdit.top, 
				//	rcEdit.bottom);

				pEdit->MoveWindow(rcEdit);

				TBInfoSetter(pToolbar,pEdit->m_combo->m_nID).size(rcEdit.Width()).text(_T(""));
				::SendMessage(TB_WIDTH_CHANGED, 
				pToolbar->CalcXFreeBarWidth();
				pToolbar->InitXFreeBarBtnRegionWidth(  pEdit->m_combo->m_nID, pEdit->m_combo->m_nID + 1 );
				pToolbar->DepartToolbar();
			}
			//}
			// m_oldx = curPoint.x;
			return TRUE;
		}

		virtual void XDetermine(XmlParserProxy& p)
		{
			XToolbarItem::XDetermine(p);
			XML::determineMember<>(strEditName,_T("editname"),p);
		}
	};
	virtual void XDetermine(XmlParserProxy& p)
	{
		XML::determineMember<Button>(m_AllToolbarItem,_T("BUTTON"),p);
		XML::determineMember<Text>(m_AllToolbarItem,_T("TEXT"),p);
		XML::determineMember<Splitter>(m_AllToolbarItem, _T("SPLITTER"),p);
		XML::determineMember<Combo>(m_AllToolbarItem,_T("COMBO"),p);
#ifdef USE_TICKER
		XML::determineMember<XTicker>(m_AllToolbarItem,_T("TICKER"),p);
#endif
		XML::determineMember<Page>(m_AllToolbarItem,_T("PAGE"),p);
		XML::determineMember<Separator>(m_AllToolbarItem,_T("SEPARATOR"),p);
		XML::determineMember<Commands>(m_commands,_T("COMMANDS"),p);
		XML::determineMember<>(m_settings,_T("SETTINGS"),p);
		XML::determineMember<>(m_bmpFileName,_T("img"),p);
		XML::determineMember<>(m_bmpHotFileName,_T("hot_img"),p);
		XML::determineMember<XString>(m_include_xmls,_T("INCLUDEXML"),p);
	}

	// COMBOs support - we can have lot of combo controls now
	std::vector<Combo::ICBandEditCtrl*> m_ctlBandEdits;
	std::vector<Splitter*>			m_splitters;
	struct IBandEdits // iterator helper
	{
		IBandEdits(const CHostToolbar* pToolbar) : m_size(pToolbar->m_pXFreeToolbar->m_ctlBandEdits.size()),m_it(pToolbar->m_pXFreeToolbar->m_ctlBandEdits.begin()),m_idx(0){}
		operator bool() {return m_idx<m_size && m_idx>=0;}
		void operator ++(INT32) {m_idx++;m_it++;}
		void operator --(INT32) {m_idx--;m_it--;}
		operator INT32() { return m_idx+1;}
		operator Combo::ICBandEditCtrl*() { return (*m_it);}
		//void operator =(Combo::ICBandEditCtrl* pEdit) { pEdit=*m_it;}
		Combo::ICBandEditCtrl* operator ->() { return (*m_it);}

	private:
		typedef vector<Combo::ICBandEditCtrl*> BandEdits;
		BandEdits::const_iterator m_it;
		INT32                       m_size;
		INT32                       m_idx;
	};

	// find comand to execute by name
	Commands::Command* selectCommand(const XString& name)
	{
		for(UINT32 c=0;c<m_commands.m_commands.size();c++)
			if(m_commands.m_commands[c]->m_name == name)
				return m_commands.m_commands[c];
		return 0;
	}
};