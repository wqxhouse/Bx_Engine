//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "MathTestBase.h"
#include "MeshTest/ObjLoaderTest.h"

int main()
{
	cout << "Start test:" << endl;
	
	RunVectorTest();
	RunObjLoaderTest();

	system("pause");

	return 0;
}