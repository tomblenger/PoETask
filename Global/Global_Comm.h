#pragma once

//#define LOCAL_TEST

#ifdef LOCAL_TEST
#define SERVERIP					"192.168.1.107"
#else
#define SERVERIP					"121.169.33.194"
#endif
#define SERVERPORT					9373


enum OPCODE
{
	OP_Keepalive,
	OP_Login_Req,
	OP_Login_Ack,
	OP_ForceLogout,
	OP_Logout,
	OP_DisconnectAccount,
};

enum LOGINRESULT
{
	LR_None = 0,
	LR_Success,
	LR_InvalidAccount,
	LR_Expired,
	LR_InvalidDevice,
	LR_InvalidToken,
	LR_Duplicate,
};
