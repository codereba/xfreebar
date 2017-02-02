#if !defined(AFX_TABPAGESSL_H__619331B3_7DE7_4DB1_A039_2103E87E8E71__INCLUDED_)
#define AFX_TABPAGESSL_H__619331B3_7DE7_4DB1_A039_2103E87E8E71__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CTabPageSSL declaration
#include <atlddx.h>
#include <atldlgs.h>
#include <atlframe.h>

class CTabPageSSL : public CDialogImpl<CTabPageSSL>, //public CUpdateUI<CTabPageSSL>,
                 public CMessageFilter, public CIdleHandler,
                 public CWinDataExchange<CTabPageSSL>,
                 public CCustomDraw<CTabPageSSL>,
                 public CDialogResize<CTabPageSSL>
{
public:
// Construction
	CTabPageSSL ()
	{
		m_bRouteCommand = false;
		m_bRouteCmdMsg = false;
		m_bRouteNotify = false;
	}// Default Constructor
	// Destruction

	~CTabPageSSL () {
	}

	BEGIN_MSG_MAP(CTabCtrlWithDisableImpl)
		//MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		//MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		//MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		MESSAGE_HANDLER(WM_COMMAND, OnCmdMsg)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		//REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGING, OnSelChanging)
		//REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnSelChange)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOK)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CTabPageSSL>, 1)
		DEFAULT_REFLECTION_HANDLER()
		//CHAIN_MSG_MAP( CDialogImpl<CTabPageSSL>)
	END_MSG_MAP()
	// Enable/disable command routing to the parent.
	void EnableRouteCommand(bool bRoute = true) { m_bRouteCommand = bRoute; };
	bool IsRouteCommand() { return m_bRouteCommand; };
	// Enable CmdMsg routing to the parent.
	void EnableRouteCmdMsg(bool bRoute = true) { m_bRouteCmdMsg = bRoute; };
	bool IsRouteCmdMsg() { return m_bRouteCmdMsg; };
	// Enable/Disable Notify routing to the parent.
	void EnableRouteNotify(bool bRoute = true) { m_bRouteNotify = bRoute; };
	bool IsRouteNotify() { return m_bRouteNotify; };

protected:
	// Message Handlers
	LRESULT OnCommand (UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// Call base class OnCommand to allow message map processing
		if (true == m_bRouteCommand)
		{
			//
			// Forward WM_COMMAND messages to the dialog's parent.
			//
			return GetParent ().SendMessage (WM_COMMAND, wParam, lParam);
		}

		return 0;
	}
	LRESULT OnNotify (UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (true == m_bRouteNotify)
		{
			//
			// Forward WM_NOTIFY messages to the dialog's parent.
			//
			return GetParent ().SendMessage (WM_NOTIFY, wParam, lParam);
		}

		return 0;
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
	}
	
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
	}

	LRESULT OnCmdMsg  (UINT nCode, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#ifndef _AFX_NO_OCC_SUPPORT
		if (true == m_bRouteCmdMsg)
		{
			//
			// Forward ActiveX control events to the dialog's parent.
			//
			//if (nCode == CN_EVENT)
				return GetParent ().SendMessage (WM_NOTIFY, wParam, lParam);
		}
#endif // !_AFX_NO_OCC_SUPPORT

		return 0;
	}

// Routing flags
	bool m_bRouteCommand;
	bool m_bRouteCmdMsg;
	bool m_bRouteNotify;
};

#endif // !defined(AFX_TABPAGE_H__619331B3_7DE7_4DB1_A039_2103E87E8E71__INCLUDED_)
