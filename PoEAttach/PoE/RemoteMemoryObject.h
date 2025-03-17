#pragma once

#include <string>

class RemoteMemoryObject
{
public:
	RemoteMemoryObject() { Address = 0; }

	template <class T> 
	static T * ReadObject(uintptr_t addressPointer) 
	{
		auto t = new T;
		((RemoteMemoryObject *)t)->Address = *(uintptr_t *)addressPointer;
		return t;
	}

	static std::wstring ReadNativeString(uintptr_t addr);
	static std::wstring ReadStringU(uintptr_t addr);
	static std::string ReadString(uintptr_t addr);


	uintptr_t Address;
};

