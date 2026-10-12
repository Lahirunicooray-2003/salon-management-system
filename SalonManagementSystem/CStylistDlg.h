#pragma once
#include "afxdialogex.h"
#include "Theme.h"

class CStylistDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStylistDlg)

public:
	CStylistDlg(CWnd* pParent = nullptr);
	virtual ~CStylistDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STYLIST_DIALOG };
#endif

public:
	CGoldButton m_goldAdd;
	CGoldButton m_goldUpdate;
	CGoldButton m_goldDelete;
	CGoldButton m_goldDetails;
	CGoldButton m_goldBack;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CString m_name;
	CString m_phone;
	CString m_email;
	CString m_spec;
	CListCtrl m_list;
	int m_selectedId = 0;

	CBrush m_bgBrush;
	CFont m_titleFont;
	HWND m_titleHwnd = nullptr;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void LoadStylists();

	afx_msg void OnBnClickedBtnTadd();
	afx_msg void OnBnClickedBtnTupdate();
	afx_msg void OnBnClickedBtnTdelete();
	afx_msg void OnBnClickedBtnTdetails();
	afx_msg void OnNMClickListStylists(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedListStylists(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEditTemail();
	afx_msg void OnEnChangeEditTname();
};