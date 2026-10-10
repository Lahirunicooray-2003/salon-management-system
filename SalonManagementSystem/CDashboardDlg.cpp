// CDashboardDlg.cpp : implementation file
//

#include "pch.h"
#include "SalonManagementSystem.h"
#include "afxdialogex.h"
#include "CDashboardDlg.h"
#include "CCustomerDlg.h"
#include "CServiceDlg.h"


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
	DDX_Control(pDX, IDC_STATIC_DTITLE, m_title);
}


BEGIN_MESSAGE_MAP(CDashboardDlg, CDialogEx)
	ON_WM_CTLCOLOR()
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
    m_title.SetFont(&m_titleFont);
    m_bgBrush.CreateSolidBrush(RGB(255, 240, 245));

    return TRUE;
}
HBRUSH CDashboardDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT);

        if (pWnd->GetDlgCtrlID() == IDC_STATIC_DTITLE)
            pDC->SetTextColor(RGB(180, 40, 100));

        return (HBRUSH)m_bgBrush.GetSafeHandle();
    }

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
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
    AfxMessageBox(L"Coming soon...");
    // TODO: Add your control notification handler code here
}

void CDashboardDlg::OnBnClickedBtnServices()
{
    CServiceDlg dlg;
    dlg.DoModal();
}

void CDashboardDlg::OnBnClickedBtnStylists()
{
    AfxMessageBox(L"Coming soon...");
    // TODO: Add your control notification handler code here
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
