
// PoETask.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PoETask.h"
#include "PoETaskDlg.h"
#include "Auth.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPoETaskApp

BEGIN_MESSAGE_MAP(CPoETaskApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPoETaskApp construction

CPoETaskApp::CPoETaskApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPoETaskApp object

CPoETaskApp theApp;


// CPoETaskApp initialization

BOOL CPoETaskApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadAppPath();
#ifdef AUTH
	WCHAR szNewVersion[MAX_PATH];
	CAuth auth;
	if (auth.CheckVersion(AUTO_VERSION, szNewVersion))
	{
		CString strAlarm;
		strAlarm = L"새로운 버젼이 있습니다. 업데이트 하시겠습니까?";
		if (AfxMessageBox(strAlarm, MB_YESNO) == IDYES)
		{
			WCHAR					szApp[MAX_PATH], szParam[512];
			WCHAR					szCmdLine[MAX_PATH];
			PROCESS_INFORMATION		pi;
			STARTUPINFO				si;
			wstring		strServerIP;

			swprintf(szApp, L"%s\\%S.exe", g_szAppPath, APP_MARK);
			strServerIP = AUTH_SERVER_IP;
			swprintf(szCmdLine, L"%s %s %s %S_%s%s %s", strServerIP.c_str(), FTP_USERID, FTP_USERPWD, APP_MARK, szNewVersion, L".exe", szApp);

			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);

			swprintf(szApp, L"%s\\Updater.exe", g_szAppPath);
			swprintf(szParam, L"\"%s\" %s", szApp, szCmdLine);
			CreateProcess(szApp, szParam, 0, 0, 0, 0, 0, g_szAppPath, &si, &pi);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			//DWORD dwEEE = GetLastError();

			ExitProcess(0);
			return FALSE;
		}
	}

#endif // endif

	CPoETaskDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

