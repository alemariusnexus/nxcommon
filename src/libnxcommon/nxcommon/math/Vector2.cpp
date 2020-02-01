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

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cmath>
#include <cstdio>


const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::One = Vector2(1.0f, 1.0f);
const Vector2 Vector2::UnitX = Vector2(1.0f, 0.0f);
const Vector2 Vector2::UnitY = Vector2(0.0f, 1.0f);
const Vector2 Vector2::NegativeUnitX = Vector2(-1.0f, 0.0f);
const Vector2 Vector2::NegativeUnitY = Vector2(0.0f, -1.0f);




Vector2::Vector2(const Vector3& other)
{
	data[0] = other.data[0];
	data[1] = other.data[1];
}


Vector2::Vector2(const Vector4& other)
{
	data[0] = other.data.f[0];
	data[1] = other.data.f[1];
}


Vector2& Vector2::operator=(const Vector2& rhv)
{
	if (this != &rhv) {
		data[0] = rhv.data[0];
		data[1] = rhv.data[1];
	}
	return *this;
}


Vector2& Vector2::operator+=(const Vector2& rhv)
{
	data[0] += rhv.data[0];
	data[1] += rhv.data[1];
	return *this;
}


Vector2& Vector2::operator-=(const Vector2& rhv)
{
	data[0] -= rhv.data[0];
	data[1] -= rhv.data[1];
	return *this;
}


Vector2& Vector2::operator*=(float rhv)
{
	data[0] *= rhv;
	data[1] *= rhv;
	return *this;
}


const Vector2 Vector2::operator-() const
{
	return Vector2(-data[0], -data[1]);
}


float Vector2::dot(const Vector2& rhv) const
{
	return data[0]*rhv.data[0] + data[1]*rhv.data[1];
}


float Vector2::normalize()
{
	float len = length();
	float invlen = 1.0f / len;

	data[0] *= invlen;
	data[1] *= invlen;

	return len;
}


float Vector2::angle(const Vector2& other) const
{
	float d = dot(other);
	d /= length()*other.length();

	if (d > 1.0f)
		d = 1.0f;
	else if (d < -1.0f)
		d = -1.0f;

	return acosf(d);
}


float Vector2::angleXY(const Vector2& other) const
{
	// See: https://stackoverflow.com/a/16544330
	float dotv = dot(other);
	float detv = data[0]*other.data[1] - data[1]*other.data[0];
	float a = atan2f(detv, dotv);
	return a - 2.0f*M_PI * floorf(a / (2.0f*M_PI)); // Wrap to range [0, 2*M_PI]
}
