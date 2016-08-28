
#include <stdio.h>
#include <exception>

#include "MemoryPool.h"

#include "../Math/Vector3.h"

//std::unordered_set<void*> MemoryPool::dynamicMemorySet;

std::unordered_map<const char*, std::unordered_set<void*>*> Memory::MemoryPool::dynamicMemoryPool;

Memory::MemoryPool::MemoryPool()
{
}

void Memory::MemoryPool::releaseAll()
{
	for (auto memoryPoolIterator = dynamicMemoryPool.begin();
		memoryPoolIterator != dynamicMemoryPool.end(); ++memoryPoolIterator)
	{
		std::unordered_set<void*>* typeTMemoryPool = memoryPoolIterator->second;
		for (auto typeSubPoolIterator = typeTMemoryPool->begin();
			typeSubPoolIterator != typeTMemoryPool->end(); ++typeSubPoolIterator)
		{
			if (*typeSubPoolIterator != nullptr)
			{
				try
				{
					free(*typeSubPoolIterator);
				}
				catch (std::exception e)
				{
					printf("Can't release the memory for: %s\n", e.what());
				}
			}
		}
		typeTMemoryPool->clear();
		delete typeTMemoryPool;
	}
	dynamicMemoryPool.clear();
}

Memory::MemoryPool::~MemoryPool()
{
	releaseAll();
}

//template <typename T>
//void MemoryPool::registerMemory(void* ptr)
//{
//	//dynamicMemorySet.insert(ptr);
//
//	const char* typeStr = typeid(T).name();
//	std::unordered_set<void*>* typeTMemoryPool = dynamicMemoryPool[typeStr];
//
//	if(dynamicMemoryPool.find(typeStr) == dynamicMemoryPool.end())
//	{
//		typeTMemoryPool = new std::unordered_set<void*>();
//		typeTMemoryPool->insert(ptr);
//		dynamicMemoryPool[typeStr] = typeTMemoryPool;
//	}
//	else
//	{
//		typeTMemoryPool->insert(ptr);
//	}
//}

//template <typename T>
//T MemoryPool::find(void * ptr)
//{
//	auto it = dynamicMemorySet.find(ptr);
//	
//	if (it != dynamicMemorySet.end())
//	{
//		return static_cast<T>(*it);
//	}
//	else
//	{
//		return nullptr;
//	}
//}

//template <typename T>
//void MemoryPool::release(void * ptr)
//{
//	/*auto it = dynamicMemorySet.find(ptr);
//	if(it != dynamicMemorySet.end())
//	{
//	free(*it);
//	dynamicMemorySet.erase(ptr);
//	}*/
//
//	const char* typeStr = typeid(T).name();
//	auto memoryPoolIterator = dynamicMemoryPool.find(typeStr);
//
//	if(memoryPoolIterator != dynamicMemoryPool.end())
//	{
//		std::unordered_set<void*>* typeTMemoryPool = memoryPoolIterator->second;
//		auto typeTMemoryPoolIterator = typeTMemoryPool->find(ptr);
//		if (typeTMemoryPoolIterator != dynamicMemorySet.end())
//		{
//			try
//			{
//				free(*typeTMemoryPoolIterator);
//				typeTMemoryPool->erase(ptr);
//			}
//			catch(std::exception e)
//			{
//				printf("Can't release the memory for: %s\n", e.what());
//			}
//		}
//	}
//}

//void MemoryPool::releaseAll()
//{
//	/*for(auto it = dynamicMemorySet.begin(); it != dynamicMemorySet.end(); ++it)
//	{
//		if(*it != nullptr)
//		{
//			try
//			{
//				free(*it);
//			}
//			catch(std::exception e)
//			{
//				printf("Can't release the memory for: %s\n", e.what());
//			}
//		}
//	}
//
//	dynamicMemorySet.clear();*/
//	for(auto memoryPoolIterator = dynamicMemoryPool.begin(); 
//		memoryPoolIterator != dynamicMemoryPool.end(); ++memoryPoolIterator)
//	{
//		std::unordered_set<void*>* typeTMemoryPool = memoryPoolIterator->second;
//		for(auto typeSubPoolIterator = typeTMemoryPool->begin(); 
//			typeSubPoolIterator != typeTMemoryPool->end(); ++typeSubPoolIterator)
//		{
//			if (*typeSubPoolIterator != nullptr)
//			{
//				try
//				{
//					free(*typeSubPoolIterator);
//				}
//				catch (std::exception e)
//				{
//					printf("Can't release the memory for: %s\n", e.what());
//				}
//			}
//		}
//		typeTMemoryPool->clear();
//		delete typeTMemoryPool;
//	}
//	dynamicMemoryPool.clear();
//}

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