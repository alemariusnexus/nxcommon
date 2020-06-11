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

#ifndef MATRIX2_H_
#define MATRIX2_H_

#include <nxcommon/config.h>
#include "Vector2.h"
#include <cmath>


/**	\brief Represents a 2x2 square matrix.
 *
 *	This class represents a 2x2 square matrix which can be used for various transforms (rotation, scaling
 *	etc.). For translation you'll need Matrix4.
 *
 *	Internally, the matrix is an array of 2x2 = 4 floats, which is stored in <b>column-major</b> ordering.
 *	This means that the vector (data[0], data[1]) is the first column, and not the first row. This is the
 *	same convention as used by OpenGL, so the matrix float array can be used in OpenGL without transposition.
 */
class Matrix2
{
public:
	static const Matrix2 Identity;

private:
	friend class Matrix3;
	friend class Matrix4;

public:
	/**	\brief Constructs the identity matrix.
	 *
	 *	You should use the pre-built constant Matrix2::Identity to save the time needed for data
	 *	initialization.
	 */
	Matrix2();

	/**	\brief Copy constructor.
	 */
	Matrix2(const Matrix2& other) { memcpy(data, other.data, 4*4); }
	Matrix2(float* data) { memcpy(this->data, data, 4*4); }
	Matrix2(float m00, float m10,
			float m01, float m11);
	const float* toArray() const { return data; }
	float* toArray() { return data; }
	Matrix2& operator+=(const Matrix2& other);
	const Matrix2 operator+(const Matrix2& rhv) const { return Matrix2(*this) += rhv; }
	Matrix2& operator-=(const Matrix2& other);
	const Matrix2 operator-(const Matrix2& rhv) const { return Matrix2(*this) -= rhv; }
	Matrix2& operator*=(float rhv);
	const Matrix2 operator*(float rhv) const { return Matrix2(*this) *= rhv; }
	Matrix2& operator*=(const Matrix2& rhv);
	const Matrix2 operator*(const Matrix2& rhv) const { return Matrix2(*this) *= rhv; }
	const Vector2 operator*(const Vector2& rhv) const;
	Matrix2& operator/=(float rhv) { return *this *= (1.0f/rhv); }
	const Matrix2 operator/(float rhv) { return Matrix2(*this) /= rhv; }
	const Vector2 row(int i) const;
	const Vector2 column(int i) const;
	void transpose();
	float determinant() const;
	void invert();
	const Matrix2 inverse() const { Matrix2 cpy(*this); cpy.invert(); return cpy; }
	void printDebug();

private:
	float data[4];
};


#endif /* MATRIX2_H_ */
