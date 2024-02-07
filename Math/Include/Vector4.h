#pragma once
#pragma once

#include <Math.h>

struct Vector4
{
	float	x, y, z, w;

	Vector4();
	Vector4(const Vector4& vec);
	Vector4(float _x, float _y, float _z, float _w);

	Vector4 operator = (const Vector4& v);

	// +
	Vector4 operator + (const Vector4& v)	const;
	Vector4 operator + (float f)	const;
	Vector4 operator + (int i)	const;

	Vector4 operator += (const Vector4& v);
	Vector4 operator += (float f);
	Vector4 operator += (int i);

	// -
	Vector4 operator - (const Vector4& v)	const;
	Vector4 operator - (float f)	const;
	Vector4 operator - (int i)	const;

	Vector4 operator -= (const Vector4& v);
	Vector4 operator -= (float f);
	Vector4 operator -= (int i);

	// *
	Vector4 operator * (const Vector4& v)	const;
	Vector4 operator * (float f)	const;
	Vector4 operator * (int i)	const;

	Vector4 operator *= (const Vector4& v);
	Vector4 operator *= (float f);
	Vector4 operator *= (int i);

	// /
	Vector4 operator / (const Vector4& v)	const;
	Vector4 operator / (float f)	const;
	Vector4 operator / (int i)	const;

	Vector4 operator /= (const Vector4& v);
	Vector4 operator /= (float f);
	Vector4 operator /= (int i);

	float Length()	const;
	void Normalize();
	static Vector4 Normalize(const Vector4& v);
	float Distance(const Vector4& v)	const;
	static float Distance(const Vector4& v1, const Vector4& v2);
	float Dot(const Vector4& v)	const;
	static float Dot(const Vector4& v1, const Vector4& v2);
	Vector4 Cross(const Vector4& v)	const;
	static Vector4 Cross(const Vector4& v1, const Vector4& v2);
};

