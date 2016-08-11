#pragma once

#include <unordered_set>

class MemoryPool
{
public:
	MemoryPool();
	static void registerMemory(void* ptr);
	static void releaseMemory(void* ptr);

	void* find(void* ptr);
	static void release();
	~MemoryPool();

	//void* operator new(size_t memorySize);
	//void operator delete(void* ptr);
private:
	static std::unordered_set<void*> dynamicMemorySet;
};

