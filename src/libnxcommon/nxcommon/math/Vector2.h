/*
	Copyright 2010-2015 David "Alemarius Nexus" Lerch

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

#ifndef VECTOR2_H_
#define VECTOR2_H_

#include <nxcommon/config.h>
#include <cstring>
#include <cmath>

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QtCore/QPoint>
#include <QtCore/QPointF>
#endif


class Vector3;
class Vector4;


class Vector2
{
public:
	static const Vector2 Zero, One;
	static const Vector2 UnitX, UnitY;
	static const Vector2 NegativeUnitX, NegativeUnitY;

private:
	friend class Vector3;
	friend class Vector4;

public:
	Vector2() { data[0] = 0.0f; data[1] = 0.0f; }
	Vector2(const Vector2& other) { data[0] = other.data[0]; data[1] = other.data[1]; }
	Vector2(const Vector3& other);
	Vector2(const Vector4& other);
	Vector2(float* data) { this->data[0] = data[0]; this->data[1] = data[1]; }
	Vector2(float x, float y) { data[0] = x; data[1] = y; }

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	Vector2(const QPoint& p) { data[0] = (float) p.x(); data[1] = (float) p.y(); }
	Vector2(const QPointF& p) { data[0] = (float) p.x(); data[1] = (float) p.y(); }
#endif

	const float* toArray() const { return data; }
	float* toArray() { return data; }
	float getX() const { return data[0]; }
	float getY() const { return data[1]; }
	void setX(float x) { data[0] = x; }
	void setY(float y) { data[1] = y; }
	float operator[](unsigned int index) const { return data[index]; }
	void scale(float x, float y) { data[0] *= x; data[1] *= y; }
	void scale(float f) { data[0] *= f; data[1] *= f; }
	Vector2& operator=(const Vector2& rhv);
	Vector2& operator+=(const Vector2& rhv);
	const Vector2 operator+(const Vector2& rhv) const { return Vector2(*this) += rhv; }
	Vector2& operator-=(const Vector2& rhv);
	const Vector2 operator-(const Vector2& rhv) const { return Vector2(*this) -= rhv; }
	Vector2& operator*=(float rhv);
	const Vector2 operator*(float rhv) const { return Vector2(*this) *= rhv; }
	float operator*(const Vector2& rhv) const { return dot(rhv); }
	Vector2& operator/=(float rhv) { return *this *= (1.0f/rhv); }
	const Vector2 operator/(float rhv) { return Vector2(*this) /= rhv; }
	const Vector2 operator-() const;
	float dot(const Vector2& rhv) const;
	float lengthSq() const { return data[0]*data[0] + data[1]*data[1]; }
	float length() const { return sqrtf(lengthSq()); }
	float normalize();
	const Vector2 normalized() const { Vector2 cpy(*this); cpy.normalize(); return cpy; }
	float angle(const Vector2& other) const;
	float angleXY(const Vector2& other) const;
	float angleYX(const Vector2& other) const { return 2.0f*M_PI - angleXY(other); }
	Vector2 lerp(const Vector2& other, float t) const { return other*t + (*this)*(1.0f-t); }

private:
	float data[2];
};

#endif /* VECTOR2_H_ */
