
#include <stdio.h>
#include <exception>

#include "../../include/Memory/MemoryPool.h"

MemoryPool::MemoryPool()
{
}

void MemoryPool::registerMemory(void* ptr)
{
	dynamicMemorySet.insert(ptr);
}

void MemoryPool::releaseMemory(void * ptr)
{
	auto it = dynamicMemorySet.find(ptr);
	if(it != dynamicMemorySet.end())
	{
		delete *it;
		dynamicMemorySet.erase(ptr);
	}
}

void * MemoryPool::find(void * ptr)
{
	auto it = dynamicMemorySet.find(ptr);
	
	return *it;
}



void MemoryPool::release()
{
	for(auto it = dynamicMemorySet.begin(); it != dynamicMemorySet.end(); it++)
	{
		if(*it != nullptr)
		{
			try
			{
				delete *it;
			}
			catch(std::exception e)
			{
				printf("Can't release the memory for: %s\n", e.what());
			}
		}
	}

	dynamicMemorySet.clear();
}

MemoryPool::~MemoryPool()
{
	release();
}
