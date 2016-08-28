#pragma once

#include <unordered_set>
#include <unordered_map>

namespace Memory
{
	class MemoryPool
	{
	public:
		MemoryPool();
		~MemoryPool();

		template <typename T>
		static void registerMemory(void* ptr)
		{
			const char* typeStr = typeid(T).name();
			std::unordered_set<void*>* typeTMemoryPool = dynamicMemoryPool[typeStr];

			if (!typeTMemoryPool)
			{
				typeTMemoryPool = new std::unordered_set<void*>();
				typeTMemoryPool->insert(ptr);
				dynamicMemoryPool[typeStr] = typeTMemoryPool;
			}
			else
			{
				typeTMemoryPool->insert(ptr);
			}
		}

		template<typename T>
		static void release(void* ptr)
		{
			const char* typeStr = typeid(T).name();
			auto memoryPoolIterator = dynamicMemoryPool.find(typeStr);

			if (memoryPoolIterator != dynamicMemoryPool.end())
			{
				std::unordered_set<void*>* typeTMemoryPool = memoryPoolIterator->second;
				auto typeTMemoryPoolIterator = typeTMemoryPool->find(ptr);
				if (typeTMemoryPoolIterator != dynamicMemorySet.end())
				{
					try
					{
						free(*typeTMemoryPoolIterator);
						typeTMemoryPool->erase(ptr);
					}
					catch (std::exception e)
					{
						printf("Can't release the memory for: %s\n", e.what());
					}
				}
			}
		}

		template<typename T>
		static T find(void* ptr)
		{
			const char* typeStr = typeid(T).name();
			std::unordered_set<void*>* typeTMemoryPool = dynamicMemoryPool[typeStr];

			if (typeTMemoryPool != nullptr)
			{
				void* memoryBlock = *(typeTMemoryPool->find(ptr));

				if (memoryBlock != nullptr)
				{
					return static_cast<T>(memoryBlock);
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}

			/*auto it = dynamicMemorySet.find(ptr);

			if (it != dynamicMemorySet.end())
			{
				return static_cast<T>(*it);
			}
			else
			{
				return nullptr;
			}*/
		}
		
		static void releaseAll();
	private:
		static ::std::unordered_map<const char*, ::std::unordered_set<void*>*> dynamicMemoryPool;
		//static std::unordered_set<void*> dynamicMemorySet;
	};


	/*template<typename T>
	void* operator new(size_t memorySize)
	{
		void* ptr = malloc(memorySize);
		MemoryPool::registerMemory<T>(ptr);

		return ptr;
	}

	template<typename T>
	void operator delete(void* ptr)
	{
		MemoryPool::release<T>(ptr);

	}*/
}