// CCustomerDlg.cpp : implementation file
//

#include "pch.h"
#include "SalonManagementSystem.h"
#include "afxdialogex.h"
#include "CCustomerDlg.h"
#include "DBConnection.h"
#include "Customer.h"
#include <string>

// CString (wide) -> std::string (UTF-8)
static std::string ToUtf8(const CString& s)
{
	CStringA a(CW2A(s, CP_UTF8));
	return std::string(a);
}

IMPLEMENT_DYNAMIC(CCustomerDlg, CDialogEx)

CCustomerDlg::CCustomerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CUSTOMER_DIALOG, pParent)
{
}

CCustomerDlg::~CCustomerDlg()
{
}

void CCustomerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CNAME, m_name);
	DDX_Text(pDX, IDC_EDIT_CPHONE, m_phone);
	DDX_Text(pDX, IDC_EDIT_CEMAIL, m_email);
	DDX_Control(pDX, IDC_LIST_CUSTOMERS, m_list);

	DDX_Control(pDX, IDC_CTITLE, m_title);
	DDX_Control(pDX, IDC_BTN_CADD, m_btnAdd);
	DDX_Control(pDX, IDC_BTN_CUPDATE, m_btnUpdate);
	DDX_Control(pDX, IDC_BTN_CDELETE, m_btnDelete);
}

BEGIN_MESSAGE_MAP(CCustomerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CADD, &CCustomerDlg::OnBnClickedBtnCadd)
	ON_BN_CLICKED(IDC_BTN_CUPDATE, &CCustomerDlg::OnBnClickedBtnCupdate)
	ON_BN_CLICKED(IDC_BTN_CDELETE, &CCustomerDlg::OnBnClickedBtnCdelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CUSTOMERS, &CCustomerDlg::OnNMClickListCustomers)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_CEMAIL, &CCustomerDlg::OnEnChangeEditCemail)
END_MESSAGE_MAP()

BOOL CCustomerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"ID", LVCFMT_LEFT, 50);
	m_list.InsertColumn(1, L"Name", LVCFMT_LEFT, 150);
	m_list.InsertColumn(2, L"Phone", LVCFMT_LEFT, 120);
	m_list.InsertColumn(3, L"Email", LVCFMT_LEFT, 180);

	m_bgBrush.CreateSolidBrush(Theme::Bg);
	m_titleFont.CreatePointFont(180, L"Segoe UI Semibold");
	m_title.SetFont(&m_titleFont);

	StyleGoldButton(m_btnAdd);
	StyleGoldButton(m_btnUpdate);
	StyleGoldButton(m_btnDelete);

	m_list.SetBkColor(Theme::ListBg);
	m_list.SetTextBkColor(Theme::ListBg);
	m_list.SetTextColor(Theme::Text);

	LoadCustomers();
	return TRUE;
}

void CCustomerDlg::LoadCustomers()
{
	m_list.DeleteAllItems();

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	MYSQL_RES* res = db.Query("SELECT CustomerID, Name, Phone, Email FROM Customer");
	if (!res) return;

	MYSQL_ROW row;
	int i = 0;
	while ((row = mysql_fetch_row(res)))
	{
		m_list.InsertItem(i, CString(CA2W(row[0] ? row[0] : "", CP_UTF8)));
		m_list.SetItemText(i, 1, CString(CA2W(row[1] ? row[1] : "", CP_UTF8)));
		m_list.SetItemText(i, 2, CString(CA2W(row[2] ? row[2] : "", CP_UTF8)));
		m_list.SetItemText(i, 3, CString(CA2W(row[3] ? row[3] : "", CP_UTF8)));
		i++;
	}
	mysql_free_result(res);
}

void CCustomerDlg::OnBnClickedBtnCadd()
{
	UpdateData(TRUE);

	try
	{
		// OBJECT: created here; validation happens inside the setters
		Customer c(ToUtf8(m_name), ToUtf8(m_phone), ToUtf8(m_email));

		DBConnection db;
		if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

		std::string sql = "INSERT INTO Customer (Name, Phone, Email) VALUES ('"
			+ db.Escape(c.getName()) + "','"
			+ db.Escape(c.getPhone()) + "','"
			+ db.Escape(c.getEmail()) + "')";

		if (db.Execute(sql)) AfxMessageBox(L"Customer added successfully.", MB_ICONINFORMATION);
		else AfxMessageBox(L"Failed to add the customer.", MB_ICONERROR);
	}
	catch (const std::invalid_argument& e)
	{
		AfxMessageBox(CString(CA2W(e.what(), CP_UTF8)), MB_ICONWARNING);
		return;
	}

	m_name = m_phone = m_email = L"";
	UpdateData(FALSE);
	LoadCustomers();
}

void CCustomerDlg::OnBnClickedBtnCupdate()
{
	UpdateData(TRUE);
	if (m_selectedId == 0) { AfxMessageBox(L"Please select a customer from the list.", MB_ICONWARNING); return; }

	DBConnection db;
	if (!db.Open()) return;

	std::string sql = "UPDATE Customer SET Name='" + db.Escape(ToUtf8(m_name))
		+ "', Phone='" + db.Escape(ToUtf8(m_phone))
		+ "', Email='" + db.Escape(ToUtf8(m_email))
		+ "' WHERE CustomerID=" + std::to_string(m_selectedId);

	if (db.Execute(sql)) AfxMessageBox(L"Customer updated successfully.", MB_ICONINFORMATION);
	else AfxMessageBox(L"Failed to update the customer.", MB_ICONERROR);
	LoadCustomers();
}

void CCustomerDlg::OnBnClickedBtnCdelete()
{
	if (m_selectedId == 0) { AfxMessageBox(L"Please select a customer from the list.", MB_ICONWARNING); return; }
	if (AfxMessageBox(L"Are you sure you want to delete this customer?", MB_YESNO | MB_ICONQUESTION) != IDYES) return;

	DBConnection db;
	if (!db.Open()) return;

	if (db.Execute("DELETE FROM Customer WHERE CustomerID=" + std::to_string(m_selectedId)))
		AfxMessageBox(L"Customer deleted successfully.", MB_ICONINFORMATION);
	else AfxMessageBox(L"Failed to delete the customer.", MB_ICONERROR);

	m_selectedId = 0;
	m_name = m_phone = m_email = L"";
	UpdateData(FALSE);
	LoadCustomers();
}

void CCustomerDlg::OnNMClickListCustomers(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (p->iItem >= 0)
	{
		m_selectedId = _ttoi(m_list.GetItemText(p->iItem, 0));
		m_name = m_list.GetItemText(p->iItem, 1);
		m_phone = m_list.GetItemText(p->iItem, 2);
		m_email = m_list.GetItemText(p->iItem, 3);
		UpdateData(FALSE);
	}
	*pResult = 0;
}
void CCustomerDlg::OnEnChangeEditCemail()
{
}

HBRUSH CCustomerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		if (pWnd->GetDlgCtrlID() == IDC_CTITLE)
			pDC->SetTextColor(Theme::GoldDark);
		else
			pDC->SetTextColor(Theme::Text);
		return (HBRUSH)m_bgBrush.GetSafeHandle();
	}
	return hbr;
}