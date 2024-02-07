#include "Vector4.h"

Vector4::Vector4() :
	x(0.f),
	y(0.f),
	z(0.f)
{
}

Vector4::Vector4(const Vector4& vec)
{
	//*this = vec;
}

Vector4::Vector4(float _x, float _y, float _z, float _w) :
	x(_x),
	y(_y),
	z(_z),
	w(_w)
{
}

Vector4 Vector4::operator=(const Vector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;

	return *this;
}

Vector4 Vector4::operator+(const Vector4& v)	const
{
	Vector4 result;
	result.x = x + v.x;
	result.y = y + v.y;
	result.z = z + v.z;
	result.w = w + v.w;

	return result;
}

Vector4 Vector4::operator+(float f)	const
{
	Vector4 result;
	result.x = x + f;
	result.y = y + f;
	result.z = z + f;
	result.w = w + f;

	return result;
}

Vector4 Vector4::operator+(int i)	const
{
	Vector4 result;
	result.x = x + (float)i;
	result.y = y + (float)i; 
	result.z = z + (float)i;
	result.w = w + (float)i;

	return result;
}

Vector4 Vector4::operator+=(const Vector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}

Vector4 Vector4::operator+=(float f)
{
	x += f;
	y += f;
	z += f;
	w += f;

	return *this;
}

Vector4 Vector4::operator+=(int i)
{
	x += i;
	y += i;
	z += i;
	w += i;

	return *this;
}


// -
Vector4 Vector4::operator-(const Vector4& v)	const
{
	Vector4 result;
	result.x = x - v.x;
	result.y = y - v.y;
	result.z = z - v.z;
	result.w = w - v.w;

	return result;
}

Vector4 Vector4::operator-(float f)	const
{
	Vector4 result;
	result.x = x - f;
	result.y = y - f;
	result.z = z - f;
	result.w = w - f;

	return result;
}

Vector4 Vector4::operator-(int i)	const
{
	Vector4 result;
	result.x = x - (float)i;
	result.y = y - (float)i;
	result.z = z - (float)i;
	result.w = w - (float)i;

	return result;
}

Vector4 Vector4::operator-=(const Vector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;

	return *this;
}

Vector4 Vector4::operator-=(float f)
{
	x -= f;
	y -= f;
	z -= f;
	w -= f;

	return *this;
}

Vector4 Vector4::operator-=(int i)
{
	x -= i;
	y -= i;
	z -= i;
	w -= i;

	return *this;
}


// *
Vector4 Vector4::operator*(const Vector4& v)	const
{
	Vector4 result;
	result.x = x * v.x;
	result.y = y * v.y;
	result.z = z * v.z;
	result.w = w * v.w;

	return result;
}

Vector4 Vector4::operator*(float f)	const
{
	Vector4 result;
	result.x = x * f;
	result.y = y * f;
	result.z = z * f;
	result.w = w * f;

	return result;
}

Vector4 Vector4::operator*(int i)	const
{
	Vector4 result;
	result.x = x * (float)i;
	result.y = y * (float)i;
	result.z = z * (float)i;
	result.w = w * (float)i;

	return result;
}

Vector4 Vector4::operator*=(const Vector4& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;

	return *this;
}

Vector4 Vector4::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;

	return *this;
}

Vector4 Vector4::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;
	w *= i;

	return *this;
}


// /
Vector4 Vector4::operator/(const Vector4& v)	const
{
	Vector4 result;
	result.x = x / v.x;
	result.y = y / v.y;
	result.z = z / v.z;
	result.w = w / v.w;

	return result;
}

Vector4 Vector4::operator/(float f)	const
{
	Vector4 result;
	result.x = x / f;
	result.y = y / f;
	result.z = z / f;
	result.w = w / f;

	return result;
}

Vector4 Vector4::operator/(int i)	const
{
	Vector4 result;
	result.x = x / (float)i;
	result.y = y / (float)i;
	result.z = z / (float)i;
	result.w = w / (float)i;

	return result;
}

Vector4 Vector4::operator/=(const Vector4& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;

	return *this;
}

Vector4 Vector4::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;

	return *this;
}

Vector4 Vector4::operator/=(int i)
{
	x /= i;
	y /= i;
	z /= i;
	w /= i;

	return *this;
}

// 음.....

float Vector4::Length() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

void Vector4::Normalize()
{
	float	fLength = Length();
	x /= fLength;
	y /= fLength;
	z /= fLength;
	w /= fLength;
}

Vector4 Vector4::Normalize(const Vector4& v)
{
	float	fLength = v.Length();
	Vector4	result = v;

	result.x /= fLength;
	result.y /= fLength;
	result.z /= fLength;
	result.w /= fLength;

	return result;
}

float Vector4::Distance(const Vector4& v) const
{
	Vector4	v4 = *this - v;

	return sqrtf(v4.x * v4.x + v4.y * v4.y + v4.z * v4.z + v4.w * v4.w);
}

float Vector4::Distance(const Vector4& v1, const Vector4& v2)
{
	Vector4	v4 = v1 - v2;

	return sqrtf(v4.x * v4.x + v4.y * v4.y + v4.z * v4.z + v4.w * v4.w);
}

float Vector4::Dot(const Vector4& v) const
{
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

float Vector4::Dot(const Vector4& v1, const Vector4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

/*
y : x
z : y
x : z

수학에서의 외적공식
v1, v2를 외적한다고 가정한다.
y1 * z2 - z1 * y2, z1 * x2 - x1 * z2, x1 * y2 - x2 * y1

z1 * x2 - x1 * z2, x1 * y2 - y1 * x2, y1 * z2 - y2 * z1
*/
Vector4 Vector4::Cross(const Vector4& v) const
{
	//return Vector4(z * v.x - x * v.z, x * v.y - y * v.x, y * v.z - v.y * z);
	return Vector4();
}

Vector4 Vector4::Cross(const Vector4& v1, const Vector4& v2)
{
	//return Vector4(v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x, v1.y * v2.z - v2.y * v1.z);
	return Vector4();
}
