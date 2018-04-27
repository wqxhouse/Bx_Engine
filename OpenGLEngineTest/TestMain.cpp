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