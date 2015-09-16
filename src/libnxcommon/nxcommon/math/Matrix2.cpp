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

#include "Matrix2.h"
#include <cstring>
#include <cstdio>

#define GET_RC(r,c) data[(c)*2+(r)]


const Matrix2 Matrix2::Identity = Matrix2();



Matrix2::Matrix2()
{
	data[0] = 1.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 1.0f;
}


Matrix2::Matrix2(	float m00, float m10,
					float m01, float m11
)
{
	data[0] = m00;
	data[1] = m10;
	data[2] = m01;
	data[3] = m11;
}


Matrix2& Matrix2::operator+=(const Matrix2& other)
{
	data[0] += other.data[0];
	data[1] += other.data[1];
	data[2] += other.data[2];
	data[3] += other.data[3];
	return *this;
}


Matrix2& Matrix2::operator-=(const Matrix2& other)
{
	data[0] -= other.data[0];
	data[1] -= other.data[1];
	data[2] -= other.data[2];
	data[3] -= other.data[3];
	return *this;
}


Matrix2& Matrix2::operator*=(float rhv)
{
	data[0] *= rhv;
	data[1] *= rhv;
	data[2] *= rhv;
	data[3] *= rhv;
	return *this;
}


const Vector2 Matrix2::row(int i) const
{
	return Vector2(data[0+i], data[2+i]);
}


const Vector2 Matrix2::column(int i) const
{
	return Vector2(data[0+i*2], data[1+i*2]);
}


Matrix2& Matrix2::operator*=(const Matrix2& rhv)
{
	Vector2 r0 = row(0);
	Vector2 r1 = row(1);
	Vector2 c0 = rhv.column(0);
	Vector2 c1 = rhv.column(1);

	data[0] = r0.dot(c0);
	data[1] = r1.dot(c0);
	data[2] = r0.dot(c1);
	data[3] = r1.dot(c1);
	return *this;
}


const Vector2 Matrix2::operator*(const Vector2& rhv) const
{
	float x = rhv.getX();
	float y = rhv.getY();

	return Vector2 (
			data[0]*x + data[2]*y,
			data[1]*x + data[3]*y
	);
}


void Matrix2::transpose()
{
	float tmp = data[1];
	data[1] = data[2];
	data[2] = tmp;
}


float Matrix2::determinant() const
{
	return data[0]*data[3] - data[1]*data[2];
}


void Matrix2::invert()
{
	float det = determinant();

	float tmp = data[0];
	data[0] = data[3];
	data[1] = -data[1];
	data[2] = -data[2];
	data[3] = tmp;

	*this *= (1.0f / det);
}


void Matrix2::printDebug()
{
	printf("(\t%f\t%f\n\t%f\t%f\n)\n", data[0], data[1], data[2], data[3]);
}
