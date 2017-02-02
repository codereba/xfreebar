///////////////////////////////////////////////////////////////////////////////
//
// WinMgrDlg.cpp : implementation file
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxtempl.h>
#include "WinMgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef CArray<int, int> CIntArray;

///////////////////////////////////////////////////////////////////////////////
CWindowsManagerDlg::CWindowsManagerDlg (CWnd* pParent /*=NULL*/)
    : CDialog(CWindowsManagerDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWindowsManagerDlg)
    //}}AFX_DATA_INIT
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::RefreshWindowList ()
{
    m_WindowList.ResetContent();

    CMDIFrameWnd* pFrame = (CMDIFrameWnd*)GetParentFrame();
    HWND hChild = ::GetWindow (pFrame->m_hWndMDIClient, GW_CHILD);
    HWND hActiveChild = pFrame->GetActiveFrame()->GetSafeHwnd();
    int nIndex = LB_ERR;

    while ( hChild != NULL )
    {
        if ( ::IsWindowVisible (hChild) )
        {
            CFrameWnd* pChild = (CFrameWnd*)CWnd::FromHandle (hChild);
            TCHAR sCaption[_MAX_PATH] = _T("");
            BOOL bModified = false;
        
            if ( pChild != NULL )
            {
                CDocument* pDoc = pChild->GetActiveDocument();

                if ( pDoc != NULL )
                {
                    _tcscpy (sCaption, pDoc->GetPathName());

                    bModified = pDoc->IsModified();
                }
            }
            if ( sCaption[0] == 0 )
            {
                ::GetWindowText (hChild, sCaption, sizeof(sCaption));
            }
            if ( bModified )
            {
                _tcscat (sCaption, _T(" *"));
            }
            nIndex = m_WindowList.AddString (sCaption);
            m_WindowList.SetItemData (nIndex, (DWORD)hChild);

            if ( hChild == hActiveChild )
            {
                m_WindowList.SetSel (nIndex);
            }
        }
        hChild = ::GetWindow (hChild, GW_HWNDNEXT);
    }
    if ( m_WindowList.GetSelItems (1, &nIndex) == 1 )
    {
        m_WindowList.SetCaretIndex (nIndex);
        m_WindowList.SetAnchorIndex (nIndex);
        GetDlgItem (IDOK)->EnableWindow (true);
    }
    else
    {
        GetDlgItem (IDCANCEL)->SetFocus();
        SetDefID (IDCANCEL);
        GetDlgItem (IDOK)->EnableWindow (false);
        GetDlgItem (IDC_SAVE_WINDOWS)->EnableWindow (false);
        GetDlgItem (IDC_CLOSE_WINDOWS)->EnableWindow (false);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::SaveDocuments (bool bClose)
{
    CIntArray arrayIndexes;
    int nCount = m_WindowList.GetSelCount();

    arrayIndexes.SetSize (nCount);
    m_WindowList.GetSelItems (nCount, arrayIndexes.GetData());

    for ( int i = 0; i < nCount; i++ )
    {
        HWND hWnd = (HWND)m_WindowList.GetItemData (arrayIndexes[i]);

        if ( ::IsWindow (hWnd) )
        {
            CFrameWnd* pChild = (CFrameWnd*)CWnd::FromHandle (hWnd);
            ASSERT(pChild != NULL);

            CDocument* pDoc = pChild->GetActiveDocument();

            if ( pDoc != NULL )
            {
                if ( bClose )
                {
                    if ( pDoc->SaveModified() )
                    {
                        pDoc->OnCloseDocument();
                    }
                }
                else
                {
                    pDoc->DoFileSave();
                }
            }
            else if ( bClose )
            {
                pChild->SendMessage (WM_CLOSE);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::UpdateButtons ()
{
    CIntArray arrayIndexes;
    int nCount = m_WindowList.GetSelCount();
    int nChildWithDoc = 0;

    arrayIndexes.SetSize (nCount);
    m_WindowList.GetSelItems (nCount, arrayIndexes.GetData());

    for ( int i = 0; i < nCount; i++ )
    {
        HWND hWnd = (HWND)m_WindowList.GetItemData (arrayIndexes[i]);

        if ( ::IsWindow (hWnd) )
        {
            CFrameWnd* pChild = (CFrameWnd*)CWnd::FromHandle (hWnd);
            ASSERT(pChild != NULL);

            CDocument* pDoc = pChild->GetActiveDocument();

            if ( pDoc != NULL )
            {
                nChildWithDoc++;
            }
        }
        else
        {
            RefreshWindowList();
            return;
        }
    }
    bool bSaveClose = nCount > 0 && nChildWithDoc > 0;

    if ( !bSaveClose && GetFocus() == GetDlgItem (IDC_CLOSE_WINDOWS) )
    {
        SetDefID (IDCANCEL);
        m_WindowList.SetFocus();
    }
    GetDlgItem (IDC_SAVE_WINDOWS)->EnableWindow (bSaveClose);
    GetDlgItem (IDC_CLOSE_WINDOWS)->EnableWindow (nCount > 0);

    if ( nCount == 1 )
    {
        GetDlgItem (IDOK)->EnableWindow (true);
        SetDefID (IDOK);
    }
    else
    {
        SetDefID (IDCANCEL);
        GetDlgItem (IDOK)->EnableWindow (false);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::MDIMessage (UINT uMsg, WPARAM nFlag)
{
    CWaitCursor wait;

    int nItems = m_WindowList.GetCount();

    if ( nItems != LB_ERR && nItems > 0 )
    {
        HWND hMDIClient = ((CMDIFrameWnd*)GetParentFrame())->m_hWndMDIClient;

        ::LockWindowUpdate (hMDIClient);

        for ( int i = nItems-1; i >= 0; i-- )
        {
            HWND hWnd = (HWND)m_WindowList.GetItemData (i);

            if ( m_WindowList.GetSel(i) > 0) ::ShowWindow (hWnd, SW_RESTORE);
            else                             ::ShowWindow (hWnd,SW_MINIMIZE);
        }
        ::SendMessage (hMDIClient, uMsg, nFlag, 0);  
        ::LockWindowUpdate (NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::DoDataExchange (CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWindowsManagerDlg)
    DDX_Control(pDX, IDOK, m_btnOk);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_TILEVERT, m_btnTileVert);
    DDX_Control(pDX, IDC_TILEHORZ, m_btnTileHorz);
    DDX_Control(pDX, IDC_SAVE_WINDOWS, m_btnSaveWindows);
    DDX_Control(pDX, IDC_MINIMIZE, m_btnMinimize);
    DDX_Control(pDX, IDC_CLOSE_WINDOWS, m_btnCloseWindows);
    DDX_Control(pDX, IDC_CASCADE, m_btnCascade);
    DDX_Control(pDX, IDC_WINDOWLIST, m_WindowList);
    //}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWindowsManagerDlg, CDialog)
    //{{AFX_MSG_MAP(CWindowsManagerDlg)
    ON_LBN_DBLCLK(IDC_WINDOWLIST, OnDblclkWindowlist)
    ON_LBN_SELCHANGE(IDC_WINDOWLIST, OnSelChangeWindowlist)
    ON_BN_CLICKED(IDC_SAVE_WINDOWS, OnSaveWindows)
    ON_BN_CLICKED(IDC_CLOSE_WINDOWS, OnCloseWindows)
    ON_BN_CLICKED(IDC_CASCADE, OnCascade)
    ON_BN_CLICKED(IDC_TILEHORZ, OnTilehorz)
    ON_BN_CLICKED(IDC_TILEVERT, OnTilevert)
    ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
BOOL CWindowsManagerDlg::OnInitDialog ()
{
    CDialog::OnInitDialog();
    RefreshWindowList();
    UpdateButtons();
    m_WindowList.SetItemHeight (0, 17);
    m_WindowList.SetHorizontalExtent (700);

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnOK ()
{
    HWND hSelectedWindow = (HWND)m_WindowList.GetItemData (m_WindowList.GetCurSel());

    if ( ::IsWindow (hSelectedWindow) )
    {
        ((CMDIFrameWnd*)GetParentFrame())->MDIActivate (CWnd::FromHandle (hSelectedWindow));
    }
    CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnDblclkWindowlist ()
{
    if ( m_WindowList.GetSelCount() == 1 )
    {
        OnOK();
    }
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnSelChangeWindowlist ()
{
    GetDlgItem (IDOK)->EnableWindow (m_WindowList.GetSelCount() == 1);
    UpdateButtons();
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnSaveWindows () 
{
    SaveDocuments();
    // Because asterix disappeared
    RefreshWindowList();
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnCloseWindows ()
{
    SaveDocuments (true);
    RefreshWindowList();
    UpdateButtons();
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnCascade ()
{
    MDIMessage (WM_MDICASCADE);
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnTilehorz ()
{
    MDIMessage (WM_MDITILE, MDITILE_HORIZONTAL); 
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnTilevert ()
{
    MDIMessage (WM_MDITILE, MDITILE_VERTICAL);   
}

///////////////////////////////////////////////////////////////////////////////
void CWindowsManagerDlg::OnMinimize ()
{
    int nItems = m_WindowList.GetCount();

    if ( nItems != LB_ERR && nItems > 0 )
    {
        for ( int i = nItems-1; i >= 0; i-- )
        {
            if ( m_WindowList.GetSel (i) > 0 )
            {
                ::ShowWindow ((HWND)m_WindowList.GetItemData (i), SW_MINIMIZE);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CListBoxWithIcon::DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);
    CRect rcItem = lpDrawItemStruct->rcItem;
    CString sItemText;

    if ( (int)lpDrawItemStruct->itemID >= 0 )
    {
        CListBox::GetText ((int)lpDrawItemStruct->itemID, sItemText);
        pDC->SetTextColor (::GetSysColor (( lpDrawItemStruct->itemState & ODS_SELECTED ) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
    }
    HICON hIcon = (HICON)::SendMessage ((HWND)lpDrawItemStruct->itemData, WM_GETICON, false, 0);

    if ( hIcon == NULL )
    {
        TCHAR sClassName[128];
        WNDCLASSEX WndCls;

        ::GetClassName ((HWND)lpDrawItemStruct->itemData, sClassName, 128);
        HINSTANCE hInst = (HINSTANCE)::GetWindowLong ((HWND)lpDrawItemStruct->itemData, GWL_HINSTANCE);
        ::GetClassInfoEx (hInst, sClassName, &WndCls);
        hIcon = WndCls.hIconSm;
    }
    rcItem.bottom--;

    if ( hIcon != NULL )
    {
        ::DrawIconEx (lpDrawItemStruct->hDC, rcItem.left+1, rcItem.top, hIcon, 16, 16, 0, NULL, DI_NORMAL);
    }
    rcItem.left += 18;
    pDC->FillSolidRect (rcItem, ::GetSysColor (( lpDrawItemStruct->itemState & ODS_SELECTED ) ? COLOR_HIGHLIGHT : COLOR_WINDOW));
    rcItem.left++;
    pDC->DrawText (sItemText, rcItem, DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);

    if ( lpDrawItemStruct->itemState & ODS_FOCUS )
    {
        pDC->DrawFocusRect (rcItem);
    }
}
