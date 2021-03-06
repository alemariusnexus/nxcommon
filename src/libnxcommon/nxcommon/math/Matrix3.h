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

#ifndef MATRIX3_H_
#define MATRIX3_H_

#include <nxcommon/config.h>
#include "Vector3.h"
#include "Matrix2.h"
#include <cmath>


/**	\brief Represents a 3x3 square matrix.
 *
 *	This class represents a 3x3 square matrix which can be used for various transforms (rotation, scaling
 *	etc.). For translation you'll need Matrix4.
 *
 *	Internally, the matrix is an array of 3x3 = 9 floats, which is stored in <b>column-major</b> ordering.
 *	This means that the vector (data[0], data[1], data[2]) is the first column, and not the first
 *	row. This is the same convention as used by OpenGL, so the matrix float array can be used in OpenGL
 *	without transposition.
 */
class Matrix3
{
public:
	static const Matrix3 Identity;

public:
	static Matrix3 fromEulerYZX(float x, float y, float z);
	static Matrix3 translation(float x, float y);
	static Matrix3 rotation(float a);
	static Matrix3 scale(const Vector3& s);
	static Matrix3 scale(float x, float y, float z) { return scale(Vector3(x, y, z)); }

private:
	friend class Matrix2;
	friend class Matrix4;

public:
	/**	\brief Constructs the identity matrix.
	 *
	 *	You should use the pre-built constant Matrix3::Identity to save the time needed for data
	 *	initialization.
	 */
	Matrix3();

	/**	\brief Copy constructor.
	 */
	Matrix3(const Matrix3& other);
	Matrix3(const Matrix2& other);
	Matrix3(float* data) { memcpy(this->data, data, 9*4); }
	Matrix3(float m00, float m10, float m20,
			float m01, float m11, float m21,
			float m02, float m12, float m22);
	Matrix3(const Vector3& c0, const Vector3& c1, const Vector3& c2);

	const float* toArray() const { return data; }
	float* toArray() { return data; }
	Matrix3& operator+=(const Matrix3& other);
	const Matrix3 operator+(const Matrix3& rhv) const { return Matrix3(*this) += rhv; }
	Matrix3& operator-=(const Matrix3& other);
	const Matrix3 operator-(const Matrix3& rhv) const { return Matrix3(*this) -= rhv; }
	Matrix3& operator*=(float rhv);
	const Matrix3 operator*(float rhv) const { return Matrix3(*this) *= rhv; }
	Matrix3& operator*=(const Matrix3& rhv);
	const Matrix3 operator*(const Matrix3& rhv) const { return Matrix3(*this) *= rhv; }
	const Vector3 operator*(const Vector3& rhv) const;
	Matrix3& operator/=(float rhv) { return *this *= (1.0f/rhv); }
	const Matrix3 operator/(float rhv) { return Matrix3(*this) /= rhv; }
	const Vector3 row(int i) const;
	const Vector3 column(int i) const;
	void transpose();
	float determinant() const;
	void invert();
	const Matrix3 inverse() const { Matrix3 cpy(*this); cpy.invert(); return cpy; }
	void printDebug();

private:
	float data[9];
};


#endif /* MATRIX3_H_ */
