//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <iostream>

#include "Math/Vector3.h"
#include "Math/Vector4.h"

using namespace std;
using namespace Math;
using namespace Memory;

//int main()
//{
//	Vector3Ptr vec3Ptr1 = Vector3::New(1, 2, 3);
//	Vector3Ptr vec3Ptr2 = Vector3::New(4, 5, 6);
//
//	Vector4Ptr vec4Ptr1 = Vector4::New(7, 8, 9, 10);
//
//	Vector3Ptr vecPtr = MemoryPool::find<Vector3Ptr>(vec3Ptr1);
//	cout << vecPtr << endl;
//
//	Vector3Ptr vecPtr2 = MemoryPool::find<Vector3Ptr>(vec3Ptr2);
//	cout << vecPtr2 << endl;
//
//	Vector4Ptr vecPtr3 = MemoryPool::find<Vector4Ptr>(vec4Ptr1);
//	cout << vecPtr3 << endl;
//
//	MemoryPool::releaseAll();
//
//	system("pause");
//	return 0;
//}