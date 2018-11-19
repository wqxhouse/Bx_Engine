//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

/*
	Test the vector class
	Author: Xiangshun Bei(bxs3514)
	Date: 2016/08/08
*/

#include "MathTestBase.h"

#define PI 3.1415926f

void Vector3Test()
{
	Vector3 vec1;
	Vector3 vec2(1, 2, 3);

	cout << "vec1: " << vec1 << endl;
	cout << "vec2: " << vec2 << endl;

	vec1.X = 4, vec1.Y = 5, vec1.Z = 6;
	cout << "vec1: " << vec1 << endl;

	cout << "Dot: " << vec1.dot(vec2) << endl;
	cout << "Dot2: " << Vector3::dot(vec1, vec2) << endl;

	Vector3 vec3 = vec1;

	vec1.crossProduct(vec2);
	cout << "Cross Product: " << vec1 << endl;

	Vector3 vec4 = Vector3::crossProduct(vec3, vec2);
	cout << "Cross Product2: " << Vector3::crossProduct(vec3, vec2) << endl;

	vec1 += Vector3(1, 1, 1);
	cout << "Add: " << vec1 << endl;

	vec1 -= Vector3(2);
	cout << "Subtract: " << vec1 << endl;

	vec1 *= Vector3(2, 2, 2);
	cout << "Multiply: " << vec1 << endl;

	vec1 /= Vector3(3, 3, 3);
	cout << "Devide: " << vec1 << endl;

	vec4 = vec1 + vec2;
	cout << "V1 + V2: " << vec4 << endl;
}

void Vector4Test()
{
	Vector4 vec1(1, 2, 3, 2);
	Vector4 vec2(4, 5, 6, 1);

	cout << vec1 << endl;
	cout << vec2 << endl;

	vec1.crossProduct(vec2);
	cout << vec1 << endl;

	Vector4 vec3 = Vector4::crossProduct(vec1, vec2);
	cout << vec3 << endl;
}

void QuaternionTest()
{
	Vector3 v(1.0f, 0.0f, 0.0f);
	float degree = -90.0f * 0.01745329f;
	Vector3 res = rotate(v, Vector3(0.0f, 1.0f, 0.0f), degree);

	cout << res << endl;
}

void RunVectorTest()
{
	cout << "Vector3 Test" << endl;
	Vector3Test();

	cout << "Vector4 Test" << endl;
	Vector4Test();

	cout << "Quaternion Test" << endl;
	QuaternionTest();
}
