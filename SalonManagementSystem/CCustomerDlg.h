#pragma once
#include "afxdialogex.h"
#include "Theme.h"

// CCustomerDlg dialog

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

	DECLARE_MESSAGE_MAP()

public:
	CString m_name;
	CString m_phone;
	CString m_email;
	CListCtrl m_list;
	int m_selectedId = 0;

	CBrush m_bgBrush;
	CFont m_titleFont;
	CStatic m_title;
	CMFCButton m_btnAdd, m_btnUpdate, m_btnDelete;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void LoadCustomers();

	afx_msg void OnBnClickedBtnCadd();
	afx_msg void OnBnClickedBtnCupdate();
	afx_msg void OnBnClickedBtnCdelete();
	afx_msg void OnNMClickListCustomers(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEditCemail();
};