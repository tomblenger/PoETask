#pragma once

#include "RemoteMemoryObject.h"

class Entity;

class Component : public RemoteMemoryObject
{
public:
	Entity Owner();
	
};