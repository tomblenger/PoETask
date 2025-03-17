#pragma once
#include "ASIO.h"

void	OnAddSession(Session *session);
void	OnRemoveSession(Session *session);
void	OnRecvPacket(Packet *packet);
void	Log(const WCHAR *szFormat, ...);
void	StartLog(HWND hWnd, UINT message);
BYTE	*GetPatch();
void	CheckClients();
void	InitAllConnections();

