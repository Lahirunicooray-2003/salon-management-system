#pragma once

#include "Theme.h"
#include "afxdialogex.h"
#include <string>

class CCustomerDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CCustomerDlg)

public:
    CCustomerDlg(CWnd* pParent = nullptr);
    virtual ~CCustomerDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CUSTOMER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    afx_msg void OnBnClickedBtnCadd();
    afx_msg void OnBnClickedBtnCupdate();
    afx_msg void OnBnClickedBtnCdelete();
    afx_msg void OnNMClickListCustomers(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMCustomdrawListCustomers(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()

private:
    void LoadCustomers();

public:
    CString m_name;
    CString m_phone;
    CString m_email;
    int m_selectedId;

    CListCtrl m_list;
    CStatic m_title;

    
    CMFCButton m_btnAdd;
    CMFCButton m_btnUpdate;
    CMFCButton m_btnDelete;
    CGoldButton m_goldBack;

    CFont m_titleFont;
    CFont m_tableFont;
    CBrush m_bgBrush;
    CBrush m_editBrush;
};