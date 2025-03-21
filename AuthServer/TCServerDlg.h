
// TCServerDlg.h : header file
//

#pragma once


// CTCServerDlg dialog
class CTCServerDlg : public CDialogEx
{
// Construction
public:
	CTCServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void InitializeLoggers();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void Initialize();
	void InitUI();


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()



private:
	CListCtrl m_listLog;
public:
	afx_msg void OnBnClickedButtonInitAllConnections();
	afx_msg void OnBnClickedButtonClearLog();
	afx_msg void OnBnClickedOk();
};
