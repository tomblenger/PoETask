#include "stdafx.h"
#include "Engine.h"
#include "Common.h"
#include "Global_Comm.h"

HWND		g_hLogWnd;
UINT		g_uLogMsg;
BYTE		*g_pbPatch;

DWORD GetCurrentAutoVersion()
{
	return 0;
}

BYTE	*GetPatch()
{
	if (!g_pbPatch)
	{
		g_pbPatch = new BYTE[0x100000];
		memset(g_pbPatch, 0, 5);
	}
	return g_pbPatch;
}

void OnAddSession(Session *session)
{

}

void OnRemoveSession(Session *session)
{

}

void	MakeEngineBuffer()
{

}

void	Log(const WCHAR *szFormat, ...)
{
	va_list	arg;
	WCHAR	szTemp[0x1000];

	if (!g_uLogMsg)
		return;
	va_start(arg, szFormat);
	vswprintf_s(szTemp, 0x1000, szFormat, arg);
	va_end(arg);
	SendMessage(g_hLogWnd, g_uLogMsg, (WPARAM)szTemp, NULL);
}

void	StartLog(HWND hWnd, UINT message)
{
	g_hLogWnd = hWnd;
	g_uLogMsg = message;
}

void Login()
{
	DWORD i, dwAccountNum = 0;
	Packet *packet;
	CString strTemp;

	packet = Allocate<Packet>();
	packet->CreatePackage(OP_Login_Req);
	for (i = 0; i < g_dwAppCount; i++)
	{
		if (!g_pAppSetting[i].account.dwUse)
			continue;

		dwAccountNum++;
	}

	packet->AddData(dwAccountNum);
	for (i = 0; i < g_dwAppCount; i++)
	{
		if (!g_pAppSetting[i].account.dwUse)
			continue;

		strTemp = g_pAppSetting[i].account.szAutoId;
		packet->AddData(strTemp);
		strTemp = g_pAppSetting[i].account.szAutoPw;
		packet->AddData(strTemp);
	}
	strTemp = g_szDeviceKey;
	packet->AddData(strTemp);

	SendPacket(GetServerSession(), packet);
}

void Logout()
{
	SendFormatedPacket(GetServerSession(), OP_Logout, "");
}

void KeepAliveThread()
{
	while (1)
	{
		::Sleep(60 * 1000);
		Session *session = GetServerSession();
		if (session != nullptr)
		{
			if (session->login == 0)
			{
				if (g_bLogin)
				{
					Login();
				}
			}
			else
			{
				if (session->lastConnTime != 0 && time(0) > session->lastConnTime + 3 * 60)		// 유지파케트가 3분이상 통신이 안되는 경우
				{
					session->DoDisconnect();
				}
				else
				{
					SendFormatedPacket(session, OP_Keepalive, "");
				}
			}
		}
	}
}

void OnRecvPacket(Packet *packet)
{
	Session	*session = packet->session;
	CString strTemp;
	UINT retCode = 0;
	DWORD i = 0;

#ifdef _PROTECT
	VM_DOLPHIN_BLACK_START
#endif	

	packet->ReadStart();
	switch (packet->bufferinternal[0])
	{
	case OP_Keepalive:
	{
		session->lastConnTime = time(0);
	}
	break;
	case OP_Login_Ack:
	{
		for (i = 0; i < g_dwAppCount; i++)
		{
			if (!g_pAppSetting[i].account.dwUse)
				continue;
			retCode = packet->Read<UINT>();
			g_pAuth[i].dwLoginResult = retCode;
			if (retCode == LR_Success)
			{
				g_pAuth[i].dwID = packet->Read<UINT>();
				packet->ReadWideString(strTemp);
				wcscpy(g_pAuth[i].szExpiredTime, strTemp.GetBuffer(0));
				packet->ReadWideString(strTemp);
				wcscpy(g_pAuth[i].szToken, strTemp.GetBuffer(0));
			}
		}

		session->login = 1;
		session->lastConnTime = time(0);

		SendMessage(g_hLogWnd, MSG_LOGINACK, 0, 0);
	}
	break;
	case OP_ForceLogout:
	{
		GetServerSession()->DoDisconnect();
		SendMessage(g_hLogWnd, MSG_FORCECLOSE, (WPARAM)retCode, 0);
	}
	break;
	case OP_DisconnectAccount:
	{
		DWORD dwDisconnectedID = packet->Read<UINT>();
		SendMessage(g_hLogWnd, MSG_DISCONNECT, (WPARAM)dwDisconnectedID, 0);
	}
	break;
	default:
		break;
	}

#ifdef _PROTECT
		VM_DOLPHIN_BLACK_END
#endif
}
