#pragma once
#include "Theme.h"  
#include "afxdialogex.h"
#include <string>

class CAppointmentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAppointmentDlg)

public:
	CAppointmentDlg(CWnd* pParent = nullptr);
	virtual ~CAppointmentDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPOINTMENT_DIALOG };
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
	CComboBox m_comboCustomer;
	CComboBox m_comboStylist;
	CComboBox m_comboService;
	CString m_date;
	CString m_time;
	CListCtrl m_list;
	int m_selectedId = 0;

	CBrush m_bgBrush;
	CFont m_titleFont;
	HWND m_titleHwnd = nullptr;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void FillCombo(CComboBox& box, const char* sql);
	void LoadAppointments();
	int SelectedData(CComboBox& box);
	void SelectByData(CComboBox& box, int id);
	bool ValidateInput(int& custId, int& styId, int& svcId);
	bool HasConflict(int styId, const std::string& date, const std::string& time, int excludeId);

	afx_msg void OnBnClickedBtnAadd();
	afx_msg void OnBnClickedBtnAupdate();
	afx_msg void OnBnClickedBtnAdelete();
	afx_msg void OnNMClickListAppts(NMHDR* pNMHDR, LRESULT* pResult);
};