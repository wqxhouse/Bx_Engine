#pragma once

#include <unordered_set>

class MemoryPool
{
public:
	MemoryPool();
	void registerMemory(void* ptr);
	void releaseMemory(void* ptr);

	void* find(void* ptr);
	void release();
	~MemoryPool();
private:
	std::unordered_set<void*> dynamicMemorySet;
};

