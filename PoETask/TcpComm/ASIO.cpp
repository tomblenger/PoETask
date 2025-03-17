#include <stdafx.h>
#include "ASIO.h"
#include "Common.h"
#include "Global_Comm.h"

void	Log(const WCHAR *szFormat, ...);
BYTE	*GetPatch();

BYTE								serverrunning;
boost::asio::io_context				ios;
boost::asio::ip::tcp::acceptor		acceptor(ios);
boost::thread_group					asiothreads;	
tcp::resolver						resolver(ios);

List<Packet *>						readpacketlist;
Map<Session *>						sessionmap;
Session								*serversession;

OnAddRemoveSession_Type				onaddsessionfunc = NULL;
OnAddRemoveSession_Type				onremovesessionfunc = NULL;
OnRecvPacket_Type					onrecvpacketfunc = NULL;


void StartASIO(BOOL bAccept = TRUE);
void startacceptfunc();

Session *FindSession(UINT id)
{
	return sessionmap[id];
}

void ProcThread()
{
	Packet			*packet;

	while(serverrunning)
	{
		if(readpacketlist.PopFront(packet))
		{
			packet->Decrypt();
#ifdef LGT_PACK
			if (packet->bufferinternal[0] == OP_Login_Ack)
			{
				CString strTemp;
				UINT nRetCode = 0;

				packet->ReadStart();
				nRetCode = packet->Read<UINT>();		// RetCode
				if (nRetCode == LR_Success)
				{
					packet->Read<UINT>();		// Multi count
					packet->ReadWideString(strTemp);	// Expired Time
					packet->ReadWideString(strTemp);	// Token
					wcscpy(g_pInfoPacking->szToken, strTemp.GetBuffer(0));
					g_pInfoPacking->dwOpcode = PP_LoginInfo;
					SetEvent(g_hEventPackNotify);
				}
			}
#endif
			if(onrecvpacketfunc)
				onrecvpacketfunc(packet);
			Release<Packet>(packet);
		}
		else
			boost::this_thread::sleep_for(boost::chrono::nanoseconds(10));
	}
}

void	StartClient(OnAddRemoveSession_Type OnAddSessionFunc, OnAddRemoveSession_Type OnRemoveSessionFunc, OnRecvPacket_Type OnRecvPacketFunc)
{
	serverrunning = 1;

	onaddsessionfunc = OnAddSessionFunc;
	onremovesessionfunc = OnRemoveSessionFunc;
	onrecvpacketfunc = OnRecvPacketFunc;
	GetServerSession();
}

void	StartServer(UINT port, OnAddRemoveSession_Type OnAddSessionFunc, OnAddRemoveSession_Type OnRemoveSessionFunc, OnRecvPacket_Type OnRecvPacketFunc, BYTE wait)
{
	int		i;

	serverrunning = 1;

	boost::asio::ip::tcp::endpoint ep(tcp::v4(), port);
	acceptor.open(ep.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor.bind(ep);
	acceptor.listen();
	onaddsessionfunc = OnAddSessionFunc;
	onremovesessionfunc = OnRemoveSessionFunc;
	onrecvpacketfunc = OnRecvPacketFunc;


	for(i = 0; i < ASIO_THREAD_NUM; i++)
	{
		asiothreads.create_thread(boost::bind(StartASIO, TRUE));
		asiothreads.create_thread(ProcThread);
	}

	Log(L"Server Started");
	if(wait)
		asiothreads.join_all();
}

void accept_procfunc(boost::system::error_code ec, tcp::socket socket)
{
	if (!ec)
	{
		Session	*session = Allocate<Session>();
		session->socket = std::move(socket);
		session->SetPatch(GetPatch());
		if(onaddsessionfunc)
			onaddsessionfunc(session);
		session->DoReadHeader(TRUE);
	}
	else
	{
		Log(L"Socket Accept Error\\%S", ec.message().c_str());
	}
	startacceptfunc();
}

void startacceptfunc()
{
	acceptor.async_accept(accept_procfunc);
}

void StartASIO(BOOL bAccept)
{
	if(bAccept)
		startacceptfunc();
	ios.run();
}

Session::Session() :socket(ios), writepacketlist()
{
	Init();
}

Session::~Session()
{
	Deinit();
}

void Session::DoWrite(BOOL bStart)
{
	auto		self(this);
	BYTE		*buffer;

	boost::lock_guard<boost::mutex>	lock(writepacketmutex);
	if(bStart)
	{
		if(writepacket)
			return;
		if(!writepacketlist.PopFront(writepacket))
			return;
		writeoffset = 0;
	}
	else
	{
		if(!writepacket)
		{
			if(!writepacketlist.PopFront(writepacket))
				return;
			writeoffset = 0;
		}
	}
	if (writepacket->bufferexternal)
		buffer = writepacket->bufferexternal;
	else
		buffer = writepacket->bufferinternal;
	socket.async_write_some(boost::asio::buffer(buffer+writeoffset, writepacket->used-writeoffset), [this, self](boost::system::error_code ec, std::size_t sizewrite)
	{
		if(!ec)
		{
			writeoffset += (DWORD)sizewrite;
			if(writeoffset == writepacket->used)
			{
				BYTE	islast;

				islast = writepacket->islastpacket;
				Release<Packet>(writepacket);

				writepacketmutex.lock();
				writepacket = NULL;
				writepacketmutex.unlock();

				if(islast)
				{
					DoDisconnect();
					return;
				}
				DoWrite(TRUE);
			}
			else
			{
				DoWrite();
			}
		}
		else
		{
				DoDisconnect();
		}
	});

}

void Session::DoReadBody(BOOL bStart)
{
	auto		self(this);

	if(bStart)
		readoffset = 0;
	socket.async_read_some(boost::asio::buffer(readpacket.bufferinternal+ REAL_BUFFER_OFFSET+readoffset, *(DWORD *)(readpacket.bufferinternal + 1)-readoffset), [this, self](boost::system::error_code ec, std::size_t readsize)
	{
		if(!ec)
		{
			readoffset += (DWORD)readsize;
			if(readoffset != *(DWORD *)(readpacket.bufferinternal + 1))
			{
				DoReadBody();
			}
			else
			{
				Packet		*packet = Allocate<Packet>();

				readpacket.used += *(DWORD *)(readpacket.bufferinternal + 1);

				packet->Copy(&readpacket);
				packet->session = this;
				readpacketlist.PushBack(packet);
				DoReadHeader(TRUE);
			}
		}
		else
		{
			DoDisconnect();
		}
	});
}

void	MakeEngineBuffer();

void Session::DoReadPatch( BOOL bStart)
{
	auto		self(this);

	if (bStart)
		readoffset = 0;
	socket.async_read_some(boost::asio::buffer(patch + REAL_BUFFER_OFFSET + readoffset, *(DWORD *)(patch + 1) - readoffset), [this, self](boost::system::error_code ec, std::size_t readsize)
	{
		if (!ec)
		{
			readoffset += (DWORD)readsize;
			if (readoffset != *(DWORD *)(patch + 1))
			{
				DoReadPatch();
			}
			else
			{
				MakeEngineBuffer();
				DoReadHeader(TRUE);
			}
		}
		else
		{
			DoDisconnect();
		}
	});
}

void Session::DoReadHeader(BOOL bStart)
{
	auto		self(this);

	if(bStart)
		readoffset = 0;
	socket.async_read_some(boost::asio::buffer(readpacket.bufferinternal+readoffset, REAL_BUFFER_OFFSET - readoffset), [this, self](boost::system::error_code ec, std::size_t readsize)
	{
		if(!ec)
		{
			readoffset += (DWORD)readsize;
			if(readoffset != REAL_BUFFER_OFFSET)
			{
				DoReadHeader();
			}
			else
			{
				/*if (readpacket.bufferinternal[0] == OPCODE_PATCH)
				{
					memcpy(patch, readpacket.bufferinternal, REAL_BUFFER_OFFSET);
					DoReadPatch(TRUE);
				}
				else */if (*(DWORD *)(readpacket.bufferinternal + 1) < MAX_BUFFER_SIZE - 3)
				{
					readpacket.used = REAL_BUFFER_OFFSET;
					if(*(DWORD *)(readpacket.bufferinternal+1) == 0)
					{
						Packet		*packet = Allocate<Packet>();

						packet->Copy(&readpacket);
						packet->session = this;
						readpacketlist.PushBack(packet);
						DoReadHeader(TRUE);
					}
					else
					{
						DoReadBody(TRUE);
					}
				}
				else
				{
					DoDisconnect();
				}
			}
		}
		else
		{
			DoDisconnect();
		}
	});
}

void	SendPatch(Session *session, BYTE *pbPatch)
{
	Packet *packet = Allocate<Packet>();

	if (*(DWORD *)(pbPatch + 1) == 0)
	{
		Log(L"No Patch");
		return;
	}
	packet->bufferexternal = pbPatch;
	packet->used = *(DWORD *)(pbPatch + 1) + REAL_BUFFER_OFFSET;
	session->writepacketlist.PushBack(packet);
	session->DoWrite(TRUE);
}

/*
void Session::TryReconnect(int trycount)
{
	boost::system::error_code errorcode;
	int						tryno = 0;
	CStringA				port;

	boost::lock_guard<boost::mutex>	lock(writepacketmutex);
	if(socket.is_open())
	{
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorcode);
		socket.close(errorcode);
	}

	if(writepacket)
	{
		writepacketlist.PushFront(writepacket);
		writepacket = NULL;
	}

	//
	port.Format("%d", portmap[id]);
	auto endpoints = resolver.resolve("127.0.0.1", port.GetBuffer());

	while(tryno < trycount)
	{
		socket.connect(endpoints->endpoint(), errorcode);
		if(!errorcode)
			break;
		tryno++;
		LogMessage(1, "Reconnecting to server %d: attempts = %d %s", id, tryno, errorcode.message().c_str());
		boost::this_thread::sleep_for(boost::chrono::milliseconds(ASIO_RECONNECT_PERIOD));
	}
	if(tryno >= trycount)
	{
		LogMessage(1, "Can't reconnect to server %d", id);
		return;
	}
	DoReadHeader(TRUE);
	DoWrite(TRUE);
}
*/

void Session::DoDisconnect(void)
{
	boost::system::error_code errorcode;
	Packet					*packet;

	if(socket.is_open())
	{
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorcode);
		socket.close(errorcode);
	}
	boost::lock_guard<boost::mutex>	lock(writepacketmutex);
	while(writepacketlist.PopFront(packet))
	{
		Release<Packet>(packet);
	}
	writepacket = NULL;
	if(onremovesessionfunc)
		onremovesessionfunc(this);
	if (this == serversession)
		serversession = NULL;
	Release<Session>(this);
}

void	SendPacket(Session *session, Packet *packet)
{
	packet->Encrypt();
	session->writepacketlist.PushBack(packet);
	session->DoWrite(TRUE);
}

void	SendFormatedPacket(Session *session, BYTE bOpcode, const char *szFormat, ...)
{
	va_list				arg;
	int					i, nLen;
	Packet				*packet;
	Map<UINT>			*map;

	if(!session)
		return;
	nLen = (int)strlen(szFormat);
	va_start(arg, szFormat);

	packet = Allocate<Packet>();
	packet->CreatePackage(bOpcode);
	for (i = 0; i < nLen; i++)
	{
		switch (szFormat[i])
		{
			case 'b':
			packet->AddData((BYTE)va_arg(arg, BYTE));
			break;
			case 'w':
			packet->AddData((WORD)va_arg(arg, WORD));
			break;
			case 'u':
			packet->AddData((UINT)va_arg(arg, UINT));
			break;
			case 'c':
			packet->AddData((Syncable *)va_arg(arg, Syncable *));
			break;
			case 's':
			{
				CStringA s = (char *)va_arg(arg, char *);
				packet->AddData(s);
			}
			break;
			case 'S':
			{
				CString s = (WCHAR *)va_arg(arg, WCHAR *);
#ifdef LGT_PACK
				if (bOpcode == OP_Login_Req)
				{
					if (i == 0)
					{
						wcscpy(g_pInfoPacking->szAccount, s.GetBuffer(0));
					}
				}
#endif
				packet->AddData(s);
			}
			break;
			case 'U':
			{
				packet->AddData((INT64)va_arg(arg, INT64));
			}
			break;
			case 'r':
			{
				UINT	no, count;
				char**	array;
				CStringA	str;

				count = va_arg(arg, UINT);
				array = va_arg(arg, char**);

				for(no = 0; no < count; no++)
				{
					str = array[no];
					packet->AddData(str);
				}
			}
			break;
			case 'p':
			{
				UINT		id, x;
				map = va_arg(arg, Map<UINT> *);
				packet->AddData((WORD)map->Count());
				while(map->PopFront(id, x))
				{
					packet->AddData(id);
					packet->AddData(x);
				}
			}
			break;
			case 'x':
			packet->islastpacket = 1;
			break;
		}
	}
	va_end(arg); 
	SendPacket(session, packet);
}

Session*	GetServerSession()
{
	Session						*session;
	CStringA					port;
	boost::system::error_code	ec;

	port.Format("%d", SERVERPORT);

	if (serversession)
		return serversession;

	session = Allocate<Session>();
	session->SetPatch(GetPatch());

	auto endpoints = resolver.resolve(SERVERIP, port.GetBuffer());

	session->socket.connect(endpoints->endpoint(), ec);
	if(ec)
	{
		try
		{
			session->socket.close();
		}
		catch(...)
		{
		}
		Release<Session>(session);
		Log(L"Error in connecting server: %S", ec.message().c_str());
		return NULL;
	}

	session->DoReadHeader(TRUE);
	serversession = session;
	boost::asio::socket_base::keep_alive option(true);
	serversession->socket.set_option(option);

	asiothreads.interrupt_all();
	ios.reset();

	for (int i = 0; i < ASIO_THREAD_NUM; i++)
	{
		asiothreads.create_thread(boost::bind(StartASIO, FALSE));
		asiothreads.create_thread(ProcThread);
	}

	return session;
}

