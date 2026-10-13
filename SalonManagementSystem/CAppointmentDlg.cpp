#include "pch.h"
#include "SalonManagementSystem.h"
#include "afxdialogex.h"
#include "CAppointmentDlg.h"
#include "DBConnection.h"
#include "Theme.h"
#include <string>

static std::string ToUtf8(const CString& s)
{
	CStringA a(CW2A(s, CP_UTF8));
	return std::string(a);
}

static CString FromUtf8(const char* s)
{
	return CString(CA2W(s ? s : "", CP_UTF8));
}

static bool IsValidDate(const CString& s)
{
	int y = 0, m = 0, d = 0;
	if (s.GetLength() != 10) return false;
	if (swscanf_s(s, L"%4d-%2d-%2d", &y, &m, &d) != 3) return false;
	return y >= 2000 && y <= 2100 && m >= 1 && m <= 12 && d >= 1 && d <= 31;
}

static bool IsValidTime(const CString& s)
{
	int h = 0, mi = 0;
	if (s.GetLength() != 5) return false;
	if (swscanf_s(s, L"%2d:%2d", &h, &mi) != 2) return false;
	return h >= 0 && h <= 23 && mi >= 0 && mi <= 59;
}

IMPLEMENT_DYNAMIC(CAppointmentDlg, CDialogEx)

CAppointmentDlg::CAppointmentDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APPOINTMENT_DIALOG, pParent)
{
}

CAppointmentDlg::~CAppointmentDlg()
{
}

void CAppointmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACUST, m_comboCustomer);
	DDX_Control(pDX, IDC_COMBO_ASTYLIST, m_comboStylist);
	DDX_Control(pDX, IDC_COMBO_ASERVICE, m_comboService);
	DDX_Text(pDX, IDC_EDIT_ADATE, m_date);
	DDX_Text(pDX, IDC_EDIT_ATIME, m_time);
	DDX_Control(pDX, IDC_LIST_APPTS, m_list);

	DDX_Control(pDX, IDC_BTN_AADD, m_goldAdd);
	DDX_Control(pDX, IDC_BTN_AUPDATE, m_goldUpdate);
	DDX_Control(pDX, IDC_BTN_ADELETE, m_goldDelete);
}

BEGIN_MESSAGE_MAP(CAppointmentDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_AADD, &CAppointmentDlg::OnBnClickedBtnAadd)
	ON_BN_CLICKED(IDC_BTN_AUPDATE, &CAppointmentDlg::OnBnClickedBtnAupdate)
	ON_BN_CLICKED(IDC_BTN_ADELETE, &CAppointmentDlg::OnBnClickedBtnAdelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_APPTS, &CAppointmentDlg::OnNMClickListAppts)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CAppointmentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Gold Buttons styling
	StyleGoldButton(m_goldAdd);
	StyleGoldButton(m_goldUpdate);
	StyleGoldButton(m_goldDelete);
	m_goldBack.SubclassDlgItem(IDCANCEL, this);

	// smaller table font so every column fits neatly
	static CFont tableFont;
	if (tableFont.GetSafeHandle() == NULL)
		tableFont.CreatePointFont(100, L"Segoe UI");
	m_list.SetFont(&tableFont);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_list.InsertColumn(0, L"ID", LVCFMT_LEFT, 40);
	m_list.InsertColumn(1, L"Customer", LVCFMT_LEFT, 130);
	m_list.InsertColumn(2, L"Stylist", LVCFMT_LEFT, 130);
	m_list.InsertColumn(3, L"Service", LVCFMT_LEFT, 130);
	m_list.InsertColumn(4, L"Date", LVCFMT_LEFT, 90);
	m_list.InsertColumn(5, L"Time", LVCFMT_LEFT, 60);
	m_list.InsertColumn(6, L"Status", LVCFMT_LEFT, 70);

	m_bgBrush.CreateSolidBrush(Theme::Bg);
	m_titleFont.CreatePointFont(180, L"Segoe UI Semibold");

	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		CString txt;
		pChild->GetWindowText(txt);
		if (txt == L"Appointment Management")
		{
			pChild->SetFont(&m_titleFont);
			m_titleHwnd = pChild->GetSafeHwnd();
			break;
		}
		pChild = pChild->GetNextWindow();
	}

	m_list.SetBkColor(RGB(255, 250, 240));
	m_list.SetTextBkColor(RGB(255, 250, 240));
	m_list.SetTextColor(RGB(90, 65, 45));

	FillCombo(m_comboCustomer, "SELECT CustomerID, Name FROM Customer ORDER BY Name");
	FillCombo(m_comboStylist, "SELECT StylistID, Name FROM Stylist ORDER BY Name");
	FillCombo(m_comboService, "SELECT ServiceID, ServiceName FROM Service ORDER BY ServiceName");
	LoadAppointments();
	return TRUE;
}

void CAppointmentDlg::FillCombo(CComboBox& box, const char* sql)
{
	box.ResetContent();

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	MYSQL_RES* res = db.Query(sql);
	if (!res) return;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res)))
	{
		int idx = box.AddString(FromUtf8(row[1]));
		box.SetItemData(idx, (DWORD_PTR)atoi(row[0] ? row[0] : "0"));
	}
	mysql_free_result(res);
}

int CAppointmentDlg::SelectedData(CComboBox& box)
{
	int idx = box.GetCurSel();
	if (idx == CB_ERR) return 0;
	return (int)box.GetItemData(idx);
}

void CAppointmentDlg::SelectByData(CComboBox& box, int id)
{
	for (int i = 0; i < box.GetCount(); i++)
	{
		if ((int)box.GetItemData(i) == id) { box.SetCurSel(i); return; }
	}
	box.SetCurSel(-1);
}

void CAppointmentDlg::LoadAppointments()
{
	m_list.DeleteAllItems();

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	MYSQL_RES* res = db.Query(
		"SELECT a.AppointmentID, c.Name, s.Name, sv.ServiceName, a.AppointmentDate, "
		"TIME_FORMAT(a.AppointmentTime,'%H:%i'), a.Status "
		"FROM Appointment a "
		"JOIN Customer c ON a.CustomerID = c.CustomerID "
		"JOIN Stylist s ON a.StylistID = s.StylistID "
		"JOIN Service sv ON a.ServiceID = sv.ServiceID "
		"ORDER BY a.AppointmentDate, a.AppointmentTime");
	if (!res) return;

	MYSQL_ROW row;
	int i = 0;
	while ((row = mysql_fetch_row(res)))
	{
		m_list.InsertItem(i, FromUtf8(row[0]));
		for (int c = 1; c <= 6; c++)
			m_list.SetItemText(i, c, FromUtf8(row[c]));
		i++;
	}
	mysql_free_result(res);

	int cols = m_list.GetHeaderCtrl()->GetItemCount();
	m_list.SetColumnWidth(0, 60);
	for (int c = 1; c < cols; c++)
		m_list.SetColumnWidth(c, LVSCW_AUTOSIZE_USEHEADER);
}

bool CAppointmentDlg::ValidateInput(int& custId, int& styId, int& svcId)
{
	UpdateData(TRUE);
	m_date.Trim();
	m_time.Trim();

	custId = SelectedData(m_comboCustomer);
	styId = SelectedData(m_comboStylist);
	svcId = SelectedData(m_comboService);

	if (custId == 0 || styId == 0 || svcId == 0)
	{
		AfxMessageBox(L"Please select a customer, a stylist and a service.", MB_ICONWARNING);
		return false;
	}
	if (!IsValidDate(m_date))
	{
		AfxMessageBox(L"Enter the date as YYYY-MM-DD, for example 2026-10-15.", MB_ICONWARNING);
		return false;
	}
	if (!IsValidTime(m_time))
	{
		AfxMessageBox(L"Enter the time as HH:MM in 24-hour format, for example 14:30.", MB_ICONWARNING);
		return false;
	}
	return true;
}

bool CAppointmentDlg::HasConflict(int styId, const std::string& date, const std::string& time, int excludeId)
{
	DBConnection db;
	if (!db.Open()) return false;

	std::string sql = "SELECT COUNT(*) FROM Appointment WHERE StylistID=" + std::to_string(styId)
		+ " AND AppointmentDate='" + db.Escape(date)
		+ "' AND AppointmentTime='" + db.Escape(time)
		+ "' AND AppointmentID<>" + std::to_string(excludeId);

	bool conflict = false;
	MYSQL_RES* res = db.Query(sql);
	if (res)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row && row[0]) conflict = atoi(row[0]) > 0;
		mysql_free_result(res);
	}
	return conflict;
}

void CAppointmentDlg::OnBnClickedBtnAadd()
{
	int c, s, v;
	if (!ValidateInput(c, s, v)) return;

	std::string date = ToUtf8(m_date);
	std::string time = ToUtf8(m_time);

	if (HasConflict(s, date, time, 0))
	{
		AfxMessageBox(L"This stylist already has an appointment at that date and time.", MB_ICONWARNING);
		return;
	}

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	std::string sql = "INSERT INTO Appointment (CustomerID, StylistID, ServiceID, AppointmentDate, AppointmentTime, Status) VALUES ("
		+ std::to_string(c) + "," + std::to_string(s) + "," + std::to_string(v) + ",'"
		+ db.Escape(date) + "','" + db.Escape(time) + "','Booked')";

	if (db.Execute(sql)) AfxMessageBox(L"Appointment booked successfully.", MB_ICONINFORMATION);
	else AfxMessageBox(L"Failed to book the appointment. Please check the date.", MB_ICONERROR);

	LoadAppointments();
}

void CAppointmentDlg::OnBnClickedBtnAupdate()
{
	if (m_selectedId == 0) { AfxMessageBox(L"Please select an appointment from the list.", MB_ICONWARNING); return; }

	int c, s, v;
	if (!ValidateInput(c, s, v)) return;

	std::string date = ToUtf8(m_date);
	std::string time = ToUtf8(m_time);

	if (HasConflict(s, date, time, m_selectedId))
	{
		AfxMessageBox(L"This stylist already has an appointment at that date and time.", MB_ICONWARNING);
		return;
	}

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	std::string sql = "UPDATE Appointment SET CustomerID=" + std::to_string(c)
		+ ", StylistID=" + std::to_string(s)
		+ ", ServiceID=" + std::to_string(v)
		+ ", AppointmentDate='" + db.Escape(date)
		+ "', AppointmentTime='" + db.Escape(time)
		+ "' WHERE AppointmentID=" + std::to_string(m_selectedId);

	if (db.Execute(sql)) AfxMessageBox(L"Appointment updated successfully.", MB_ICONINFORMATION);
	else AfxMessageBox(L"Failed to update the appointment.", MB_ICONERROR);

	LoadAppointments();
}

void CAppointmentDlg::OnBnClickedBtnAdelete()
{
	if (m_selectedId == 0) { AfxMessageBox(L"Please select an appointment from the list.", MB_ICONWARNING); return; }
	if (AfxMessageBox(L"Are you sure you want to delete this appointment?", MB_YESNO | MB_ICONQUESTION) != IDYES) return;

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	if (db.Execute("DELETE FROM Appointment WHERE AppointmentID=" + std::to_string(m_selectedId)))
		AfxMessageBox(L"Appointment deleted successfully.", MB_ICONINFORMATION);
	else
		AfxMessageBox(L"Failed to delete the appointment.", MB_ICONERROR);

	m_selectedId = 0;
	m_date = m_time = L"";
	UpdateData(FALSE);
	LoadAppointments();
}

void CAppointmentDlg::OnNMClickListAppts(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (p->iItem >= 0)
	{
		m_selectedId = _ttoi(m_list.GetItemText(p->iItem, 0));

		DBConnection db;
		if (db.Open())
		{
			MYSQL_RES* res = db.Query(
				"SELECT CustomerID, StylistID, ServiceID, AppointmentDate, TIME_FORMAT(AppointmentTime,'%H:%i') "
				"FROM Appointment WHERE AppointmentID=" + std::to_string(m_selectedId));
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				if (row)
				{
					SelectByData(m_comboCustomer, atoi(row[0] ? row[0] : "0"));
					SelectByData(m_comboStylist, atoi(row[1] ? row[1] : "0"));
					SelectByData(m_comboService, atoi(row[2] ? row[2] : "0"));
					m_date = FromUtf8(row[3]);
					m_time = FromUtf8(row[4]);
					UpdateData(FALSE);
				}
				mysql_free_result(res);
			}
		}
	}
	*pResult = 0;
}

HBRUSH CAppointmentDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		static CBrush white(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetTextColor(RGB(60, 40, 20));
		return (HBRUSH)white;
	}
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(110, 72, 20));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CAppointmentDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	// background photo, then ONE cream card behind all controls
	Theme::PaintImageBg(*pDC, rc, _T("appointmentsbg.jpg"), 60);
	Theme::DrawFormPanel(this, *pDC);

	return TRUE;
}