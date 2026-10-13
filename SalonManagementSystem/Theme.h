#pragma once
#include <afxbutton.h>
#include <atlimage.h>
#include <map>
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

    // soft cream gradient background
    inline void PaintGradient(CDC& dc, CRect rc)
    {
        TRIVERTEX v[2] = {
            { rc.left,  rc.top,    0xFF00, 0xFB00, 0xF200, 0 },
            { rc.right, rc.bottom, 0xF800, 0xEA00, 0xD000, 0 } };
        GRADIENT_RECT g = { 0, 1 };
        dc.GradientFill(v, 2, &g, 1, GRADIENT_FILL_RECT_V);
    }

    // background photo (next to the .exe), cropped to fill. Falls back to gradient.
    // overlay: 0 = photo only, 255 = solid cream (a little overlay keeps the photo soft)
    // The scaled picture is built ONCE per window size and cached, so painting is instant.
    inline void PaintImageBg(CDC& dc, const CRect& rc, LPCTSTR fileName, int overlay = 0)
    {
        struct Entry
        {
            CImage* src = NULL;
            bool loaded = false;
            CBitmap comp;
            int w = 0, h = 0, ov = -1;
        };
        static std::map<CString, Entry> cache;

        CString key(fileName);
        Entry& e = cache[key];

        if (!e.loaded)
        {
            e.loaded = true;
            CImage* img = new CImage();
            TCHAR p[MAX_PATH];
            GetModuleFileName(NULL, p, MAX_PATH);
            CString path(p);
            path = path.Left(path.ReverseFind(_T('\\')) + 1) + key;
            if (FAILED(img->Load(path))) { delete img; img = NULL; }
            e.src = img;
        }

        int W = rc.Width(), H = rc.Height();
        if (e.src == NULL || e.src->IsNull() || W <= 0 || H <= 0)
        {
            PaintGradient(dc, rc);
            return;
        }

        if (e.comp.GetSafeHandle() == NULL || e.w != W || e.h != H || e.ov != overlay)
        {
            e.comp.DeleteObject();
            e.comp.CreateCompatibleBitmap(&dc, W, H);

            CDC mem;
            mem.CreateCompatibleDC(&dc);
            CBitmap* old = mem.SelectObject(&e.comp);

            int iw = e.src->GetWidth(), ih = e.src->GetHeight();
            int sw = iw, sh = ih;
            if (iw * H > ih * W) sw = ih * W / H;
            else                 sh = iw * H / W;
            int sx = (iw - sw) / 2, sy = (ih - sh) / 2;

            mem.SetStretchBltMode(HALFTONE);
            e.src->Draw(mem.GetSafeHdc(), 0, 0, W, H, sx, sy, sw, sh);

            if (overlay > 0)
            {
                CDC ov;
                ov.CreateCompatibleDC(&dc);
                CBitmap obmp;
                obmp.CreateCompatibleBitmap(&dc, W, H);
                CBitmap* oold = ov.SelectObject(&obmp);
                ov.FillSolidRect(0, 0, W, H, RGB(255, 251, 242));
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)overlay, 0 };
                AlphaBlend(mem.GetSafeHdc(), 0, 0, W, H, ov.GetSafeHdc(), 0, 0, W, H, bf);
                ov.SelectObject(oold);
            }

            mem.SelectObject(old);
            e.w = W; e.h = H; e.ov = overlay;
        }

        CDC mem2;
        mem2.CreateCompatibleDC(&dc);
        CBitmap* old2 = mem2.SelectObject(&e.comp);
        dc.BitBlt(rc.left, rc.top, W, H, &mem2, 0, 0, SRCCOPY);
        mem2.SelectObject(old2);
    }

    // semi-transparent cream rounded card with a gold border
    inline void DrawPanel(CDC& dc, CRect r, int alpha = 215)
    {
        if (r.IsRectEmpty()) return;

        CRgn rgn;
        rgn.CreateRoundRectRgn(r.left, r.top, r.right + 1, r.bottom + 1, 24, 24);
        int saved = dc.SaveDC();
        dc.SelectClipRgn(&rgn);

        CDC mem;
        mem.CreateCompatibleDC(&dc);
        CBitmap bmp;
        bmp.CreateCompatibleBitmap(&dc, r.Width(), r.Height());
        CBitmap* old = mem.SelectObject(&bmp);
        mem.FillSolidRect(0, 0, r.Width(), r.Height(), RGB(255, 251, 242));
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)alpha, 0 };
        AlphaBlend(dc.GetSafeHdc(), r.left, r.top, r.Width(), r.Height(),
            mem.GetSafeHdc(), 0, 0, r.Width(), r.Height(), bf);
        mem.SelectObject(old);
        dc.RestoreDC(saved);

        CPen pen(PS_SOLID, 2, RGB(224, 196, 140));
        CPen* op = dc.SelectObject(&pen);
        CBrush* ob = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
        dc.RoundRect(r, CPoint(24, 24));
        dc.SelectObject(op);
        dc.SelectObject(ob);
    }

    // one card that wraps every visible control of the dialog
    inline void DrawFormPanel(CWnd* wnd, CDC& dc)
    {
        CRect rc;
        wnd->GetClientRect(&rc);
        CRect u(0, 0, 0, 0);
        for (CWnd* c = wnd->GetWindow(GW_CHILD); c != NULL; c = c->GetWindow(GW_HWNDNEXT))
        {
            if (!c->IsWindowVisible()) continue;
            CRect r;
            c->GetWindowRect(&r);
            wnd->ScreenToClient(&r);
            u.UnionRect(u, r);
        }
        u.InflateRect(24, 16);
        u.IntersectRect(u, rc);
        DrawPanel(dc, u);
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

inline void StyleGoldButton(CMFCButton& b)
{
    b.m_bDontUseWinXPTheme = TRUE;
    b.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    b.m_bDrawFocus = FALSE;
    b.SetFaceColor(Theme::Gold, TRUE);
    b.SetTextColor(Theme::White);
    b.SetTextHotColor(Theme::White);
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

// rose-gold card button: caption format "Title|Short description"
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

        // metallic rose-gold gradient: dark edges, light centre
        int e = down ? 0xB000 : 0xC900;      // edge red
        int eg = down ? 0x7C00 : 0x9400;     // edge green
        int eb = down ? 0x6200 : 0x7E00;     // edge blue
        int m = down ? 0xD800 : 0xE800;      // middle red
        int mg = down ? 0xA200 : 0xC000;     // middle green
        int mb = down ? 0x8C00 : 0xAC00;     // middle blue
        int midX = rc.left + rc.Width() / 2;
        TRIVERTEX v[3] = {
            { rc.left,  rc.top,    (COLOR16)e, (COLOR16)eg, (COLOR16)eb, 0 },
            { midX,     rc.bottom, (COLOR16)m, (COLOR16)mg, (COLOR16)mb, 0 },
            { rc.right, rc.bottom, (COLOR16)e, (COLOR16)eg, (COLOR16)eb, 0 } };
        GRADIENT_RECT g[2] = { { 0, 1 }, { 1, 2 } };
        dc->GradientFill(v, 3, g, 2, GRADIENT_FILL_RECT_H);

        // thin light border
        CPen pen(PS_SOLID, 1, RGB(244, 222, 205));
        CBrush* ob = (CBrush*)dc->SelectStockObject(NULL_BRUSH);
        CPen* op = dc->SelectObject(&pen);
        dc->Rectangle(rc);
        dc->SelectObject(op);
        dc->SelectObject(ob);

        int H = rc.Height();
        int dia = H * 46 / 100;
        CRect ic(rc.left + 20, rc.top + (H - dia) / 2,
            rc.left + 20 + dia, rc.top + (H - dia) / 2 + dia);
        CBrush ib(RGB(246, 232, 200));
        CPen ip(PS_SOLID, 1, RGB(246, 232, 200));
        ob = (CBrush*)dc->SelectObject(&ib); op = dc->SelectObject(&ip);
        dc->Ellipse(ic);
        dc->SelectObject(ob); dc->SelectObject(op);

        CFont fi, ft, fs, fo;
        fi.CreateFontW(-(dia * 55 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        ft.CreateFontW(-(H * 24 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        fs.CreateFontW(-(H * 14 / 100), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        fo.CreateFontW(-(H * 14 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

        dc->SetBkMode(TRANSPARENT);

        CFont* of = dc->SelectObject(&fi);
        dc->SetTextColor(Theme::GoldDark);
        dc->DrawText(title.Left(1), ic, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        int tx = ic.right + 20;
        dc->SelectObject(&ft);
        dc->SetTextColor(RGB(60, 35, 20));
        dc->TextOut(tx, rc.top + H * 12 / 100, title);

        dc->SelectObject(&fs);
        dc->SetTextColor(RGB(90, 55, 38));
        dc->TextOut(tx, rc.top + H * 44 / 100, sub);

        dc->SelectObject(&fo);
        dc->SetTextColor(RGB(120, 70, 30));
        dc->TextOut(tx, rc.top + H * 68 / 100, L"Open  →");

        dc->SelectObject(of);
    }
};