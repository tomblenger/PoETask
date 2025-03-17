#include "stdafx.h"
#include "RemoteMemoryObject.h"


std::wstring RemoteMemoryObject::ReadNativeString(uintptr_t addr)
{
	if (addr != 0)
		return *(std::wstring *)addr;
	return std::wstring();
}

std::wstring RemoteMemoryObject::ReadStringU(uintptr_t addr)
{
	if (addr != 0)
		return std::wstring((wchar_t *)addr);
	return std::wstring();
}

std::string RemoteMemoryObject::ReadString(uintptr_t addr)
{
	if (addr != 0)
		return std::string((char *)addr);
	return std::string();
}