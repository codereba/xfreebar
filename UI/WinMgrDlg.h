///////////////////////////////////////////////////////////////////////////////
//
// WinMgrDlg.h : header file
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ButtonXP.h"
#include "ListBoxXP.h"
#include "../Resource.h"

/////////////////////////////////////////////////////////////////////////////
class CListBoxWithIcon : public CListBoxXP
{
    virtual void DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);
};

/////////////////////////////////////////////////////////////////////////////
// CWindowsManagerDlg dialog

class CWindowsManagerDlg : public CDialog
{
// Construction
public:
    CWindowsManagerDlg (CWnd* pParent = NULL);

// Dialog Data
    //{{AFX_DATA(CWindowsManagerDlg)
    enum { IDD = IDD_WINDOWS_MANAGER };
    CButtonXP   m_btnOk;
    CButtonXP   m_btnCancel;
    CButtonXP   m_btnTileVert;
    CButtonXP   m_btnTileHorz;
    CButtonXP   m_btnSaveWindows;
    CButtonXP   m_btnMinimize;
    CButtonXP   m_btnCloseWindows;
    CButtonXP   m_btnCascade;
    CListBoxWithIcon m_WindowList;
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWindowsManagerDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    void RefreshWindowList ();
    void SaveDocuments (bool bClose = false);
    void UpdateButtons ();
    void MDIMessage (UINT uMsg, WPARAM nFlag = 0);

    // Generated message map functions
    //{{AFX_MSG(CWindowsManagerDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkWindowlist();
    virtual void OnOK();
    afx_msg void OnSelChangeWindowlist();
    afx_msg void OnSaveWindows();
    afx_msg void OnCloseWindows();
    afx_msg void OnCascade();
    afx_msg void OnTilehorz();
    afx_msg void OnTilevert();
    afx_msg void OnMinimize();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
