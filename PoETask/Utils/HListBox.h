#pragma once
#include "afxwin.h"
class CHListBox :
	public CListBox
{
public:
	CHListBox();
	~CHListBox();
	int AddString(LPCTSTR lpszItem);
	int DeleteString(int n);
	int InsertString(int nIndex, LPCTSTR lpszItem);
	void ResetContent();
	void UpdateWidth(LPCTSTR s);
	int width;
};

