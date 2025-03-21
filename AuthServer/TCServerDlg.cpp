
// TCServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include <fstream>
#include "TCServer.h"
#include "TCServerDlg.h"
#include "afxdialogex.h"
#include "TCPComm/Engine.h"
#include "Common/Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


WCHAR	g_szPath[MAX_PATH];


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


// CTCServerDlg dialog



CTCServerDlg::CTCServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TCSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_listLog);
}

BEGIN_MESSAGE_MAP(CTCServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_INIT_ALL_CONNECTIONS, &CTCServerDlg::OnBnClickedButtonInitAllConnections)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CTCServerDlg::OnBnClickedButtonClearLog)
	ON_BN_CLICKED(IDOK, &CTCServerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTCServerDlg message handlers

BOOL CTCServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	Initialize();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTCServerDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

uintptr_t MyBase;

void	DumpStack(DWORD dwRSP, DWORD dwDepth)
{
	DWORD	i;
	DWORD dwTemp = 0;

	for (i = 0; i < dwDepth; i++)
	{
		dwTemp = *(DWORD *)(dwRSP + i * 8);
		if (dwTemp > MyBase)
		{
			dwTemp = dwTemp - MyBase;
		}
		else
			dwTemp = 0;
		if (dwTemp == 0)
			LogFile(L"Stack %08X: %08X", i * 8, *(DWORD *)(dwRSP + i * 8));
		else
			LogFile(L"Stack %08X: %08X %08X", i * 8, *(DWORD *)(dwRSP + i * 8), dwTemp);
	}
}

LONG	WINAPI VectoredHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	PCONTEXT		pContext = (PCONTEXT)ExceptionInfo->ContextRecord;
	//Log(L"EX: %08X %08X %08X %08X", ExceptionInfo->ExceptionRecord->ExceptionCode, pContext->Eip, pContext->Eip - MyBase, MyBase);
	if (ExceptionInfo->ExceptionRecord->ExceptionCode >= 0x80000000 && ExceptionInfo->ExceptionRecord->ExceptionCode <= 0xCFFFFFFF)
	{
		//DumpStack(pContext->Esp, 0x200);
		TerminateProcess(GetCurrentProcess(), 1);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void SetExceptionHandler()
{
	MyBase = (DWORD)GetModuleHandle(NULL);
	AddVectoredExceptionHandler(1000, VectoredHandler);
}

void CTCServerDlg::Initialize()
{
	DWORD dwThreadID = 0;
	
	SetExceptionHandler();
	InitializeLoggers();
	InitUI();
	StartLog(m_hWnd, MSG_LOG);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckClients, 0, 0, &dwThreadID);
	StartServer(SERVERPORT, OnAddSession, OnRemoveSession, OnRecvPacket, 0);
}

void CTCServerDlg::InitUI()
{
	m_listLog.SetExtendedStyle(m_listLog.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_listLog.InsertColumn(1, L"Time", LVCFMT_LEFT, 120);
	m_listLog.InsertColumn(2, L"Log", LVCFMT_LEFT, 900);
}

LRESULT CTCServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	WCHAR		szTime[MAX_PATH];

	switch (message)
	{
	case MSG_LOG:
	{
		SYSTEMTIME	time;
		int			nItem;

		GetLocalTime(&time);

		swprintf(szTime, L"%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		nItem = m_listLog.GetItemCount();
		if (nItem >= 10000)
		{
			m_listLog.DeleteItem(0);
			nItem--;
		}
		m_listLog.InsertItem(nItem, szTime);
		m_listLog.SetItemText(nItem, 1, (WCHAR *)wParam);
		m_listLog.EnsureVisible(nItem, FALSE);
		LogFile((WCHAR *)wParam);
	}
	break;
	case WM_CLOSE:
	{
		TerminateProcess(GetCurrentProcess(), 0);
	}
	break;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}

void CTCServerDlg::InitializeLoggers()
{
	WCHAR szPath[MAX_PATH], szTemp[MAX_PATH];
	time_t szClock;
	tm newTime;
	size_t i = 0;

	GetModuleFileName(NULL, g_szPath, MAX_PATH);
	i = wcslen(g_szPath);
	while (g_szPath[i] != L'\\')
		i--;
	g_szPath[i] = 0;

	time(&szClock);
	localtime_s(&newTime, &szClock);

	std::locale::global(std::locale(""));
	swprintf(szPath, L"%s\\Log", g_szPath);
	CreateDirectory(szPath, NULL);

	swprintf(szTemp, L"%s", szPath);
	CreateDirectory(szTemp, NULL);

	swprintf(g_pszLogPath, L"%s\\%04d_%02d_%02d_%02d_%02d_%02d.txt", szPath, newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
	LogFile(L"Init");
}

void CTCServerDlg::OnBnClickedButtonInitAllConnections()
{
	if (AfxMessageBox(L"Are you sure All Format?", MB_YESNO) == IDNO)
		return;
	InitAllConnections();
}


void CTCServerDlg::OnBnClickedButtonClearLog()
{
	m_listLog.DeleteAllItems();
}

void CTCServerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
