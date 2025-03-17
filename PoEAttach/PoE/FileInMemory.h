#pragma once

#include <vector>

class FileInMemory
{
public:
	FileInMemory() { Address = 0; }
	std::vector<uintptr_t> RecordAddresses();

	uintptr_t Address;
};
