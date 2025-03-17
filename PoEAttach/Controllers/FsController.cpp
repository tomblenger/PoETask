#include "stdafx.h"
#include "FsController.h"
#include "Offsets.h"
#include "RemoteMemoryObject.h"


FsController::FsController()
{
	_files = GetAllFiles();
}

std::map<std::wstring, uintptr_t> FsController::GetAllFiles()
{
	std::map<std::wstring, uintptr_t> fileList;
	uintptr_t fileRoot = TO_POE(Offsets::Instance->FileRoot);
	uintptr_t start = *(uintptr_t *)(fileRoot + 0x8);

	for (auto curFile = *(uintptr_t *)start; curFile != start && curFile != 0; curFile = *(uintptr_t *)curFile)
	{
		auto str = RemoteMemoryObject::ReadStringU(*(uintptr_t *)(curFile + 0x10));
		if (str.empty() || !*(uintptr_t *)(curFile + 0x18))
			continue;

		if (fileList.count(str) > 0)
		{
			fileList[str] = *(uintptr_t *)(curFile + 0x18);
		}
	}

	return fileList;
}

uintptr_t FsController::FindFile(std::wstring name)
{
	if (_files.count(name) > 0)
		return _files[name];
	return 0;
}
