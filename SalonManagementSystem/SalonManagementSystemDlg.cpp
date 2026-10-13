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

// Loads logo.png (next to the .exe) once and removes its plain background,
// so the logo sits directly on the card without a visible box.
// Fast version: scales the logo down first and works on raw pixel memory.
static CImage* GetLogoArgb()
{
    static CImage argb;
    static bool tried = false;
    if (!tried)
    {
        tried = true;

        TCHAR p[MAX_PATH];
        GetModuleFileName(NULL, p, MAX_PATH);
        CString path(p);
        path = path.Left(path.ReverseFind(_T('\\')) + 1) + _T("logo.png");

        CImage src;
        if (FAILED(src.Load(path))) return NULL;

        int sw = src.GetWidth(), sh = src.GetHeight();
        if (sw <= 0 || sh <= 0) return NULL;

        // work at a sensible size (max 700 px) -> much faster
        int w = sw, h = sh;
        if (w > 700) { h = h * 700 / w; w = 700; }
        if (h > 700) { w = w * 700 / h; h = 700; }

        CImage tmp;
        if (!tmp.Create(w, h, 24)) return NULL;
        HDC hd = tmp.GetDC();
        SetStretchBltMode(hd, HALFTONE);
        SetBrushOrgEx(hd, 0, 0, NULL);
        src.StretchBlt(hd, 0, 0, w, h, SRCCOPY);
        tmp.ReleaseDC();

        if (!argb.Create(w, h, 32, CImage::createAlphaChannel)) return NULL;

        // background colour = average of the four corner pixels (BGR order in memory)
        BYTE* top = (BYTE*)tmp.GetPixelAddress(0, 0);
        BYTE* bot = (BYTE*)tmp.GetPixelAddress(0, h - 1);
        int br = (top[2] + top[3 * (w - 1) + 2] + bot[2] + bot[3 * (w - 1) + 2]) / 4;
        int bg = (top[1] + top[3 * (w - 1) + 1] + bot[1] + bot[3 * (w - 1) + 1]) / 4;
        int bb = (top[0] + top[3 * (w - 1) + 0] + bot[0] + bot[3 * (w - 1) + 0]) / 4;

        for (int y = 0; y < h; y++)
        {
            BYTE* s = (BYTE*)tmp.GetPixelAddress(0, y);
            BYTE* d = (BYTE*)argb.GetPixelAddress(0, y);
            for (int x = 0; x < w; x++, s += 3, d += 4)
            {
                int b = s[0], g = s[1], r = s[2];

                int dist = abs(r - br);
                int dg = abs(g - bg); if (dg > dist) dist = dg;
                int db = abs(b - bb); if (db > dist) dist = db;

                int a = (dist - 18) * 255 / 40;           // 0 near the background colour, 255 on the ink
                if (a < 0) a = 0;
                if (a > 255) a = 255;

                int pr = r - br * (255 - a) / 255; if (pr < 0) pr = 0;   // premultiplied
                int pg = g - bg * (255 - a) / 255; if (pg < 0) pg = 0;
                int pb = b - bb * (255 - a) / 255; if (pb < 0) pb = 0;

                d[0] = (BYTE)pb;
                d[1] = (BYTE)pg;
                d[2] = (BYTE)pr;
                d[3] = (BYTE)a;
            }
        }
    }
    return argb.IsNull() ? NULL : &argb;
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

    m_bgBrush.CreateSolidBrush(Theme::Bg);
    m_editBrush.CreateSolidBrush(RGB(243, 232, 205));      // beige edit boxes
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

    // smaller, elegant "Username:" / "Password:" labels
    static CFont labelFont;
    if (labelFont.GetSafeHandle() == NULL)
        labelFont.CreatePointFont(150, L"Segoe UI Semibold");

    for (CWnd* c = GetWindow(GW_CHILD); c != NULL; c = c->GetWindow(GW_HWNDNEXT))
    {
        CString txt;
        c->GetWindowText(txt);
        if (txt.Find(L"Username") >= 0 || txt.Find(L"Password") >= 0)
        {
            TCHAR cls[32] = { 0 };
            GetClassName(c->GetSafeHwnd(), cls, 31);
            if (_tcsicmp(cls, _T("Static")) != 0) continue;

            c->SetFont(&labelFont);
            c->ModifyStyle(0, SS_CENTERIMAGE);   // vertically centred next to the box
        }
    }

    // placeholder text inside the boxes
    ::SendMessage(GetDlgItem(IDC_EDIT_USERNAME)->GetSafeHwnd(), EM_SETCUEBANNER, TRUE, (LPARAM)L"Username");
    ::SendMessage(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd(), EM_SETCUEBANNER, TRUE, (LPARAM)L"Password");

    // preload logo + background now, so the first paint is instant
    GetLogoArgb();
    {
        CClientDC dcPre(this);
        CRect rcPre;
        GetClientRect(&rcPre);
        Theme::PaintImageBg(dcPre, rcPre, _T("loginbg.jpg"));
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

    // beige Username / Password edit boxes
    if (nCtlColor == CTLCOLOR_EDIT)
    {
        pDC->SetTextColor(RGB(75, 55, 40));
        pDC->SetBkColor(RGB(243, 232, 205));

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

    // 1) Background photo (cached, built once per window size)
    Theme::PaintImageBg(*pDC, rc, _T("loginbg.jpg"));

    // 2) Logo area above the Username box, keeping the logo's own proportions
    CRect ur;
    GetDlgItem(IDC_EDIT_USERNAME)->GetWindowRect(&ur);
    ScreenToClient(&ur);

    CImage* lg = GetLogoArgb();
    int top = 22;
    int h = ur.top - top - 16;
    if (h < 40) h = 40;
    int w = lg ? h * lg->GetWidth() / lg->GetHeight() : h * 420 / 300;
    if (w > rc.Width() - 100)
    {
        w = rc.Width() - 100;
        if (lg) h = w * lg->GetHeight() / lg->GetWidth();
    }
    CRect logo((rc.Width() - w) / 2, top, (rc.Width() + w) / 2, top + h);

    // 3) Card rectangle that wraps all controls + the logo
    CRect card = logo;
    for (CWnd* c = GetWindow(GW_CHILD); c != NULL; c = c->GetWindow(GW_HWNDNEXT))
    {
        if (!c->IsWindowVisible()) continue;
        CRect r;
        c->GetWindowRect(&r);
        ScreenToClient(&r);
        card.UnionRect(card, r);
    }
    card.InflateRect(30, 14);
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
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 205, 0 };
    AlphaBlend(pDC->GetSafeHdc(), card.left, card.top, card.Width(), card.Height(),
        mem.GetSafeHdc(), 0, 0, card.Width(), card.Height(), bf);
    mem.SelectObject(old);
    pDC->RestoreDC(saved);

    // thin gold border
    CPen pen(PS_SOLID, 1, RGB(232, 210, 160));
    CPen* op = pDC->SelectObject(&pen);
    CBrush* ob = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    pDC->RoundRect(card, CPoint(28, 28));
    pDC->SelectObject(op);
    pDC->SelectObject(ob);

    // 4) logo on the card (transparent background)
    if (lg)
    {
        pDC->SetStretchBltMode(HALFTONE);
        lg->Draw(pDC->GetSafeHdc(), logo);
    }
    else
    {
        Theme::DrawLogo(*pDC, logo);
    }
    return TRUE;
}