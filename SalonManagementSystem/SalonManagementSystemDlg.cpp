
// SalonManagementSystemDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SalonManagementSystem.h"
#include "SalonManagementSystemDlg.h"
#include "afxdialogex.h"
#include "CDashboardDlg.h"
#include "DBConnection.h"
#include <string>

static std::string ToUtf8(const CString& s)
{
	CStringA a(CW2A(s, CP_UTF8));
	return std::string(a);
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSalonManagementSystemDlg dialog



CSalonManagementSystemDlg::CSalonManagementSystemDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SALONMANAGEMENTSYSTEM_DIALOG, pParent)
	, m_username(_T(""))
	, m_password(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSalonManagementSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
}

BEGIN_MESSAGE_MAP(CSalonManagementSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CSalonManagementSystemDlg::OnBnClickedBtnLogin)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSalonManagementSystemDlg message handlers

BOOL CSalonManagementSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_bgBrush.CreateSolidBrush(Theme::Bg);
	m_titleFont.CreatePointFont(260, L"Segoe UI Semibold");

	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		CString txt;
		pChild->GetWindowText(txt);
		if (txt == L"SALON AURA")
		{
			pChild->SetFont(&m_titleFont);
			m_titleHwnd = pChild->GetSafeHwnd();
			break;
		}
		pChild = pChild->GetNextWindow();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSalonManagementSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSalonManagementSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSalonManagementSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSalonManagementSystemDlg::OnBnClickedBtnLogin()
{
	UpdateData(TRUE);

	bool ok = false;
	{
		DBConnection db;
		if (!db.Open())
		{
			AfxMessageBox(L"Unable to connect to the database.");
			return;
		}

		std::string sql = "SELECT Role FROM Users WHERE Username='"
			+ db.Escape(ToUtf8(m_username)) + "' AND Password='"
			+ db.Escape(ToUtf8(m_password)) + "'";

		MYSQL_RES* res = db.Query(sql);
		if (res)
		{
			ok = mysql_num_rows(res) > 0;
			mysql_free_result(res);
		}
	}

	if (ok)
	{
		ShowWindow(SW_HIDE);
		CDashboardDlg dlg;
		dlg.DoModal();
		ShowWindow(SW_SHOW);

		m_username = L"";
		m_password = L"";
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(L"Incorrect username or password.");
	}
}	// TODO: Add your control notification handler code here}
HBRUSH CSalonManagementSystemDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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