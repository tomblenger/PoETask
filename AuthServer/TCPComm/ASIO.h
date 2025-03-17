#pragma once
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/interprocess/ipc/message_queue.hpp>
#include "Buffer.h"
#include "../../Global/Global_Comm.h"

#define ASIO_THREAD_NUM	4
#define	ASIO_RECONNECT_PERIOD	5000


// General Msg Req Format
// PortString|TypeString|OtherString1|OtherString2|....OtherStringEnd

// General Msg Ack Format
// TypeString||OtherString1|OtherString2|....OtherStringEnd

// Term Ack Format
// 1byte

using boost::asio::ip::tcp;
using boost::thread;
using namespace  boost::interprocess;

class Session;

class Packet : public Buffer
{
public:
	Packet() :Buffer()
	{
	}
	virtual ~Packet()
	{

	}
	virtual void Init()
	{
		session = NULL;
		islastpacket = 0;
		__super::Init();
	}
	virtual void Deinit()
	{
		session = NULL;
		islastpacket = 0;
		__super::Deinit();
	}
public:
	Session		*session;
	BYTE		islastpacket;
};

class Session : public Syncable
{
public:
	Session();
	virtual ~Session();
	void	DoReadHeader(BOOL bStart = 0);
	void	DoWrite(BOOL bStart = 0);
	void	DoReadBody(BOOL bStart = 0);
	void	DoReadPatch(BOOL bStart = 0);
	void	SetPatch(BYTE *pbPatch)
	{
		patch = pbPatch;
	}
	void	DoDisconnect();
//	void	TryReconnect(int trycount = INT_MAX);
	virtual void Init()
	{
		readpacket.session = this;
		lastcommtick = time(0);
		writepacket = NULL;
		__super::Init();
	}
	virtual void	Deinit() 
	{
		Packet *packet;

		ids.clear();
		writepacket = NULL;
		while(writepacketlist.PopFront(packet))
		{
			Release<Packet>(packet);
		}
		__super::Deinit();
	}
public:
	DWORD								readoffset, writeoffset;
	tcp::socket							socket;
	boost::asio::io_service::strand		strand_;
	Packet								readpacket;
	BYTE								*patch;
	mutable boost::atomic<time_t>		lastcommtick;
	List<Packet *>						writepacketlist;
	Packet *							writepacket;
	boost::mutex						writepacketmutex;
	std::vector<int>					ids;
};

typedef void (*OnAddRemoveSession_Type)(Session *session);
typedef void (*OnRecvPacket_Type)(Packet *packet);
typedef void (*Log_Type)(int nType, int ServerID, char *szFormat, ...);

void		StartServer(UINT port, OnAddRemoveSession_Type OnAddSessionFunc, OnAddRemoveSession_Type OnRemoveSessionFunc, OnRecvPacket_Type OnRecvPacketFunc, BYTE wait);
void		SendPacket(Session *session, Packet *packet);
void		SendFormatedPacket(Session *session, BYTE bOpcode, const char *szFormat, ...);
Session*	GetServerSession();
Session		*FindSession(UINT id);
void		SendPatch(Session *session, BYTE *pbPatch);
void		StartClient(OnAddRemoveSession_Type OnAddSessionFunc, OnAddRemoveSession_Type OnRemoveSessionFunc, OnRecvPacket_Type OnRecvPacketFunc);

extern	Map<Session *>					sessionmap;
extern	BYTE							serverrunning;
extern	Map<UINT>						portmap;