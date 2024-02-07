#include "Vector3.h"

Vector3::Vector3() :
	x(0.f),
	y(0.f),
	z(0.f)
{
}

Vector3::Vector3(const Vector3& vec)
{
	*this = vec;
	//x = vec.x;
	//y = vec.y;
	//z = vec.z;
}

Vector3::Vector3(float _x, float _y, float _z) :
	x(_x),
	y(_y),
	z(_z)
{
}

Vector3& Vector3::operator=(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}

Vector3 Vector3::operator+(const Vector3& v)	const
{
	Vector3 result;
	result.x = x + v.x;
	result.y = y + v.y;
	result.z = z + v.z;

	return result;
}

Vector3 Vector3::operator+(float f)	const
{
	Vector3 result;
	result.x = x + f;
	result.y = y + f;
	result.z = z + f;

	return result;
}

Vector3 Vector3::operator+(int i)	const
{
	Vector3 result;
	result.x = x + (float)i;
	result.y = y + (float)i;
	result.z = z + (float)i;

	return result;
}

Vector3 Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vector3 Vector3::operator+=(float f)
{
	x += f;
	y += f;
	z += f;

	return *this;
}

Vector3 Vector3::operator+=(int i)
{
	x += i;
	y += i;
	z += i;

	return *this;
}


// -
Vector3 Vector3::operator-(const Vector3& v)	const
{
	Vector3 result;
	result.x = x - v.x;
	result.y = y - v.y;
	result.z = z - v.z;

	return result;
}
 
Vector3 Vector3::operator-(float f)	const
{
	Vector3 result;
	result.x = x - f;
	result.y = y - f;
	result.z = z - f;

	return result;
}

Vector3 Vector3::operator-(int i)	const
{
	Vector3 result;
	result.x = x - (float)i;
	result.y = y - (float)i;
	result.z = z - (float)i;

	return result;
}

Vector3 Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

Vector3 Vector3::operator-=(float f)
{
	x -= f;
	y -= f;
	z -= f;

	return *this;
}

Vector3 Vector3::operator-=(int i)
{
	x -= i;
	y -= i;
	z -= i;

	return *this;
}


// *
Vector3 Vector3::operator*(const Vector3& v)	const
{
	Vector3 result;
	result.x = x * v.x;
	result.y = y * v.y;
	result.z = z * v.z;

	return result;
}

Vector3 Vector3::operator*(float f)	const
{
	Vector3 result;
	result.x = x * f;
	result.y = y * f;
	result.z = z * f;

	return result;
}

Vector3 Vector3::operator*(int i)	const
{
	Vector3 result;
	result.x = x * (float)i;
	result.y = y * (float)i;
	result.z = z * (float)i;

	return result;
}

Vector3 Vector3::operator*=(const Vector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

Vector3 Vector3::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

Vector3 Vector3::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;

	return *this;
}


// /
Vector3 Vector3::operator/(const Vector3& v)	const
{
	Vector3 result;
	result.x = x / v.x;
	result.y = y / v.y;
	result.z = z / v.z;

	return result;
}

Vector3 Vector3::operator/(float f)	const
{
	Vector3 result;
	result.x = x / f;
	result.y = y / f;
	result.z = z / f;

	return result;
}

Vector3 Vector3::operator/(int i)	const
{
	Vector3 result;
	result.x = x / (float)i;
	result.y = y / (float)i;
	result.z = z / (float)i;

	return result;
}

Vector3 Vector3::operator/=(const Vector3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

Vector3 Vector3::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;

	return *this;
}

Vector3 Vector3::operator/=(int i)
{
	x /= i;
	y /= i;
	z /= i;

	return *this;
}

float Vector3::Length() const
{
	if ((x * x + y * y + z * z) == 0.f)
		return 0.f;

	return sqrtf(x * x + y * y + z * z);
}

void Vector3::Normalize()
{
	float	fLength = Length();

	if (fLength == 0.f)
	{
		x = y = z = 0.f;
	}

	else
	{
		x /= fLength;
		y /= fLength;
		z /= fLength;
	}
}

Vector3 Vector3::Normalize(const Vector3& v)
{
	float	fLength = v.Length();
	Vector3	result = v;

	if (fLength == 0.f)
	{
		result.x = result.y = result.z = 0.f;
	}

	else
	{
		result.x /= fLength;
		result.y /= fLength;
		result.z /= fLength;
	}

	return result;
}

float Vector3::Distance(const Vector3& v) const
{
	Vector3	v3 = *this - v;

	if ((v3.x * v3.x + v3.y * v3.y + v3.z * v3.z) == 0.f)
		return 0.f;

	return sqrtf(v3.x * v3.x + v3.y * v3.y + v3.z * v3.z);
}

float Vector3::Distance(const Vector3& v1, const Vector3& v2)
{
	Vector3	v3 = v1 - v2;

	if ((v3.x * v3.x + v3.y * v3.y + v3.z * v3.z) == 0.f)
		return 0.f;

	return sqrtf(v3.x * v3.x + v3.y * v3.y + v3.z * v3.z);
}

float Vector3::Dot(const Vector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
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
Vector3 Vector3::Cross(const Vector3& v) const
{
	return Vector3(z * v.x - x * v.z, x * v.y - y * v.x, y * v.z - v.y * z);
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x, v1.y * v2.z - v2.y * v1.z);
}
