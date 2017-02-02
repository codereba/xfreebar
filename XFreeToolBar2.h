#pragma once

#include "resource.h"
#include <exdispid.h>	// dispatch IDs for DWebBrowserEvents2
#include <map>
#include "CmnCtrl.h"
#include "xmlparserimpl.h"
#include "ChevDrop.h"
#include "ChevBtn.h"
#include "XToolBarOperation.h"
#include "ConfigHost.h"
#include "XToolBarConfig.h"
#include "GlobalDef.h"
#include "XHtmlSearch.h"
#include "XHtmlFunction.h"
#include "XResponsor.h"

using namespace XML;
using namespace std;

#ifndef TBSTYLE_EX_MIXEDBUTTONS
#define TBSTYLE_EX_MIXEDBUTTONS		0x00000008
#define TBSTYLE_EX_HIDECLIPPEDBUTTON	0x00000010
#define CCM_SETVERSION				(CCM_FIRST + 7)
#define CCM_GETVERSION				(CCM_FIRST + 8)
#endif

#ifndef BTNS_SHOWTEXT
#define BTNS_SHOWTEXT		0x0040
#endif

#ifndef BTNS_WHOLEDROPDOWN
#define	BTNS_WHOLEDROPDOWN	0x0080
#endif


#define INIT_BTN_ID(index) ( WM_USER + index )

class XFreeToolBar;
class XToolBarHost;
class XFreeToolBarReflector :
	public CWindowImpl<XFreeToolBarReflector, CToolBarCtrl>, 
	public CCustomDraw<XFreeToolBarReflector>
	//public CCommandBarCtrlImpl<XFreeToolBarReflector, CCommandBarCtrlBase>
{
public:
	XFreeToolBarReflector(void){}
	virtual ~XFreeToolBarReflector(void)
	{
		if(::IsWindow(m_hWnd))
			DestroyWindow();
		m_pToolBarHost = NULL;
	}
#if defined(BLUE_ON_OVER_TEXT) || defined(USE_TICKER)
    DWORD OnPrePaint(INT32 , LPNMCUSTOMDRAW ) {return CDRF_NOTIFYITEMDRAW;}
	DWORD OnItemPrePaint(INT32 , LPNMCUSTOMDRAW pNmHdr);
#endif
#define REFLECT_WND_CLASS_NAME _T("XFreeBarReflector")
	DECLARE_WND_SUPERCLASS( REFLECT_WND_CLASS_NAME, CToolBarCtrl::GetWndClassName())

    // Make sure we reflect everything back to the child
	BEGIN_MSG_MAP_EX(XFreeToolBarReflector)
		//MESSAGE_HANDLER(WM_PAINT, OnHookPaint)
		//if(uMsg == WM_PAINT)
		//{ 
		//bHandled = TRUE; 
		//lResult = OnHookPaint(uMsg, wParam, lParam, bHandled); 
		//if(bHandled) 
		//	return TRUE;
		//}
		CHAIN_MSG_MAP( CCustomDraw<XFreeToolBarReflector> )
		
		if(uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->idFrom >= 0 && ((LPNMHDR)lParam)->idFrom <= 0xffffffff) 
		{ 
			bHandled = TRUE; 

			ATLTRACE( _T( "Currnet notified code is %u, Tooltip notified code is %u\r\n" ), 
				((LPNMHDR)lParam)->code, 
				TTN_NEEDTEXT  );
			lResult = OnNotifyRangeHandler((int)wParam, (LPNMHDR)lParam, bHandled); 
			if(bHandled) 
				return TRUE; 
		}

		//NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnToolTip)

		if(uMsg == WM_NOTIFY && TTN_GETDISPINFO == ((LPNMHDR)lParam)->code)
		{

			bHandled = TRUE;
			lResult = OnToolTip((int)wParam, (LPNMHDR)lParam, bHandled);
			if(bHandled)
				return TRUE;
		}
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	XToolBarHost *m_pToolBarHost;
protected:
    LRESULT OnToolTip(INT32,LPNMHDR lParam,BOOL&);
	LRESULT OnNotifyRangeHandler(INT32,LPNMHDR lParam,BOOL&); 
	LRESULT OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		//ATLTRACE("Enter the fake toolbar paint\n");
		return 0;
	}
protected:
};


typedef struct tagXBTNINFO
{
	TBBUTTON *pBtnData;
	DWORD dwRealWidth;
	DWORD dwStyle;
	DWORD dwGroup;
	BOOL bRealHide;
	LPVOID lpExtension;
} XBTNINFO;

#include "XHtmlOption.h"
struct XToolBarConfig;
class XFreeToolBar : //public CWindowImpl<XFreeToolBar, CToolBarCtrl>
						 public CCommandBarCtrlImpl<XFreeToolBar, CCommandBarCtrlBase>,
						 public CPugiObjWrap,
						 public CCustomDraw< XFreeToolBar >,
						 public IConfigHost,
						 public IComboHost,
						 public IHtmlResponsor

{
public:
	XFreeToolBar(void);
	virtual ~XFreeToolBar(void);

	std::map<char, char> tipMaskLen;
	std::map<XString, INT32> m_CustomImage;
	typedef CCommandBarCtrlImpl<XFreeToolBar, CCommandBarCtrlBase>	_baseClass;

#define XFREEBAR_CLASS_NAME _T("XFreeBar") 
	DECLARE_WND_SUPERCLASS(XFREEBAR_CLASS_NAME, CToolBarCtrl::GetWndClassName())

	HWND CreateSimpleToolBarCtrl(HWND hWndParent, DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = ATL_IDW_TOOLBAR);
	//bool UpdateToolBarCtrlType(TextOptions nTextOption);
	void UpdateToolBarElement(INT32 nID, bool bEnable);
	void EnableHighlight(BOOL e);
	void CalcXFreeBarWidth();
	IHTMLDocument2Ptr GetHtmlDocument();
	INT32 DepartToolBar();
#define MODE_ALL_WIDTH 0x03
#define MODE_FIXED_WIDTH 0x02
#define MODE_FREE_WIDTH 0x01
	INT32 SetBtnsWidth( DWORD dwMode  = MODE_ALL_WIDTH );
	INT32 SetBtnsWidth( INT32 uStartIndex, INT32 uEndIndex );
	INT32 SetBtnsWidthByIndexes( UINT32 *pBtnIndexes, UINT32 uBtnCount );
	INT32 MoveButton(INT32 nIndexOld, INT32 nIndexNew);
	INT32 DrawDragInsertMark( INT32 nPos, CDCHandle dc);
	VOID HideBtnIfNeeded(INT32 nIndexOrID,const XString& strCmd, BOOL bIsIndex = TRUE );
	VOID RunSearch();
	BOOL AddCustomImage(XString strImg, XString strHotImg = _T(""));
	INT32 GetImageId(XString strImg /*Not Hot Image*/);
	INT32 AddXBtns(  UINT32 uBtnCount, XBTNINFO *pBtnInfo, BOOL bChevronInited = TRUE);
	INT32 DeleteXBtns( UINT32 uBtnIndex );

	void SetToolBarButtonInfo(INT32 nID, DWORD dwMask, BYTE fsStyle);
	//void SetWebBrowser(IWebBrowser2* pWebBrowser) {m_pWebBrowser = pWebBrowser;}
	INT32 UpdateSearchedWords();
	INT32 GetBtnRealWidth( INT32 nIndex );
	VOID SetBtnWidthInfo( INT32 nIndex, INT32 nButtonWidth);
	void UpdateWidth();
	virtual XFreeToolBar* ParentToolBar() {return this;}
	INT32 BuildToolBar();
	bool parseXML(XString& path );
	// load XML files, other media files and check that we load them all
	bool ParseToolbarFromXML( bool breadreg = true);
	void UpdateToolTipInfo(UINT uNewInfo, XString strCommand);
	INT32 XToolBarUpdate();
	INT32 GetBtnConfigInfo( INT32 nIndex, XToolbarItem **ppBtnConf );
	BOOL SubclassRebarHost();
	BOOL UnsubclassRebarHost();
	INT32 SetFonts();
	INT32 GetXBtnInfo( INT32 nIndex, XBTNINFO **ppInfo );
	LPCTSTR GetErrorMsg( INT32 nErrorCode );

public:
	BEGIN_MSG_MAP_EX( XFreeToolBar )
		CHAIN_MSG_MAP( CCustomDraw<XFreeToolBar> )
		REFLECTED_COMMAND_RANGE_HANDLER( BUTTON_COMMAND_ID_BEGIN, BUTTON_COMMAND_ID_END, OnButton )
		COMMAND_RANGE_HANDLER( MENU_COMMAND_ID_BEGIN, MENU_COMMAND_ID_END, OnCommand )
		REFLECTED_NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnButtonDropDown)
		REFLECTED_NOTIFY_CODE_HANDLER(TBN_QUERYINSERT, OnQueryInsert)
		REFLECTED_NOTIFY_CODE_HANDLER(TBN_QUERYDELETE, OnQueryDelete)
		MESSAGE_HANDLER(GetGetBarMessage(), OnInternalGetBar)
		DEFAULT_REFLECTION_HANDLER()
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnHookMouseMove)
		//MESSAGE_HANDLER(WM_MOUSELEAVE, OnHookMouseLeave )
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnHookLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnHookLButtonUp)
		//MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnHookMouseMove)  
		MESSAGE_HANDLER(WM_SETCURSOR, OnHookSetCursor)
		MESSAGE_HANDLER(WM_PAINT, OnHookPaint)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnHookEraseBkGnd )
		MESSAGE_HANDLER(WM_TIMER, OnHookTimer)
		MESSAGE_HANDLER(WM_SETCURSOR, OnHookSetCursor)


		//MESSAGE_HANDLER(TB_HITTEST, OnHookTBHitTest)
		//MESSAGE_HANDLER(TB_GETITEMRECT, OnHookGetItemRect)


		MESSAGE_HANDLER(WM_SIZE, OnHookSize)
		//MSG_WM_SIZE(OnSize);
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuItem)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		//MSG_WM_ACTIVATE(OnInit)

		//handle the rebar chevron messages, does not require reflection
	ALT_MSG_MAP(1)
		// to make hook and catch keyboard
		MESSAGE_HANDLER(GetGetBarMessage(), OnParentInternalGetBar)
		//COMMAND_RANGE_HANDLER(WM_USER+0, ALL_COMMAND_BEGIN, OnButton)
		//NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnButtonDropDown)

		//MESSAGE_HANDLER(WM_MOUSEMOVE, OnHookMouseMove) // other toolbars
		//MESSAGE_HANDLER(WM_LBUTTONDOWN, OnHookMouseMove) // other toolbars
		//MESSAGE_HANDLER(WM_SIZE, OnHookRebarSize)

	//ALT_MSG_MAP(2)
		//NOTIFY_CODE_HANDLER(RBN_CHEVRONPUSHED, OnChevronPushed)
		//MESSAGE_HANDLER(WM_SIZE, OnHookIEFrameSize)

	ALT_MSG_MAP(3)		// Message hook messages
		//MESSAGE_HANDLER(WM_KEYUP, OnHookKeyDown)
		//MESSAGE_HANDLER(WM_KEYDOWN, OnHookKeyDown)
	END_MSG_MAP()



	STDMETHOD(TranslateAcceleratorIO)(LPMSG pMsg);

public:
	//handle the toolbar resource
	//struct _AtlToolBarData
	//{
	//	WORD wVersion;
	//	WORD wWidth;
	//	WORD wHeight;
	//	WORD wItemCount;
	//	WORD *items()
	//	{ return (WORD*)(this + 1); }
	//};

	LRESULT OnHookTimer(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHookPaint(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookEraseBkGnd(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookGetItemRect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHookTBHitTest(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnMenuItem(UINT , WPARAM wParam, LPARAM lParam, BOOL& ) {return CPugiObjWrap::OnMenuItem(wParam);}  
	LRESULT OnHookKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookMouseLeave( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled );
	//LRESULT OnHookRebarSize(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	//LRESULT OnHookIEFrameSize(HWND hWnd, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHookLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	void  OnTimer(UINT wParam, TIMERPROC lParam);
	LRESULT OnButtonDropDown(INT32 , LPNMHDR pNmHdr, BOOL& bHandled);
	LRESULT OnQueryDelete(INT32 idCtrl, LPNMHDR pNmHdr, BOOL& bHandled);
	LRESULT OnQueryInsert(INT32 idCtrl, LPNMHDR pNmHdr, BOOL& bHandled);
	LRESULT OnChevronPushed(INT32, LPNMHDR pNmHdr, BOOL& bHandled);

	INT32 OnHtmlMouseBtnDown( LPVOID pParam );
	void OnSelChange( DISPID id, VARIANT* pVarResult );
	//LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnConfig();
	LRESULT OnAbout();
	LRESULT OnToolbarNeedText(INT32 /*idCtrl*/, LPNMHDR pNmHdr, BOOL &/*bHandled*/);
	HRESULT OnIEBorderResize(LPCRECT prcIEBorder);
	LRESULT OnHookSize(UINT32 uMsg, UINT wParam, LPARAM lParam, BOOL &bHandled);
	INT32 XFreeBarCustomize();
	INT32 InitChevronBtn();
	INT32 AddChevronBtn();
	INT32 AddRightSideBtns();
	INT32 AddLeftSideBtns();
	INT32 DeleteChevronBtn();
	INT32 MapBtnIDFromIndex( INT32 nIndex );
	INT32 MapBtnIndexFromID( WORD wBtnID );
	INT32 ReplaceButtonText( INT32 nButtonSrc, INT32 nButtonDst);
	INT32 HideHilightBtn( INT32 nIndex, BOOL bHide = TRUE, INT32 nRealHide = -1 );
	INT32 SetHilightBtnText(INT32 nIndex, TCHAR *lpszText);
	INT32 AddHilightBtns( );
	INT32 InitNormBtn();
	INT32 IsRealHideBtn( INT32 nButtonInfoIndex );
	INT32 RealHideBtn( INT32 nIndex, BOOL bDispHide = TRUE, INT32 nRealHide = -1);
	VOID SetBtnRealWidth( INT32 nIndex, INT32 nButtonWidth);
	INT32 DumpAllButtonInfo();
	INT32 CalcFixedRightSideBtnWidth();
	INT32 CalcFixedLeftSideBtnWidth();
	INT32 IsFixedButton( INT32 nIndex );
	INT32 IsSplitter( INT32 nIndex );
	XToolbarItem* GetBtnConf( INT32 nIndex );
	VOID SetHtmlAnalyzer( XHtmlSearch *pHtmlSearch )
	{
		m_pHtmlSearch = pHtmlSearch;
	}

	VOID SetHtmlFunction( XHtmlFunction *pHtmlFunction )
	{
		m_pHtmlFunc = pHtmlFunction;
	}

	INT32 InitXBtnInfos();
	INT32 DestroyXBtnInfos();
	INT32 ReloadToolBar();
	INT32 WhenLButtonDown();
	INT32 WhenLButtonUp();
	INT32 GetToolTipInfo( WORD wID, TCHAR **lppszTooltip );
	INT32 TuneToolBarSizeInBorder();

	LRESULT OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
public:

#ifdef _ODBC_DIRECT
#include "Toolband7\ToolbandInc.h"
#endif

	XToolBarConfig *GetToolBarConfig()
	{
		return m_pXConfig;
	}

	//Get CReBar instance
	HWND FindRebar(HWND hwndStart);

	void DrawItem3D( LPDRAWITEMSTRUCT lpDrawItemStruct )
	{
		ATLTRACE( _T( "Enter DrawItem3D\r\n" ) );
		_MenuItemData* pmd = (_MenuItemData*)lpDrawItemStruct->itemData;
		CDCHandle dc = lpDrawItemStruct->hDC;
		const RECT& rcItem = lpDrawItemStruct->rcItem;
		XFreeToolBar* pT = static_cast<XFreeToolBar*>(this);

		if(pmd->fType & MFT_SEPARATOR)
		{
			// draw separator
			RECT rc = rcItem;
			rc.top += (rc.bottom - rc.top) / 2;	// vertical center
			dc.DrawEdge(&rc, EDGE_ETCHED, BF_TOP);	// draw separator line
		}
		else		// not a separator
		{
			BOOL bDisabled = lpDrawItemStruct->itemState & ODS_GRAYED;
			BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
			BOOL bChecked = lpDrawItemStruct->itemState & ODS_CHECKED;
			BOOL bHasImage = FALSE;

			if(LOWORD(lpDrawItemStruct->itemID) == (WORD)-1)
				bSelected = FALSE;
			RECT rcButn = { rcItem.left, rcItem.top, rcItem.left + m_szButton.cx, rcItem.top + m_szButton.cy };			// button rect
			::OffsetRect(&rcButn, 0, ((rcItem.bottom - rcItem.top) - (rcButn.bottom - rcButn.top)) / 2);	// center vertically

			INT32 iButton = pmd->iButton;
			if(iButton >= 0)
			{
				bHasImage = TRUE;

				// calc drawing point
				SIZE sz = { rcButn.right - rcButn.left - m_szBitmap.cx, rcButn.bottom - rcButn.top - m_szBitmap.cy };
				sz.cx /= 2;
				sz.cy /= 2;
				POINT point = { rcButn.left + sz.cx, rcButn.top + sz.cy };

				// draw disabled or normal
				if(!bDisabled)
				{
					// normal - fill background depending on state
					if(!bChecked || bSelected)
					{
						dc.FillRect(&rcButn, (bChecked && !bSelected) ? COLOR_3DLIGHT : COLOR_MENU);
					}
					else
					{
						COLORREF crTxt = dc.SetTextColor(::GetSysColor(COLOR_BTNFACE));
						COLORREF crBk = dc.SetBkColor(::GetSysColor(COLOR_BTNHILIGHT));
						CBrush hbr(CDCHandle::GetHalftoneBrush());
						dc.SetBrushOrg(rcButn.left, rcButn.top);
						dc.FillRect(&rcButn, hbr);
						dc.SetTextColor(crTxt);
						dc.SetBkColor(crBk);
					}

					// draw pushed-in or popped-out edge
					if(bSelected || bChecked)
					{
						RECT rc2 = rcButn;
						dc.DrawEdge(&rc2, bChecked ? BDR_SUNKENOUTER : BDR_RAISEDINNER, BF_RECT);
						if(m_HotList.m_hImageList)
							m_HotList.Draw(dc,iButton, point, ILD_TRANSPARENT);
					}
					else
					{
						// draw the image
						::ImageList_Draw(m_hImageList, iButton, dc, point.x, point.y, ILD_TRANSPARENT);
					}
				}
				else
				{
					HBRUSH hBrushBackground = ::GetSysColorBrush(COLOR_MENU);
					pT->DrawBitmapDisabled(dc, iButton, point, hBrushBackground);
				}
			}
			else
			{
				// no image - look for custom checked/unchecked bitmaps
				CMenuItemInfo info;
				info.fMask = MIIM_CHECKMARKS | MIIM_TYPE;
				::GetMenuItemInfo((HMENU)lpDrawItemStruct->hwndItem, lpDrawItemStruct->itemID, MF_BYCOMMAND, &info);
				if(bChecked || info.hbmpUnchecked != NULL)
				{
					BOOL bRadio = ((info.fType & MFT_RADIOCHECK) != 0);
					bHasImage = pT->DrawCheckmark(dc, rcButn, bSelected, bDisabled, bRadio, bChecked ? info.hbmpChecked : info.hbmpUnchecked);
				}
			}

			// draw item text
			INT32 cxButn = m_szButton.cx;
			COLORREF colorBG = ::GetSysColor(bSelected ? COLOR_HIGHLIGHT : COLOR_MENU);
			if(bSelected || lpDrawItemStruct->itemAction == ODA_SELECT)
			{
				RECT rcBG = rcItem;
				if(bHasImage)
					rcBG.left += cxButn + s_kcxGap;
				dc.FillRect(&rcBG, bSelected ? COLOR_HIGHLIGHT : COLOR_MENU);
			}

			// calc text rectangle and colors
			RECT rcText = rcItem;
			rcText.left += cxButn + s_kcxGap + s_kcxTextMargin;
			rcText.right -= cxButn;
			dc.SetBkMode(TRANSPARENT);
			COLORREF colorText = ::GetSysColor(bDisabled ?  (bSelected ? COLOR_GRAYTEXT : COLOR_3DSHADOW) : (bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT));

			// font already selected by Windows
			if(bDisabled && (!bSelected || colorText == colorBG))
			{
				// disabled - draw shadow text shifted down and right 1 pixel (unles selected)
				RECT rcDisabled = rcText;
				::OffsetRect(&rcDisabled, 1, 1);
				pT->DrawMenuText(dc, rcDisabled, pmd->lpstrText, ::GetSysColor(COLOR_3DHILIGHT));
			}
			pT->DrawMenuText(dc, rcText, pmd->lpstrText, colorText); // finally!
		}
	}

	public:
		DWORD OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			BOOL bHandled = FALSE;
			
			if(idCtrl == 0) return 0;
			return CDRF_NOTIFYPOSTPAINT;
		}

		DWORD OnPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			BOOL bHandled = FALSE;
			if(idCtrl == 0) return 0;
			OnHookPaint(WM_PAINT, 0, 0,bHandled);
			return CDRF_NOTIFYITEMDRAW;
		}

		DWORD OnPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			if(idCtrl == 0) return 0;
			
			return CDRF_DODEFAULT;
		}

		DWORD OnPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			if(idCtrl == 0) return 0;
			
			return CDRF_DODEFAULT;
		}

		DWORD OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			if(idCtrl == 0) return 0;
			
			return CDRF_DODEFAULT;
		}

		DWORD OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			if(idCtrl == 0) return 0;
			
			return CDRF_NOTIFYPOSTERASE;
		}

		DWORD OnItemPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			if(idCtrl == 0) return 0;
			
			return CDRF_DODEFAULT;
		}

		DWORD OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
		{
			if(idCtrl == 0) return 0;
			
			return CDRF_DODEFAULT;
		}

protected:
	INT32 OnOptionCheckBoxOpen( IHTMLDocument3* pHtmlDoc3 ,XString strSetting, XString strID );
	INT32 OnOptionCheckBoxClick( XString strCurID, XString strOrgID, XString strSetting );
	INT32 OnOptionRadioOpen( IHTMLDocument3* pHtmlDoc3 );
	INT32 OnOptionRadioClick( XString strID );
	INT32 OnOptionSelectOpen( IHTMLDocument3 *pHtmlDoc3, XString strID, XString strSetting );
	INT32 OnOptionSelectSelChange( XString strID );
	INT32 OnOptionButtonClick( XString strID );
	INT32 OnCommandSpecialApply( XString strType, XString strName, XString strDisplay );
	INT32 OnCommandScriptApply( LPCTSTR lpszBody );
	INT32 OnCommandPopClockApply( XString strBlockType );
	INT32 OnCommandPopClockPressed();
	INT32 OnCommandWebJumpApply( XString strURL, XString strPost, XString strNewWindow, XString strOption, XString strTarget );
	INT32 OnWebJumpLocalHTMLApply();
	INT32 OnCommandShellApply( INT32 nType, XString strCmd, XString strCantFound);
	INT32 OnCommandHilightApply();
	INT32 OnCommandHilightPressed();
	INT32 OnToolbarItemPrePaint( LPNMTBCUSTOMDRAW lpTBCustomDraw );
	INT32 OnToolbarItemChange( INT32 nIndex );
	INT32 OnItemSeparatorChange( INT32 nIndex );
	INT32 OnItemSeparatorOptionChange( INT32 nIndex, XString strVisId );
	INT32 OnDragHandleDragStop( VOID *pParam );
	INT32 OnMenuCommandAppendMenu( CMenuHandle &HanldeMenu, XString &strCaption, INT32 nImgIndex, XString strCheckKey, XString strCmd, INT32& nMenuIndex, INT32 nMenuItemIndex );
	INT32 OnItemButtonOptionChange( VOID *Param );
	INT32 OnItemButtonChange( VOID *pParam );
	INT32 OnItemPageOptionChange( VOID *pParam );
	INT32 OnItemPageChange( VOID *pParam );
	INT32 OnItemComboChange( VOID *pParam );
	INT32 OnItemComboOptinoChange( VOID *pParam );
	INT32 OnDragHandleOptionChange( VOID *pParam );
	INT32 OnPopupMenuAppenMenu( CMenuHandle &HandleMenu, LPCTSTR lpszTitle, CMenuHandle &MenuPopup );
	XString GetCommandHighlightColor( INT32 nIndex );
	INT32 OnMenuSeparatorAppendMenu( CMenuHandle &HandleMenu );
	INT32 OnConfigWebBrowserBeforeNavigate( XString strURL );
	INT32 OnComboProcess();
	INT32 OnComboCommand();
	INT32 OnComboSetFocus();
	INT32 OnComboKillFocus();
	INT32 OnSplitterGetRegion( INT32 nID, LPRECT pRect );
	INT32 ClearTempMenuItems();
	INT32 OnHtmlPageDownloaded( VOID *pParam );
	INT32 OnMouseBtnDown( VOID *pParam );
	INT32 HtmlRespIsInited();
	INT32 InvalidateButtonByIndex( INT32 nIndex );
	INT32 InvalidateButtonByID( INT32 nID );
	INT32 CursorIsOnChevronMenu();

public:
	CContainedWindow m_RebarContainer;
	//CContainedWindow m_WebBrowserContainer;
	CReBarCtrl m_ctrlRebar;
	CChevDrop *m_pChevDrop;
	XResponsor *m_pChevResponsor;
	//IWebBrowser2Ptr m_pWebBrowser;
	XToolBarHost *m_pToolBarHost;
	XHtmlSearch *m_pHtmlSearch;
	XHtmlFunction *m_pHtmlFunc;
	CFontHandle m_Font;
	HMODULE m_hImageDll;
	HANDLE m_hTickerThread;
	XBTNINFO *m_pXBtnInfos;
	TBBUTTON *m_pBtnDatas;
	XToolBarConfig *m_pXConfig;

	vector<string> m_VectorAllFindKeyWord;
	BOOL m_bToolbarInited;
	//XChevBtn ChevBtn;
	BOOL m_bLButtonDown;
	UINT32 m_uBtnCount;
	BOOL m_bHtmlRespInited;
	//INT32 m_nKeyWordCount;

	INT32 m_nCurWidth;
	INT32 m_nMinWidth;
	INT32 m_nButtonCount;
	INT32 m_nLastPresedButton;
	BOOL m_bLBtnDown;
	BOOL m_bSplitFocused;
	INT32 m_nChevBtnIndex;
	INT32 m_nDragingImage;
	INT32 m_nDragingButton;
	INT32 m_nNormalButtonCount;
};

struct _XFreeMenuItemData : CCommandBarCtrlImpl< XFreeToolBar, CCommandBarCtrlBase >::_MenuItemData
{
	_XFreeMenuItemData() { MenuItems.push_back( this );}
	~_XFreeMenuItemData() 
	{
	}

	static XML::vectorDel< _XFreeMenuItemData* > MenuItems;
};