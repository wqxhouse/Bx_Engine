
#include <stdio.h>
#include <exception>

#include "../../include/Memory/MemoryPool.h"

#include "../../include/Math/Vector3.h"

std::unordered_set<void*> MemoryPool::dynamicMemorySet;

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
		free(*it);
		dynamicMemorySet.erase(ptr);
	}
}

void * MemoryPool::find(void * ptr)
{
	auto it = dynamicMemorySet.find(ptr);
	
	if (it != dynamicMemorySet.end())
	{
		return *it;
	}
	else
	{
		return nullptr;
	}
}



void MemoryPool::release()
{
	for(auto it = dynamicMemorySet.begin(); it != dynamicMemorySet.end(); it++)
	{
		if(*it != nullptr)
		{
			try
			{
				free(*it);
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

//void* operator new(size_t memorySize)
//{
//	void* ptr = malloc(memorySize);
//	//MemoryPool::registerMemory(ptr);
//
//	return ptr;
//}
//
//void operator delete(void* ptr)
//{
//	//MemoryPool::releaseMemory(ptr);
//}