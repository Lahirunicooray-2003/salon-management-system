#pragma once
#include "afxdialogex.h"
class CDashboardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDashboardDlg)

public:
	CDashboardDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDashboardDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DASHBOARD1_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	CFont m_titleFont;
	CBrush m_bgBrush;
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_title;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnCustomers();
	afx_msg void OnBnClickedBtnAppointments();
	afx_msg void OnBnClickedBtnServices();
	afx_msg void OnBnClickedBtnStylists();
	afx_msg void OnBnClickedBtnLogout();
	afx_msg void OnStnClickedStaticDtitle();
};
