
// PoETaskDlg.h : header file
//

#pragma once

#include "ComboListCtrl.h"
#include "Common.h"

struct	RUNINFO
{
	HANDLE	hProcess;
	BYTE	isRunGame;
	DWORD	dwRunTime;
};


// CPoETaskDlg dialog
class CPoETaskDlg : public CDialogEx
{
// Construction
public:
	CPoETaskDlg(CWnd* pParent = nullptr);	// standard constructor

	void Initialize();
	void InitPCI();
	void InitUI();
	void LogUI(DWORD dwNo, const WCHAR *szFormat, ...);
	void UpdateLog(DWORD dwNo);
	void LoadSetting();
	void SaveSetting();
	void LoadEnvSetting();
	void SaveEnvSetting();
	void UpdateMainList();
	void ExitProgram();
	void ExitApp(DWORD dwIndex);
	BYTE * DecryptBlock(BYTE *pbBlock, int nLen, int &nOutLen);
	int GetEncryptedResource(int nID, BYTE *pbByte, BOOL bPlain);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POETASK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonAccountAdd();
	afx_msg void OnBnClickedButtonAccountRemove();
	afx_msg void OnBnClickedCheckLog();
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonLogClear();
	afx_msg void OnBnClickedButtonSelectGamepath();
	afx_msg void OnNMClickListMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT PopulateComboList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ValidateComboList(WPARAM wParam, LPARAM lParam);

public:
	CComboListCtrl m_listMain;
	CButton m_chkShowLog;
	CListCtrl *m_wndActiveLog;
	CListCtrl *m_pListCtrlLog;
	CString m_strAutoId;
	CString m_strAutoPw;
	CString m_strGameId;
	CString m_strGamePw;
	CString m_strGamePath;
	afx_msg void OnBnClickedButtonAttach();
};
