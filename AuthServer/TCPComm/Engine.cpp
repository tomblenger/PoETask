#include "stdafx.h"
#include <time.h>
#include <vector>
//#include <boost/uuid/sha1.hpp>
#include <boost/uuid/name_generator.hpp>
#include "Engine.h"
#include "Common/Common.h"
#include "../../Global/Global_Comm.h"
#include "../Game/GameData.h"
#include "../Global/Global.h"

#define CHECK_CLIENTS_INTERVAL			30 * 60 * 1000
#define NOINDEX							0xFFFFFFFF

HWND		g_hLogWnd;
UINT		g_uLogMsg;
BYTE		*g_pbPatch;
int			g_nServerPortIndex = 0;
Array<std::string>	g_playerList;

CString get_sha1(char *pBuf, int nSize)
{
	boost::uuids::detail::sha1 sha1;
	sha1.process_bytes(pBuf, nSize);
	unsigned hash[5] = { 0 };
	sha1.get_digest(hash);

	// Back to string
	wchar_t buf[41] = { 0 };

	for (int i = 0; i < 5; i++)
	{
		swprintf(buf + (i << 3), L"%08x", hash[i]);
	}

	return CString(buf);
}

void SetUserConnState(CString strid, BOOL bOnline)
{

}

void SetUserConnState(int nId, BOOL bOnline)
{
	SQLResult *sql = 0;

	sql = SQLFirst("UPDATE users SET user_con_state = %d, user_token = '' WHERE NO = %d", bOnline, nId);
	switch (sql->error)
	{
	case -1:
	{
		Log(L"SQL: Can not connect (users) for update in SetUserOnline");
	}
	break;
	case -2:
	{
		Log(L"SQL: Can not query (users) for update in SetUserOnline");
	}
	break;
	}
	SQLClose(sql);
}

void CheckAnotherLogin(int nId, Session *session)
{
	if (sessionmap.Find(nId))		// Aready login with same Id
	{
		if (session == sessionmap[nId])
		{
			Log(L"Relogin ID = %d, Socket = %08X", nId, (SOCKET)sessionmap[nId]->socket.native_handle());
		}
		else
		{
			Log(L"Duplicate login ID = %d, Socket = %08X", nId, (SOCKET)sessionmap[nId]->socket.native_handle());
			SendFormatedPacket(sessionmap[nId], OP_DisconnectAccount, "uu", nId, LR_Duplicate);
			auto pos = std::find(sessionmap[nId]->ids.begin(), sessionmap[nId]->ids.end(), nId);
			if (pos != sessionmap[nId]->ids.end())
			{
				sessionmap[nId]->ids.erase(pos);
			}
			
			sessionmap.Remove(nId);
			SetUserConnState(nId, 0);
		}
		
		//sessionmap[nId]->DoDisconnect();
	}
}

void UpdateUserInfo(int nId, CString strDeviceNum, CString strStartTime, CString strFinishTime, int nConnState, CString strToken, Session *session)
{
	SQLResult *sql = 0;

	sql = SQLFirst("UPDATE users SET user_device_num = '%S', user_start_datetime = '%S', user_end_datetime = '%S', user_token = '%S', user_con_state = %d, user_connect_time = NOW(), user_ip = '%s' WHERE NO = %d", 
		strDeviceNum.GetBuffer(0), strStartTime.GetBuffer(0), strFinishTime.GetBuffer(0), strToken.GetBuffer(0), nConnState, session->socket.remote_endpoint().address().to_string().c_str(), nId);
	switch (sql->error)
	{
	case -1:
	{
		Log(L"SQL: Can not connect (users) for update in UpdateUserInfo");
	}
	break;
	case -2:
	{
		Log(L"SQL: Can not query (users) for update in UpdateUserInfo");
	}
	break;
	}
	SQLClose(sql);
}

CString GenerateToken(CString strDeviceNum, CString strAccount, int nId)
{
	CTime currentTime;
	CString strTime;
	char buffer[200];
	int nBufSize = 0;

	// Device Number
	memcpy(buffer + nBufSize, strDeviceNum.GetBuffer(0), strDeviceNum.GetLength() * sizeof(WCHAR));
	nBufSize += strDeviceNum.GetLength() * sizeof(WCHAR);
	// Account 
	memcpy(buffer + nBufSize, strAccount.GetBuffer(0), strAccount.GetLength() * sizeof(WCHAR));
	nBufSize += strAccount.GetLength() * sizeof(WCHAR);
	// Current local time
	currentTime = CTime::GetCurrentTime();
	strTime = currentTime.Format("%Y-%m-%d %H:%M:%S");
	memcpy(buffer + nBufSize, strTime.GetBuffer(0), strTime.GetLength() * sizeof(WCHAR));
	nBufSize += strTime.GetLength() * sizeof(WCHAR);
	// DB id
	memcpy(buffer + nBufSize, &nId, sizeof(int));
	nBufSize += sizeof(int);

	return get_sha1(buffer, nBufSize);
}

int CheckLogin(Session *session, CString strId, CString strPwd, CString strDeviceNum, int &nID, CString &strExpiredTime, CString &strToken)
{
	COleDateTime	myDtTime;
	CTime			sysTime, expTime = CTime::GetCurrentTime();
	CString			strTemp, strStartTime;
	SYSTEMTIME		st;
	SQLResult		*sql;
	int				nRet = LR_InvalidAccount, nAccountTimeType = 0;

	nID = 0;
	strExpiredTime = L"";
	strToken = L"";

	if (strId.IsEmpty() || strPwd.IsEmpty() || strDeviceNum.IsEmpty())	// Uploading Packet check.
	{
		Log(L"Invalid packet");
		return LR_InvalidAccount;
	}

	sql = SQLFirst("SELECT no, user_password, user_device_num, user_start_datetime, user_end_datetime, user_con_state, user_acc_type FROM users WHERE user_id = '%S'", strId.GetBuffer(0));
	switch (sql->error)
	{
	case -1:
	{
		Log(L"SQL: Can not connect (users) in CheckLogin");
	}
	break;
	case -2:
	{
		Log(L"SQL: Can not query (users) in CheckLogin");
	}
	break;
	default:
	{
		if (SQLNext(sql))
		{
			strTemp.Format(L"%S", sql->row[1]);
			if (strPwd == strTemp) // Psw Confirm
			{
				strTemp.Format(L"%S", sql->row[2]);
				if (strDeviceNum == strTemp || strTemp.IsEmpty() || strTemp == L"(null)") // DeviceId Confirm
				{
					strExpiredTime.Format(L"%S", sql->row[4]);
					if (myDtTime.ParseDateTime(strExpiredTime))
					{
						if (myDtTime.GetAsSystemTime(st))
						{
							expTime = st;
						}
					}
					GetLocalTime(&st);
					sysTime = st;
					if (expTime > sysTime)				// Exp
					{
						// 
						strStartTime.Format(L"%S", sql->row[3]);
						if (strStartTime.IsEmpty() || strStartTime == L"(null)" || strStartTime == L"0000-00-00 00:00:00")		//  
						{
							strStartTime = sysTime.Format("%Y-%m-%d %H:%M:%S");
							nAccountTimeType = atoi(sql->row[6]);
							expTime = sysTime + nAccountTimeType * 24 * 3600;
							strExpiredTime = expTime.Format("%Y-%m-%d %H:%M:%S");
						}

						// 
						nID = atoi(sql->row[0]);
						CheckAnotherLogin(nID, session);	// 
						strToken = GenerateToken(strDeviceNum, strId, nID);
						UpdateUserInfo(nID, strDeviceNum, strStartTime, strExpiredTime, 1, strToken, session);
						nRet = LR_Success;
					}
					else
					{
						Log(L"Expired Account [%s]", strExpiredTime.GetBuffer(0));
						nRet = LR_Expired;
					}
				}
				else
				{
					Log(L"Invalid Device [%s]", strTemp.GetBuffer(0));
					nRet = LR_InvalidAccount;
				}
			}
			else
			{
				Log(L"Invalid Password [%s]", strTemp.GetBuffer(0));
				nRet = LR_InvalidAccount;
			}
		}
		else
		{
			Log(L"SQL: Can not fetch (users) in CheckLogin: %s", strId.GetBuffer());
			nRet = LR_InvalidAccount;
		}
	}
	}
	SQLClose(sql);
	return nRet;
}


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
	session->ids.clear();
	//Log(L"New Session [IP = %S]", session->socket.remote_endpoint().address().to_string().c_str());
}

void OnRemoveSession(Session *session)
{
	Log(L"Disconnect IDs = %d, Socket = %08X", (int)session->ids.size(), (SOCKET)session->socket.native_handle());
	for each (auto id in session->ids)
	{
		sessionmap.Remove(id);
		SetUserConnState(id, 0);
	}
	session->ids.clear();
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

void CheckClients()
{
	SQLResult *sql;
	int nExpiredID = 0;

	while (1)
	{
		Sleep(CHECK_CLIENTS_INTERVAL);
		
		// Check clietns which is expired.
		sql = SQLFirst("SELECT NO FROM users WHERE user_end_datetime < NOW() AND user_con_state = 1");
		switch (sql->error)
		{
		case -1:
		{
			Log(L"SQL: Can not connect (users) in CheckClients");
		}
		break;
		case -2:
		{
			Log(L"SQL: Can not query (users) in CheckClients");
		}
		break;
		default:
		{
			while (SQLNext(sql))
			{
				nExpiredID = atoi(sql->row[0]);
				if (sessionmap.Find(nExpiredID))		// 
				{
					Log(L"Expired ID = %d", nExpiredID);
					SendFormatedPacket(sessionmap[nExpiredID], OP_DisconnectAccount, "uu", nExpiredID, LR_Expired);
					auto pos = std::find(sessionmap[nExpiredID]->ids.begin(), sessionmap[nExpiredID]->ids.end(), nExpiredID);
					if (pos != sessionmap[nExpiredID]->ids.end())
					{
						sessionmap[nExpiredID]->ids.erase(pos);
					}
					sessionmap.Remove(nExpiredID);
					SetUserConnState(nExpiredID, 0);
					//sessionmap[nExpiredID]->DoDisconnect();
				}
			}
		}
		break;
		}

		// Check clients which is not communicated.
		Storage<Session>	&storage = GetStorage<Session>();
		List<Session *>		sessionlist;
		Session				*session;
		time_t				curTime = time(0);

		sessionlist = storage.list;
		while (sessionlist.PopFront(session))
		{
			if (curTime > session->lastcommtick.load(boost::memory_order_acquire) + 120)
			{
				Log(L"Not connected [IDs = %d, Socket = %08X]", (int)session->ids.size(), (SOCKET)session->socket.native_handle());
				session->DoDisconnect();
			}
		}
	}
}

void InitAllConnections()
{
	Storage<Session>	&storage = GetStorage<Session>();
	List<Session *>		sessionlist;
	Session				*session;
	SQLResult			*sql;

	Log(L"Initialize all connections");
	sessionlist = storage.list;
	while (sessionlist.PopFront(session))
	{
		for each (auto id in session->ids)
		{
			sessionmap.Remove(id);
			SetUserConnState(id, 0);
		}

		SendFormatedPacket(session, OP_ForceLogout, "");
		session->ids.clear();
	}

	sql = SQLFirst("UPDATE users SET user_con_state = 0, user_token = ''");
	switch (sql->error)
	{
	case -1:
	{
		Log(L"SQL: Can not connect (users) for update in InitAllConnections");
	}
	break;
	case -2:
	{
		Log(L"SQL: Can not query (users) for update in InitAllConnections");
	}
	break;
	}
	SQLClose(sql);
}

void HandleLogin(Packet *packet)
{
	Session	*session = packet->session;
	CString strId, strPwd, strDeviceNum, strExpiredTime, strToken;
	std::vector<CString> lstAccount, lstPassword;
	std::vector<int> lstLogincode;
	DWORD dwAccountNum, i, j;
	Packet	*ackPacket;
	int nID;

	dwAccountNum = packet->Read<DWORD>();
	if (dwAccountNum >= MAX_ACCOUNT_NUM)
		return;
	for (i = 0; i < dwAccountNum; i++)
	{
		packet->ReadWideString(strId);
		packet->ReadWideString(strPwd);
		lstAccount.push_back(strId);
		lstPassword.push_back(strPwd);
	}
	packet->ReadWideString(strDeviceNum);

	Log(L"OP = %d, Count = %d, Device = %s, Socket = %08X", packet->bufferinternal[0], dwAccountNum, strDeviceNum.GetBuffer(0), (SOCKET)session->socket.native_handle());
	ackPacket = Allocate<Packet>();
	ackPacket->CreatePackage(OP_Login_Ack);
	for (i = 0; i < dwAccountNum; i++)
	{
		Log(L"Account = %s, Pwd = %s", lstAccount[i].GetBuffer(0), lstPassword[i].GetBuffer(0));

		int nRetCode = LR_None;
		for (j = i + 1; j < dwAccountNum; j++)
		{
			if (lstAccount[i] == lstAccount[j])
			{
				nRetCode = LR_Duplicate;
				break;
			}
		}
		if (nRetCode == LR_None)
			nRetCode = CheckLogin(session, lstAccount[i], lstPassword[i], strDeviceNum, nID, strExpiredTime, strToken);
		Log(L"---- RetCode = %d, ID = %d, Expired = %s, Token = %s", nRetCode, nID, strExpiredTime, strToken);

		ackPacket->AddData(nRetCode);
		if (nRetCode == LR_Success)
		{
			session->ids.push_back(nID);
			sessionmap.SetAt(nID, session);
			
			ackPacket->AddData(nID);
			ackPacket->AddData(strExpiredTime);
			ackPacket->AddData(strToken);
		}
		else
		{
		}
	}
	
	SendPacket(session, ackPacket);
}

void OnRecvPacket(Packet *packet)
{
	Session	*session = packet->session;	
	CStringA strTemp;
	int nID = 0, nRetCode = 0, nMultiNum = 0;

	session->lastcommtick.store(time(0), boost::memory_order_release);

	packet->ReadStart();
	if (packet->used == 0)
		return;

	if (packet->bufferinternal[0] == OP_Keepalive)			// 
	{
		SendFormatedPacket(session, OP_Keepalive, "");
	}
	else
	{
		if (session->ids.empty())			// 
		{
			switch (packet->bufferinternal[0])
			{
			case OP_Login_Req:
			{
				HandleLogin(packet);
			}
			break;
			default:
			{
				SendFormatedPacket(session, OP_ForceLogout, "");
			}
			break;
			}
		}
		else							// 
		{
			switch (packet->bufferinternal[0])
			{
			case OP_Logout:
			{
				Log(L"Logout IDs = %d, Socket = %08X", (int)session->ids.size(), (SOCKET)session->socket.native_handle());
				for each (auto id in session->ids)
				{
					sessionmap.Remove(id);
					SetUserConnState(id, 0);
				}
				session->ids.clear();
			}
			break;
			}
		}
	}
}
