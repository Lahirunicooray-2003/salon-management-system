// CDashboardDlg.cpp : implementation file
//

#include "pch.h"
#include "SalonManagementSystem.h"
#include "afxdialogex.h"
#include "CDashboardDlg.h"
#include "CCustomerDlg.h"
#include "CServiceDlg.h"
#include "CStylistDlg.h"
#include "Theme.h"
#include "CAppointmentDlg.h"

// CDashboardDlg dialog

IMPLEMENT_DYNAMIC(CDashboardDlg, CDialogEx)

CDashboardDlg::CDashboardDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DASHBOARD1_DIALOG, pParent)
{

}

CDashboardDlg::~CDashboardDlg()
{
}

void CDashboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


 
}


BEGIN_MESSAGE_MAP(CDashboardDlg, CDialogEx)
	ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_BTN_CUSTOMERS, &CDashboardDlg::OnBnClickedBtnCustomers)
    ON_BN_CLICKED(IDC_BTN_APPOINTMENTS, &CDashboardDlg::OnBnClickedBtnAppointments)
    ON_BN_CLICKED(IDC_BTN_SERVICES, &CDashboardDlg::OnBnClickedBtnServices)
    ON_BN_CLICKED(IDC_BTN_STYLISTS, &CDashboardDlg::OnBnClickedBtnStylists)
    ON_BN_CLICKED(IDC_BTN_LOGOUT, &CDashboardDlg::OnBnClickedBtnLogout)
    ON_STN_CLICKED(IDC_STATIC_DTITLE, &CDashboardDlg::OnStnClickedStaticDtitle)
END_MESSAGE_MAP()


// CDashboardDlg message handlers
BOOL CDashboardDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_titleFont.CreatePointFont(160, L"Segoe UI Semibold");

    CWnd* pTitle = GetDlgItem(IDC_STATIC_DTITLE);
    if (pTitle != nullptr)
        pTitle->SetFont(&m_titleFont);

    m_bgBrush.CreateSolidBrush(Theme::Bg);

    m_goldCustomers.SubclassDlgItem(IDC_BTN_CUSTOMERS, this);
    m_goldServices.SubclassDlgItem(IDC_BTN_SERVICES, this);
    m_goldStylists.SubclassDlgItem(IDC_BTN_STYLISTS, this);
    m_goldAppointments.SubclassDlgItem(IDC_BTN_APPOINTMENTS, this);
    m_goldLogout.SubclassDlgItem(IDC_BTN_LOGOUT, this);

    return TRUE;
}
HBRUSH CDashboardDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT);

        if (pWnd->GetDlgCtrlID() == IDC_STATIC_DTITLE)
            pDC->SetTextColor(Theme::GoldDark);

        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }

    return hbr;
}

void CDashboardDlg::OnBnClickedBtnCustomers()
{
    CCustomerDlg dlg;
    dlg.DoModal();
    // TODO: Add your control notification handler code here
}

void CDashboardDlg::OnBnClickedBtnAppointments()
{
    CAppointmentDlg dlg;
    dlg.DoModal();
}

void CDashboardDlg::OnBnClickedBtnServices()
{
    CServiceDlg dlg;
    dlg.DoModal();
}

void CDashboardDlg::OnBnClickedBtnStylists()
{
    CStylistDlg dlg;
    dlg.DoModal();
}

void CDashboardDlg::OnBnClickedBtnLogout()
{
    EndDialog(IDOK);
    // TODO: Add your control notification handler code here
}

void CDashboardDlg::OnStnClickedStaticDtitle()
{
    // TODO: Add your control notification handler code here
}



BOOL CDashboardDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    GetClientRect(rc);
    Theme::PaintGradient(*pDC, rc);

    CRect banner(0, 0, rc.Width(), 150);
    Theme::DrawBanner(*pDC, banner);

    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(255, 255, 255));

    CFont f1, f2;
    f1.CreateFontW(-38, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    f2.CreateFontW(-17, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

    CFont* old = pDC->SelectObject(&f1);
    pDC->TextOut(40, 36, L"Welcome, Admin");
    pDC->SelectObject(&f2);
    pDC->TextOut(42, 90, L"Salon Aura  -  Manage customers, services, stylists and appointments");
    pDC->SelectObject(old);
    return TRUE;
}