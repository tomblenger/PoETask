#pragma once
#include "ASIO.h"

extern HWND g_hLogWnd;
void OnAddSession(Session *session);
void OnRemoveSession(Session *session);
void OnRecvPacket(Packet *packet);
void Log(const WCHAR *szFormat, ...);
void StartLog(HWND hWnd, UINT message);
void Login();
void Logout();
void KeepAliveThread();
BYTE *GetPatch();