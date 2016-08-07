/*
	Test the vector class
	Author: Xiangshun Bei(bxs3514)
	Date: 2016/08/08
*/

#include "MathTestBase.h"

void Vector3Test()
{
	Vector3 vec1;
	Vector3 vec2(1, 2, 3);

	cout << "vec1: " << vec1;
	cout << "vec2: " << vec2;

	vec1.X = 4, vec1.Y = 5, vec1.Z = 6;
	cout << "vec1: " << vec1;

	cout << "Dot: " << vec1.dot(vec2) << endl;
	cout << "Dot2: " << Vector3::dot(vec1, vec2) << endl;

	Vector3 vec3 = vec1;

	vec1.crossProduct(vec2);
	cout << "Cross Product: " << vec1;

	Vector3 vec4 = Vector3::crossProduct(vec3, vec2);
	cout << "Cross Product2: " << vec4;

	vec1 += Vector3(1, 1, 1);
	cout << "Add: " << vec1;

	vec1 -= Vector3(2);
	cout << "Subtract: " << vec1;

	vec1 *= Vector3(2, 2, 2);
	cout << "Multiply: " << vec1;

	vec1 /= Vector3(3, 3, 3);
	cout << "Devide: " << vec1;

	vec4 = vec1 + vec2;
	cout << "V1 + V2: " << vec4;
}

void Vector4Test()
{
	Vector4 vec1(1, 2, 3, 1);
	Vector4 vec2(4, 5, 6, 1);

	cout << vec1;
	cout << vec2;

	vec1.crossProduct(vec2);
	cout << vec1;

	Vector4 vec3 = Vector4::crossProduct(vec1, vec2);
	cout << vec3;
}

int main()
{
	cout << "Vector3 Test" << endl;
	Vector3Test();

	cout << "Vector4 Test" << endl;
	Vector4Test();

	system("pause");
}