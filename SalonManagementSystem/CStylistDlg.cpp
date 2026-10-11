#include "pch.h"
#include "SalonManagementSystem.h"
#include "afxdialogex.h"
#include "CStylistDlg.h"
#include "DBConnection.h"
#include "Stylist.h"
#include "Theme.h"
#include <string>
#include <memory>

static std::string ToUtf8(const CString& s)
{
	CStringA a(CW2A(s, CP_UTF8));
	return std::string(a);
}

static CString FromUtf8(const std::string& s)
{
	return CString(CA2W(s.c_str(), CP_UTF8));
}

IMPLEMENT_DYNAMIC(CStylistDlg, CDialogEx)

CStylistDlg::CStylistDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STYLIST_DIALOG, pParent)
{
}

CStylistDlg::~CStylistDlg()
{
}

void CStylistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TNAME, m_name);
	DDX_Text(pDX, IDC_EDIT_TPHONE, m_phone);
	DDX_Text(pDX, IDC_EDIT_TEMAIL, m_email);
	DDX_Text(pDX, IDC_EDIT_TSPEC, m_spec);
	DDX_Control(pDX, IDC_LIST_STYLISTS, m_list);
}

BEGIN_MESSAGE_MAP(CStylistDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_TADD, &CStylistDlg::OnBnClickedBtnTadd)
	ON_BN_CLICKED(IDC_BTN_TUPDATE, &CStylistDlg::OnBnClickedBtnTupdate)
	ON_BN_CLICKED(IDC_BTN_TDELETE, &CStylistDlg::OnBnClickedBtnTdelete)
	ON_BN_CLICKED(IDC_BTN_TDETAILS, &CStylistDlg::OnBnClickedBtnTdetails)
	ON_NOTIFY(NM_CLICK, IDC_LIST_STYLISTS, &CStylistDlg::OnNMClickListStylists)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_STYLISTS, &CStylistDlg::OnLvnItemchangedListStylists)
	ON_EN_CHANGE(IDC_EDIT_TEMAIL, &CStylistDlg::OnEnChangeEditTemail)
	ON_EN_CHANGE(IDC_EDIT_TNAME, &CStylistDlg::OnEnChangeEditTname)
END_MESSAGE_MAP()

BOOL CStylistDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"ID", LVCFMT_LEFT, 40);
	m_list.InsertColumn(1, L"Name", LVCFMT_LEFT, 120);
	m_list.InsertColumn(2, L"Phone", LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, L"Email", LVCFMT_LEFT, 140);
	m_list.InsertColumn(4, L"Speciality", LVCFMT_LEFT, 100);

	m_bgBrush.CreateSolidBrush(Theme::Bg);
	m_titleFont.CreatePointFont(180, L"Segoe UI Semibold");

	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		CString txt;
		pChild->GetWindowText(txt);
		if (txt == L"Stylist Management")
		{
			pChild->SetFont(&m_titleFont);
			m_titleHwnd = pChild->GetSafeHwnd();
			break;
		}
		pChild = pChild->GetNextWindow();
	}

	m_list.SetBkColor(Theme::ListBg);
	m_list.SetTextBkColor(Theme::ListBg);
	m_list.SetTextColor(Theme::Text);

	LoadStylists();
	return TRUE;
}

void CStylistDlg::LoadStylists()
{
	m_list.DeleteAllItems();

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database."); return; }

	MYSQL_RES* res = db.Query("SELECT StylistID, Name, Phone, Email, Speciality FROM Stylist");
	if (!res) return;

	MYSQL_ROW row;
	int i = 0;
	while ((row = mysql_fetch_row(res)))
	{
		m_list.InsertItem(i, FromUtf8(row[0] ? row[0] : ""));
		for (int c = 1; c <= 4; c++)
			m_list.SetItemText(i, c, FromUtf8(row[c] ? row[c] : ""));
		i++;
	}
	mysql_free_result(res);
}

void CStylistDlg::OnBnClickedBtnTadd()
{
	UpdateData(TRUE);

	try
	{
		Stylist s(ToUtf8(m_name), ToUtf8(m_phone), ToUtf8(m_email), ToUtf8(m_spec));

		DBConnection db;
		if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database."); return; }

		std::string sql = "INSERT INTO Stylist (Name, Phone, Email, Speciality) VALUES ('"
			+ db.Escape(s.getName()) + "','"
			+ db.Escape(s.getPhone()) + "','"
			+ db.Escape(s.getEmail()) + "','"
			+ db.Escape(s.getSpeciality()) + "')";

		if (db.Execute(sql)) AfxMessageBox(L"Stylist added successfully.");
		else AfxMessageBox(L"Failed to add the stylist.");
	}
	catch (const std::invalid_argument& e)
	{
		AfxMessageBox(FromUtf8(e.what()));
		return;
	}

	m_name = m_phone = m_email = m_spec = L"";
	UpdateData(FALSE);
	LoadStylists();
}

void CStylistDlg::OnBnClickedBtnTupdate()
{
	UpdateData(TRUE);
	if (m_selectedId == 0) { AfxMessageBox(L"Please select a stylist from the list."); return; }

	try
	{
		Stylist s(ToUtf8(m_name), ToUtf8(m_phone), ToUtf8(m_email), ToUtf8(m_spec));

		DBConnection db;
		if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database."); return; }

		std::string sql = "UPDATE Stylist SET Name='" + db.Escape(s.getName())
			+ "', Phone='" + db.Escape(s.getPhone())
			+ "', Email='" + db.Escape(s.getEmail())
			+ "', Speciality='" + db.Escape(s.getSpeciality())
			+ "' WHERE StylistID=" + std::to_string(m_selectedId);

		if (db.Execute(sql)) AfxMessageBox(L"Stylist updated successfully.");
		else AfxMessageBox(L"Failed to update the stylist.");
	}
	catch (const std::invalid_argument& e)
	{
		AfxMessageBox(FromUtf8(e.what()));
		return;
	}
	LoadStylists();
}

void CStylistDlg::OnBnClickedBtnTdelete()
{
	if (m_selectedId == 0) { AfxMessageBox(L"Please select a stylist from the list."); return; }
	if (AfxMessageBox(L"Are you sure you want to delete this stylist?", MB_YESNO) != IDYES) return;

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database."); return; }

	if (db.Execute("DELETE FROM Stylist WHERE StylistID=" + std::to_string(m_selectedId)))
		AfxMessageBox(L"Stylist deleted successfully.");
	else
		AfxMessageBox(L"Failed to delete the stylist. They may have appointments.");

	m_selectedId = 0;
	m_name = m_phone = m_email = m_spec = L"";
	UpdateData(FALSE);
	LoadStylists();
}

void CStylistDlg::OnBnClickedBtnTdetails()
{
	if (m_selectedId == 0) { AfxMessageBox(L"Please select a stylist from the list."); return; }

	try
	{
		// POLYMORPHISM: a Person pointer calls the Stylist version of getDetails()
		std::unique_ptr<Person> p = std::make_unique<Stylist>(
			ToUtf8(m_name), ToUtf8(m_phone), ToUtf8(m_email), ToUtf8(m_spec));
		AfxMessageBox(FromUtf8(p->getDetails()));
	}
	catch (const std::invalid_argument& e)
	{
		AfxMessageBox(FromUtf8(e.what()));
	}
}

void CStylistDlg::OnNMClickListStylists(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (p->iItem >= 0)
	{
		m_selectedId = _ttoi(m_list.GetItemText(p->iItem, 0));
		m_name = m_list.GetItemText(p->iItem, 1);
		m_phone = m_list.GetItemText(p->iItem, 2);
		m_email = m_list.GetItemText(p->iItem, 3);
		m_spec = m_list.GetItemText(p->iItem, 4);
		UpdateData(FALSE);
	}
	*pResult = 0;
}
void CStylistDlg::OnLvnItemchangedListStylists(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CStylistDlg::OnEnChangeEditTemail()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CStylistDlg::OnEnChangeEditTname()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
HBRUSH CStylistDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		if (pWnd->GetSafeHwnd() == m_titleHwnd)
			pDC->SetTextColor(Theme::GoldDark);
		else
			pDC->SetTextColor(Theme::Text);
		return (HBRUSH)m_bgBrush.GetSafeHandle();
	}
	return hbr;
}