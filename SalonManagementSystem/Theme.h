#pragma once
#include <afxbutton.h>
#include <atlimage.h>
#pragma comment(lib, "msimg32.lib")

namespace Theme
{
	const COLORREF Bg = RGB(250, 244, 235);
	const COLORREF Gold = RGB(196, 150, 70);
	const COLORREF GoldDark = RGB(160, 115, 45);
	const COLORREF Text = RGB(90, 62, 28);
	const COLORREF ListBg = RGB(255, 252, 247);
	const COLORREF White = RGB(255, 255, 255);
	const COLORREF Cream = RGB(253, 248, 238);
}

inline void StyleGoldButton(CMFCButton& b)
{
	b.m_bDontUseWinXPTheme = TRUE;
	b.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	b.m_bDrawFocus = FALSE;
	b.SetFaceColor(Theme::Gold, TRUE);
	b.SetTextColor(Theme::White);
	b.SetTextHotColor(Theme::White);
}

// soft cream gradient background
inline void PaintGradient(CDC& dc, CRect rc) {
    TRIVERTEX v[2] = {
        { rc.left,  rc.top,    0xFF00, 0xFB00, 0xF200, 0 },
        { rc.right, rc.bottom, 0xF800, 0xEA00, 0xD000, 0 } };
    GRADIENT_RECT g = { 0, 1 };
    dc.GradientFill(v, 2, &g, 1, GRADIENT_FILL_RECT_V);
}

// draws logo.png that sits next to the .exe
inline void DrawLogo(CDC& dc, CRect rc) {
    TCHAR p[MAX_PATH]; GetModuleFileName(NULL, p, MAX_PATH);
    CString s(p); s = s.Left(s.ReverseFind(L'\\') + 1) + L"logo.png";
    CImage img;
    if (SUCCEEDED(img.Load(s))) {
        dc.SetStretchBltMode(HALFTONE);
        img.Draw(dc.GetSafeHdc(), rc);
    }
}


// rounded gold button
class CGoldButton : public CButton 
{
public:
    void DrawItem(LPDRAWITEMSTRUCT d) override 
    {
        CDC* dc = CDC::FromHandle(d->hDC);
        CRect rc = d->rcItem;
        bool down = (d->itemState & ODS_SELECTED) != 0;
        COLORREF c = down ? Theme::GoldDark : Theme::Gold;
        dc->FillSolidRect(rc, Theme::Cream);
        CBrush br(c); CPen pen(PS_SOLID, 1, c);
        CBrush* ob = dc->SelectObject(&br); CPen* op = dc->SelectObject(&pen);
        dc->RoundRect(rc, CPoint(14, 14));
        dc->SelectObject(ob); dc->SelectObject(op);
        CString t; GetWindowText(t);
        dc->SetBkMode(TRANSPARENT);
        dc->SetTextColor(RGB(255, 255, 255));
        CFont* of = dc->SelectObject(GetParent()->GetFont());
        dc->DrawText(t, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        dc->SelectObject(of);
    }
};

namespace Theme
{
    // soft cream gradient background
    inline void PaintGradient(CDC& dc, CRect rc)
    {
        TRIVERTEX v[2] = {
            { rc.left,  rc.top,    0xFF00, 0xFB00, 0xF200, 0 },
            { rc.right, rc.bottom, 0xF800, 0xEA00, 0xD000, 0 } };
        GRADIENT_RECT g = { 0, 1 };
        dc.GradientFill(v, 2, &g, 1, GRADIENT_FILL_RECT_V);
    }

    // draws logo.png that sits next to the .exe
    inline void DrawLogo(CDC& dc, CRect rc)
    {
        TCHAR p[MAX_PATH];
        GetModuleFileName(NULL, p, MAX_PATH);
        CString s(p);
        s = s.Left(s.ReverseFind(L'\\') + 1) + L"logo.png";

        CImage img;
        if (SUCCEEDED(img.Load(s)))
        {
            dc.SetStretchBltMode(HALFTONE);
            img.Draw(dc.GetSafeHdc(), rc);
        }
    }
}

namespace Theme
{
    // draws banner.png (next to the .exe), falls back to plain gold
    inline void DrawBanner(CDC& dc, CRect rc)
    {
        TCHAR p[MAX_PATH];
        GetModuleFileName(NULL, p, MAX_PATH);
        CString s(p);
        s = s.Left(s.ReverseFind(L'\\') + 1) + L"banner.png";

        CImage img;
        if (SUCCEEDED(img.Load(s)))
        {
            dc.SetStretchBltMode(HALFTONE);
            img.Draw(dc.GetSafeHdc(), rc);
        }
        else
            dc.FillSolidRect(rc, RGB(196, 146, 60));
    }
}

// card style button: caption format "Title|Short description"
class CCardButton : public CButton
{
public:
    void DrawItem(LPDRAWITEMSTRUCT d) override
    {
        CDC* dc = CDC::FromHandle(d->hDC);
        CRect rc = d->rcItem;
        bool down = (d->itemState & ODS_SELECTED) != 0;

        CString cap; GetWindowText(cap);
        CString title = cap, sub;
        int p = cap.Find(L'|');
        if (p >= 0) { title = cap.Left(p); sub = cap.Mid(p + 1); }

        dc->FillSolidRect(rc, Theme::Cream);
        CBrush br(down ? RGB(246, 238, 222) : RGB(255, 255, 255));
        CPen pen(PS_SOLID, 1, RGB(232, 215, 180));
        CBrush* ob = dc->SelectObject(&br);
        CPen* op = dc->SelectObject(&pen);
        dc->RoundRect(rc, CPoint(18, 18));
        dc->SelectObject(ob); dc->SelectObject(op);

        int H = rc.Height();
        int dia = H * 46 / 100;
        CRect ic(rc.left + 18, rc.top + (H - dia) / 2,
            rc.left + 18 + dia, rc.top + (H - dia) / 2 + dia);
        CBrush ib(RGB(246, 232, 196));
        CPen ip(PS_SOLID, 1, RGB(246, 232, 196));
        ob = dc->SelectObject(&ib); op = dc->SelectObject(&ip);
        dc->Ellipse(ic);
        dc->SelectObject(ob); dc->SelectObject(op);

        CFont fi, ft, fs, fo;
        fi.CreateFontW(-(dia * 55 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        ft.CreateFontW(-(H * 26 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        fs.CreateFontW(-(H * 15 / 100), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        fo.CreateFontW(-(H * 15 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

        dc->SetBkMode(TRANSPARENT);

        CFont* of = dc->SelectObject(&fi);
        dc->SetTextColor(Theme::GoldDark);
        dc->DrawText(title.Left(1), ic, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        int tx = ic.right + 20;
        dc->SelectObject(&ft);
        dc->SetTextColor(RGB(50, 40, 25));
        dc->TextOut(tx, rc.top + H * 12 / 100, title);

        dc->SelectObject(&fs);
        dc->SetTextColor(RGB(120, 110, 95));
        dc->TextOut(tx, rc.top + H * 44 / 100, sub);

        dc->SelectObject(&fo);
        dc->SetTextColor(Theme::Gold);
        dc->TextOut(tx, rc.top + H * 68 / 100, L"Open  \u2192");

        dc->SelectObject(of);
    }
};