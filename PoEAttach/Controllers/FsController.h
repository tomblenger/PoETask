#pragma once

#include <map>
#include <string>

class FsController
{
public:
	FsController();

	std::map<std::wstring, uintptr_t> GetAllFiles();
	uintptr_t FindFile(std::wstring name);

	std::map<std::wstring, uintptr_t>	_files;
};