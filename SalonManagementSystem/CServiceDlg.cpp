#include "pch.h"
#include "SalonManagementSystem.h"
#include "afxdialogex.h"
#include "CServiceDlg.h"
#include "DBConnection.h"
#include "Theme.h"
#include <string>

static std::string ToUtf8(const CString& s)
{
	CStringA a(CW2A(s, CP_UTF8));
	return std::string(a);
}

IMPLEMENT_DYNAMIC(CServiceDlg, CDialogEx)

CServiceDlg::CServiceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVICE_DIALOG, pParent)
{
}

CServiceDlg::~CServiceDlg()
{
}

void CServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVICES, m_list);
	DDX_Text(pDX, IDC_EDIT_SNAME, m_name);
	DDX_Text(pDX, IDC_EDIT_SPRICE, m_price);

}

BEGIN_MESSAGE_MAP(CServiceDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SADD, &CServiceDlg::OnBnClickedBtnSadd)
	ON_BN_CLICKED(IDC_BTN_SUPDATE, &CServiceDlg::OnBnClickedBtnSupdate)
	ON_BN_CLICKED(IDC_BTN_SDELETE, &CServiceDlg::OnBnClickedBtnSdelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SERVICES, &CServiceDlg::OnNMClickListServices)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CServiceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_goldAdd.SubclassDlgItem(IDC_BTN_SADD, this);
	m_goldUpdate.SubclassDlgItem(IDC_BTN_SUPDATE, this);
	m_goldDelete.SubclassDlgItem(IDC_BTN_SDELETE, this);
	m_goldBack.SubclassDlgItem(IDCANCEL, this);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"ID", LVCFMT_LEFT, 50);
	m_list.InsertColumn(1, L"Service", LVCFMT_LEFT, 170);
	m_list.InsertColumn(2, L"Price (Rs.)", LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, L"Duration (min)", LVCFMT_LEFT, 110);

	m_bgBrush.CreateSolidBrush(Theme::Bg);
	m_titleFont.CreatePointFont(180, L"Segoe UI Semibold");

	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		CString txt;
		pChild->GetWindowText(txt);
		if (txt == L"Service Management")
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

	LoadServices();
	return TRUE;
}

void CServiceDlg::LoadServices()
{
	m_list.DeleteAllItems();

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	MYSQL_RES* res = db.Query("SELECT ServiceID, ServiceName, Price, Duration FROM Service");
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

	int cols = m_list.GetHeaderCtrl()->GetItemCount();
	m_list.SetColumnWidth(0, 60);                                  // ID
	for (int c = 1; c < cols; c++)
		m_list.SetColumnWidth(c, LVSCW_AUTOSIZE_USEHEADER);        // other columns fit content
}

void CServiceDlg::OnBnClickedBtnSadd()
{
	UpdateData(TRUE);
	double price = _tstof(m_price);
	int duration = _ttoi(m_duration);
	if (m_name.IsEmpty() || price <= 0 || duration <= 0)
	{
		AfxMessageBox(L"Enter a service name, a valid price, and a duration in minutes.", MB_ICONWARNING);
		return;
	}

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	std::string sql = "INSERT INTO Service (ServiceName, Price, Duration) VALUES ('"
		+ db.Escape(ToUtf8(m_name)) + "',"
		+ std::to_string(price) + ","
		+ std::to_string(duration) + ")";

	if (db.Execute(sql)) AfxMessageBox(L"Service added successfully.", MB_ICONINFORMATION);
	else AfxMessageBox(L"Failed to add the service.", MB_ICONERROR);

	m_name = m_price = m_duration = L"";
	UpdateData(FALSE);
	LoadServices();
}

void CServiceDlg::OnBnClickedBtnSupdate()
{
	UpdateData(TRUE);
	if (m_selectedId == 0) { AfxMessageBox(L"Please select a service from the list.", MB_ICONWARNING); return; }

	double price = _tstof(m_price);
	int duration = _ttoi(m_duration);
	if (m_name.IsEmpty() || price <= 0 || duration <= 0)
	{
		AfxMessageBox(L"Enter a service name, a valid price, and a duration in minutes.", MB_ICONWARNING);
		return;
	}

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	std::string sql = "UPDATE Service SET ServiceName='" + db.Escape(ToUtf8(m_name))
		+ "', Price=" + std::to_string(price)
		+ ", Duration=" + std::to_string(duration)
		+ " WHERE ServiceID=" + std::to_string(m_selectedId);

	if (db.Execute(sql)) AfxMessageBox(L"Service updated successfully.", MB_ICONINFORMATION);
	else AfxMessageBox(L"Failed to update the service.", MB_ICONERROR);
	LoadServices();
}

void CServiceDlg::OnBnClickedBtnSdelete()
{
	if (m_selectedId == 0) { AfxMessageBox(L"Please select a service from the list.", MB_ICONWARNING); return; }
	if (AfxMessageBox(L"Are you sure you want to delete this service?", MB_YESNO | MB_ICONQUESTION) != IDYES) return;

	DBConnection db;
	if (!db.Open()) { AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR); return; }

	if (db.Execute("DELETE FROM Service WHERE ServiceID=" + std::to_string(m_selectedId)))
		AfxMessageBox(L"Service deleted successfully.", MB_ICONINFORMATION);
	else
		AfxMessageBox(L"Failed to delete the service.", MB_ICONERROR);

	m_selectedId = 0;
	m_name = m_price = m_duration = L"";
	UpdateData(FALSE);
	LoadServices();
}

void CServiceDlg::OnNMClickListServices(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (p->iItem >= 0)
	{
		m_selectedId = _ttoi(m_list.GetItemText(p->iItem, 0));
		m_name = m_list.GetItemText(p->iItem, 1);
		m_price = m_list.GetItemText(p->iItem, 2);
		m_duration = m_list.GetItemText(p->iItem, 3);
		UpdateData(FALSE);
	}
	*pResult = 0;
}
HBRUSH CServiceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(166, 118, 38));   // gold-brown, cream එක උඩ පැහැදිලිව පේන්න
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}