#pragma once
#include "afxdialogex.h"
#include "Theme.h"

class CServiceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServiceDlg)

public:
	CServiceDlg(CWnd* pParent = nullptr);
	virtual ~CServiceDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVICE_DIALOG };
#endif

public:
	CGoldButton m_goldAdd;
	CGoldButton m_goldUpdate;
	CGoldButton m_goldDelete;
	CGoldButton m_goldBack;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CString m_name;
	CString m_price;
	CString m_duration;
	CListCtrl m_list;
	int m_selectedId = 0;

	CBrush m_bgBrush;
	CFont m_titleFont;
	HWND m_titleHwnd = nullptr;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void LoadServices();

	afx_msg void OnBnClickedBtnSadd();
	afx_msg void OnBnClickedBtnSupdate();
	afx_msg void OnBnClickedBtnSdelete();
	afx_msg void OnNMClickListServices(NMHDR* pNMHDR, LRESULT* pResult);
};