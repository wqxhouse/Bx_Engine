#include <iostream>

#include "Memory/MemoryPool.h"
#include "Math/Vector3.h"

using namespace std;
using namespace Math;

int main()
{
	Vector3Ptr vec3Ptr1 = new Vector3(1, 2, 3);
	Vector3Ptr vec3Ptr2 = new Vector3(4, 5, 6);

	//MemoryPool::registerMemory(vec3Ptr1);
	//MemoryPool::registerMemory(vec3Ptr2);

	Vector3Ptr vecPtr = MemoryPool::find<Vector3Ptr>(vec3Ptr1);
	cout << vecPtr << endl;
	//m_vec3_memory.releaseMemory(vecPtr);

	Vector3Ptr vecPtr2 = MemoryPool::find<Vector3Ptr>(vec3Ptr2);
	cout << vecPtr2 << endl;

	system("pause");
	return 0;
}