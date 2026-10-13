#pragma once

#include "Theme.h"

// CSalonManagementSystemDlg dialog
class CSalonManagementSystemDlg : public CDialogEx
{
    // Construction
public:
    CGoldButton m_goldLogin;
    CSalonManagementSystemDlg(CWnd* pParent = nullptr);    // standard constructor

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SALONMANAGEMENTSYSTEM_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:
    HICON m_hIcon;
    CBrush m_bgBrush;
    CBrush m_editBrush;      // beige brush for the Username / Password edit boxes
    CFont m_titleFont;
    HWND m_titleHwnd = nullptr;

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

public:
    CString m_username;
    CString m_password;
    afx_msg void OnBnClickedBtnLogin();
};