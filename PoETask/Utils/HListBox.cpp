#include "stdafx.h"
#include "HListBox.h"


CHListBox::CHListBox()
{
	width = 0;
}


CHListBox::~CHListBox()
{
}

int CHListBox::AddString(LPCTSTR lpszItem)
{
	int result = CListBox::AddString(lpszItem);
	if (result < 0)
		return result;
	UpdateWidth(lpszItem);
	return result;
}

int CHListBox::InsertString(int i, LPCTSTR s)
{
	int result = CListBox::InsertString(i, s);
	if (result < 0)
		return result;
	UpdateWidth(s);
	return result;
}

void CHListBox::UpdateWidth(LPCTSTR s)
{
	CClientDC dc(this);

	CFont * f = CListBox::GetFont();
	dc.SelectObject(f);

	CSize sz = dc.GetTextExtent(s, (int)_tcslen(s));
	sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
	if (sz.cx > width)
	{ /* extend */
		width = sz.cx;
		CListBox::SetHorizontalExtent(width);
	} /* extend */
}

void CHListBox::ResetContent()
{
	CListBox::ResetContent();
	width = 0;
}

int CHListBox::DeleteString(int n)
{
	int result = CListBox::DeleteString(n);
	if (result < 0)
		return result;
	CClientDC dc(this);

	CFont * f = CListBox::GetFont();
	dc.SelectObject(f);

	width = 0;
	for (int i = 0; i < CListBox::GetCount(); i++)
	{ /* scan strings */
		CString s;
		CListBox::GetText(i, s);
		CSize sz = dc.GetTextExtent(s);
		sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
		if (sz.cx > width)
			width = sz.cx;
	} /* scan strings */
	CListBox::SetHorizontalExtent(width);
	return result;
}