local ffi = require("ffi")

ffi.cdef[[
	typedef struct Mat2 { float m[4]; } Mat2;
	typedef struct Mat3 { float m[9]; } Mat3;
	typedef struct Mat4 { float m[16]; } Mat4;
]]

Mat2 = ffi.typeof("Mat2")
Mat3 = ffi.typeof("Mat3")
Mat4 = ffi.typeof("Mat4")



-- Possible constructors:
--
--		MatX(num[1], num[2], ..., num[X*X])	- Initialize fully with numbers
--		MatX(num)							- Initialize with num along the diagonal and 0 everywhere else
--		MatX(MatY other)					- Initialize upper-left part of the matrix with upper-left part of
--											  another matrix and copy the rest (if any) from the identity matrix
--		MatX(VecX, VecX, VecX, VecX)		- Initialize with column vectors



local _MatXInitFromMatYTable = {
	[4] = {
		[4] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = b.m[2]
			a.m[3] = b.m[3]
			a.m[4] = b.m[4]
			a.m[5] = b.m[5]
			a.m[6] = b.m[6]
			a.m[7] = b.m[7]
			a.m[8] = b.m[8]
			a.m[9] = b.m[9]
			a.m[10] = b.m[10]
			a.m[11] = b.m[11]
			a.m[12] = b.m[12]
			a.m[13] = b.m[13]
			a.m[14] = b.m[14]
			a.m[15] = b.m[15]
		end,
		
		[3] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = b.m[2]
			a.m[3] = 0
			a.m[4] = b.m[3]
			a.m[5] = b.m[4]
			a.m[6] = b.m[5]
			a.m[7] = 0
			a.m[8] = b.m[6]
			a.m[9] = b.m[7]
			a.m[10] = b.m[8]
			a.m[11] = 0
			a.m[12] = 0
			a.m[13] = 0
			a.m[14] = 0
			a.m[15] = 1
		end,
		
		[2] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = 0
			a.m[3] = 0
			a.m[4] = b.m[2]
			a.m[5] = b.m[3]
			a.m[6] = 0
			a.m[7] = 0
			a.m[8] = 0
			a.m[9] = 0
			a.m[10] = 1
			a.m[11] = 0
			a.m[12] = 0
			a.m[13] = 0
			a.m[14] = 0
			a.m[15] = 1
		end
	},
	
	[3] = {
		[4] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = b.m[2]
			a.m[3] = b.m[4]
			a.m[4] = b.m[5]
			a.m[5] = b.m[6]
			a.m[6] = b.m[8]
			a.m[7] = b.m[9]
			a.m[8] = b.m[10]
		end,
		
		[3] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = b.m[2]
			a.m[3] = b.m[3]
			a.m[4] = b.m[4]
			a.m[5] = b.m[5]
			a.m[6] = b.m[6]
			a.m[7] = b.m[7]
			a.m[8] = b.m[8]
		end,
		
		[2] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = 0
			a.m[3] = b.m[2]
			a.m[4] = b.m[3]
			a.m[5] = 0
			a.m[6] = 0
			a.m[7] = 0
			a.m[8] = 1
		end
	},
	
	[2] = {
		[4] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = b.m[4]
			a.m[3] = b.m[5]
		end,
		
		[3] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = b.m[3]
			a.m[3] = b.m[4]
		end,
		
		[2] = function(a, b)
			a.m[0] = b.m[0]
			a.m[1] = b.m[1]
			a.m[2] = b.m[2]
			a.m[3] = b.m[3]
		end
	}
}

function _MatXInitFromMatY(self, other, dim)
	local odim = 0
			
	if ffi.istype(Mat4, other) then odim = 4
	elseif ffi.istype(Mat3, other) then odim = 3
	else odim = 2
	end
	
	local initfunc = _MatXInitFromMatYTable[dim][odim]
	initfunc(self, other)
end






----------------------------------------------------------
--														--
--							Mat2						--
--														--
----------------------------------------------------------


local function _makemat2(m0, m1, m2, m3)
	-- Internal function, used only to create matrices while the constructor isn't set up yet
	
	local m = ffi.new(Mat2)
	m.m[0] = m0
	m.m[1] = m1
	m.m[2] = m2
	m.m[3] = m3
	return m
end


local _Mat2Members = {
	Identity = _makemat2(1, 0, 0, 1),
	Zero = _makemat2(0, 0, 0, 0),
	One = _makemat2(1, 1, 1, 1),
	
	scale = function(x, y)
		if type(x) ~= 'number' then
			-- First parameter is a Vec2
			y = x.y
			x = x.x
		end
		
		return Mat2 (
			x, 0,
			0, y
		)
	end,
	
	rotation = function(a)
		local s = math.sin(a)
		local c = math.cos(a)
		return Mat2 (
			c,	s,
			-s,	c
		)
	end,


	
	copy = function(m)
		return Mat2(m)
	end,

	column = function(m, i)
		i = (i-1) * 2
		return Vec2(m.m[i], m.m[i+1])
	end,
	
	row = function(m, i)
		return Vec2(m.m[i-1], m.m[i+1])
	end,
	
	transpose = function(self)
		self.m[1], self.m[2] = self.m[2], self.m[1]
		return self
	end,
	
	transposed = function(self)
		local cpy = self:copy()
		return cpy:transpose()
	end,
	
	toArray = function(m)
		return {
			m.m[0],		m.m[1],
			m.m[2],		m.m[3]
		}
	end,
	
	transformvec = function(m, v)
		local x = m:row(1) * v
		v.y = m:row(2) * v
		v.x = x
		return v
	end,
	
	determinant = function(m)
		return m.m[0]*m.m[3] - m.m[1]*m.m[2];
	end,
	
	invert = function(m)
		local invdet = 1.0 / determinant()
		m.m[0], m.m[3] = m.m[3] * invdet, m.m[0] * invdet
		m.m[1] = -m.m[1] * invdet;
		m.m[2] = -m.m[2] * invdet;
		return m
	end,
	
	inverse = function(m)
		return m:copy():invert()
	end
}

ffi.metatype(Mat2, {
	__new = function(ct, m0, m1, m2, m3)
		local m = ffi.new(Mat2)
		
		if type(m0) == 'number' then
			if m1 ~= nil then
				-- Full numeric initialization
				m.m[0] = m0
				m.m[1] = m1
				m.m[2] = m2
				m.m[3] = m3
			else
				-- Initialize diagonal with single value, rest zero
				m.m[0] = m0
				m.m[1] = 0
				m.m[2] = 0
				m.m[3] = m0
			end
		elseif ffi.istype(Vec2, m0) then
			-- Initialize from column vectors
			m.m[0] = m0.x
			m.m[1] = m0.y
			m.m[2] = m1.x
			m.m[3] = m1.y
		elseif ffi.istype(Mat4, m0) or ffi.istype(Mat3, m0) or ffi.istype(Mat2, m0) then
			-- Copy upper-left from other matrix
			_MatXInitFromMatY(m, m0, 2)
		end
		
		return m
	end,
	
	__add = function(a, b)
		a.m[0] = a.m[0] + b.m[0]
		a.m[1] = a.m[1] + b.m[1]
		a.m[2] = a.m[2] + b.m[2]
		a.m[3] = a.m[3] + b.m[3]
	end,
	
	__sub = function(a, b)
		a.m[0] = a.m[0] - b.m[0]
		a.m[1] = a.m[1] - b.m[1]
		a.m[2] = a.m[2] - b.m[2]
		a.m[3] = a.m[3] - b.m[3]
	end,
	
	__mul = function(m, v)
		if type(v) == 'number' then
			return Mat2 (
				m.m[0]*v, m.m[1]*v,
				m.m[2]*v, m.m[3]*v
			)
		elseif ffi.istype(Vec2, v) then
			return Vec2 (
				m:row(1) * v,
				m:row(2) * v
			)
		elseif ffi.istype(Mat2, v) then
			return Mat2 (
				m:row(1)*v:column(1),	m:row(2)*v:column(1),
				m:row(1)*v:column(2),	m:row(2)*v:column(2)
			)
		end
	end,
	
	__tostring = function(self)
		return string.format("%-12f %-12f\n%-12f %-12f\n",
			self.m[0], self.m[1], self.m[2], self.m[3])
	end,
	
	__index = _Mat2Members
})






----------------------------------------------------------
--														--
--							Mat3						--
--														--
----------------------------------------------------------

local function _makemat3(m0, m1, m2, m3, m4, m5, m6, m7, m8)
	-- Internal function, used only to create matrices while the constructor isn't set up yet
	
	local m = ffi.new(Mat3)
	m.m[0] = m0
	m.m[1] = m1
	m.m[2] = m2
	m.m[3] = m3
	m.m[4] = m4
	m.m[5] = m5
	m.m[6] = m6
	m.m[7] = m7
	m.m[8] = m8
	return m
end


local _Mat3Members = {
	Identity = _makemat3(1, 0, 0, 0, 1, 0, 0, 0, 1),
	Zero = _makemat3(0, 0, 0, 0, 0, 0, 0, 0, 0),
	One = _makemat3(1, 1, 1, 1, 1, 1, 1, 1, 1),
	
	scale = function(x, y, z)
		if type(x) ~= 'number' then
			-- First parameter is a Vec3
			z = x.z
			y = x.y
			x = x.x
		end
		
		return Mat3 (
			x, 0, 0,
			0, y, 0,
			0, 0, z
		)
	end,
	
	rotationX = function(a)
		local s = math.sin(a);
		local c = math.cos(a);
		return Mat3 (
				1.0,		0.0,		0.0,
				0.0,		c,			s,
				0.0,		-s,			c
		)
	end,
	
	
	rotationY = function(a)
		local s = math.sin(a);
		local c = math.cos(a);
		return Mat3 (
				c,			0.0,		-s,
				0.0,		1.0,		0.0,
				s,			0.0,		c
		)
	end,
	
	
	rotationZ = function(a)
		local s = math.sin(a);
		local c = math.cos(a);
		return Mat3 (
				c,			s,			0.0,
				-s,			c,			0.0,
				0.0,		0.0,		1.0
		)
	end,
	
	rotation = function(angle, axis)
		local naxis = axis:normalized()
		local x = naxis.x;
		local y = naxis.y;
		local z = naxis.z;
		local s = math.sin(angle);
		local c = math.cos(angle);
	
		return Mat3 (
				x*x*(1-c)+c,	x*y*(1-c)+z*s,	x*z*(1-c)-y*s,
				x*y*(1-c)-z*s,	y*y*(1-c)+c,	y*z*(1-c)+x*s,
				x*z*(1-c)+y*s,	y*z*(1-c)-x*s,	z*z*(1-c)+c
		)
	end,


	
	copy = function(m)
		return Mat3(m)
	end,

	column = function(m, i)
		i = (i-1) * 3
		return Vec3(m.m[i], m.m[i+1], m.m[i+2])
	end,
	
	row = function(m, i)
		return Vec3(m.m[i-1], m.m[i+2], m.m[i+5])
	end,
	
	transpose = function(self)
		self.m[1], self.m[3] = self.m[3], self.m[1]
		self.m[2], self.m[6] = self.m[6], self.m[2]
		self.m[5], self.m[7] = self.m[7], self.m[5]
		return self
	end,
	
	transposed = function(self)
		local cpy = self:copy()
		return cpy:transpose()
	end,
	
	toArray = function(m)
		return {
			m.m[0],		m.m[1],		m.m[2],
			m.m[3],		m.m[4],		m.m[5],
			m.m[6],		m.m[7],		m.m[8]
		}
	end,
	
	transformvec = function(m, v)
		local x = m:row(1) * v
		local y = m:row(2) * v
		v.z = m:row(3) * v
		v.y = y
		v.x = x
		return v
	end,
	
	determinant = function(m)
		return  m.m[0] * m.m[4] * m.m[8]
	        +   m.m[1] * m.m[5] * m.m[6]
	        +   m.m[2] * m.m[3] * m.m[7]
	        -   m.m[0] * m.m[5] * m.m[7]
	        -   m.m[2] * m.m[4] * m.m[6]
	        -   m.m[1] * m.m[3] * m.m[8]
	end,
	
	invert = function(m)
		local data = ffi.new(Mat3)
		ffi.copy(data, m, 9*4)
		
		local invdet = 1.0 / m:determinant()
	
        m.m[0] = data.m[4]*data.m[8] - data.m[7]*data.m[5]
        m.m[1] = data.m[7]*data.m[2] - data.m[1]*data.m[8]
        m.m[2] = data.m[1]*data.m[5] - data.m[4]*data.m[2]
        m.m[3] = data.m[6]*data.m[5] - data.m[3]*data.m[8]
        m.m[4] = data.m[0]*data.m[8] - data.m[6]*data.m[2]
        m.m[5] = data.m[3]*data.m[2] - data.m[0]*data.m[5]
        m.m[6] = data.m[3]*data.m[7] - data.m[6]*data.m[4]
        m.m[7] = data.m[6]*data.m[1] - data.m[0]*data.m[7]
        m.m[8] = data.m[0]*data.m[4] - data.m[3]*data.m[1]
        
        m.m[0] = m.m[0] * invdet
        m.m[1] = m.m[1] * invdet
        m.m[2] = m.m[2] * invdet
        m.m[3] = m.m[3] * invdet
        m.m[4] = m.m[4] * invdet
        m.m[5] = m.m[5] * invdet
        m.m[6] = m.m[6] * invdet
        m.m[7] = m.m[7] * invdet
        m.m[8] = m.m[8] * invdet
        
        return m
	end,
	
	inverse = function(m)
		return m:copy():invert()
	end
}

ffi.metatype(Mat3, {
	__new = function(ct, m0, m1, m2, m3, m4, m5, m6, m7, m8)
		local m = ffi.new(Mat3)
		
		if type(m0) == 'number' then
			if m1 ~= nil then
				-- Full numeric initialization
				m.m[0] = m0
				m.m[1] = m1
				m.m[2] = m2
				m.m[3] = m3
				m.m[4] = m4
				m.m[5] = m5
				m.m[6] = m6
				m.m[7] = m7
				m.m[8] = m8
			else
				-- Initialize diagonal with single value, rest zero
				m.m[0] = m0
				m.m[1] = 0
				m.m[2] = 0
				m.m[3] = 0
				m.m[4] = m0
				m.m[5] = 0
				m.m[6] = 0
				m.m[7] = 0
				m.m[8] = m0
			end
		elseif ffi.istype(Vec3, m0) then
			-- Initialize from column vectors
			m.m[0] = m0.x
			m.m[1] = m0.y
			m.m[2] = m0.z
			m.m[3] = m1.x
			m.m[4] = m1.y
			m.m[5] = m1.z
			m.m[6] = m2.x
			m.m[7] = m2.y
			m.m[8] = m2.z
		elseif ffi.istype(Mat4, m0) or ffi.istype(Mat3, m0) or ffi.istype(Mat2, m0) then
			-- Copy upper-left from other matrix
			_MatXInitFromMatY(m, m0, 3)
		end
		
		return m
	end,
	
	__add = function(a, b)
		a.m[0] = a.m[0] + b.m[0]
		a.m[1] = a.m[1] + b.m[1]
		a.m[2] = a.m[2] + b.m[2]
		a.m[3] = a.m[3] + b.m[3]
		a.m[4] = a.m[4] + b.m[4]
		a.m[5] = a.m[5] + b.m[5]
		a.m[6] = a.m[6] + b.m[6]
		a.m[7] = a.m[7] + b.m[7]
		a.m[8] = a.m[8] + b.m[8]
	end,
	
	__sub = function(a, b)
		a.m[0] = a.m[0] - b.m[0]
		a.m[1] = a.m[1] - b.m[1]
		a.m[2] = a.m[2] - b.m[2]
		a.m[3] = a.m[3] - b.m[3]
		a.m[4] = a.m[4] - b.m[4]
		a.m[5] = a.m[5] - b.m[5]
		a.m[6] = a.m[6] - b.m[6]
		a.m[7] = a.m[7] - b.m[7]
		a.m[8] = a.m[8] - b.m[8]
	end,
	
	__mul = function(m, v)
		if type(v) == 'number' then
			return Mat3 (
				m.m[0]*v, m.m[1]*v, m.m[2]*v,
				m.m[3]*v, m.m[4]*v, m.m[5]*v,
				m.m[6]*v, m.m[7]*v, m.m[8]*v
			)
		elseif ffi.istype(Vec3, v) then
			return Vec3 (
				m:row(1) * v,
				m:row(2) * v,
				m:row(3) * v
			)
		elseif ffi.istype(Mat3, v) then
			return Mat3 (
				m:row(1)*v:column(1),	m:row(2)*v:column(1),	m:row(3)*v:column(1),
				m:row(1)*v:column(2),	m:row(2)*v:column(2),	m:row(3)*v:column(2),
				m:row(1)*v:column(3),	m:row(2)*v:column(3),	m:row(3)*v:column(3)
			)
		end
	end,
	
	__tostring = function(self)
		return string.format("%-12f %-12f %-12f\n%-12f %-12f %-12f\n%-12f %-12f %-12f\n",
			self.m[0], self.m[1], self.m[2], self.m[3], self.m[4], self.m[5], self.m[6],
			self.m[7], self.m[8])
	end,
	
	__index = _Mat3Members
})







----------------------------------------------------------
--														--
--							Mat4						--
--														--
----------------------------------------------------------

local function _makemat4(m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15)
	-- Internal function, used only to create matrices while the constructor isn't set up yet
	
	local m = ffi.new(Mat4)
	m.m[0] = m0
	m.m[1] = m1
	m.m[2] = m2
	m.m[3] = m3
	m.m[4] = m4
	m.m[5] = m5
	m.m[6] = m6
	m.m[7] = m7
	m.m[8] = m8
	m.m[9] = m9
	m.m[10] = m10
	m.m[11] = m11
	m.m[12] = m12
	m.m[13] = m13
	m.m[14] = m14
	m.m[15] = m15
	return m
end


local _Mat4Members = {
	Identity = _makemat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1),
	Zero = _makemat4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
	One = _makemat4(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1),

	translation = function(x, y, z)
		if type(x) ~= 'number' then
			-- First parameter is a Vec3
			z = x.z
			y = x.y
			x = x.x
		end
		
		return Mat4 (
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1
		)
	end,
	
	scale = function(x, y, z)
		if type(x) ~= 'number' then
			-- First parameter is a Vec3
			z = x.z
			y = x.y
			x = x.x
		end
		
		return Mat4 (
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		)
	end,
	
	rotationX = function(a)
		local s = math.sin(a);
		local c = math.cos(a);
		return Mat4 (
				1.0,		0.0,		0.0,		0.0,
				0.0,		c,			s,			0.0,
				0.0,		-s,			c,			0.0,
				0.0,		0.0,		0.0,		1.0
		)
	end,
	
	
	rotationY = function(a)
		local s = math.sin(a);
		local c = math.cos(a);
		return Mat4 (
				c,			0.0,		-s,			0.0,
				0.0,		1.0,		0.0,		0.0,
				s,			0.0,		c,			0.0,
				0.0,		0.0,		0.0,		1.0
		)
	end,
	
	
	rotationZ = function(a)
		local s = math.sin(a);
		local c = math.cos(a);
		return Mat4 (
				c,			s,			0.0,		0.0,
				-s,			c,			0.0,		0.0,
				0.0,		0.0,		1.0,		0.0,
				0.0,		0.0,		0.0,		1.0
		)
	end,
	
	rotation = function(angle, axis)
		local naxis = axis:normalized()
		local x = naxis.x;
		local y = naxis.y;
		local z = naxis.z;
		local s = math.sin(angle);
		local c = math.cos(angle);
	
		return Mat4 (
				x*x*(1-c)+c,	x*y*(1-c)+z*s,	x*z*(1-c)-y*s,	0,
				x*y*(1-c)-z*s,	y*y*(1-c)+c,	y*z*(1-c)+x*s,	0,
				x*z*(1-c)+y*s,	y*z*(1-c)-x*s,	z*z*(1-c)+c,	0,
				0,				0,				0,				1
		)
	end,
	
	lookAt = function(target, up)
		local f = target:normalized()
		local upn = up:normalized()
		local s = f:cross(upn)
		local u = s:cross(f)
		
		return Mat4 (
				s.x,	u.x,	-f.x,	0,
				s.y,	u.y,	-f.y,	0,
				s.z,	u.z,	-f.z,	0,
				0,		0,		0,		1
		)
	end,

	
	copy = function(m)
		return Mat4(m)
	end,

	column = function(m, i)
		i = (i-1) * 4
		return Vec4(m.m[i], m.m[i+1], m.m[i+2], m.m[i+3])
	end,
	
	row = function(m, i)
		return Vec4(m.m[i-1], m.m[i+3], m.m[i+7], m.m[i+11])
	end,
	
	transpose = function(self)
		for r = 0, 3 do
			for c = r+1, 3 do
				self.m[r*4 + c], self.m[c*4 + r] = self.m[c*4 + r], self.m[r*4 + c]
			end
		end
		
		return self
	end,
	
	transposed = function(self)
		local cpy = self:copy()
		return cpy:transpose()
	end,
	
	toArray = function(m)
		return {
			m.m[0],		m.m[1],		m.m[2],		m.m[3],
			m.m[4],		m.m[5],		m.m[6],		m.m[7],
			m.m[8],		m.m[9],		m.m[10],	m.m[11],
			m.m[12],	m.m[13],	m.m[14],	m.m[15]
		}
	end,
	
	transformvec = function(m, v)
		local x = m:row(1) * v
		local y = m:row(2) * v
		local z = m:row(3) * v
		v.w = m:row(4) * v
		v.z = z
		v.y = y
		v.x = x
		return v
	end,
	
	determinant = function(m)
        return    m.m[12] * m.m[9] * m.m[6] * m.m[3]
                - m.m[8] * m.m[13] * m.m[6] * m.m[3]
                - m.m[12] * m.m[5] * m.m[10] * m.m[3]
                + m.m[4] * m.m[13] * m.m[10] * m.m[3]
                + m.m[8] * m.m[5] * m.m[14] * m.m[3]
                - m.m[4] * m.m[9] * m.m[14] * m.m[3]
                - m.m[12] * m.m[9] * m.m[2] * m.m[7]
                + m.m[8] * m.m[13] * m.m[2] * m.m[7]
                + m.m[12] * m.m[1] * m.m[10] * m.m[7]
                - m.m[0] * m.m[13] * m.m[10] * m.m[7]
                - m.m[8] * m.m[1] * m.m[14] * m.m[7]
                + m.m[0] * m.m[9] * m.m[14] * m.m[7]
                + m.m[12] * m.m[5] * m.m[2] * m.m[11]
                - m.m[4] * m.m[13] * m.m[2] * m.m[11]
                - m.m[12] * m.m[1] * m.m[6] * m.m[11]
                + m.m[0] * m.m[13] * m.m[6] * m.m[11]
                + m.m[4] * m.m[1] * m.m[14] * m.m[11]
                - m.m[0] * m.m[5] * m.m[14] * m.m[11]
                - m.m[8] * m.m[5] * m.m[2] * m.m[15]
                + m.m[4] * m.m[9] * m.m[2] * m.m[15]
                + m.m[8] * m.m[1] * m.m[6] * m.m[15]
                - m.m[0] * m.m[9] * m.m[6] * m.m[15]
                - m.m[4] * m.m[1] * m.m[10] * m.m[15]
                + m.m[0] * m.m[5] * m.m[10] * m.m[15]
	end,
	
	invert = function(m)
		local data = ffi.new(Mat4)
		ffi.copy(data, m, 16*4)
		
		local invdet = 1.0 / m:determinant()
		
		m.m[0] =      data.m[9]*data.m[14]*data.m[7] - data.m[13]*data.m[10]*data.m[7]
					+ data.m[13]*data.m[6]*data.m[11] - data.m[5]*data.m[14]*data.m[11]
                    - data.m[9]*data.m[6]*data.m[15] + data.m[5]*data.m[10]*data.m[15]
        m.m[4] =      data.m[12]*data.m[10]*data.m[7] - data.m[8]*data.m[14]*data.m[7]
                    - data.m[12]*data.m[6]*data.m[11] + data.m[4]*data.m[14]*data.m[11]
                    + data.m[8]*data.m[6]*data.m[15] - data.m[4]*data.m[10]*data.m[15]
        m.m[8] =      data.m[8]*data.m[13]*data.m[7] - data.m[12]*data.m[9]*data.m[7]
                    + data.m[12]*data.m[5]*data.m[11] - data.m[4]*data.m[13]*data.m[11]
                    - data.m[8]*data.m[5]*data.m[15] + data.m[4]*data.m[9]*data.m[15]
        m.m[12] =     data.m[12]*data.m[9]*data.m[6] - data.m[8]*data.m[13]*data.m[6]
                    - data.m[12]*data.m[5]*data.m[10] + data.m[4]*data.m[13]*data.m[10]
                    + data.m[8]*data.m[5]*data.m[14] - data.m[4]*data.m[9]*data.m[14]
        m.m[1] =      data.m[13]*data.m[10]*data.m[3] - data.m[9]*data.m[14]*data.m[3]
                    - data.m[13]*data.m[2]*data.m[11] + data.m[1]*data.m[14]*data.m[11]
                    + data.m[9]*data.m[2]*data.m[15] - data.m[1]*data.m[10]*data.m[15]
        m.m[5] =      data.m[8]*data.m[14]*data.m[3] - data.m[12]*data.m[10]*data.m[3]
                    + data.m[12]*data.m[2]*data.m[11] - data.m[0]*data.m[14]*data.m[11]
                    - data.m[8]*data.m[2]*data.m[15] + data.m[0]*data.m[10]*data.m[15]
        m.m[9] =      data.m[12]*data.m[9]*data.m[3] - data.m[8]*data.m[13]*data.m[3]
                    - data.m[12]*data.m[1]*data.m[11] + data.m[0]*data.m[13]*data.m[11]
                    + data.m[8]*data.m[1]*data.m[15] - data.m[0]*data.m[9]*data.m[15]
        m.m[13] =     data.m[8]*data.m[13]*data.m[2] - data.m[12]*data.m[9]*data.m[2]
                    + data.m[12]*data.m[1]*data.m[10] - data.m[0]*data.m[13]*data.m[10]
                    - data.m[8]*data.m[1]*data.m[14] + data.m[0]*data.m[9]*data.m[14]
        m.m[2] =      data.m[5]*data.m[14]*data.m[3] - data.m[13]*data.m[6]*data.m[3]
                    + data.m[13]*data.m[2]*data.m[7] - data.m[1]*data.m[14]*data.m[7]
                    - data.m[5]*data.m[2]*data.m[15] + data.m[1]*data.m[6]*data.m[15]
        m.m[6] =      data.m[12]*data.m[6]*data.m[3] - data.m[4]*data.m[14]*data.m[3]
                    - data.m[12]*data.m[2]*data.m[7] + data.m[0]*data.m[14]*data.m[7]
                    + data.m[4]*data.m[2]*data.m[15] - data.m[0]*data.m[6]*data.m[15]
        m.m[10] =     data.m[4]*data.m[13]*data.m[3] - data.m[12]*data.m[5]*data.m[3]
                    + data.m[12]*data.m[1]*data.m[7] - data.m[0]*data.m[13]*data.m[7]
                    - data.m[4]*data.m[1]*data.m[15] + data.m[0]*data.m[5]*data.m[15]
        m.m[14] =     data.m[12]*data.m[5]*data.m[2] - data.m[4]*data.m[13]*data.m[2]
                    - data.m[12]*data.m[1]*data.m[6] + data.m[0]*data.m[13]*data.m[6]
                    + data.m[4]*data.m[1]*data.m[14] - data.m[0]*data.m[5]*data.m[14]
        m.m[3] =      data.m[9]*data.m[6]*data.m[3] - data.m[5]*data.m[10]*data.m[3]
                    - data.m[9]*data.m[2]*data.m[7] + data.m[1]*data.m[10]*data.m[7]
                    + data.m[5]*data.m[2]*data.m[11] - data.m[1]*data.m[6]*data.m[11]
        m.m[7] =      data.m[4]*data.m[10]*data.m[3] - data.m[8]*data.m[6]*data.m[3]
                    + data.m[8]*data.m[2]*data.m[7] - data.m[0]*data.m[10]*data.m[7]
                    - data.m[4]*data.m[2]*data.m[11] + data.m[0]*data.m[6]*data.m[11]
        m.m[11] =     data.m[8]*data.m[5]*data.m[3] - data.m[4]*data.m[9]*data.m[3]
                    - data.m[8]*data.m[1]*data.m[7] + data.m[0]*data.m[9]*data.m[7]
                    + data.m[4]*data.m[1]*data.m[11] - data.m[0]*data.m[5]*data.m[11]
        m.m[15] =     data.m[4]*data.m[9]*data.m[2] - data.m[8]*data.m[5]*data.m[2]
                    + data.m[8]*data.m[1]*data.m[6] - data.m[0]*data.m[9]*data.m[6]
                    - data.m[4]*data.m[1]*data.m[10] + data.m[0]*data.m[5]*data.m[10]
        
        m.m[0] = m.m[0] * invdet
        m.m[1] = m.m[1] * invdet
        m.m[2] = m.m[2] * invdet
        m.m[3] = m.m[3] * invdet
        m.m[4] = m.m[4] * invdet
        m.m[5] = m.m[5] * invdet
        m.m[6] = m.m[6] * invdet
        m.m[7] = m.m[7] * invdet
        m.m[8] = m.m[8] * invdet
        m.m[9] = m.m[9] * invdet
        m.m[10] = m.m[10] * invdet
        m.m[11] = m.m[11] * invdet
        m.m[12] = m.m[12] * invdet
        m.m[13] = m.m[13] * invdet
        m.m[14] = m.m[14] * invdet
        m.m[15] = m.m[15] * invdet
        
        return m
	end,
	
	inverse = function(m)
		return m:copy():invert()
	end
}

ffi.metatype(Mat4, {
	__new = function(ct, m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15)
		local m = ffi.new(Mat4)
		
		if type(m0) == 'number' then
			if m1 ~= nil then
				-- Full numeric initialization
				m.m[0] = m0
				m.m[1] = m1
				m.m[2] = m2
				m.m[3] = m3
				m.m[4] = m4
				m.m[5] = m5
				m.m[6] = m6
				m.m[7] = m7
				m.m[8] = m8
				m.m[9] = m9
				m.m[10] = m10
				m.m[11] = m11
				m.m[12] = m12
				m.m[13] = m13
				m.m[14] = m14
				m.m[15] = m15
			else
				-- Initialize diagonal with single value, rest zero
				m.m[0] = m0
				m.m[1] = 0
				m.m[2] = 0
				m.m[3] = 0
				m.m[4] = 0
				m.m[5] = m0
				m.m[6] = 0
				m.m[7] = 0
				m.m[8] = 0
				m.m[9] = 0
				m.m[10] = m0
				m.m[11] = 0
				m.m[12] = 0
				m.m[13] = 0
				m.m[14] = 0
				m.m[15] = m0
			end
		elseif ffi.istype(Vec4, m0) then
			-- Initialize from column vectors
			m.m[0] = m0.x
			m.m[1] = m0.y
			m.m[2] = m0.z
			m.m[3] = m0.w
			m.m[4] = m1.x
			m.m[5] = m1.y
			m.m[6] = m1.z
			m.m[7] = m1.w
			m.m[8] = m2.x
			m.m[9] = m2.y
			m.m[10] = m2.z
			m.m[11] = m2.w
			m.m[12] = m3.x
			m.m[13] = m3.y
			m.m[14] = m3.z
			m.m[15] = m3.w
		elseif ffi.istype(Mat4, m0) or ffi.istype(Mat3, m0) or ffi.istype(Mat2, m0) then
			-- Copy upper-left from other matrix
			_MatXInitFromMatY(m, m0, 4)
		end
		
		return m
	end,
	
	__add = function(a, b)
		a.m[0] = a.m[0] + b.m[0]
		a.m[1] = a.m[1] + b.m[1]
		a.m[2] = a.m[2] + b.m[2]
		a.m[3] = a.m[3] + b.m[3]
		a.m[4] = a.m[4] + b.m[4]
		a.m[5] = a.m[5] + b.m[5]
		a.m[6] = a.m[6] + b.m[6]
		a.m[7] = a.m[7] + b.m[7]
		a.m[8] = a.m[8] + b.m[8]
		a.m[9] = a.m[9] + b.m[9]
		a.m[10] = a.m[10] + b.m[10]
		a.m[11] = a.m[11] + b.m[11]
		a.m[12] = a.m[12] + b.m[12]
		a.m[13] = a.m[13] + b.m[13]
		a.m[14] = a.m[14] + b.m[14]
		a.m[15] = a.m[15] + b.m[15]
	end,
	
	__sub = function(a, b)
		a.m[0] = a.m[0] - b.m[0]
		a.m[1] = a.m[1] - b.m[1]
		a.m[2] = a.m[2] - b.m[2]
		a.m[3] = a.m[3] - b.m[3]
		a.m[4] = a.m[4] - b.m[4]
		a.m[5] = a.m[5] - b.m[5]
		a.m[6] = a.m[6] - b.m[6]
		a.m[7] = a.m[7] - b.m[7]
		a.m[8] = a.m[8] - b.m[8]
		a.m[9] = a.m[9] - b.m[9]
		a.m[10] = a.m[10] - b.m[10]
		a.m[11] = a.m[11] - b.m[11]
		a.m[12] = a.m[12] - b.m[12]
		a.m[13] = a.m[13] - b.m[13]
		a.m[14] = a.m[14] - b.m[14]
		a.m[15] = a.m[15] - b.m[15]
	end,
	
	__mul = function(m, v)
		if type(v) == 'number' then
			return Mat4 (
				m.m[0]*v, m.m[1]*v, m.m[2]*v, m.m[3]*v,
				m.m[4]*v, m.m[5]*v, m.m[6]*v, m.m[7]*v,
				m.m[8]*v, m.m[9]*v, m.m[10]*v, m.m[11]*v,
				m.m[12]*v, m.m[13]*v, m.m[14]*v, m.m[15]*v
			)
		elseif ffi.istype(Vec4, v) then
			return Vec4 (
				m:row(1) * v,
				m:row(2) * v,
				m:row(3) * v,
				m:row(4) * v
			)
		elseif ffi.istype(Mat4, v) then
			return Mat4 (
				m:row(1)*v:column(1),	m:row(2)*v:column(1),	m:row(3)*v:column(1),	m:row(4)*v:column(1),
				m:row(1)*v:column(2),	m:row(2)*v:column(2),	m:row(3)*v:column(2),	m:row(4)*v:column(2),
				m:row(1)*v:column(3),	m:row(2)*v:column(3),	m:row(3)*v:column(3),	m:row(4)*v:column(3),
				m:row(1)*v:column(4),	m:row(2)*v:column(4),	m:row(3)*v:column(4),	m:row(4)*v:column(4)
			)
		end
	end,
	
	__tostring = function(self)
		return string.format("%-12f %-12f %-12f %-12f\n%-12f %-12f %-12f %-12f\n"
			.. "%-12f %-12f %-12f %-12f\n%-12f %-12f %-12f %-12f\n", self.m[0], self.m[1],
			self.m[2], self.m[3], self.m[4], self.m[5], self.m[6], self.m[7], self.m[8],
			self.m[9], self.m[10], self.m[11], self.m[12], self.m[13], self.m[14], self.m[15])
	end,
	
	__index = _Mat4Members
})

