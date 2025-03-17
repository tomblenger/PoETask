#pragma once

#define BOOST_THREAD_NO_CXX11_HDR_INITIALIZER_LIST

#include <boost/container/list.hpp>
#include <boost/container/map.hpp>
#include <boost/container/vector.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include "../../Global/Global_Comm.h"

#ifndef _NO_SQL_
#include "include/mysql.h"
#endif

#define	MAX_BUFFER_SIZE		4000
#define REAL_BUFFER_OFFSET	6

class Buffer;

class Syncable
{
public:
	virtual ~Syncable();

	virtual	void	Init() {}
	virtual void	Deinit() {}
	virtual void	Read(Buffer *bufferinternal) {}
	virtual void	Write(Buffer *bufferinternal) {}
};

template<typename X> class List
{
public:
	List(List<X> &Other) { boost::lock_guard<boost::mutex> lock(mutex); boost::lock_guard<boost::mutex> lockother(Other.mutex); list.clear(); list = Other.list; }
	List() { list.clear(); }
	~List() { list.clear(); }

	BOOL	IsEmpty() { boost::lock_guard<boost::mutex>	lock(mutex); return list.empty(); }
	void	PushFront(X p) { boost::lock_guard<boost::mutex>	lock(mutex); list.push_front(p); }
	void	PushBack(X p) { boost::lock_guard<boost::mutex>	lock(mutex); list.push_back(p); }
	BOOL	PopFront(X &p) { boost::lock_guard<boost::mutex>	lock(mutex); if(list.empty())return FALSE; p = list.front(); list.pop_front(); return TRUE; }
	BOOL	PopBack(X &p) { boost::lock_guard<boost::mutex>	lock(mutex); if(list.empty())return FALSE; p = list.back(); list.pop_back(); return TRUE; }
	BOOL	PopFront() { boost::lock_guard<boost::mutex>	lock(mutex); if(list.empty())return FALSE; list.pop_front(); return TRUE; }
	BOOL	PopBack() { boost::lock_guard<boost::mutex>	lock(mutex); if(list.empty())return FALSE; list.pop_back(); return TRUE; }
	int		Count() { boost::lock_guard<boost::mutex>	lock(mutex); return (int)list.size(); }
	void	Clear() { boost::lock_guard<boost::mutex>	lock(mutex); list.clear(); }
	void	Unique() { boost::lock_guard<boost::mutex>	lock(mutex); list.unique(); }
	void	Remove(X p) { boost::lock_guard<boost::mutex>	lock(mutex); list.remove(p); }
	BOOL	Find(X p) { boost::lock_guard<boost::mutex>	lock(mutex); for(auto it = list.begin(); it != list.end(); ++it) if(*it == p)return TRUE; return FALSE; }
	List<X>&	operator=(const List<X>& Other) { boost::lock_guard<boost::mutex> lock(mutex); boost::lock_guard<boost::mutex> lockother(((List<X> *)(&Other))->mutex);  list.clear(); list = Other.list; return *this; }

protected:
	boost::mutex								mutex;
	boost::container::list<X>					list;
};

typedef List<Syncable *> SList;

template<typename X> class Map
{
public:
	Map(const Map<X>& Other) { boost::lock_guard<boost::mutex> lock(mutex); boost::lock_guard<boost::mutex> lockother(((Map<X> *)(&Other))->mutex); map.clear(); map = Other.map; }
	Map() { map.clear(); }
	~Map() { map.clear(); }

	BOOL	IsEmpty() { boost::lock_guard<boost::mutex>	lock(mutex); return map.empty(); }
	BOOL	PopFront(X &x) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.empty())return FALSE; auto it = map.begin(); x = it->second; map.erase(it); return TRUE; }
	BOOL	PopFront(UINT &i, X &x) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.empty())return FALSE; auto it = map.begin(); i = it->first; x = it->second; map.erase(it); return TRUE; }
	BOOL	PopBack(X &x) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.empty())return FALSE; auto it = map.rbegin(); x = it->second; map.erase(it); return TRUE; }
	BOOL	PopBack(UINT &i, X &x) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.empty())return FALSE; auto it = map.rbegin(); i = it->first; x = it->second; map.erase(it); return TRUE; }
	int		Count() { boost::lock_guard<boost::mutex>	lock(mutex); return (int)map.size(); }
	void	Clear() { boost::lock_guard<boost::mutex>	lock(mutex); map.clear(); }
	void	Remove(const UINT &i) { boost::lock_guard<boost::mutex>	lock(mutex); map.erase(i); }
	void	SetAt(const UINT &i, const X& x) { boost::lock_guard<boost::mutex>	lock(mutex); map[i] = x; }
	BOOL	ExclusiveSetAt(const UINT &i, const X& x) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.find(i) != map.end())return FALSE; map[i] = x; return TRUE; }
	BOOL	Find(const UINT &i) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.find(i) == map.end())return FALSE; else return TRUE; }
	BOOL	Find(const UINT &i, X& x) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.find(i) == map.end())return FALSE; x = map[i]; return TRUE; }
	X		operator[](const UINT &i) { boost::lock_guard<boost::mutex>	lock(mutex); if(map.find(i) != map.end())return map[i]; return NULL;}
	Map<X>&		operator=(const Map<X>& Other) { boost::lock_guard<boost::mutex> lock(mutex); boost::lock_guard<boost::mutex> lockother(((Map<X> *)(&Other))->mutex); map.clear(); map = Other.map; return *this; }

protected:
	boost::mutex							mutex;
	boost::container::map<UINT, X>			map;
};

template<typename X> class Array
{
public:
	Array(Array<X> &Other) { boost::lock_guard<boost::mutex> lock(mutex); boost::lock_guard<boost::mutex> lockother(Other.mutex); arr.clear(); arr = Other.arr; }
	Array() { arr.clear(); }
	~Array() { arr.clear();}

	BOOL	IsEmpty() { boost::lock_guard<boost::mutex>	lock(mutex); return arr.empty(); }
	void	PushFront(X p) { boost::lock_guard<boost::mutex>	lock(mutex); arr.push_front(p); }
	void	PushBack(X p) { boost::lock_guard<boost::mutex>	lock(mutex); arr.push_back(p); }
	BOOL	PopFront(X &p) { boost::lock_guard<boost::mutex>	lock(mutex); if (arr.empty())return FALSE; p = arr.front(); arr.pop_front(); return TRUE; }
	BOOL	PopBack(X &p) { boost::lock_guard<boost::mutex>	lock(mutex); if (arr.empty())return FALSE; p = arr.back(); arr.pop_back(); return TRUE; }
	BOOL	PopFront() { boost::lock_guard<boost::mutex>	lock(mutex); if (arr.empty())return FALSE; arr.pop_front(); return TRUE; }
	BOOL	PopBack() { boost::lock_guard<boost::mutex>	lock(mutex); if (arr.empty())return FALSE; arr.pop_back(); return TRUE; }
	int		Count() { boost::lock_guard<boost::mutex>	lock(mutex); return (int)arr.size(); }
	void	Clear() { boost::lock_guard<boost::mutex>	lock(mutex); arr.clear(); }
	void	Unique() { boost::lock_guard<boost::mutex>	lock(mutex); arr.unique(); }
	void	Remove(X p) { boost::lock_guard<boost::mutex>	lock(mutex); arr.remove(p); }
	BOOL	Find(X p) { boost::lock_guard<boost::mutex>	lock(mutex); for (auto it = arr.begin(); it != arr.end(); ++it) if (*it == p)return TRUE; return FALSE; }
	Array<X>&	operator=(const Array<X>& Other) { boost::lock_guard<boost::mutex> lock(mutex); boost::lock_guard<boost::mutex> lockother(((Array<X> *)(&Other))->mutex);  arr.clear(); arr = Other.arr; return *this; }

public:
	boost::mutex								mutex;
	boost::container::vector<X>					arr;
};

typedef Map<Syncable *> SMap;

template<typename X> class	Storage
{
public:
	Storage() :list(), freelist() { }
	~Storage() {}
	X *Allocate()
	{
		X	*p = NULL;

		if(freelist.Count() >= 10)
		{
			if(!freelist.PopFront(p))
			{
				p = new X;
			}
		}
		else
		{
			p = new X;
		}
		p->Init();
		list.PushBack(p);
		list.Unique();
		return p;
	}

	void Release(X *p)
	{
		if(list.Find(p))
		{
			p->Deinit();
			list.Remove(p);
			freelist.PushBack(p);
			freelist.Unique();
		}
	}
	List<X*>			list, freelist;
};

template<typename X> Storage<X>& GetStorage()
{
	static Storage<X>	storage;

	return storage;
}

template<typename X> X* Allocate()
{
	Storage<X> &storage = GetStorage<X>();

	return storage.Allocate();
}

template<typename X> void Release(X *x)
{
	Storage<X> &storage = GetStorage<X>();

	storage.Release(x);
}

class Buffer : public Syncable
{
public:
	Buffer();
	virtual ~Buffer();
	virtual	void	Init()
	{
		bufferexternal = NULL;
		EmptyUsed();
		__super::Init();
	}
	virtual void	Deinit()
	{
		bufferexternal = NULL;
		__super::Deinit();
	}

	void EmptyUsed();
	void AddData(const BYTE * pData, DWORD wSize, BOOL bDontChangeLength = 0);
	template<typename X> void AddData(X data)
	{
		AddData(reinterpret_cast<const BYTE*>(&data), sizeof(X));
	}
	template<typename X> void AddData(Map<X> map, DWORD dwSize = 0)
	{
		X		x;

		if(!dwSize)
		{
			AddData((DWORD)map.Count(););
		}
		while(map.PopFront(x))
		{
			AddData(x);
		}
	}
	template<typename X> void AddData(List<X> list, DWORD dwSize = 0)
	{
		X	x;

		if(!dwSize)
		{
			AddData((DWORD)list.Count());
		}
		while(list.PopFront(x))
		{
			AddData(x);
		}
	}
	void AddData(SMap map, DWORD dwSize = 0)
	{
		Syncable	*syncable;

		if(!dwSize)
		{
			AddData((DWORD)map.Count());
		}
		while(map.PopFront(syncable))
		{
			AddData(syncable);
		}
	}
	void AddData(SList list, DWORD dwSize = 0)
	{
		Syncable	*syncable;

		if(!dwSize)
		{
			AddData((DWORD)list.Count());
		}
		while(list.PopFront(syncable))
		{
			AddData(syncable);
		}
	}
	void AddData(CStringA &str, DWORD wSize = 0);
	void AddData(CStringW &str, DWORD wSize = 0);
	void AddData(Syncable *pSyncable);
	void CreatePackage(BYTE bOpcode);

	void ReadStart();
	void Read(BYTE *pData, DWORD wSize);
	template<typename X> X Read()
	{
		X	value;

		if(readoffset + sizeof(X) > used)
			return 0;
		value = *(X *)(bufferinternal + readoffset);
		readoffset += sizeof(X);
		return value;
	}
	template<typename X> Map<X> ReadMap(DWORD dwSize = 0)
	{
		WORD	i;
		Map<X>	map;

		if(!dwSize)
		{
			wSize = Read<DWORD>();
			wSize = min(dwSize, MAX_PATH);
		}
		for(i = 0; i < dwSize; i++)
		{
			map[(UINT)i] = Read<X>();
		}
		return map;
	}
	template<typename X> List<X> ReadList(DWORD dwSize = 0)
	{
		WORD	i;
		List<X>	list;

		if(!wSize)
		{
			wSize = Read<DWORD>();
			wSize = min(dwSize, MAX_PATH);
		}
		for(i = 0; i < dwSize; i++)
		{
			list.PushBack(Read<X>());
		}
		return list;
	}
	template<typename X> SMap ReadSMap(DWORD dwSize = 0)
	{
		WORD	i;
		SMap	map;
		X		*p;

		if(!dwSize)
		{
			dwSize = Read<DWORD>();
			dwSize = min(dwSize, MAX_PATH);
		}
		for(i = 0; i < dwSize; i++)
		{
			p = Allocate<X>();
			p->Read(this);
			map[i] = (Syncable *)p;
		}
		return map;
	}
	template<typename X> SList ReadSList(DWORD dwSize = 0)
	{
		WORD	i;
		SList	list;
		X		*p;

		if(!dwSize)
		{
			dwSize = Read<DWORD>();
			dwSize = min(dwSize, MAX_PATH);
		}
		for(i = 0; i < dwSize; i++)
		{
			p = Allocate<X>();
			p->Read(this);
			list.PushBack((Syncable *)p);
		}
		return list;
	}
	void ReadMultiString(CStringA &str, DWORD wSize = 0);
	void ReadWideString(CStringW &str, DWORD wSize = 0);
	void Read(Buffer *buffer);
	void Write(Buffer *buffer);

	void Copy(Buffer *buffer);
	void Encrypt();
	void Decrypt();

	DWORD    used;
	BYTE	bufferinternal[MAX_BUFFER_SIZE];
	BYTE	*bufferexternal;
	DWORD	readoffset;
};

#ifndef _NO_SQL_

class SQLResult : public Syncable
{
public:
	virtual void Init()
	{
		error = 0;
		row = NULL;
		res = NULL;
		//mysql_init(&mysql);
#ifdef LOCAL_TEST
		//if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "", "globalera", 3306, 0, 0))
#else
		//if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "lightman0801*&^%allow!Db", "globalera", 3306, 0, 0))
#endif
		{
			error = -1;
		}
	}
	virtual void Deinit()
	{
		//if (res)
			//mysql_free_result(res);
		//mysql_close(&mysql);
	}
	MYSQL_ROW		row;
	MYSQL_RES		*res;
	MYSQL			mysql;
	int				error;
};


SQLResult	*SQLFirst(const char *queryformat, ...);
BOOL		SQLNext(SQLResult *sql);
void		SQLClose(SQLResult *sql);

#endif 