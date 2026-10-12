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
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CSalonManagementSystemDlg message handlers

BOOL CSalonManagementSystemDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_goldLogin.SubclassDlgItem(IDC_BTN_LOGIN, this);

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
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    m_bgBrush.CreateSolidBrush(Theme::Bg);
    m_editBrush.CreateSolidBrush(RGB(255, 252, 246)); // Edit box එකට ක්‍රීම් පාට පසුබිම
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
            AfxMessageBox(L"Unable to connect to the database.", MB_ICONERROR);
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
        AfxMessageBox(L"Incorrect username or password.", MB_ICONWARNING);
    }
}

HBRUSH CSalonManagementSystemDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetTextColor(RGB(166, 118, 38));
        pDC->SetBkMode(TRANSPARENT);
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }

    // Username සහ Password Edit boxes වල පාට වෙනස් කිරීම
    if (nCtlColor == CTLCOLOR_EDIT)
    {
        pDC->SetTextColor(RGB(75, 55, 40));
        pDC->SetBkColor(RGB(255, 252, 246));

        return m_editBrush.GetSafeHandle()
            ? (HBRUSH)m_editBrush.GetSafeHandle()
            : (HBRUSH)GetStockObject(WHITE_BRUSH);
    }

    return hbr;
}

BOOL CSalonManagementSystemDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    GetClientRect(&rc);

    // 1) Background photo (loaded once), cropped to fill without stretching
    static CImage bg;
    static bool tried = false;
    if (!tried)
    {
        tried = true;
        TCHAR p[MAX_PATH];
        GetModuleFileName(NULL, p, MAX_PATH);
        CString path(p);
        path = path.Left(path.ReverseFind(_T('\\')) + 1) + _T("loginbg.jpg");
        bg.Load(path);
    }

    if (!bg.IsNull())
    {
        int iw = bg.GetWidth(), ih = bg.GetHeight();
        int sw = iw, sh = ih;
        // crop source so its aspect ratio matches the dialog
        if (iw * rc.Height() > ih * rc.Width()) sw = ih * rc.Width() / rc.Height();
        else                                    sh = iw * rc.Height() / rc.Width();
        int sx = (iw - sw) / 2, sy = (ih - sh) / 2;
        pDC->SetStretchBltMode(HALFTONE);
        bg.Draw(pDC->GetSafeHdc(), 0, 0, rc.Width(), rc.Height(), sx, sy, sw, sh);
    }
    else
    {
        Theme::PaintGradient(*pDC, rc);
    }

    // 2) Card rectangle that wraps all controls + the logo area
    CRect ur;
    GetDlgItem(IDC_EDIT_USERNAME)->GetWindowRect(&ur);
    ScreenToClient(&ur);

    int availH = ur.top - 30;
    int h = availH;
    int w = h * 420 / 300;
    CRect logo((rc.Width() - w) / 2, 10, (rc.Width() + w) / 2, 10 + h);

    CRect card = logo;
    for (CWnd* c = GetWindow(GW_CHILD); c != NULL; c = c->GetWindow(GW_HWNDNEXT))
    {
        if (!c->IsWindowVisible()) continue;
        CRect r;
        c->GetWindowRect(&r);
        ScreenToClient(&r);
        card.UnionRect(card, r);
    }
    card.InflateRect(30, 12);
    card.IntersectRect(card, rc);

    // semi-transparent cream card with rounded corners
    CRgn rgn;
    rgn.CreateRoundRectRgn(card.left, card.top, card.right + 1, card.bottom + 1, 28, 28);
    int saved = pDC->SaveDC();
    pDC->SelectClipRgn(&rgn);

    CDC mem;
    mem.CreateCompatibleDC(pDC);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(pDC, card.Width(), card.Height());
    CBitmap* old = mem.SelectObject(&bmp);
    mem.FillSolidRect(0, 0, card.Width(), card.Height(), RGB(255, 251, 242));
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 225, 0 };
    AlphaBlend(pDC->GetSafeHdc(), card.left, card.top, card.Width(), card.Height(),
        mem.GetSafeHdc(), 0, 0, card.Width(), card.Height(), bf);
    mem.SelectObject(old);
    pDC->RestoreDC(saved);

    // gold border
    CPen pen(PS_SOLID, 2, RGB(224, 196, 140));
    CPen* op = pDC->SelectObject(&pen);
    CBrush* ob = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    pDC->RoundRect(card, CPoint(28, 28));
    pDC->SelectObject(op);
    pDC->SelectObject(ob);

    // 3) logo on the card
    Theme::DrawLogo(*pDC, logo);
    return TRUE;
}