#include "pch.h"
#include "SalonManagementSystem.h"
#include "afxdialogex.h"
#include "CCustomerDlg.h"
#include "DBConnection.h"
#include "Customer.h"
#include "Theme.h"
#include "SearchBox.h"

#include <string>
#include <stdexcept>

#include <afxcmn.h>
#include <afxwin.h>

// CString -> UTF-8
static std::string ToUtf8(const CString& s)
{
    CStringA a(CW2A(s, CP_UTF8));
    return std::string(a);
}

// search box that filters the list while typing
static CSearchBox s_search;

IMPLEMENT_DYNAMIC(CCustomerDlg, CDialogEx)

CCustomerDlg::CCustomerDlg(CWnd* pParent)
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

    ON_BN_CLICKED(IDC_BTN_CADD,
        &CCustomerDlg::OnBnClickedBtnCadd)

    ON_BN_CLICKED(IDC_BTN_CUPDATE,
        &CCustomerDlg::OnBnClickedBtnCupdate)

    ON_BN_CLICKED(IDC_BTN_CDELETE,
        &CCustomerDlg::OnBnClickedBtnCdelete)

    ON_NOTIFY(NM_CLICK, IDC_LIST_CUSTOMERS,
        &CCustomerDlg::OnNMClickListCustomers)

    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_CUSTOMERS,
        &CCustomerDlg::OnNMCustomdrawListCustomers)

    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()

    ON_STN_CLICKED(IDC_CTITLE, &CCustomerDlg::OnStnClickedCtitle)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CUSTOMERS, &CCustomerDlg::OnLvnItemchangedListCustomers)
END_MESSAGE_MAP()


BOOL CCustomerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Back button
    m_goldBack.SubclassDlgItem(IDCANCEL, this);

    // Title
    m_titleFont.CreatePointFont(180, L"Segoe UI Semibold");
    m_title.SetFont(&m_titleFont);

    // Gold buttons
    StyleGoldButton(m_btnAdd);
    StyleGoldButton(m_btnUpdate);
    StyleGoldButton(m_btnDelete);

    // Customer table
    m_list.SetExtendedStyle(
        LVS_EX_FULLROWSELECT |
        LVS_EX_GRIDLINES |
        LVS_EX_DOUBLEBUFFER
    );

    // Insert columns
    m_list.InsertColumn(0, L"ID", LVCFMT_LEFT, 60);
    m_list.InsertColumn(1, L"Name", LVCFMT_LEFT, 170);
    m_list.InsertColumn(2, L"Phone", LVCFMT_LEFT, 150);
    m_list.InsertColumn(3, L"Email", LVCFMT_LEFT, 230);

    // Cream table background
    m_list.SetBkColor(RGB(255, 250, 240));
    m_list.SetTextBkColor(RGB(255, 250, 240));
    m_list.SetTextColor(RGB(90, 65, 45));

    // Table font
    m_tableFont.CreatePointFont(100, L"Segoe UI");
    m_list.SetFont(&m_tableFont);

    // Background brushes
    m_bgBrush.CreateSolidBrush(RGB(255, 248, 235));
    m_editBrush.CreateSolidBrush(RGB(255, 252, 246));

    // Initial selection
    m_selectedId = 0;

    // Search box above the table
    s_search.Init(this, &m_list, 9100, L"Search...");

    // Load database records
    LoadCustomers();

    return TRUE;
}


// ---------------------------------------------
// LOAD CUSTOMERS
// ---------------------------------------------

void CCustomerDlg::LoadCustomers()
{
    m_list.DeleteAllItems();

    DBConnection db;

    if (!db.Open())
    {
        AfxMessageBox(
            L"Unable to connect to the database.",
            MB_ICONERROR
        );
        return;
    }

    MYSQL_RES* res = db.Query(
        "SELECT CustomerID, Name, Phone, Email "
        "FROM Customer ORDER BY CustomerID"
    );

    if (!res)
    {
        AfxMessageBox(
            L"Unable to load customer records.",
            MB_ICONERROR
        );
        return;
    }

    MYSQL_ROW row;
    int i = 0;

    while ((row = mysql_fetch_row(res)))
    {
        m_list.InsertItem(
            i,
            CString(CA2W(row[0] ? row[0] : "", CP_UTF8))
        );

        m_list.SetItemText(
            i, 1,
            CString(CA2W(row[1] ? row[1] : "", CP_UTF8))
        );

        m_list.SetItemText(
            i, 2,
            CString(CA2W(row[2] ? row[2] : "", CP_UTF8))
        );

        m_list.SetItemText(
            i, 3,
            CString(CA2W(row[3] ? row[3] : "", CP_UTF8))
        );

        ++i;
    }

    mysql_free_result(res);

    // Adjust column widths
    m_list.SetColumnWidth(0, 60);
    m_list.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    m_list.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
    m_list.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);

    s_search.Capture();   // remember all rows, re-apply the filter

    m_list.Invalidate();
}


// ---------------------------------------------
// ADD CUSTOMER
// ---------------------------------------------

void CCustomerDlg::OnBnClickedBtnCadd()
{
    if (!UpdateData(TRUE))
        return;

    try
    {
        Customer c(
            ToUtf8(m_name),
            ToUtf8(m_phone),
            ToUtf8(m_email)
        );

        DBConnection db;

        if (!db.Open())
        {
            AfxMessageBox(
                L"Unable to connect to the database.",
                MB_ICONERROR
            );
            return;
        }

        std::string sql =
            "INSERT INTO Customer (Name, Phone, Email) VALUES ('" +
            db.Escape(c.getName()) + "','" +
            db.Escape(c.getPhone()) + "','" +
            db.Escape(c.getEmail()) + "')";

        if (db.Execute(sql))
        {
            AfxMessageBox(
                L"Customer added successfully.",
                MB_ICONINFORMATION
            );

            m_selectedId = 0;
            m_name.Empty();
            m_phone.Empty();
            m_email.Empty();

            UpdateData(FALSE);
            LoadCustomers();
        }
        else
        {
            AfxMessageBox(
                L"Failed to add the customer.",
                MB_ICONERROR
            );
        }
    }
    catch (const std::invalid_argument& e)
    {
        AfxMessageBox(
            CString(CA2W(e.what(), CP_UTF8)),
            MB_ICONWARNING
        );
    }
}


// ---------------------------------------------
// UPDATE CUSTOMER
// ---------------------------------------------

void CCustomerDlg::OnBnClickedBtnCupdate()
{
    if (!UpdateData(TRUE))
        return;

    if (m_selectedId == 0)
    {
        AfxMessageBox(
            L"Please select a customer from the list.",
            MB_ICONWARNING
        );
        return;
    }

    try
    {
        Customer c(
            ToUtf8(m_name),
            ToUtf8(m_phone),
            ToUtf8(m_email)
        );

        DBConnection db;

        if (!db.Open())
        {
            AfxMessageBox(
                L"Unable to connect to the database.",
                MB_ICONERROR
            );
            return;
        }

        std::string sql =
            "UPDATE Customer SET Name='" +
            db.Escape(c.getName()) +
            "', Phone='" +
            db.Escape(c.getPhone()) +
            "', Email='" +
            db.Escape(c.getEmail()) +
            "' WHERE CustomerID=" +
            std::to_string(m_selectedId);

        if (db.Execute(sql))
        {
            AfxMessageBox(
                L"Customer updated successfully.",
                MB_ICONINFORMATION
            );

            LoadCustomers();
        }
        else
        {
            AfxMessageBox(
                L"Failed to update the customer.",
                MB_ICONERROR
            );
        }
    }
    catch (const std::invalid_argument& e)
    {
        AfxMessageBox(
            CString(CA2W(e.what(), CP_UTF8)),
            MB_ICONWARNING
        );
    }
}


// ---------------------------------------------
// DELETE CUSTOMER
// ---------------------------------------------

void CCustomerDlg::OnBnClickedBtnCdelete()
{
    if (m_selectedId == 0)
    {
        AfxMessageBox(
            L"Please select a customer from the list.",
            MB_ICONWARNING
        );
        return;
    }

    if (AfxMessageBox(
        L"Are you sure you want to delete this customer?",
        MB_YESNO | MB_ICONQUESTION
    ) != IDYES)
    {
        return;
    }

    DBConnection db;

    if (!db.Open())
    {
        AfxMessageBox(
            L"Unable to connect to the database.",
            MB_ICONERROR
        );
        return;
    }

    std::string sql =
        "DELETE FROM Customer WHERE CustomerID=" +
        std::to_string(m_selectedId);

    if (db.Execute(sql))
    {
        AfxMessageBox(
            L"Customer deleted successfully.",
            MB_ICONINFORMATION
        );

        m_selectedId = 0;
        m_name.Empty();
        m_phone.Empty();
        m_email.Empty();

        UpdateData(FALSE);
        LoadCustomers();
    }
    else
    {
        AfxMessageBox(
            L"Failed to delete the customer.",
            MB_ICONERROR
        );
    }
}


// ---------------------------------------------
// SELECT CUSTOMER FROM TABLE
// ---------------------------------------------

void CCustomerDlg::OnNMClickListCustomers(
    NMHDR* pNMHDR,
    LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNMHDR);

    int index = m_list.GetNextItem(
        -1, LVNI_SELECTED
    );

    if (index >= 0)
    {
        m_selectedId = _ttoi(
            m_list.GetItemText(index, 0)
        );

        m_name = m_list.GetItemText(index, 1);
        m_phone = m_list.GetItemText(index, 2);
        m_email = m_list.GetItemText(index, 3);

        UpdateData(FALSE);
    }

    *pResult = 0;
}


// ---------------------------------------------
// TABLE COLOUR STYLING
// ---------------------------------------------

void CCustomerDlg::OnNMCustomdrawListCustomers(
    NMHDR* pNMHDR,
    LRESULT* pResult)
{
    LPNMLVCUSTOMDRAW pLVCD =
        reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

    switch (pLVCD->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        return;

    case CDDS_ITEMPREPAINT:
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
        return;

    case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
    {
        int row = static_cast<int>(
            pLVCD->nmcd.dwItemSpec
            );

        // Alternating cream shades
        if (row % 2 == 0)
        {
            pLVCD->clrTextBk = RGB(255, 250, 240);
        }
        else
        {
            pLVCD->clrTextBk = RGB(247, 238, 220);
        }

        pLVCD->clrText = RGB(90, 65, 45);

        *pResult = CDRF_DODEFAULT;
        return;
    }
    }

    *pResult = CDRF_DODEFAULT;
}


// ---------------------------------------------
// LABEL AND INPUT COLOURS
// ---------------------------------------------

HBRUSH CCustomerDlg::OnCtlColor(
    CDC* pDC,
    CWnd* pWnd,
    UINT nCtlColor)
{
    int id = pWnd->GetDlgCtrlID();

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(110, 72, 35));

        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }

    if (nCtlColor == CTLCOLOR_EDIT)
    {
        pDC->SetTextColor(RGB(75, 55, 40));
        pDC->SetBkColor(RGB(255, 252, 246));

        return m_editBrush.GetSafeHandle()
            ? (HBRUSH)m_editBrush.GetSafeHandle()
            : (HBRUSH)GetStockObject(WHITE_BRUSH);
    }

    if (nCtlColor == CTLCOLOR_LISTBOX)
    {
        pDC->SetTextColor(RGB(90, 65, 45));
        pDC->SetBkColor(RGB(255, 250, 240));

        return (HBRUSH)m_bgBrush.GetSafeHandle();
    }

    UNREFERENCED_PARAMETER(id);

    return CDialogEx::OnCtlColor(
        pDC, pWnd, nCtlColor
    );
}


// ---------------------------------------------
// BACKGROUND IMAGE AND PANEL
// ---------------------------------------------

BOOL CCustomerDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    GetClientRect(&rc);

    
    Theme::PaintImageBg(
        *pDC, rc, _T("customersbg.jpg")
    );

    // Cream form panel
    Theme::DrawFormPanel(this, *pDC);

    return TRUE;
}
void CCustomerDlg::OnStnClickedCtitle()
{
    // TODO: Add your control notification handler code here
}
void CCustomerDlg::OnLvnItemchangedListCustomers(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}
