#pragma once

#include <Math.h>

struct Vector3
{
	float	x, y, z;

	Vector3();
	Vector3(const Vector3& vec);
	Vector3(float _x, float _y, float _z);

	Vector3& operator = (const Vector3& v);

	// +
	Vector3 operator + (const Vector3& v)	const;
	Vector3 operator + (float f)	const;
	Vector3 operator + (int i)	const;

	Vector3 operator += (const Vector3& v);
	Vector3 operator += (float f);
	Vector3 operator += (int i);

	// -
	Vector3 operator - (const Vector3& v)	const;
	Vector3 operator - (float f)	const;
	Vector3 operator - (int i)	const;

	Vector3 operator -= (const Vector3& v);
	Vector3 operator -= (float f);
	Vector3 operator -= (int i);

	// *
	Vector3 operator * (const Vector3& v)	const;
	Vector3 operator * (float f)	const;
	Vector3 operator * (int i)	const;

	Vector3 operator *= (const Vector3& v);
	Vector3 operator *= (float f);
	Vector3 operator *= (int i);

	// /
	Vector3 operator / (const Vector3& v)	const;
	Vector3 operator / (float f)	const;
	Vector3 operator / (int i)	const;

	Vector3 operator /= (const Vector3& v);
	Vector3 operator /= (float f);
	Vector3 operator /= (int i);

	float Length()	const;
	void Normalize();
	static Vector3 Normalize(const Vector3& v);
	float Distance(const Vector3& v)	const;
	static float Distance(const Vector3& v1, const Vector3& v2);
	float Dot(const Vector3& v)	const;
	static float Dot(const Vector3& v1, const Vector3& v2);
	Vector3 Cross(const Vector3& v)	const;
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);
};

