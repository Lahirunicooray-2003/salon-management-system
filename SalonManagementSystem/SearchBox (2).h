#pragma once
// SearchBox.h - a small search box that filters a CListCtrl while you type.
// Header-only: just add this file to the project and #include it.
#include <afxwin.h>
#include <afxcmn.h>
#include <vector>

class CSearchBox : public CEdit
{
public:
	// Creates the box just above the list and shrinks the list to make room.
	void Init(CWnd* parent, CListCtrl* list, UINT id, LPCTSTR hint = L"Search...")
	{
		m_list = list;

		CRect rc;
		list->GetWindowRect(&rc);
		parent->ScreenToClient(&rc);

		const int h = 30;     // box height
		const int gap = 10;   // space between box and list
		const int w = (rc.Width() < 320) ? rc.Width() : 320;   // neat, not full width

		// small, clean font (the dialog font is too big for this box)
		if (m_font.GetSafeHandle() == NULL)
			m_font.CreatePointFont(95, L"Segoe UI");

		CreateEx(0, L"EDIT", L"",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,
			CRect(rc.left, rc.top, rc.left + w, rc.top + h), parent, id);
		SetFont(&m_font);
		SetMargins(8, 8);
		SendMessage(EM_SETCUEBANNER, TRUE, (LPARAM)hint);

		// push the list down so it does not overlap
		list->SetWindowPos(NULL, rc.left, rc.top + h + gap, rc.Width(), rc.Height() - h - gap,
			SWP_NOZORDER);
	}

	// Call at the END of every Load...() function (after the list is filled).
	void Capture()
	{
		if (!m_list) return;
		m_rows.clear();
		int cols = m_list->GetHeaderCtrl()->GetItemCount();
		for (int i = 0; i < m_list->GetItemCount(); i++)
		{
			std::vector<CString> r;
			for (int c = 0; c < cols; c++) r.push_back(m_list->GetItemText(i, c));
			m_rows.push_back(r);
		}
		Apply();
	}

	void Apply()
	{
		if (!m_list || !GetSafeHwnd()) return;

		CString key;
		GetWindowText(key);
		key.Trim();
		key.MakeLower();
		if (key.IsEmpty() && (int)m_rows.size() == m_list->GetItemCount()) return;

		m_list->SetRedraw(FALSE);
		m_list->DeleteAllItems();
		int n = 0;
		for (size_t i = 0; i < m_rows.size(); i++)
		{
			bool match = key.IsEmpty();
			for (size_t c = 0; c < m_rows[i].size() && !match; c++)
			{
				CString cell = m_rows[i][c];
				cell.MakeLower();
				if (cell.Find(key) >= 0) match = true;
			}
			if (!match) continue;
			m_list->InsertItem(n, m_rows[i][0]);
			for (size_t c = 1; c < m_rows[i].size(); c++)
				m_list->SetItemText(n, (int)c, m_rows[i][c]);
			n++;
		}
		m_list->SetRedraw(TRUE);
		m_list->Invalidate();
	}

protected:
	// MFC reflects EN_CHANGE back to the control itself
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{
		if (message == WM_COMMAND && HIWORD(wParam) == EN_CHANGE)
		{
			Apply();
			return TRUE;
		}
		return CEdit::OnChildNotify(message, wParam, lParam, pResult);
	}

private:
	CFont m_font;
	CListCtrl* m_list = nullptr;
	std::vector<std::vector<CString>> m_rows;
};
