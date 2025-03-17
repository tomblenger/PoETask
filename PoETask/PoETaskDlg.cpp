
// PoETaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>
#include <TlHelp32.h>
#include "PoETask.h"
#include "PoETaskDlg.h"
#include "afxdialogex.h"
#include "Common.h"
#include "InjectCore.h"
#include "XorStr.h"
#include "Engine.h"
#include "Global_Comm.h"
#include "DeviceInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum MainListColumn
{
	MLC_No = 0,
	MLC_AutoID,
	MLC_AutoPW,
	MLC_GameID,
	MLC_GamePW,
	MLC_Character,
	MLC_Log,
	MLC_Max,
};

CPoETaskDlg			*g_pDlg;
HANDLE				g_hLoginEvent;
RUNINFO				g_pRunInfo[MAX_ACCOUNT_NUM];

DWORD	CommThread(DWORD dwNo)
{
	while (1)
	{
		WaitForSingleObject(g_hEventNotify[dwNo], INFINITE);
		g_pDlg->SendMessage(MSG_LOG, dwNo, 0);
		SetEvent(g_hEventAnswer[dwNo]);
	}
	return 0;
}

DWORD GetProcessIdByNameAndParent(const WCHAR *szProcessName, DWORD dwParentID)
{
	HANDLE	hProcSnap;
	PROCESSENTRY32W tEntry = { 0 };
	DWORD	dwProcessID = 0;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcSnap == INVALID_HANDLE_VALUE)
		return 0;

	tEntry.dwSize = sizeof(PROCESSENTRY32W);

	// Iterate threads
	for (BOOL success = Process32FirstW(hProcSnap, &tEntry);
		success != FALSE;
		success = Process32NextW(hProcSnap, &tEntry))
	{
		if (!wcscmp(tEntry.szExeFile, szProcessName) && tEntry.th32ParentProcessID == dwParentID)
		{
			return tEntry.th32ProcessID;
		}
	}
	CloseHandle(hProcSnap);

	return 0;
}

DWORD GetProcessIdByName(const WCHAR *szProcessName)
{
	HANDLE	hProcSnap;
	PROCESSENTRY32W tEntry = { 0 };
	DWORD	dwProcessID = 0;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcSnap == INVALID_HANDLE_VALUE)
		return 0;

	tEntry.dwSize = sizeof(PROCESSENTRY32W);

	// Iterate threads
	for (BOOL success = Process32FirstW(hProcSnap, &tEntry);
		success != FALSE;
		success = Process32NextW(hProcSnap, &tEntry))
	{
		if (!wcscmp(tEntry.szExeFile, szProcessName))
		{
			return tEntry.th32ProcessID;
		}
	}
	CloseHandle(hProcSnap);

	return 0;
}

void RunThread(LPVOID param)
{
	CString					strPath;
	CFile					file;
	DWORD					dwNo = 0, dwPid;
	BYTE					*pbBuffer = NULL;
	int						nBufferLen = 0, nID = 0;
	WCHAR					szApp[MAX_PATH], szAppParam[MAX_PATH], szFormat[MAX_PATH];
	STARTUPINFO				si = { 0 };
	PROCESS_INFORMATION		pi = { 0 };
	GAMEPARAM				gameParam;
	CustomArgs_t			args;

#ifdef _PROTECT
	VM_DOLPHIN_BLACK_START
#endif	

	dwNo = (DWORD)(uintptr_t)param;

	wcscpy(szFormat, textwonce(L"%s\\PathOfExile.exe"));
	swprintf(szApp, szFormat, g_szGamePath);
	wcscpy(szAppParam, textwonce(L"PathOfExile.exe"));

	memset(&gameParam, 0, sizeof(GAMEPARAM));
	gameParam.nConnection = dwNo;

	wcscpy(g_pGameSetting[dwNo]->szGameID, g_pAppSetting[dwNo].account.szGameId);
	wcscpy(g_pGameSetting[dwNo]->szGamePW, g_pAppSetting[dwNo].account.szGamePw);
	g_pGameSetting[dwNo]->dwCharNo = g_pAppSetting[dwNo].account.dwCharacterNo;

	args.push_back(&param, sizeof(GAMEPARAM));

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	CreateProcess(szApp, szAppParam, NULL, NULL, NULL, NULL, NULL, g_szGamePath, &si, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	g_pRunInfo[dwNo].hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, 0, pi.dwProcessId);
	g_pRunInfo[dwNo].isRunGame = 1;
	
	while (true)
	{
		Sleep(10);
		dwPid = GetProcessIdByNameAndParent(textwonce(L"PathOfExile_x64.exe"), pi.dwProcessId);
		if (dwPid)
			break;
	}

#ifdef POE_RELEASE
	nID = IDR_DATA_POEATTACH;
	nBufferLen = g_pDlg->GetEncryptedResource(nID, NULL, FALSE);
	pbBuffer = new BYTE[nBufferLen];
	g_pDlg->GetEncryptedResource(nID, pbBuffer, FALSE);
#else
	//strPath.Format(L"%s\\..\\Release\\POEAttach.dll", g_szAppPath);
	strPath.Format(L"%s\\POEAttach.dll", g_szAppPath);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeBinary))
	{
		return;
	}

	nBufferLen = (int)file.GetLength();
	pbBuffer = new BYTE[nBufferLen];
	file.Read(pbBuffer, nBufferLen);
	file.Close();
#endif

	CloseHandle(g_pRunInfo[dwNo].hProcess);
	g_pRunInfo[dwNo].hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, 0, dwPid);
	ManualMap(dwPid, pbBuffer, nBufferLen, NoFlags, L"", "", &args);
	delete[] pbBuffer;

	g_pRunInfo[dwNo].isRunGame = 1;

	HWND hGameWnd = NULL;
	while (true)
	{
		Sleep(5000);

		HWND hCurWnd = NULL;
		do
		{
			hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, textwonce(L"Path of Exile"));
			DWORD dwProcessID = 0;
			GetWindowThreadProcessId(hCurWnd, &dwProcessID);
			if (dwProcessID == dwPid)
			{
				hGameWnd = hCurWnd;
				break;
			}
		} while (hCurWnd != NULL);

		if (hGameWnd)
		{
			break;
		}
	}

	int nWidth = 802;
	int nHeight = 632;
	DWORD dwRow, dwCol;
	dwRow = (dwNo % 4) / 2;
	dwCol = (dwNo % 4) % 2;

	::MoveWindow(hGameWnd, nWidth * dwCol, nHeight *dwRow, nWidth, nHeight, TRUE);
	Sleep(10000);

	SetEvent(g_hLoginEvent);
	WaitForSingleObject(g_pRunInfo[dwNo].hProcess, INFINITE);
	CloseHandle(g_pRunInfo[dwNo].hProcess);

	g_pRunInfo[dwNo].isRunGame = 0;
	g_pRunInfo[dwNo].hProcess = NULL;

#ifdef _PROTECT
	VM_DOLPHIN_BLACK_END
#endif	

}

void RunMainThread(LPVOID param)
{
	DWORD i = 0, dwCount;



	dwCount = (DWORD)(uintptr_t)param;
	if (dwCount >= MAX_ACCOUNT_NUM)
		return;

	memset(g_pRunInfo, 0, sizeof(g_pRunInfo));

	i = 0;
	while (true)
	{
#ifdef _PROTECT
		VM_DOLPHIN_BLACK_START
#endif

		if (i >= dwCount)
			i = 0;

		if (g_pAppSetting[i].account.dwUse && g_pAuth[i].dwLoginResult == LR_Success)
		{
			if (g_pRunInfo[i].isRunGame)
			{
				i++;
				continue;
			}

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RunThread, (LPVOID)(uintptr_t)i, NULL, NULL);
			WaitForSingleObject(g_hLoginEvent, INFINITE);
		}
		Sleep(1000);
		i++;

#ifdef _PROTECT
		VM_DOLPHIN_BLACK_END
#endif	
	}


}

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


// CPoETaskDlg dialog



CPoETaskDlg::CPoETaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POETASK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_pDlg = this;
	m_wndActiveLog = NULL;
	m_pListCtrlLog = NULL;
}

void CPoETaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_MAIN, m_listMain);
	DDX_Control(pDX, IDC_CHECK_LOG, m_chkShowLog);
	DDX_Text(pDX, IDC_EDIT_AUTO_ID, m_strAutoId);
	DDX_Text(pDX, IDC_EDIT_AUTO_PW, m_strAutoPw);
	DDX_Text(pDX, IDC_EDIT_GAME_ID, m_strGameId);
	DDX_Text(pDX, IDC_EDIT_GAME_PW, m_strGamePw);
	DDX_Text(pDX, IDC_EDIT_GAMEPATH, m_strGamePath);
}

BEGIN_MESSAGE_MAP(CPoETaskDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ACCOUNT_ADD, &CPoETaskDlg::OnBnClickedButtonAccountAdd)
	ON_BN_CLICKED(IDC_BUTTON_ACCOUNT_REMOVE, &CPoETaskDlg::OnBnClickedButtonAccountRemove)
	ON_BN_CLICKED(IDC_CHECK_LOG, &CPoETaskDlg::OnBnClickedCheckLog)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CPoETaskDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_START, &CPoETaskDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CPoETaskDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_LOG_CLEAR, &CPoETaskDlg::OnBnClickedButtonLogClear)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_GAMEPATH, &CPoETaskDlg::OnBnClickedButtonSelectGamepath)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MAIN, &CPoETaskDlg::OnNMClickListMain)
	ON_MESSAGE(WM_SET_ITEMS, PopulateComboList)
	ON_MESSAGE(WM_VALIDATE, ValidateComboList)
	ON_BN_CLICKED(IDC_BUTTON_ATTACH, &CPoETaskDlg::OnBnClickedButtonAttach)
END_MESSAGE_MAP()


// CPoETaskDlg message handlers

BOOL CPoETaskDlg::OnInitDialog()
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

void CPoETaskDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPoETaskDlg::OnPaint()
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
HCURSOR CPoETaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPoETaskDlg::Initialize()
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, textwonce(L"PoETask_App"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		AfxMessageBox(L"Already Running.");
		TerminateProcess(GetCurrentProcess(), 0);
		return;
	}

	InitPCI();
	InitUI();
	LoadSetting();
	UpdateMainList();
	UpdateData(FALSE);

	GetDeviceKey(g_szDeviceKey);
	StartLog(this->GetSafeHwnd(), 0);
	StartClient(OnAddSession, OnRemoveSession, OnRecvPacket);
	::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)KeepAliveThread, NULL, NULL, NULL);
}

void CPoETaskDlg::InitPCI()
{
	HANDLE hParam;
	WCHAR szMapParam[MAX_PATH], szTemp[MAX_PATH];
	DWORD i = 0;

#ifdef _PROTECT
	VM_DOLPHIN_BLACK_START
#endif	

	hParam = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_EXECUTE_READWRITE, 0, sizeof(GAMEMAINSETTING) * 2, MAINSETTING_MAPPING_NAME);
	g_pMainSettingApp = (GAMEMAINSETTING *)MapViewOfFile(hParam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	memset(g_pMainSettingApp, 0, sizeof(GAMEMAINSETTING) * 2);
	for (i = 0; i < MAX_ACCOUNT_NUM; i++)
	{
		swprintf(szMapParam, L"%s%d", GAMESETTING_MAPPING_NAME, i);
		hParam = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_EXECUTE_READWRITE, 0, sizeof(GAMESETTING) * 2, szMapParam);
		g_pGameSetting[i] = (GAMESETTING *)MapViewOfFile(hParam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		memset(g_pGameSetting[i], 0, sizeof(GAMESETTING) * 2);
		wcscpy(g_pGameSetting[i]->szAppPath, g_szAppPath);
		swprintf(szMapParam, L"%s%d", GAMEINFO_MAPPING_NAME, i);
		hParam = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_EXECUTE_READWRITE, 0, sizeof(GAMEINFO), szMapParam);
		g_pGameInfo[i] = (GAMEINFO *)MapViewOfFile(hParam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		memset(g_pGameInfo[i], 0, sizeof(GAMEINFO));
		swprintf(szMapParam, L"%s%d", EVENT_NOTIFY_NAME, i);
		g_hEventNotify[i] = CreateEvent(NULL, NULL, FALSE, szMapParam);
		swprintf(szMapParam, L"%s%d", EVENT_ANSWER_NAME, i);
		g_hEventAnswer[i] = CreateEvent(NULL, NULL, TRUE, szMapParam);


		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CommThread, (LPVOID)(size_t)i, 0, 0);
	}

#ifdef _PROTECT
	VM_DOLPHIN_BLACK_END
#endif	

	swprintf(szTemp, L"%s\\Log", g_szAppPath);
	CreateDirectory(szTemp, NULL);
	swprintf(szTemp, L"%s\\Setting", g_szAppPath);
	CreateDirectory(szTemp, NULL);
	for (i = 0; i < MAX_ACCOUNT_NUM; i++)
	{
		swprintf(szTemp, L"%s\\Log\\%d", g_szAppPath, i);
		CreateDirectory(szTemp, NULL);
	}
}

void CPoETaskDlg::InitUI()
{
	RECT rt;
	DWORD i;
	WCHAR szTemp[MAX_PATH];

	swprintf(szTemp, L"%S # V.%s", APP_MARK, AUTO_VERSION);
	SetWindowText(szTemp);

	m_listMain.SetExtendedStyle(m_listMain.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	m_listMain.InsertColumn(MLC_No, L"No", LVCFMT_CENTER, 50);
	m_listMain.InsertColumn(MLC_AutoID, L"AutoAccount", LVCFMT_CENTER, 100);
	m_listMain.InsertColumn(MLC_AutoPW, L"AutoPassword", LVCFMT_CENTER, 100);
	m_listMain.InsertColumn(MLC_GameID, L"GameAccount", LVCFMT_CENTER, 230);
	m_listMain.InsertColumn(MLC_GamePW, L"GamePassword", LVCFMT_CENTER, 150);
	m_listMain.InsertColumn(MLC_Character, L"Character", LVCFMT_CENTER, 50);
	m_listMain.InsertColumn(MLC_Log, L"Log", LVCFMT_LEFT, 300);

	m_listMain.SetReadOnlyColumns(MLC_No);
	m_listMain.SetComboColumns(MLC_Character);
	m_listMain.SetReadOnlyColumns(MLC_Log);

	m_listMain.GetWindowRect(&rt);
	m_pListCtrlLog = new CListCtrl[MAX_ACCOUNT_NUM];
	for (i = 0; i < MAX_ACCOUNT_NUM; i++)
	{
		m_pListCtrlLog[i].CreateEx(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, LVS_REPORT | WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(5, rt.bottom, rt.right - rt.left, rt.bottom + 290), this, 0x1800 + i);
		m_pListCtrlLog[i].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		m_pListCtrlLog[i].InsertColumn(0, L"Log", LVCFMT_CENTER, rt.right - rt.left - 30);
		m_pListCtrlLog[i].ShowWindow(SW_HIDE);
	}
	m_wndActiveLog = NULL;

#ifdef POE_TEST
	GetDlgItem(IDC_BUTTON_ATTACH)->ShowWindow(1);
#endif
}

void CPoETaskDlg::LogUI(DWORD dwNo, WCHAR *szFormat, ...)
{
	va_list			arg;
	SYSTEMTIME		time;
	WCHAR			szBuffer[5000];

	if (dwNo != -1)
	{
		GetLocalTime(&time);

		//swprintf(szBuffer, L"%02d:%02d:%02d - ", time.wHour, time.wMinute, time.wSecond);
		va_start(arg, szFormat);
		//vswprintf(szBuffer+11, szFormat, arg);
		vswprintf(szBuffer, szFormat, arg);
		va_end(arg);

		if (m_pListCtrlLog[dwNo])
		{
			int nCount = m_pListCtrlLog[dwNo].GetItemCount();
			if (m_pListCtrlLog[dwNo].GetItemCount() > 2000)
			{
				m_pListCtrlLog[dwNo].DeleteItem(0);
				nCount = m_pListCtrlLog[dwNo].GetItemCount();
			}
			m_pListCtrlLog[dwNo].InsertItem(nCount, szBuffer);
			m_pListCtrlLog[dwNo].EnsureVisible(nCount, FALSE);
		}

		m_listMain.SetItemText(dwNo, MLC_Log, szBuffer);
	}
}

void CPoETaskDlg::UpdateLog(DWORD dwNo)
{
	if (dwNo >= MAX_ACCOUNT_NUM)
		return;

	g_pGameInfo[dwNo]->szLog[0x1000 - 1] = 0;
	if (!g_pGameInfo[dwNo]->szLog[0])
		return;
	
	LogUI(dwNo, L"%s", g_pGameInfo[dwNo]->szLog);
}

void CPoETaskDlg::LoadSetting()
{
	FILE	*fp;
	WCHAR szPath[MAX_PATH];
	DWORD 	i;

	LoadEnvSetting();

	for (i = 0; i < MAX_ACCOUNT_NUM; i++)
	{
		memset(&g_pAppSetting[i].account, 0, sizeof(AccountInfo));
	}

	swprintf(szPath, L"%s\\Account.dat", g_szAppPath);
	fp = _wfopen(szPath, L"rb");
	if (!fp)
		return;

	fread(&g_dwAppCount, sizeof(DWORD), 1, fp);
	for (i = 0; i < g_dwAppCount; i++)
	{
		fread(&g_pAppSetting[i].account, sizeof(AccountInfo), 1, fp);
	}
	fclose(fp);
}

void CPoETaskDlg::SaveSetting()
{
	FILE	*fp;
	WCHAR	szPath[MAX_PATH];
	DWORD 	i;

	SaveEnvSetting();

	for (i = 0; i < g_dwAppCount; i++)
	{
		g_pAppSetting[i].account.dwUse = (DWORD)m_listMain.GetCheck(i);
	}

	swprintf(szPath, L"%s\\Account.dat", g_szAppPath);
	fp = _wfopen(szPath, L"wb");
	if (!fp)
		return;

	fwrite(&g_dwAppCount, sizeof(DWORD), 1, fp);
	for (i = 0; i < g_dwAppCount; i++)
	{
		fwrite(&g_pAppSetting[i].account, sizeof(AccountInfo), 1, fp);
	}
	fclose(fp);
}

void CPoETaskDlg::LoadEnvSetting()
{
	FILE	*fp;
	WCHAR	szPath[MAX_PATH];

	swprintf(szPath, L"%s\\Env.dat", g_szAppPath);
	fp = _wfopen(szPath, L"rb");
	if (!fp)
		return;

	fread(g_szGamePath, 1, sizeof(g_szGamePath), fp);
	fclose(fp);

	m_strGamePath = g_szGamePath;
}

void CPoETaskDlg::SaveEnvSetting()
{
	FILE	*fp;
	WCHAR	szPath[MAX_PATH];

	swprintf(szPath, L"%s\\Env.dat", g_szAppPath);
	fp = _wfopen(szPath, L"wb");
	if (!fp)
		return;

	fwrite(g_szGamePath, 1, sizeof(g_szGamePath), fp);
	fclose(fp);
}

void CPoETaskDlg::UpdateMainList()
{
	DWORD i = 0;
	WCHAR szPath[MAX_PATH];

	m_listMain.DeleteAllItems();
	for (i = 0; i < g_dwAppCount; i++)
	{
		swprintf(szPath, L"%d", i + 1);
		m_listMain.InsertItem(i, szPath);
		m_listMain.SetCheck(i, g_pAppSetting[i].account.dwUse);

		m_listMain.SetItemText(i, MLC_AutoID, g_pAppSetting[i].account.szAutoId);
		m_listMain.SetItemText(i, MLC_AutoPW, g_pAppSetting[i].account.szAutoPw);
		m_listMain.SetItemText(i, MLC_GameID, g_pAppSetting[i].account.szGameId);
		m_listMain.SetItemText(i, MLC_GamePW, g_pAppSetting[i].account.szGamePw);
		swprintf(szPath, L"%d", g_pAppSetting[i].account.dwCharacterNo + 1);
		m_listMain.SetItemText(i, MLC_Character, szPath);
	}
}

void CPoETaskDlg::ExitProgram()
{
	DWORD i = 0;

	Logout();

	for (i = 0; i < g_dwAppCount; i++)
	{
		ExitApp(i);
		g_pRunInfo[i].isRunGame = 1;
	}

	SaveSetting();

	GetServerSession()->DoDisconnect();
	TerminateProcess(GetCurrentProcess(), 0);
}

void CPoETaskDlg::ExitApp(DWORD dwIndex)
{
	if (g_pRunInfo[dwIndex].isRunGame)
	{
		TerminateProcess(g_pRunInfo[dwIndex].hProcess, 0);
		g_pRunInfo[dwIndex].hProcess = NULL;
		g_pRunInfo[dwIndex].isRunGame = 0;
	}
}

BYTE *	CPoETaskDlg::DecryptBlock(BYTE	*pbBlock, int nLen, int &nOutLen)
{
	BYTE				*pbBuffer;
	int					i;

	pbBuffer = new BYTE[nLen];
	pbBuffer[0] = pbBlock[40];
	for (i = 1; i < nLen; i++)
	{
		pbBuffer[i] = pbBlock[40 + i] ^ pbBlock[40 + i - 1];
	}
	nOutLen = *(int *)pbBuffer;
	memmove(pbBuffer, pbBuffer + 4, nOutLen);
	return pbBuffer;
}

int CPoETaskDlg::GetEncryptedResource(int nID, BYTE *pbByte, BOOL bPlain)
{
	int					nLen;
	BYTE				*pbCipher, *pbPlain;
	HRSRC				hRSrc;
	HGLOBAL				hGlobal;
	HMODULE				hMod = AfxGetInstanceHandle();

	hRSrc = FindResource(hMod, MAKEINTRESOURCE(nID), L"DATA");
	hGlobal = LoadResource(hMod, hRSrc);
	nLen = SizeofResource(hMod, hRSrc);
	if (pbByte == NULL)
		return nLen;

	pbCipher = new BYTE[nLen];
	memcpy(pbCipher, LockResource(hGlobal), nLen);
	if (bPlain)
		pbPlain = pbCipher;
	else
		pbPlain = DecryptBlock(pbCipher, nLen, nLen);

	memcpy(pbByte, pbPlain, nLen);

	if (!bPlain)
		delete[] pbPlain;
	delete[] pbCipher;
	return nLen;
}

BOOL CPoETaskDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDOK || wParam == IDCANCEL)
		return TRUE;
	return CDialogEx::OnCommand(wParam, lParam);
}

LRESULT CPoETaskDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD i;
	
	switch (message)
	{
	case MSG_LOG:
	{
		UpdateLog((DWORD)wParam);
	}
	break;
	case MSG_LOGINACK:
	{
		for (i = 0; i < g_dwAppCount; i++)
		{
			if (!g_pAppSetting[i].account.dwUse)
				continue;

			switch (g_pAuth[i].dwLoginResult)
			{
			case LR_Success:
			{
				LogUI(i, L"LogIn Success");
			}
			break;
			case LR_InvalidAccount:
			{
				LogUI(i, L"Incorrect Account.");
			}
			break;
			case LR_Expired:
			{
				LogUI(i, L"Account Not Expire.");
			}
			break;
			case LR_InvalidDevice:
			{
				LogUI(i, L"Incorrect Device.");
			}
			break;
			case LR_Duplicate:
			{
				LogUI(i, L"Duplicate Connect.");
			}
			break;
			case LR_InvalidToken:
			default:
			{
				LogUI(i, L"LogIn Fail");
			}
			break;
			}
		}

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

	}
	break;
	case MSG_FORCECLOSE:
	{
		ExitProgram();
	}
	break;
	case MSG_DISCONNECT:
	{
		DWORD dwDisconnectedID = (DWORD)wParam;
		for (i = 0; i < g_dwAppCount; i++)
		{
			if (!g_pAppSetting[i].account.dwUse)
				continue;

			if (g_pAuth[i].dwID == dwDisconnectedID)
			{
				switch (g_pAuth[i].dwLoginResult)
				{
				case LR_Success:
				{
					LogUI(i, L"LogIn Success");
				}
				break;
				case LR_InvalidAccount:
				{
					LogUI(i, L"Incorrect Account.");
				}
				break;
				case LR_Expired:
				{
					LogUI(i, L"Account Not Expire.");
				}
				break;
				case LR_InvalidDevice:
				{
					LogUI(i, L"Incorrect Device.");
				}
				break;
				case LR_Duplicate:
				{
					LogUI(i, L"Duplicate Connect");
				}
				break;
				case LR_InvalidToken:
				default:
				{
					LogUI(i, L"LogIn Fail");
				}
				break;
				}

				if (g_pAuth[i].dwLoginResult != LR_Success)
				{
					ExitApp(i);
				}

				break;
			}
		}
	}
	break;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}

void CPoETaskDlg::OnBnClickedButtonAccountAdd()
{
	if (g_dwAppCount >= MAX_ACCOUNT_NUM)
	{
		AfxMessageBox(L"Max limit 30.");
		return;
	}

	UpdateData(TRUE);
	if (m_strAutoId.IsEmpty() || m_strAutoPw.IsEmpty() || m_strGameId.IsEmpty() || m_strGamePw.IsEmpty())
		return;

	for (DWORD i = 0; i < g_dwAppCount; i++)
	{
		if (m_strAutoId == g_pAppSetting[g_dwAppCount].account.szAutoId)
		{
			AfxMessageBox(L"Duplicate Auto Account.");
			return;
		}
	}
	
	g_pAppSetting[g_dwAppCount].account.dwUse = 1;
	wcscpy(g_pAppSetting[g_dwAppCount].account.szAutoId, m_strAutoId.GetBuffer());
	wcscpy(g_pAppSetting[g_dwAppCount].account.szAutoPw, m_strAutoPw.GetBuffer());
	wcscpy(g_pAppSetting[g_dwAppCount].account.szGameId, m_strGameId.GetBuffer());
	wcscpy(g_pAppSetting[g_dwAppCount].account.szGamePw, m_strGamePw.GetBuffer());
	g_dwAppCount++;

	UpdateMainList();
}

void CPoETaskDlg::OnBnClickedButtonAccountRemove()
{
	int			nPos, i;
	POSITION	pos;

	pos = m_listMain.GetFirstSelectedItemPosition();
	if (!pos)
	{
		return;
	}
	nPos = m_listMain.GetNextSelectedItem(pos);
	for (i = nPos; i < (int)(g_dwAppCount - 1); i++)
		g_pAppSetting[i] = g_pAppSetting[i + 1];
	g_dwAppCount--;

	UpdateMainList();

	// Select current column
	m_listMain.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	m_listMain.SetItemState(nPos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_listMain.EnsureVisible(nPos, false);
	m_listMain.SetFocus();
}

void CPoETaskDlg::OnBnClickedCheckLog()
{
	BOOL isShowLog = m_chkShowLog.GetCheck();
	RECT rect;
	g_pDlg->GetWindowRect(&rect);
	if (isShowLog)
	{
		g_pDlg->SetWindowPos(NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top + 300, SWP_NOMOVE | SWP_NOZORDER);
		if (m_wndActiveLog == NULL)
			m_wndActiveLog = &m_pListCtrlLog[0];
		if (m_wndActiveLog)
		{
			m_wndActiveLog->ShowWindow(SW_SHOW);
			CListCtrl *tempCtrl = (CListCtrl *)m_wndActiveLog;
			int nCount = tempCtrl->GetItemCount();
			if (nCount > 0)
				tempCtrl->EnsureVisible(nCount - 1, FALSE);
		}
	}
	else
	{
		if (m_wndActiveLog)
			m_wndActiveLog->ShowWindow(SW_HIDE);
		g_pDlg->SetWindowPos(NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top - 300, SWP_NOMOVE | SWP_NOZORDER);
	}
}


void CPoETaskDlg::OnBnClickedButtonLogin()
{
	SaveSetting();

	GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(FALSE);

	Login();
}


void CPoETaskDlg::OnBnClickedButtonStart()
{
	DWORD dwCount;

	SaveSetting();
	if (!g_szGamePath[0])
	{
		AfxMessageBox(L"Please Select GamePath.");
		return;
	}

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	dwCount = g_dwAppCount;
	g_hLoginEvent = CreateEvent(0, 0, 0, textwonce(L"PoETask(SignalEvent)"));
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RunMainThread, (LPVOID)(uintptr_t)dwCount, NULL, NULL);
}


void CPoETaskDlg::OnBnClickedButtonExit()
{
	ExitProgram();
}


void CPoETaskDlg::OnBnClickedButtonLogClear()
{
	if (!m_wndActiveLog)
		return;

	m_wndActiveLog->DeleteAllItems();
}


void CPoETaskDlg::OnBnClickedButtonSelectGamepath()
{
	ITEMIDLIST*  pildBrowse;
	TCHAR   pszPathname[MAX_PATH];

	BROWSEINFO  bInfo;
	UpdateData(TRUE);
	memset(&bInfo, 0, sizeof(bInfo));
	bInfo.hwndOwner = GetSafeHwnd();
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = pszPathname;
	bInfo.lpszTitle = _T("Please Game Install Path.");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	bInfo.lpfn = NULL;
	bInfo.lParam = (LPARAM)g_szGamePath;
	bInfo.lParam = (LPARAM)NULL;
	pildBrowse = (ITEMIDLIST*)::SHBrowseForFolder(&bInfo);
	if (pildBrowse)
	{
		SHGetPathFromIDList(pildBrowse, pszPathname);
		CString strInfoFilePath;
		strInfoFilePath.Format(L"%s\\PathOfExile.exe", pszPathname);
		if (!PathFileExists(strInfoFilePath))
		{
			return;
		}
		wcscpy(g_szGamePath, pszPathname);
		m_strGamePath = g_szGamePath;
		UpdateData(FALSE);
	}
}

void CPoETaskDlg::OnNMClickListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POSITION	pos;
	int			nItem;

	if (!m_chkShowLog.GetCheck())
		return;

	pos = m_listMain.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	nItem = m_listMain.GetNextSelectedItem(pos);
	if (nItem < (int)g_dwAppCount)
	{
		if (m_wndActiveLog && m_pListCtrlLog[nItem])
		{
			m_wndActiveLog->ShowWindow(SW_HIDE);
			m_wndActiveLog = &m_pListCtrlLog[nItem];
			m_wndActiveLog->ShowWindow(SW_SHOW);
			CListCtrl *tempCtrl = (CListCtrl *)m_wndActiveLog;
			int nCount = tempCtrl->GetItemCount();
			if (nCount > 0)
				tempCtrl->EnsureVisible(nCount - 1, FALSE);
		}
	}
	*pResult = 0;
}

LRESULT CPoETaskDlg::PopulateComboList(WPARAM wParam, LPARAM lParam)
{
	int			i;
	int			nItemCase = (int)(wParam >> 16);
	CComboBox	*pInPlaceCombo = static_cast<CComboBox*> (GetFocus());
	CRect		obWindowRect;
	CString		strTemp;

	pInPlaceCombo->GetWindowRect(&obWindowRect);

	CPoint		obInPlaceComboTopLeft(obWindowRect.TopLeft());
	int iColIndex = wParam & 0xFFFF;

	CStringList* pComboList = reinterpret_cast<CStringList*>(lParam);
	pComboList->RemoveAll();
	if (obWindowRect.PtInRect(obInPlaceComboTopLeft))
	{
		switch (nItemCase)
		{
		case 1:
		{
			switch (iColIndex)
			{
			case MLC_Character:
			{
				for (i = 0; i < 20; i++)
				{
					strTemp.Format(L"%d", i + 1);
					pComboList->AddTail(strTemp);
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
	return 0;
}

LRESULT CPoETaskDlg::ValidateComboList(WPARAM wParam, LPARAM lParam)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO *)lParam;
	int nItem = pDispInfo->item.iItem;
	int nSubItem = pDispInfo->item.iSubItem;
	int nLen = 0;

	if (nItem >= (int)g_dwAppCount)
		return 0;

	switch (nSubItem)
	{
	case MLC_AutoID:
		wcscpy(g_pAppSetting[nItem].account.szAutoId, pDispInfo->item.pszText);
		break;
	case MLC_AutoPW:
		wcscpy(g_pAppSetting[nItem].account.szAutoPw, pDispInfo->item.pszText);
		break;
	case MLC_GameID:
		wcscpy(g_pAppSetting[nItem].account.szGameId, pDispInfo->item.pszText);
		break;
	case MLC_GamePW:
		wcscpy(g_pAppSetting[nItem].account.szGamePw, pDispInfo->item.pszText);
		break;
	case MLC_Character:
		g_pAppSetting[nItem].account.dwCharacterNo = _wtoi(pDispInfo->item.pszText) - 1;
		break;
	}
	
	return 0;
}

void AttachThread()
{
#ifdef POE_TEST
	CString	strPath;
	CFile file;
	DWORD dwPid;
	BYTE *pbBuffer = NULL;
	int	 nBufferLen = 0, nID = 0;

	dwPid = GetProcessIdByName(L"PathOfExile_x64.exe");
	while (!dwPid)
	{
		Sleep(500);
		dwPid = GetProcessIdByName(L"PathOfExile_x64.exe");
	}
	// 	if (!dwPid)
	// 		return;

	strPath.Format(L"%s\\POEAttach.dll", g_szAppPath);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeBinary))
	{
		return;
	}

	nBufferLen = (int)file.GetLength();
	pbBuffer = new BYTE[nBufferLen];
	file.Read(pbBuffer, nBufferLen);
	file.Close();
	ManualMap(dwPid, pbBuffer, nBufferLen, NoFlags, L"", "");
	//NativeInject(dwPid, strPath.GetBuffer(0));
	//LoadLibrary(strPath.GetBuffer(0));
#endif
}

void CPoETaskDlg::OnBnClickedButtonAttach()
{
	DWORD dwNo = 0;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AttachThread, (LPVOID)(size_t)dwNo, NULL, NULL);
}
