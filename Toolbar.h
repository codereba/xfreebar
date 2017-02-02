struct Toolbar : public XmlObject
{
  ~Toolbar() {}
  
  // Settings class - collection of settings that appear in Options page
  struct Settings : public XmlObject
  {
    Settings() {}
	zstring  m_updateVer;
	zstring  m_wordfindHint;

    // fonst used in toolbar
    struct Font : public XmlObject
    {
      int     m_size;
      zstring m_name;
      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<>(m_size,_T("size"),p);
        XML::determineMember<>(m_name,_T("name"),p);
      }
    };
    XML::vectorDel<Font*>  m_fonts;

    // colors for HightLighting
    struct Highlight : public XmlObject
    {
      vector<zstring> m_colors;
      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<zstring>(m_colors,_T("COLOR"),p);
      }
    }m_highlight;

    zstring  m_popuponload;

    // store setting to show popup messages
    struct Popup : public XmlObject
    {
      Popup() : m_time(0) {}
      zstring m_url;
      int     m_width;
      int     m_height;
      zstring m_show;
      int     m_time;
      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<zstring>(m_url,_T("url"),p);
        XML::determineMember<zstring>(m_height,_T("height"),p);
        XML::determineMember<zstring>(m_width,_T("width"),p);
        XML::determineMember<zstring>(m_show,_T("Show"),p);
        XML::determineMember<zstring>(m_time,_T("Time"),p);
      }
    }m_popup;

    void init()
    {
      for(unsigned int j=0;j<m_settings.size();j++)
        m_settings[j]->init();
    }

    // Setting class - parent for all objects that can be used in Options page
    struct Setting : public XmlObject
    {
      zstring  m_id;
      zstring  m_default;
      void init() { setValue(m_id,get());}
      zstring get() { return getValue(m_id,m_default.size() ? m_default : _T("1"));}
      virtual void onOpen(CXmlObj* ctrl, IHTMLDocument3* pHtmlDoc3) = 0;
      virtual void onSelChange(IHTMLElementPtr p,zstring id) { };
      virtual void onClick(CXmlObj* ctrl,zstring id) { };
      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<>(m_id,_T("id"),p);
        XML::determineMember<>(m_default,_T("default"),p);
      }
    };

    // check box in options page, also can set some default parameters for toolbar 
    struct CheckBox : public Setting
    {
      virtual void onOpen(CXmlObj *ctrl,IHTMLDocument3* pHtmlDoc3)
      {
        if(get()==zstring(_T("1")))
        {
          IHTMLElementPtr el;
          pHtmlDoc3->getElementById(_bstr_t(m_id.c_str()),&el);
          if(el)
          {
            if(IHTMLInputElementPtr i = el)
              i->put_checked(VARIANT_TRUE);
          }
        }
      }
      virtual void onClick(CXmlObj *ctrl,zstring id)
      {
        if(id==m_id)
          setValue(m_id,get()==zstring(_T("1")) ? _T("0") : _T("1") );
      }
    };

#ifndef NO_OPTIONS_PAGE
    // we can put some buttons in HTML page and assign some command to it
    struct Button : public Setting
    {
      zstring m_command;
	  virtual void onOpen(CXmlObj *ctrl, IHTMLDocument3* pHtmlDoc3) {}
	  virtual void onClick(CXmlObj *ctrl,zstring id)
      {
        if(id==m_id)
        {
          if(Commands::Command* c = ctrl->m_toolbar->selectCommand(m_command))
            c->apply(ctrl);
        }
      }
      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<>(m_command,_T("command"),p);
        Setting::determine(p);
      }
    };

    // radio control on option page
    struct Radio : public Setting
    {
      zstring m_name;
      vector<zstring>  m_ids;
      virtual void onOpen(CXmlObj* ctrl,IHTMLDocument3* pHtmlDoc3)
      {
        IHTMLElementPtr el;
        for(unsigned int i=0;i<m_ids.size();i++)
        {
          pHtmlDoc3->getElementById(_bstr_t(m_ids[i].c_str()),&el);
          if(el!=NULL && getValue(m_name,m_default)==zstring(IntToStr(i)))
            el->click();
        }
      }
      virtual void onClick(CXmlObj* ctrl,zstring id)
      {
        for(unsigned int i=0;i<m_ids.size();i++)
          if(m_ids[i]==id)
            setValue(m_name,zstring(IntToStr(i)));
      }
      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<zstring>(m_ids,_T("id"),p);
        XML::determineMember<>(m_name,_T("name"),p);
        Setting::determine(p);
      }
    };
    // combo box control in options page - used only to seleect default search engine
    struct Select : public Setting
    {
      virtual void onOpen(CXmlObj* ctrl, IHTMLDocument3* pHtmlDoc3)
      {
        IHTMLElementPtr e3 = NULL;
        pHtmlDoc3->getElementById(_bstr_t(m_id.c_str()),&e3);
        if(e3)
        {
          IHTMLDocument2Ptr pHtmlDoc2 = pHtmlDoc3;
          IHTMLSelectElementPtr e = e3;
          if(e)
          {
            // register handler
            e->put_onchange(_variant_t(CHtmlEventObject<CXmlObj>::CreateHandler(ctrl, CXmlObj::OnSelChange, 1 )));

            for(unsigned int b=0;b<ctrl->m_toolbar->m_toolbarItems.size();b++)
            {
                zstring command = ctrl->m_toolbar->m_toolbarItems[b]->m_command;
                if(Commands::Command* c = ctrl->m_toolbar->selectCommand(command))
                {
                  if(c->canAddToSelect())
                  {
                    IHTMLElementPtr element;
                    pHtmlDoc2->createElement(_bstr_t(_T("OPTION")),&element);
                    IHTMLOptionElementPtr option = element;
                    option->put_text(_bstr_t(ctrl->m_toolbar->m_toolbarItems[b]->m_caption.c_str()));
                    e->add(element,_variant_t(long(-1)));
                  }
                }
            }

            zstring ss = get();
            long ind = ss[0] - '0';
            e->put_selectedIndex(ind);
          }
        }
      }
      virtual void onSelChange(IHTMLElementPtr p,zstring id)
      {
         IHTMLSelectElementPtr s = p;
         long ind;
         s->get_selectedIndex(&ind);
         TCHAR ss[] = _T("0");
         ss[0] += static_cast<TCHAR>(ind);
         setValue(m_id,ss);
      }
    };
#endif

    virtual void determine(XmlParserProxy& p)
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
    zstring m_hint;
    zstring m_command;
    zstring m_vis_id; // id of visiable "group"
    virtual void appendMenu(CMenuHandle,int& ,int) = 0;   // all this popup menu (if any)
    virtual IMenuItem* getItem(int& items,int c) = 0;     // get child menu item by position
    bool shouldShow() {return m_vis_id.empty() || isGetValue(m_vis_id);}
  };

  // vector of all commands that can our toolbar execute (may be make it a map?)
  struct Commands : public XmlObject
  {
    // parent of all command classes
    struct Command : public XmlObject
    {
      zstring m_name;
      virtual bool check() {return true;}              // load (and check) additional files (if any)
      virtual bool canAddToSelect() {return false;}    // is it search "engine command"?
      virtual void apply(CXmlObj* ) = 0;      // execute this command
      virtual void applyPresed(CXmlObj* ctrl) {} // execute this command after button (hightlight) was pressed
      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<>(m_name,_T("name"),p);
      }
    };

    // all "speacial" types of Commans (uninstall, highlight, etc.)
    struct Special : public Command
    {
      zstring m_type;
      zstring m_display;

      virtual void determine(XmlParserProxy& p)
      {
        Command::determine(p);
        XML::determineMember<>(m_type,_T("type"),p);
        XML::determineMember<>(m_display,_T("display"),p);
      }

      virtual void apply(CXmlObj* ctrl)
      {
        if(m_type==_T("Hide toolbar"))
        {
          ShowBandObject(ctrl->m_pBand->m_pWebBrowser,bandUUID,false);
        }
        else if(m_type==_T("Message"))
        {
          ::MessageBox(0,_T("Searchit IE Toolbar 1.0"),_T("Searchit IE Toolbar 1.0"),MB_OKCANCEL);
        }
        else if(m_type==_T("Search"))
        {
          ctrl->RunSearch();
        }
        else if(m_type==_T("ClearHistory"))
        {
          CRegKey keyAppID;
          keyAppID.Attach(GetAppRegistryKey());
          keyAppID.RecurseDeleteKey(_T("History1"));
          keyAppID.RecurseDeleteKey(_T("History2"));
          for(Toolbar::IBandEdits it(ctrl);it;it++)
            it->ClearHistory();
        }
        else if(m_type==_T("Uninstall"))
        {
          if(MessageBox(0,uninstallMsg,alertTitle,MB_YESNO)==IDYES)
          {
            ctrl->m_pBand->m_pWebBrowser->Quit();
            DllUnregisterServer();
          }
        }
        else if(m_type==_T("Update"))
        {
          setValue(_T("gUpdate"),_T("1"));
          ctrl->checkUpdate();
          IWebBrowser2 *pIE = ctrl->m_pBand->m_pWebBrowser;
          //CloseAllRuningIE();
          //IWebBrowser2 *pIE = 0;
          //if(RunIe(pIE))
          {
            _variant_t varURL = urlAfterUpdate;
#ifdef _2FIND
            zstring verFile = _T("version.txt");
            zstring oldCRC = getValue(verFile);
            if(checkFile(verFile,true))
            {
              zstring newCRC = getValue(verFile);
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
          for(Toolbar::IBandEdits it(ctrl);it;it++)
          {
            gLastSearchWords[it->getName()] = it->GetText();
          }
#ifdef _GERMAN
 #include "Toolband21\SwitchTo.cpp"
#endif
          ctrl->onOptionChange();
          if(ToolbarItem* item = ctrl->m_toolbar->Find2("search"))
          {
            item->m_caption = m_display;
            gAppendCommandDisplay = m_display;
          }
          ctrl->checkUpdate();
        }
#ifdef _ODBC_DIRECT
 #include "Toolband7\startAutoLogin.cpp"
#endif
      }
    };
    // execute java script on current page
    struct Script : public Command
    {
      zstring m_body;
      virtual void apply(CXmlObj* ctrl)
      {
          IHTMLDocument2Ptr pHtmlDoc = ctrl->GetHtmlDocument();
          if(pHtmlDoc)
          {
            CComPtr<IDispatch> spDisp;
      		  pHtmlDoc->get_Script( &spDisp );
			      CComQIPtr<IHTMLWindow2> spWin = spDisp;
            VARIANT pvarRet;
            spWin->execScript(_bstr_t(m_body.c_str()),_bstr_t("JavaScript"),&pvarRet);
          }
      }
      virtual void determine(XmlParserProxy& p)
      {
        Command::determine(p);
        XML::determineMember<>(m_body,_T("body"),p);
      }
    };

    // open web URL in browser
    struct WebJump : public Command
    {
      zstring m_href;
      zstring m_newbrowserwindow;
      zstring m_post;
      zstring m_target;
      zstring m_options;
      zstring m_addtoselect;

      // I don't know better solution now
      virtual bool canAddToSelect() {return m_addtoselect==_T("yes");}

      virtual void apply(CXmlObj* ctrl) { do_apply(ctrl,m_href); }
      void do_apply(CXmlObj* ctrl,zstring s)
      {
        IHTMLDocument2Ptr pHtmlDoc = ctrl->GetHtmlDocument();
        zstring post = m_post;

        for(Toolbar::IBandEdits it(ctrl);it;it++)
        {
           gLastSearchWords[it->getName()] = it->GetText(); // GetText knows about CBS_DROPDOWNLIST style
           zstring strEdit = replaceSpace((urlencode(zstring(it->GetText()))),vector<zstring>());
#ifdef _BIBLE
           if(it->m_combo->m_name==zstring("book"))
             strEdit = IntToStr(it->FindStringExact(strEdit.c_str())+1);
#endif
           zstring mask = _T("%")+it->getName();

           if((replace(s,mask,strEdit) || replace(post,mask,strEdit)) && it->m_combo->m_style!=CBS_DROPDOWNLIST)
             it->AddinHistoryAndUpdateWords(it->GetText());
        }

        CComBSTR  url;
        CComBSTR  domain;
        CComBSTR  title;
        if(pHtmlDoc)
        {
          if(pHtmlDoc->get_URL(&url)==0)
            gurl = urlencode(zstring(_bstr_t(url))).c_str();
          pHtmlDoc->get_domain(&domain);
          pHtmlDoc->get_title(&title);
        }

        replace(s,_T("%d"),_bstr_t(domain));
        replace(s,_T("%title"),_bstr_t(title));
        
        _variant_t varURL = replaceUrl(s);
        _variant_t postData = getPostData((const TCHAR*)replaceUrl(post));
        _variant_t varHeaders = _T("");

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
            IHTMLDocument2Ptr pHtmlDoc = ctrl->GetHtmlDocument();
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
            dlg->m_parentBrowser = ctrl->m_pBand->m_pWebBrowser;
            dlg->m_url = varURL;
            dlg->Create(*ctrl);
            dlg->ShowWindow(SW_SHOW);
            dlg->m_wasShow = true;

            CRect rc,rc2;
            ctrl->GetItemRect(ctrl->m_lastPresedButton, rc);
            ctrl->ClientToScreen(rc);
            dlg->GetWindowRect(&rc2);
            dlg->MoveWindow(rc.left,rc.bottom,rc2.Width(),rc2.Height());
#endif
          }
          else
          {
            _variant_t target = m_target.c_str();
            ctrl->m_pBand->m_pWebBrowser->Navigate2(&varURL, &varEmpty, &target, &postData, &varHeaders);
          }
        }
      }
      virtual void determine(XmlParserProxy& p)
      {
        Command::determine(p);
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
      zstring m_ref_path;
      virtual bool check()
      {
        m_ref_path = m_href;
        if(m_newbrowserwindow==_T("toolbarwindow"))
          return true;
        return checkFile(m_ref_path)!=0;
      }
      virtual void apply(CXmlObj* ctrl)
      {
        if(check())
          do_apply(ctrl,m_ref_path);
      }
    };

    // execute some local executable or some shell command (create email)
    struct ShellExecute : public Command
    {
      zstring m_command;
      zstring m_not_found;

      virtual void apply(CXmlObj* ctrl)
      {
        HINSTANCE ret = ::ShellExecute(NULL,NULL,m_command.c_str(),NULL,NULL,SW_SHOWNORMAL);
        if(int(ret)<32)
          MessageBox(0,m_not_found.c_str(),alertTitle,0);
      }
      virtual void determine(XmlParserProxy& p)
      {
        Command::determine(p);
        XML::determineMember<>(m_command,_T("command"),p);
        XML::determineMember<>(m_not_found,_T("not_found"),p);
      }
    };

#ifndef NO_HIGHTLIGHT
    // process ala Google Highlight
    struct Highlight : public Command
    {
      virtual void applyPresed(CXmlObj* ctrl)
      {
        for(unsigned int c=0;c<ctrl->m_toolbar->m_settings.m_highlight.m_colors.size();c++)
        {
          ctrl->ClearSearchResults();
        }
        ctrl->CheckButton(WM_USER+ctrl->m_toolbar->Find(_T("Highlight")),FALSE);
      }
      virtual void apply(CXmlObj* ctrl)
      {
        applyPresed(ctrl);

        zstring str0;
        for(Toolbar::IBandEdits it(ctrl);it;it++)
          str0 += it->GetText() + " ";
        // Get the text in the edit box
        std::vector<zstring> v;
        replaceSpace(str0,v);

        for(int id=0;id<v.size();id++)
        {
          zstring val = v[id];
          replace(val,_T("\""),zstring(_T("")));
          ctrl->FindText2(val.c_str(),tomMatchWord,getColor(ctrl,id % ctrl->m_toolbar->m_settings.m_highlight.m_colors.size()));
        }
        ctrl->MoveFocusToHTML();

        ctrl->CheckButton(WM_USER+ctrl->m_toolbar->Find(_T("Highlight")),TRUE);
      }
      _bstr_t getColor(CXmlObj* ctrl,int c)
      {
        return (_T("color:#000000;background-color:") + ctrl->m_toolbar->m_settings.m_highlight.m_colors[c] + _T(";")).c_str();
      }
    };
#endif

    virtual void determine(XmlParserProxy& p)
    {
      XML::determineMember<WebJump>(m_commands,_T("WEBJUMP"),p);
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
  struct ToolbarItem : public XmlObject
  {
    ToolbarItem() : m_id(-2),m_command_id(0) {}
    int                m_id;
    Commands::Command* m_command_id;
    zstring            m_hint;
    zstring            m_caption;
    zstring            m_command;
    zstring            m_vis_id; // id of visiable "group"
    zstring& getName()    {return m_caption;}
    zstring& getToolTip() {return m_hint;}

    virtual void onCommand(CXmlObj* ) {}   // button pressed
    virtual void Invalidate(CXmlObj* ) {}  // invalidate owner-drawn controls
    virtual DWORD OnItemPrePaint(CXmlObj* ctrl,LPNMTBCUSTOMDRAW lpTBCustomDraw)
    {
      bool hot = (lpTBCustomDraw->nmcd.uItemState & CDIS_HOT) == CDIS_HOT;
      bool selected = (lpTBCustomDraw->nmcd.uItemState & CDIS_SELECTED) == CDIS_SELECTED;
      if(hot||selected)
	    {
        lpTBCustomDraw->clrText = 200<<16;
        //lpTBCustomDraw->clrBtnHighlight = 234;
        
        CBrushHandle back(CreateSolidBrush(255));
        
        CDCHandle dc = lpTBCustomDraw->nmcd.hdc;
        dc.SetTextColor(lpTBCustomDraw->clrText);
        dc.SetBkColor(255);
        dc.SetBkMode(lpTBCustomDraw->nStringBkMode);
        dc.SelectBrush(back);
        //HFONT hFont = GetFont();
        //HFONT hFontOld = NULL;
        //if(hFont != NULL)
			    //hFontOld = dc.SelectFont(hFont);
    //    RoundRect(lpTBCustomDraw->nmcd.hdc,lpTBCustomDraw->nmcd.rc.left,lpTBCustomDraw->nmcd.rc.top,lpTBCustomDraw->nmcd.rc.right,lpTBCustomDraw->nmcd.rc.bottom,20,20);
	    }
      return CDRF_DODEFAULT;
    }

    virtual bool isShownAlways() {return false;}  // used to calculate min width of toolbar

    virtual IMenuItem* getItem(int c) {return 0;} // get menu item (if any) by position

    virtual BOOL StopMouseEvents() {return FALSE;} // used to disalbe mouse input for TEXT control

    image_index getBitmap(){return m_img;}         // working with icons
    void setBitmap(image_index img){m_img = img;}

    virtual bool check() {return true;}           // load (and check) additional files (if any)
    virtual void onOptionChange(CXmlObj* ctrl) {}; // called after option page changed
    // show Popup menu
    virtual void processMenu(HWND wnd,int x, int y) {ATLASSERT(false);} 
    // called during building toolbar to apply some custom control's attributes
    virtual void change(CXmlObj* ctrl) 
    {
      zstring s = getName();
      s += _T('\0');
      ctrl->AddStrings(s.c_str());
    };
    virtual void determine(XmlParserProxy& p)
    {
      XML::determineMember<>(m_caption,_T("caption"),p);
      XML::determineMember<>(m_img,_T("img"),p);
      XML::determineMember<>(m_command,_T("command"),p);
      XML::determineMember<>(m_hint,_T("hint"),p);
      XML::determineMember<>(m_vis_id,_T("vis_id"),p);
    }
   private:
    image_index  m_img;
  };

  // Separator control, need to say more? :)
  struct Separator  : public ToolbarItem
  {
    virtual void change(CXmlObj* ctrl) 
    {
      ToolbarItem::change(ctrl);
      TBInfo(ctrl,m_id).style(TBSTYLE_SEP).image(8); 
    }
    virtual void onOptionChange(CXmlObj* ctrl)
    {
      ctrl->hideIfNeeded(m_id,m_vis_id);// to hide last by default
    }
  };

  //  Button control
  struct Button : public ToolbarItem
  {
    ~Button() {}

    struct MenuItem  : public IMenuItem
    {
      virtual IMenuItem* getItem(int& items,int c) {return 0;}
      virtual void determine(XmlParserProxy& p)
      {
     	  XML::determineMember<>(m_hint,_T("hint"),p);
        XML::determineMember<>(m_command,_T("command"),p);
        XML::determineMember<>(m_vis_id,_T("vis_id"),p);
      }
    };

    virtual IMenuItem* getItem(int c) {return m_menu.getItem(c);}

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

      CBandToolBarCtrlFake m_ctrlBarFake;
      zstring m_type;
      zstring m_img;
      zstring m_hot_img;

      // "normal" menu item = icon + text + command to execute
      struct Command  : public MenuItem
      {
        image_index getBitmap(){return m_img;}
        void setBitmap(image_index img){m_img = img;}

        zstring m_caption;
        virtual void appendMenu(CMenuHandle handle,int& j,int id)
        {
#ifdef USE_SIMPLE_MENU
          handle.AppendMenu(MF_STRING,WM_MENU_USER + MAX_MENU_SIZE*id +j,m_caption.c_str());
#else
          CXmlObj::_MenuItemData2* pMI = new CXmlObj::_MenuItemData2();
          pMI->fType = MF_STRING;
          pMI->lpstrText = (LPTSTR)m_caption.c_str();
          pMI->iButton = GetBitmap(getBitmap());
          handle.AppendMenu(MF_OWNERDRAW,WM_MENU_USER + MAX_MENU_SIZE*id +j,LPCTSTR(pMI));
#endif
          if(m_command==gAppendCommand)
             handle.CheckMenuRadioItem(WM_MENU_USER + MAX_MENU_SIZE*id +0,WM_MENU_USER + MAX_MENU_SIZE*id +j,WM_MENU_USER + MAX_MENU_SIZE*id +j,MF_BYCOMMAND);
        }
        virtual void determine(XmlParserProxy& p)
        {
          XML::determineMember<>(m_img,_T("img"),p);
          MenuItem::determine(p);
          XML::determineMember<>(m_caption,_T("caption"),p);
        }
        private:
          image_index  m_img;
      };

      // separator in menu - very usefull thing :)
      struct Separator  : public MenuItem
      {
        virtual void determine(XmlParserProxy& p)  {}
        virtual void appendMenu(CMenuHandle handle,int&,int)
        {
#ifdef USE_SIMPLE_MENU
          handle.AppendMenu(MF_SEPARATOR);
#else
          CXmlObj::_MenuItemData2* pMI = new CXmlObj::_MenuItemData2();
          pMI->fType = MF_SEPARATOR;
          handle.AppendMenu(MF_OWNERDRAW,0,LPCTSTR(pMI));
#endif
        }
      };

      // we need more levels, more menus
      struct PopupMenu  : public MenuItem, public IMenu
      {
        zstring m_caption;
        virtual void determine(XmlParserProxy& p)
        {
          MenuItem::determine(p);
          XML::determineMember<>(m_caption,_T("caption"),p);
          XML::determineMember<Command>(m_items,_T("ITEM"),p);
          XML::determineMember<Separator>(m_items,_T("SEPARATOR"),p);
          XML::determineMember<PopupMenu>(m_items,_T("MENU"),p);
        }
        virtual void appendMenu(CMenuHandle handle,int& items,int id)
        {
          CMenuHandle menuPopup;
          menuPopup.CreatePopupMenu();

          for(unsigned int j=0;j<m_items.size();j++)
          {
            if(m_items[j]->shouldShow())
            {
              m_items[j]->appendMenu(menuPopup,items,id); /// pass right values
              items++;
            }
          }

#ifdef USE_SIMPLE_MENU
          handle.AppendMenu(MF_POPUP,(unsigned int)(HMENU)menuPopup,m_caption.c_str());
#else
          CXmlObj::_MenuItemData2* pMI = new CXmlObj::_MenuItemData2();
          pMI->fType = MF_STRING;
          pMI->lpstrText = (LPTSTR)m_caption.c_str();
          pMI->iButton = -1;//m_img;
          handle.AppendMenu(MF_OWNERDRAW|MF_POPUP,(unsigned int)(HMENU)menuPopup,LPCTSTR(pMI));
#endif
        }
        IMenuItem* getItem(int& items, int c)
        {
          for(unsigned int j=0;j<m_items.size();j++)
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

      IMenuItem* getItem(int c)
      {
        int items = 0;
        for(unsigned int j=0;j<m_items.size();j++)
        {
          if(IMenuItem* s = m_items[j]->getItem(items,c))
            return s;
          if(items==c)
            return m_items[j];
          items++;
        }
        return 0;
      }

      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<Command>(m_items,_T("ITEM"),p);
        XML::determineMember<Separator>(m_items,_T("SEPARATOR"),p);
        XML::determineMember<PopupMenu>(m_items,_T("MENU"),p);
        XML::determineMember<>(m_type,_T("type"),p);
        XML::determineMember<>(m_img,_T("img"),p);
        XML::determineMember<>(m_hot_img,_T("hot_img"),p);
      }
    }m_menu;


    virtual void processMenu(HWND wnd,int x, int y)
    {
      CMenu menuPopup;
      menuPopup.CreatePopupMenu();
      int items = 0;
      for(unsigned int j=0;j<m_menu.m_items.size();j++)
      {
        if(m_menu.m_items[j]->shouldShow())
        {
          m_menu.m_items[j]->appendMenu((CMenuHandle)menuPopup,items,m_id);
          items++;
        }
      }
      ::TrackPopupMenu(menuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, wnd, NULL);
      CXmlObj::_MenuItemData2::MenuItems.clear();
    }

    virtual void onOptionChange(CXmlObj* ctrl)
    {
      TBBUTTONINFO tbi;
      tbi.cbSize = sizeof TBBUTTONINFO;
      tbi.dwMask = TBIF_TEXT | TBIF_IMAGE;
#ifdef _HUNTBAR
  #include "Toolband12\DescriptiveText.cpp"
#else
      if(!isGetValue(_T("DescriptiveText")))
        tbi.pszText = _T("");
      else
        tbi.pszText = const_cast<TCHAR*>(getName().c_str());
      if(!isGetValue(_T("DescriptiveImage")))
        tbi.iImage = -2;
      else
        tbi.iImage = GetBitmap(getBitmap());
      ctrl->SetButtonInfo(WM_USER+m_id, &tbi);
      ctrl->hideIfNeeded(m_id,m_vis_id);
#endif

      if(!m_menu.m_img.empty())
      {
        RECT rc;
        ctrl->GetItemRect(m_id, &rc);
        m_menu.m_ctrlBarFake.MoveWindow(&rc);
      }
    }
    virtual bool check()
    {
      if(m_menu.m_img.empty())
        return true;
      zstring menuimg = m_menu.m_img;
      zstring hotimage = m_menu.m_hot_img;
#ifdef _ODBC_DIRECT
      return true;
#endif
      return checkFile(menuimg) && (hotimage.size()==0 || checkFile(hotimage));
    }
    virtual void change(CXmlObj* ctrl)
    {
      ToolbarItem::change(ctrl);
      RECT rc;
      ctrl->GetItemRect(m_id, &rc);
      if(!m_menu.m_type.empty())
      {
        TBInfo(ctrl,m_id).style(BTNS_SHOWTEXT | TBSTYLE_AUTOSIZE | (m_menu.m_type == _T("Single") ? BTNS_WHOLEDROPDOWN : TBSTYLE_DROPDOWN) ); 
      }
      if(!m_menu.m_img.empty())
      {
        CBitmap(ctrl->LoadImage(m_menu.m_img)).GetSize(m_menu.m_ctrlBarFake.m_size);

#ifdef _DYNABAR
        m_menu.m_ctrlBarFake.m_size.cx /=2;
#endif
        CDC dc = ::GetDC(NULL);
        dc.SelectFont(ctrl->font.m_hFont);
        SIZE size;
        GetTextExtentPoint(dc,m_caption.c_str(),m_caption.size(),&size);

        {TBInfo(ctrl,m_id).style(TBSTYLE_BUTTON).size(size.cx + m_menu.m_ctrlBarFake.m_size.cx + ADD_TO_DROP_MENU);}

        m_menu.m_ctrlBarFake.m_id = m_id;
        m_menu.m_ctrlBarFake.setroot(ctrl);

        rc.bottom = TB_HEIGHT;

        m_menu.m_ctrlBarFake.Create(*ctrl, rc, NULL,
          WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
          CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN |
          TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST ,0); // | TBSTYLE_TRANSPARENT,0);
      }
    }
    virtual void determine(XmlParserProxy& p)
    {
      ToolbarItem::determine(p);
      XML::determineMember<>(m_menu,_T("MENU"),p);
    }
  }; // end of Button class

  // Text = Button - mouse events
  struct Text : public Button
  {
    virtual BOOL StopMouseEvents() {return TRUE;}
  };
 // we would like to insert some IE's into our toolbar
  struct Page : public ToolbarItem
  {
    Page() : m_width(90),dlg(m_width) {}
    ~Page(){if(dlg.m_hWnd)dlg.DestroyWindow();}

    zstring m_url;

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
          ctrl->m_pBand->m_pWebBrowser->Navigate2(&_variant_t(url), &varEmpty, &varEmpty, &varEmpty, &varEmpty);
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
            ctrl->onOptionChange();
          }
        }
        return S_OK;
      }
      IWebBrowser2Ptr         m_browser;
      CXmlObj*					ctrl;
      bool                    m_wasComplete;
    }dlg;

    virtual void onOptionChange(CXmlObj* ctrl)
    {
      static bool old = g_DO_HTML;
      if((dlg.m_browser==NULL) && g_DO_HTML)
      {
        AtlAxCreateControlEx(replaceUrl(m_url),dlg.m_hWnd, NULL, (IUnknown**)&dlg.m_browser,NULL,DIID_DWebBrowserEvents2,(IUnknown*)&dlg);
       ::SendMessage(ctrl->m_hWnd, WM_SIZE, 0, m_width);
      }
      dlg.ShowWindow(SW_SHOW);// : SW_HIDE);
      ctrl->hideIfNeeded(m_id,m_vis_id);

      CRect rc;
      ctrl->GetItemRect(m_id, &rc);

      TBInfo(ctrl,m_id).style(TBSTYLE_BUTTON).state(TBSTATE_INDETERMINATE).size(m_width); 

      rc.right = rc.left + m_width;
      rc.top += 2;
      rc.bottom -= 2;
      rc.bottom += TB_HEIGHT + 8; 
      dlg.MoveWindow(rc);
    }
    virtual void change(CXmlObj* ctrl)
    {
      ToolbarItem::change(ctrl);

      if(!dlg.m_hWnd)
      {
        CRect rc;
        ctrl->GetItemRect(m_id, &rc);
        dlg.ctrl = ctrl;
        dlg.Create(*ctrl, rc, NULL, WS_CHILD|WS_TABSTOP|WS_VISIBLE);
        if(g_DO_HTML)
          AtlAxCreateControlEx(replaceUrl(m_url),dlg.m_hWnd, NULL, (IUnknown**)&dlg.m_browser,NULL,DIID_DWebBrowserEvents2,(IUnknown*)&dlg);
      }
    }
    virtual void determine(XmlParserProxy& p)
    {
      ToolbarItem::determine(p);
      XML::determineMember<>(m_url,_T("url"),p);
    }
    long m_width;
  }; // end of Page class

  // Combo (Edit) box
  struct Combo : public ToolbarItem
  {
    Combo():m_ctlBandEdit(0){}
    ~Combo(){if(m_ctlBandEdit) m_ctlBandEdit->DestroyWindow();delete m_ctlBandEdit;}
    virtual bool isShownAlways() {return true;}
    virtual void onOptionChange(CXmlObj* ctrl)
    {
      if(!m_vis_id.empty())
      {
        ctrl->HideButton(WM_USER+m_id,!isGetValue(m_vis_id));
        m_ctlBandEdit->ShowWindow(isGetValue(m_vis_id));
      }

      ctrl->GetItemRect(m_id, &m_rc);

      if(m_rc.bottom-m_rc.top<100)
      {
        m_rc.top = ((m_rc.bottom-m_rc.top) - 22 /*like get system metrix combo height*/)/2;
        m_rc.bottom+=100;
      }

      zstring p = getValue(_T("EditWidth")+m_name);
      unsigned int ip = p[0]-'0';
      if(ip>=m_widths.size())
        ip = 0;
      int new_width = (ctrl->m_start_width*m_widths[ip])/(100);
      m_rc.right = m_rc.left + new_width;

      m_ctlBandEdit->MoveWindow(&m_rc);

      TBInfo(ctrl,m_id).size(new_width).text(_T("")); 
      m_ctlBandEdit->SetFont(ctrl->font);
    }
    virtual void change(CXmlObj* ctrl)
    {
      ToolbarItem::change(ctrl);
      ctrl->GetItemRect(m_id, &m_rc);
      m_rc.right += 50;
      m_rc.bottom+=100;

      { TBInfo(ctrl,m_id).size(m_rc.right-m_rc.left).style(TBSTYLE_BUTTON | BTNS_SHOWTEXT);} 

      if(!m_ctlBandEdit)
      {
        m_ctlBandEdit = new CBandComboBoxCtrl;
        m_ctlBandEdit->m_toolbar = ctrl;
        m_ctlBandEdit->m_combo = this;
        m_ctlBandEdit->Create(ctrl);
      }

      m_ctlBandEdit->ResetContent();

      ctrl->m_toolbar->m_ctlBandEdits.push_back(m_ctlBandEdit);

      if(m_values.empty())
      {
        for(RegIterator it(m_name);it;it++)
            m_ctlBandEdit->InsertString(-1,it);

        m_ctlBandEdit->SetWindowText(gLastSearchWords[m_name].c_str());

        //m_ctlBandEdit->SetEditSel(0,0);

        ctrl->UpdateAutoWords();
      }
      else
      {
        for(unsigned int it=0;it<m_values.size();it++)
        {
          m_ctlBandEdit->InsertString(it,m_values[it]->m_display.empty() ? m_values[it]->m_value.c_str() : m_values[it]->m_display.c_str());
			    m_ctlBandEdit->SetItemDataPtr(it,&m_values[it]->m_value);
        }
#ifdef _PROXY_CHANGE_TOOLBAR
#include "Toolband26\initProxyList.inl"
#endif
        int idx = zstring(getValue(_T("SearchUsing"),_T("0")))[0] - '0';
        m_ctlBandEdit->SetCurSel(idx);
      }

      AtlInitCommonControls(ICC_WIN95_CLASSES);

      //m_ctlBandEdit.m_edit2.SetFocus();
      ctrl->EnableButton(WM_USER+m_id,FALSE);
    }
    virtual void determine(XmlParserProxy& p)
    {
      ToolbarItem::determine(p);
      XML::determineMember<>(m_name,_T("name"),p);
      XML::determineMember<>(m_limit,_T("limit"),p);
      XML::determineMember<>(m_histmax,_T("histmax"),p);
      XML::determineMember<int>(m_widths,_T("WIDTH"),p);
      XML::determineMember<Value>(m_values,_T("VALUE"),p);
    }

  public:

     #include "BandEditCtrl.inl" // here is CComboBox (CEdit) hidden

     ICBandEditCtrl* m_ctlBandEdit;

     RECT            m_rc;
     int             m_limit;
     vector<int>     m_widths;
     zstring         m_name;
     int             m_style;
     int             m_histmax;

     struct Value : public XmlObject
     {
		    zstring m_display;
		    zstring m_value;
		    virtual void determine(XmlParserProxy& p)
		    {
			    XML::determineMember<>(m_display,_T("display"),p);
			    m_value = p.load();
		    }
	   };
	   XML::vectorDel<Value*> m_values;
  };

  // here is Ticker struct - colorfull Ticker to show good news 
#ifdef USE_TICKER
#include "TickerControl.inl"
#endif

  zstring                      m_bmpFileName;
  zstring                      m_bmpHotFileName;
  vector<zstring>              m_include_xmls;
  XML::vectorDel<ToolbarItem*> m_toolbarItems;

  // sometimes to want to access some buttons "directly" so here are 2 find helpers
  int Find(const zstring& str)
  {
    for(unsigned int b=0;b<m_toolbarItems.size();b++)
      if(m_toolbarItems[b]->m_command==str)
        return b;
    return -1;
  }

  ToolbarItem* Find2(const zstring& str)
  {
    int b = Find(str);
    if(b>=0)
      return m_toolbarItems[b];
    return 0;
  }

  IMenuItem* getMenuItem(int wID)
  {
    unsigned int code = wID-WM_MENU_USER;
    unsigned int id = code/MAX_MENU_SIZE;
    if(id>=m_toolbarItems.size())
      return 0;
    return m_toolbarItems[id]->getItem(code - id*MAX_MENU_SIZE);
  }

  // do all files loaded (bmp, html)?
  bool check()
  {
    if(!m_bmpFileName.empty() && !checkFile(zstring(m_bmpFileName)))
      return false;
    if(!m_bmpHotFileName.empty() && !checkFile(zstring(m_bmpHotFileName)))
      return false;

    for(unsigned int i = 0; i < m_toolbarItems.size(); i++)
      if(!m_toolbarItems[i]->check())
        return false;
      for(unsigned int j = 0; j < m_commands.m_commands.size(); j++)
        if(!m_commands.m_commands[j]->check())
          return false;
        return true;
  }

  virtual void determine(XmlParserProxy& p)
  {
    XML::determineMember<Button>(m_toolbarItems,_T("BUTTON"),p);
    XML::determineMember<Text>(m_toolbarItems,_T("TEXT"),p);
    XML::determineMember<Combo>(m_toolbarItems,_T("COMBO"),p);
#ifdef USE_TICKER
    XML::determineMember<Ticker>(m_toolbarItems,_T("TICKER"),p);
#endif
    XML::determineMember<Page>(m_toolbarItems,_T("PAGE"),p);
    XML::determineMember<Separator>(m_toolbarItems,_T("SEPARATOR"),p);
    XML::determineMember<Commands>(m_commands,_T("COMMANDS"),p);
    XML::determineMember<>(m_settings,_T("SETTINGS"),p);
    XML::determineMember<>(m_bmpFileName,_T("img"),p);
    XML::determineMember<>(m_bmpHotFileName,_T("hot_img"),p);
    XML::determineMember<zstring>(m_include_xmls,_T("INCLUDEXML"),p);
  }

  // COMBOs support - we can have lot of combo controls now
  std::vector<Combo::ICBandEditCtrl*> m_ctlBandEdits;
  struct IBandEdits // iterator helper
  {
    IBandEdits(const CXmlObj* ctrl) : m_size(ctrl->m_toolbar->m_ctlBandEdits.size()),m_it(ctrl->m_toolbar->m_ctlBandEdits.begin()),m_idx(0){}
    operator bool() {return m_idx<m_size && m_idx>=0;}
    void operator ++(int) {m_idx++;m_it++;}
    void operator --(int) {m_idx--;m_it--;}
    operator int() { return m_idx+1;}
    Combo::ICBandEditCtrl* operator ->() { return (*m_it);}

  private:
    typedef vector<Combo::ICBandEditCtrl*> BandEdits;
    BandEdits::const_iterator m_it; //The iterator is a member of the class, it can direct convert to the data
    int                       m_size;
    int                       m_idx;
  };

  // find comand to execute by name
  Commands::Command* selectCommand(const zstring& name)
  {
    for(unsigned int c=0;c<m_commands.m_commands.size();c++)
      if(m_commands.m_commands[c]->m_name == name)
        return m_commands.m_commands[c];
    return 0;
  }
};


struct Toolbar : public XmlObject
{
	struct Commands : public XmlObject
	{
		struct Command : public XmlObject
		{
			zstring m_name;
			virtual bool check() {return true;}              // load (and check) additional files (if any)
			virtual bool canAddToSelect() {return false;}    // is it search "engine command"?
			virtual void apply(CBandToolBarCtrl* ) = 0;      // execute this command
			virtual void applyPresed(CBandToolBarCtrl* ctrl) {} // execute this command after button (hightlight) was pressed
			virtual void determine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_name,_T("name"),p);
			}
		};

		struct ShellExecute : public Command
		{
			zstring m_command;
			zstring m_not_found;
			zstring alertTitle;

			virtual void apply(CBandToolBarCtrl* ctrl)
			{
				HINSTANCE ret = ::ShellExecute(NULL,NULL,m_command.c_str(),NULL,NULL,SW_SHOWNORMAL);
				if(int(ret)<32)
					MessageBox(0,m_not_found.c_str(),alertTitle.c_str(),0);
			}
			virtual void determine(XmlParserProxy& p)
			{
				Command::determine(p);
				XML::determineMember<>(m_command,_T("command"),p);
				XML::determineMember<>(m_not_found,_T("not_found"),p);
			}
		};

		virtual void determine(XmlParserProxy& p)
		{
		};

		XML::vectorDel<Command*> m_commands;
	};

	struct IMenuItem  : public XmlObject
	{
		zstring m_hint;
		zstring m_command;
		zstring m_vis_id; // id of visiable "group"
		virtual void appendMenu(CMenuHandle,int& ,int) = 0;   // all this popup menu (if any)
		virtual IMenuItem* getItem(int& items,int c) = 0;     // get child menu item by position
		bool shouldShow() {return m_vis_id.empty() || isGetValue(m_vis_id);}
	};

	// parent of all Toolbar's controls
	struct ToolbarItem : public XmlObject
	{
		ToolbarItem() : m_id(-2),m_command_id(0) {}
		int                m_id;
		Commands::Command* m_command_id;
		zstring            m_hint;
		zstring            m_caption;
		zstring            m_command;
		zstring            m_vis_id; // id of visiable "group"
		zstring& getName()    {return m_caption;}
		zstring& getToolTip() {return m_hint;}

		virtual void onCommand(CBandToolBarCtrl* ) {}   // button pressed
		virtual void Invalidate(CBandToolBarCtrl* ) {}  // invalidate owner-drawn controls
		virtual DWORD OnItemPrePaint(CBandToolBarCtrl* ctrl,LPNMTBCUSTOMDRAW lpTBCustomDraw)
		{
			bool hot = (lpTBCustomDraw->nmcd.uItemState & CDIS_HOT) == CDIS_HOT;
			bool selected = (lpTBCustomDraw->nmcd.uItemState & CDIS_SELECTED) == CDIS_SELECTED;
			if(hot||selected)
			{
				lpTBCustomDraw->clrText = 200<<16;
				//lpTBCustomDraw->clrBtnHighlight = 234;

				CBrushHandle back(CreateSolidBrush(255));

				CDCHandle dc = lpTBCustomDraw->nmcd.hdc;
				dc.SetTextColor(lpTBCustomDraw->clrText);
				dc.SetBkColor(255);
				dc.SetBkMode(lpTBCustomDraw->nStringBkMode);
				dc.SelectBrush(back);
				//HFONT hFont = GetFont();
				//HFONT hFontOld = NULL;
				//if(hFont != NULL)
				//hFontOld = dc.SelectFont(hFont);
				//    RoundRect(lpTBCustomDraw->nmcd.hdc,lpTBCustomDraw->nmcd.rc.left,lpTBCustomDraw->nmcd.rc.top,lpTBCustomDraw->nmcd.rc.right,lpTBCustomDraw->nmcd.rc.bottom,20,20);
			}
			return CDRF_DODEFAULT;
		}

		virtual bool isShownAlways() {return false;}  // used to calculate min width of toolbar

		virtual IMenuItem* getItem(int c) {return 0;} // get menu item (if any) by position

		virtual BOOL StopMouseEvents() {return FALSE;} // used to disalbe mouse input for TEXT control

		image_index getBitmap(){return m_img;}         // working with icons
		void setBitmap(image_index img){m_img = img;}

		virtual bool check() {return true;}           // load (and check) additional files (if any)
		virtual void onOptionChange(CBandToolBarCtrl* ctrl) {}; // called after option page changed
		// show Popup menu
		virtual void processMenu(HWND wnd,int x, int y) {ATLASSERT(false);} 
		// called during building toolbar to apply some custom control's attributes
		virtual void change(CBandToolBarCtrl* ctrl) 
		{
			zstring s = getName();
			s += _T('\0');
			ctrl->AddStrings(s.c_str());
		};
		virtual void determine(XmlParserProxy& p)
		{
			XML::determineMember<>(m_caption,_T("caption"),p);
			XML::determineMember<>(m_img,_T("img"),p);
			XML::determineMember<>(m_command,_T("command"),p);
			XML::determineMember<>(m_hint,_T("hint"),p);
			XML::determineMember<>(m_vis_id,_T("vis_id"),p);
		}
	private:
		image_index  m_img;
	};
	//struct ToolbarItem : public XmlObject
	//{
	//	zstring& getName()    {return m_caption;}
	//	zstring& getToolTip() {return m_hint;}
	//	virtual bool check() {return true;}           // load (and check) additional files (if any)
	//	virtual void determine(XmlParserProxy& p)
	//	{
	//		XML::determineMember<>(m_caption,_T("caption"),p);
	//		XML::determineMember<>(m_img,_T("img"),p);
	//		XML::determineMember<>(m_command,_T("command"),p);
	//		XML::determineMember<>(m_hint,_T("hint"),p);
	//		XML::determineMember<>(m_vis_id,_T("vis_id"),p);
	//	};

	//	image_index getBitmap(){return m_img;}         // working with icons
	//	ToolbarItem() : m_id(-2),m_command_id(0) {}
	//	virtual IMenuItem* getItem(int c) {return 0;} // get menu item (if any) by position

	//	int                m_id;
	//	Commands::Command* m_command_id;
	//	zstring            m_hint;
	//	zstring            m_caption;
	//	zstring            m_command;
	//	zstring            m_vis_id; // id of visiable "group"
	//private:
	//	image_index  m_img;
	//};

	struct Settings : public XmlObject
	{
		Settings() {}

		// fonst used in toolbar
		struct Font : public XmlObject
		{
			int     m_size;
			zstring m_name;
			virtual void determine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_size,_T("size"),p);
				XML::determineMember<>(m_name,_T("name"),p);
			}
		};

		// colors for HightLighting
		struct Highlight : public XmlObject
		{
			vector<zstring> m_colors;
			virtual void determine(XmlParserProxy& p)
			{
				XML::determineMember<zstring>(m_colors,_T("COLOR"),p);
			}
		};

		// store setting to show popup messages
		struct Popup : public XmlObject
		{
			Popup() : m_time(0) {}
			zstring m_url;
			int     m_width;
			int     m_height;
			zstring m_show;
			int     m_time;
			virtual void determine(XmlParserProxy& p)
			{
				XML::determineMember<zstring>(m_url,_T("url"),p);
				XML::determineMember<zstring>(m_height,_T("height"),p);
				XML::determineMember<zstring>(m_width,_T("width"),p);
				XML::determineMember<zstring>(m_show,_T("Show"),p);
				XML::determineMember<zstring>(m_time,_T("Time"),p);
			}
		};

		void init()
		{
			for(unsigned int j=0;j<m_settings.size();j++)
				m_settings[j]->init();
		}

		// Setting class - parent for all objects that can be used in Options page
		struct Setting : public XmlObject
		{
			zstring  m_id;
			zstring  m_default;
			void init() { setValue(m_id,get());}
			zstring get() { return getValue(m_id,m_default.size() ? m_default : _T("1"));}
			virtual void onOpen(CBandToolBarCtrl*ctrl, IHTMLDocument3* pHtmlDoc3) = 0;
			virtual void onSelChange(IHTMLElementPtr p,zstring id) { };
			virtual void onClick(CBandToolBarCtrl*ctrl,zstring id) { };
			virtual void determine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_id,_T("id"),p);
				XML::determineMember<>(m_default,_T("default"),p);
			}
		};

		// check box in options page, also can set some default parameters for toolbar 
		struct CheckBox : public Setting
		{
			virtual void onOpen(CBandToolBarCtrl*ctrl,IHTMLDocument3* pHtmlDoc3)
			{
				if(get()==zstring(_T("1")))
				{
					IHTMLElementPtr el;
					pHtmlDoc3->getElementById(_bstr_t(m_id.c_str()),&el);
					if(el)
					{
						if(IHTMLInputElementPtr i = el)
							i->put_checked(VARIANT_TRUE);
					}
				}
			}
			virtual void onClick(CBandToolBarCtrl*ctrl,zstring id)
			{
				if(id==m_id)
					setValue(m_id,get()==zstring(_T("1")) ? _T("0") : _T("1") );
			}
		};

#ifndef NO_OPTIONS_PAGE
		// we can put some buttons in HTML page and assign some command to it
		struct Button : public Setting
		{
			zstring m_command;
			virtual void onOpen(CBandToolBarCtrl*ctrl, IHTMLDocument3* pHtmlDoc3) {}
			virtual void onClick(CBandToolBarCtrl*ctrl,zstring id)
			{
				if(id==m_id)
				{
					if(Commands::Command* c = ctrl->m_toolbar->selectCommand(m_command))
						c->apply(ctrl);
				}
			}
			virtual void determine(XmlParserProxy& p)
			{
				XML::determineMember<>(m_command,_T("command"),p);
				Setting::determine(p);
			}
		};

		// radio control on option page
		struct Radio : public Setting
		{
			zstring m_name;
			vector<zstring>  m_ids;
			virtual void onOpen(CBandToolBarCtrl*ctrl,IHTMLDocument3* pHtmlDoc3)
			{
				IHTMLElementPtr el;
				for(unsigned int i=0;i<m_ids.size();i++)
				{
					pHtmlDoc3->getElementById(_bstr_t(m_ids[i].c_str()),&el);
					if(el!=NULL && getValue(m_name,m_default)==zstring(IntToStr(i)))
						el->click();
				}
			}
			virtual void onClick(CBandToolBarCtrl*ctrl,zstring id)
			{
				for(unsigned int i=0;i<m_ids.size();i++)
					if(m_ids[i]==id)
						setValue(m_name,zstring(IntToStr(i)));
			}
			virtual void determine(XmlParserProxy& p)
			{
				XML::determineMember<zstring>(m_ids,_T("id"),p);
				XML::determineMember<>(m_name,_T("name"),p);
				Setting::determine(p);
			}
		};
		// combo box control in options page - used only to seleect default search engine
		struct Select : public Setting
		{
			virtual void onOpen(CBandToolBarCtrl*ctrl, IHTMLDocument3* pHtmlDoc3)
			{
				IHTMLElementPtr e3 = NULL;
				pHtmlDoc3->getElementById(_bstr_t(m_id.c_str()),&e3);
				if(e3)
				{
					IHTMLDocument2Ptr pHtmlDoc2 = pHtmlDoc3;
					IHTMLSelectElementPtr e = e3;
					if(e)
					{
						// register handler
						e->put_onchange(_variant_t(CHtmlEventObject<CBandToolBarCtrl>::CreateHandler(ctrl, CBandToolBarCtrl::OnSelChange, 1 )));

						for(unsigned int b=0;b<ctrl->m_toolbar->m_toolbarItems.size();b++)
						{
							zstring command = ctrl->m_toolbar->m_toolbarItems[b]->m_command;
							if(Commands::Command* c = ctrl->m_toolbar->selectCommand(command))
							{
								if(c->canAddToSelect())
								{
									IHTMLElementPtr element;
									pHtmlDoc2->createElement(_bstr_t(_T("OPTION")),&element);
									IHTMLOptionElementPtr option = element;
									option->put_text(_bstr_t(ctrl->m_toolbar->m_toolbarItems[b]->m_caption.c_str()));
									e->add(element,_variant_t(long(-1)));
								}
							}
						}

						zstring ss = get();
						long ind = ss[0] - '0';
						e->put_selectedIndex(ind);
					}
				}
			}
			virtual void onSelChange(IHTMLElementPtr p,zstring id)
			{
				IHTMLSelectElementPtr s = p;
				long ind;
				s->get_selectedIndex(&ind);
				TCHAR ss[] = _T("0");
				ss[0] += static_cast<TCHAR>(ind);
				setValue(m_id,ss);
			}
		};
#endif

		virtual void determine(XmlParserProxy& p)
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
		zstring  m_updateVer;
		zstring  m_wordfindHint;
		Highlight m_highlight;
		zstring  m_popuponload;
		Popup m_popup;
		XML::vectorDel<Font*>  m_fonts;
	};


 //  Button control
  struct Button : public ToolbarItem
  {
    ~Button() {}

    struct MenuItem  : public IMenuItem
    {
      virtual IMenuItem* getItem(int& items,int c) {return 0;}
      virtual void determine(XmlParserProxy& p)
      {
     	  XML::determineMember<>(m_hint,_T("hint"),p);
        XML::determineMember<>(m_command,_T("command"),p);
        XML::determineMember<>(m_vis_id,_T("vis_id"),p);
      }
    };

    virtual IMenuItem* getItem(int c) {return m_menu.getItem(c);}

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

      CBandToolBarCtrlFake m_ctrlBarFake;
      zstring m_type;
      zstring m_img;
      zstring m_hot_img;

      // "normal" menu item = icon + text + command to execute
      struct Command  : public MenuItem
      {
        image_index getBitmap(){return m_img;}
        void setBitmap(image_index img){m_img = img;}

        zstring m_caption;
        virtual void appendMenu(CMenuHandle handle,int& j,int id)
        {
#ifdef USE_SIMPLE_MENU
          handle.AppendMenu(MF_STRING,WM_MENU_USER + MAX_MENU_SIZE*id +j,m_caption.c_str());
#else
          CBandToolBarCtrl::_MenuItemData2* pMI = new CBandToolBarCtrl::_MenuItemData2();
          pMI->fType = MF_STRING;
          pMI->lpstrText = (LPTSTR)m_caption.c_str();
          pMI->iButton = GetBitmap(getBitmap());
          handle.AppendMenu(MF_OWNERDRAW,WM_MENU_USER + MAX_MENU_SIZE*id +j,LPCTSTR(pMI));
#endif
          if(m_command==gAppendCommand)
             handle.CheckMenuRadioItem(WM_MENU_USER + MAX_MENU_SIZE*id +0,WM_MENU_USER + MAX_MENU_SIZE*id +j,WM_MENU_USER + MAX_MENU_SIZE*id +j,MF_BYCOMMAND);
        }
        virtual void determine(XmlParserProxy& p)
        {
          XML::determineMember<>(m_img,_T("img"),p);
          MenuItem::determine(p);
          XML::determineMember<>(m_caption,_T("caption"),p);
        }
        private:
          image_index  m_img;
      };

      // separator in menu - very usefull thing :)
      struct Separator  : public MenuItem
      {
        virtual void determine(XmlParserProxy& p)  {}
        virtual void appendMenu(CMenuHandle handle,int&,int)
        {
#ifdef USE_SIMPLE_MENU
          handle.AppendMenu(MF_SEPARATOR);
#else
          CBandToolBarCtrl::_MenuItemData2* pMI = new CBandToolBarCtrl::_MenuItemData2();
          pMI->fType = MF_SEPARATOR;
          handle.AppendMenu(MF_OWNERDRAW,0,LPCTSTR(pMI));
#endif
        }
      };

      // we need more levels, more menus
      struct PopupMenu  : public MenuItem, public IMenu
      {
        zstring m_caption;
        virtual void determine(XmlParserProxy& p)
        {
          MenuItem::determine(p);
          XML::determineMember<>(m_caption,_T("caption"),p);
          XML::determineMember<Command>(m_items,_T("ITEM"),p);
          XML::determineMember<Separator>(m_items,_T("SEPARATOR"),p);
          XML::determineMember<PopupMenu>(m_items,_T("MENU"),p);
        }
        virtual void appendMenu(CMenuHandle handle,int& items,int id)
        {
          CMenuHandle menuPopup;
          menuPopup.CreatePopupMenu();

          for(unsigned int j=0;j<m_items.size();j++)
          {
            if(m_items[j]->shouldShow())
            {
              m_items[j]->appendMenu(menuPopup,items,id); /// pass right values
              items++;
            }
          }

#ifdef USE_SIMPLE_MENU
          handle.AppendMenu(MF_POPUP,(unsigned int)(HMENU)menuPopup,m_caption.c_str());
#else
          CBandToolBarCtrl::_MenuItemData2* pMI = new CBandToolBarCtrl::_MenuItemData2();
          pMI->fType = MF_STRING;
          pMI->lpstrText = (LPTSTR)m_caption.c_str();
          pMI->iButton = -1;//m_img;
          handle.AppendMenu(MF_OWNERDRAW|MF_POPUP,(unsigned int)(HMENU)menuPopup,LPCTSTR(pMI));
#endif
        }
        IMenuItem* getItem(int& items, int c)
        {
          for(unsigned int j=0;j<m_items.size();j++)
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

      IMenuItem* getItem(int c)
      {
        int items = 0;
        for(unsigned int j=0;j<m_items.size();j++)
        {
          if(IMenuItem* s = m_items[j]->getItem(items,c))
            return s;
          if(items==c)
            return m_items[j];
          items++;
        }
        return 0;
      }

      virtual void determine(XmlParserProxy& p)
      {
        XML::determineMember<Command>(m_items,_T("ITEM"),p);
        XML::determineMember<Separator>(m_items,_T("SEPARATOR"),p);
        XML::determineMember<PopupMenu>(m_items,_T("MENU"),p);
        XML::determineMember<>(m_type,_T("type"),p);
        XML::determineMember<>(m_img,_T("img"),p);
        XML::determineMember<>(m_hot_img,_T("hot_img"),p);
      }
    }m_menu;


    virtual void processMenu(HWND wnd,int x, int y)
    {
      CMenu menuPopup;
      menuPopup.CreatePopupMenu();
      int items = 0;
      for(unsigned int j=0;j<m_menu.m_items.size();j++)
      {
        if(m_menu.m_items[j]->shouldShow())
        {
          m_menu.m_items[j]->appendMenu((CMenuHandle)menuPopup,items,m_id);
          items++;
        }
      }
      ::TrackPopupMenu(menuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, wnd, NULL);
      CBandToolBarCtrl::_MenuItemData2::MenuItems.clear();
    }

    virtual void onOptionChange(CBandToolBarCtrl* ctrl)
    {
      TBBUTTONINFO tbi;
      tbi.cbSize = sizeof TBBUTTONINFO;
      tbi.dwMask = TBIF_TEXT | TBIF_IMAGE;
#ifdef _HUNTBAR
  #include "Toolband12\DescriptiveText.cpp"
#else
      if(!isGetValue(_T("DescriptiveText")))
        tbi.pszText = _T("");
      else
        tbi.pszText = const_cast<TCHAR*>(getName().c_str());
      if(!isGetValue(_T("DescriptiveImage")))
        tbi.iImage = -2;
      else
        tbi.iImage = GetBitmap(getBitmap());
      ctrl->SetButtonInfo(WM_USER+m_id, &tbi);
      ctrl->hideIfNeeded(m_id,m_vis_id);
#endif

      if(!m_menu.m_img.empty())
      {
        RECT rc;
        ctrl->GetItemRect(m_id, &rc);
        m_menu.m_ctrlBarFake.MoveWindow(&rc);
      }
    }
    virtual bool check()
    {
      if(m_menu.m_img.empty())
        return true;
      zstring menuimg = m_menu.m_img;
      zstring hotimage = m_menu.m_hot_img;
#ifdef _ODBC_DIRECT
      return true;
#endif
      return checkFile(menuimg) && (hotimage.size()==0 || checkFile(hotimage));
    }
    virtual void change(CBandToolBarCtrl* ctrl)
    {
      ToolbarItem::change(ctrl);
      RECT rc;
      ctrl->GetItemRect(m_id, &rc);
      if(!m_menu.m_type.empty())
      {
        TBInfo(ctrl,m_id).style(BTNS_SHOWTEXT | TBSTYLE_AUTOSIZE | (m_menu.m_type == _T("Single") ? BTNS_WHOLEDROPDOWN : TBSTYLE_DROPDOWN) ); 
      }
      if(!m_menu.m_img.empty())
      {
        CBitmap(ctrl->LoadImage(m_menu.m_img)).GetSize(m_menu.m_ctrlBarFake.m_size);

#ifdef _DYNABAR
        m_menu.m_ctrlBarFake.m_size.cx /=2;
#endif
        CDC dc = ::GetDC(NULL);
        dc.SelectFont(ctrl->font.m_hFont);
        SIZE size;
        GetTextExtentPoint(dc,m_caption.c_str(),m_caption.size(),&size);

        {TBInfo(ctrl,m_id).style(TBSTYLE_BUTTON).size(size.cx + m_menu.m_ctrlBarFake.m_size.cx + ADD_TO_DROP_MENU);}

        m_menu.m_ctrlBarFake.m_id = m_id;
        m_menu.m_ctrlBarFake.setroot(ctrl);

        rc.bottom = TB_HEIGHT;

        m_menu.m_ctrlBarFake.Create(*ctrl, rc, NULL,
          WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
          CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN |
          TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST ,0); // | TBSTYLE_TRANSPARENT,0);
      }
    }
    virtual void determine(XmlParserProxy& p)
    {
      ToolbarItem::determine(p);
      XML::determineMember<>(m_menu,_T("MENU"),p);
    }
  }; // end of Button class

	struct Text : public Button
	{
		virtual void determine(XmlParserProxy& p)
		{
		};
	};

	struct Combo : public ToolbarItem
	{
		virtual void determine(XmlParserProxy& p)
		{
		};
	};

	struct Ticker : public ToolbarItem
	{
		virtual void determine(XmlParserProxy& p)
		{
		};
	};

	struct Page : public ToolbarItem
	{
		virtual void determine(XmlParserProxy& p)
		{
		};
	};

	struct Separator : public ToolbarItem
	{
		virtual void determine(XmlParserProxy& p)
		{
		};
	};


	virtual void determine(XmlParserProxy& p)
	{
		XML::determineMember<Button>(m_toolbarItems,_T("BUTTON"),p);
		XML::determineMember<Text>(m_toolbarItems,_T("TEXT"),p);
		XML::determineMember<Combo>(m_toolbarItems,_T("COMBO"),p);
#ifdef USE_TICKER
		XML::determineMember<Ticker>(m_toolbarItems,_T("TICKER"),p);
#endif
		XML::determineMember<Page>(m_toolbarItems,_T("PAGE"),p);
		XML::determineMember<Separator>(m_toolbarItems,_T("SEPARATOR"),p);
		XML::determineMember<Commands>(m_commands,_T("COMMANDS"),p);
		XML::determineMember<>(m_settings,_T("SETTINGS"),p);
		XML::determineMember<>(m_bmpFileName,_T("img"),p);
		XML::determineMember<>(m_bmpHotFileName,_T("hot_img"),p);
		XML::determineMember<zstring>(m_include_xmls,_T("INCLUDEXML"),p);
	};

	int Find(const zstring& str)
	{
		for(unsigned int b=0;b<m_toolbarItems.size();b++)
			if(m_toolbarItems[b]->m_command==str)
				return b;
		return -1;
	}

	ToolbarItem* Find2(const zstring& str)
	{
		int b = Find(str);
		if(b>=0)
			return m_toolbarItems[b];
		return 0;
	}

	IMenuItem* getMenuItem(int wID)
	{
		unsigned int code = wID-WM_MENU_USER;
		unsigned int id = code/MAX_MENU_SIZE;
		if(id>=m_toolbarItems.size())
			return 0;
		return m_toolbarItems[id]->getItem(code - id*MAX_MENU_SIZE);
	}

	// do all files loaded (bmp, html)?
	bool check()
	{
		if(!m_bmpFileName.empty() && !checkFile(zstring(m_bmpFileName)))
			return false;
		if(!m_bmpHotFileName.empty() && !checkFile(zstring(m_bmpHotFileName)))
			return false;

		for(unsigned int i = 0; i < m_toolbarItems.size(); i++)
			if(!m_toolbarItems[i]->check())
				return false;
		for(unsigned int j = 0; j < m_commands.m_commands.size(); j++)
			if(!m_commands.m_commands[j]->check())
				return false;
		return true;
	}

	// find comand to execute by name
	Commands::Command* selectCommand(const zstring& name)
	{
		for(unsigned int c=0;c<m_commands.m_commands.size();c++)
			if(m_commands.m_commands[c]->m_name == name)
				return m_commands.m_commands[c];
		return 0;
	}

	zstring                      m_bmpFileName;
	zstring                      m_bmpHotFileName;
	vector<zstring>              m_include_xmls;
	Commands					 m_commands;
	Settings					 m_settings;
	XML::vectorDel<ToolbarItem*> m_toolbarItems;
};

