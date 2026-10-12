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

    // logo: as big as the space above the buttons allows
    int availH = 200;                                   // fallback
    CWnd* pBtn = GetDlgItem(IDC_BTN_CUSTOMERS);
    if (pBtn != nullptr)
    {
        CRect br;
        pBtn->GetWindowRect(br);
        ScreenToClient(br);
        availH = br.top - 100;                          // leaves room for "Welcome" text
    }

    int h = availH;
    int w = h * 420 / 300;                              // keep logo proportions
    CRect logo((rc.Width() - w) / 2, 10, (rc.Width() + w) / 2, 10 + h);
    Theme::DrawLogo(*pDC, logo);
    return TRUE;
}