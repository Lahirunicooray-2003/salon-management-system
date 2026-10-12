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