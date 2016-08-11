#include <iostream>

#include "Memory/MemoryPool.h"
#include "Math/Vector3.h"

using namespace std;
using namespace Math;

int main()
{
	MemoryPool m_vec3_memory;

	Vector3Ptr vec3Ptr1 = new Vector3(1, 2, 3);
	Vector3Ptr vec3Ptr2 = new Vector3(4, 5, 6);

	//MemoryPool::registerMemory(vec3Ptr1);
	//MemoryPool::registerMemory(vec3Ptr2);

	void* it = m_vec3_memory.find(vec3Ptr1);
	Vector3Ptr vecPtr = static_cast<Vector3Ptr>(it);
	cout << vecPtr << endl;
	//m_vec3_memory.releaseMemory(vecPtr);

	it = m_vec3_memory.find(vec3Ptr2);
	Vector3Ptr vecPtr2 = static_cast<Vector3Ptr>(it);
	cout << vecPtr2 << endl;

	system("pause");
	return 0;
}