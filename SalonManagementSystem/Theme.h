#pragma once
#include <afxbutton.h>

namespace Theme
{
	const COLORREF Bg = RGB(250, 244, 235);
	const COLORREF Gold = RGB(196, 150, 70);
	const COLORREF GoldDark = RGB(160, 115, 45);
	const COLORREF Text = RGB(90, 62, 28);
	const COLORREF ListBg = RGB(255, 252, 247);
	const COLORREF White = RGB(255, 255, 255);
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