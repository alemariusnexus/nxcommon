/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of nxcommon.

	nxcommon is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	nxcommon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef VECTOR4_H_
#define VECTOR4_H_

#include <nxcommon/config.h>
#include <cstring>
#include <cmath>
#include "Vector2.h"
#include "Vector3.h"

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QPoint>
#include <QPointF>
#endif


#ifdef __GNUCBLA__
typedef float v4sf __attribute__((vector_size(16)));
typedef int32_t v4i32 __attribute__((vector_size(16)));

union Vec4SF
{
	v4sf v;
	float f[4];
};

union Vec4I32
{
	v4i32 v;
	int32_t i[4];
};
#else
struct Vec4SF
{
	float f[4];
};

struct Vec4I32
{
	int32_t i[4];
};
#endif


class Vector4
{
public:
	static const Vector4 Zero, One;
	static const Vector4 UnitX, UnitY, UnitZ;
	static const Vector4 NegativeUnitX, NegativeUnitY, NegativeUnitZ;

private:
	friend class Vector2;
	friend class Vector3;

public:
	Vector4() { data.f[0] = 0.0f; data.f[1] = 0.0f; data.f[2] = 0.0f; data.f[3] = 1.0f; }
	Vector4(const Vector4& other) { memcpy(data.f, other.data.f, 4*4); }
	Vector4(const Vector3& other, float w = 1.0f)
			{ data.f[0] = other.data[0]; data.f[1] = other.data[1]; data.f[2] = other.data[2]; data.f[3] = w; }
	Vector4(const Vector2& other, float z = 0.0f, float w = 1.0f)
			{ data.f[0] = other.data[0]; data.f[1] = other.data[1]; data.f[2] = z; data.f[3] = w; }
	Vector4(float* data) { memcpy(this->data.f, data, 4*4); }
	Vector4(float x, float y, float z, float w = 1.0f) { data.f[0] = x; data.f[1] = y; data.f[2] = z; data.f[3] = w; }

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	Vector4(const QPoint& p, float z = 0.0f, float w = 1.0f)
			{ data.f[0] = (float) p.x(); data.f[1] = (float) p.y(); data.f[2] = z; data.f[3] = w; }
	Vector4(const QPointF& p, float z = 0.0f, float w = 1.0f)
			{ data.f[0] = (float) p.x(); data.f[1] = (float) p.y(); data.f[2] = z; data.f[3] = w; }
#endif

	const float* toArray() const { return data.f; }
	float* toArray() { return data.f; }
	float getX() const { return data.f[0]; }
	float getY() const { return data.f[1]; }
	float getZ() const { return data.f[2]; }
	float getW() const { return data.f[3]; }
	void setX(float x) { data.f[0] = x; }
	void setY(float y) { data.f[1] = y; }
	void setZ(float z) { data.f[2] = z; }
	void setW(float w) { data.f[3] = w; }
	float operator[](unsigned int index) const { return data.f[index]; }
	void mirrorYZ() { float y = data.f[1]; data.f[1] = data.f[2]; data.f[2] = y; }
	void scale(float x, float y, float z) { data.f[0] *= x; data.f[1] *= y; data.f[2] *= z; }
	void scale(float f) { data.f[0] *= f; data.f[1] *= f; data.f[2] *= f; }
	Vector4& operator=(const Vector4& rhv);
	Vector4& operator+=(const Vector4& rhv);
	const Vector4 operator+(const Vector4& rhv) const { return Vector4(*this) += rhv; }
	Vector4& operator-=(const Vector4& rhv);
	const Vector4 operator-(const Vector4& rhv) const { return Vector4(*this) -= rhv; }
	Vector4& operator*=(float rhv);
	const Vector4 operator*(float rhv) const { return Vector4(*this) *= rhv; }
	Vector4& operator/=(float rhv) { return *this *= (1.0f/rhv); }
	const Vector4 operator/(float rhv) { return Vector4(*this) /= rhv; }
	const Vector4 operator-() const;
	float dot(const Vector4& rhv) const;
	float lengthSq() const { return data.f[0]*data.f[0] + data.f[1]*data.f[1] + data.f[2]*data.f[2] + data.f[3]*data.f[3]; }
	float length() const { return sqrtf(lengthSq()); }
	float normalize();
	const Vector4 normalized() const { Vector4 cpy(*this); cpy.normalize(); return cpy; }
	Vector4 lerp(const Vector4& other, float t) const { return other*t + (*this)*(1.0f-t); }

private:
	Vec4SF data;
};

#endif /* VECTOR4_H_ */
