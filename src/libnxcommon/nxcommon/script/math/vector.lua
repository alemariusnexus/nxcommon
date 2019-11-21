local ffi = require("ffi")


ffi.cdef[[
	typedef struct Vec2 { float x; float y; } Vec2;
	typedef struct Vec3 { float x; float y; float z; } Vec3;
	typedef struct Vec4 { float x; float y; float z; float w; } Vec4;
]]

Vec2 = ffi.typeof("Vec2")
Vec3 = ffi.typeof("Vec3")
Vec4 = ffi.typeof("Vec4")

local _FloatPtrType = ffi.typeof("float*")


function _VecXMakeSwizzleFunc(swzStr, indices)
	local len = #swzStr
	
	if len == 4 then
		local idx1 = indices[1]
		local idx2 = indices[2]
		local idx3 = indices[3]
		local idx4 = indices[4]
		
		return function(a)
			return Vec4(a[idx1], a[idx2], a[idx3], a[idx4])
		end
	elseif len == 3 then
		local idx1 = indices[1]
		local idx2 = indices[2]
		local idx3 = indices[3]
		
		return function(a)
			return Vec3(a[idx1], a[idx2], a[idx3])
		end
	elseif len == 2 then
		local idx1 = indices[1]
		local idx2 = indices[2]
		
		return function(a)
			return Vec2(a[idx1], a[idx2])
		end
	elseif len == 1 then
		local idx1 = indices[1]
		
		return function(a)
			return a[idx1]
		end
	else
		return nil
	end
end

function _VecXBuildSwizzleTableRec(swizzleTable, swizzle, len, n, indices)
	if n == len then
		-- We have a complete combination in indices
		local str = ""
		for _, idx in ipairs(indices) do
			str = str .. string.sub(swizzle, idx+1, idx+1)
		end
		
		swizzleTable[str] = _VecXMakeSwizzleFunc(str, indices)
	else
		for i = 0, #swizzle-1 do
			indices[n+1] = i
			
			_VecXBuildSwizzleTableRec(swizzleTable, swizzle, len, n+1, indices)
		end
	end
end

function _VecXBuildSwizzleTable(swizzles)
	local ncomps = #swizzles[1]
	local swizzleTable = {}
	
	for _, swizzle in ipairs(swizzles) do
		for len = 1, ncomps do
			_VecXBuildSwizzleTableRec(swizzleTable, swizzle, len, 0, {})
		end
	end
	
	return swizzleTable
end




local _Vec2SwizzleTable = _VecXBuildSwizzleTable({"xy", "rg", "st"})
local _Vec3SwizzleTable = _VecXBuildSwizzleTable({"xyz", "rgb", "stp"})
local _Vec4SwizzleTable = _VecXBuildSwizzleTable({"xyzw", "rgba", "stpq"})







----------------------------------------------
--											--
--					Vec4					--
--											--
----------------------------------------------

local function _makevec4(x, y, z, w)
	-- Internal function, used only to create vectors while the constructor isn't set up yet
	
	local v = ffi.new(Vec4)
	v.x = x
	v.y = y
	v.z = z
	v.w = w
	return v
end

local _Vec4Members = {
	UnitX = _makevec4(1, 0, 0, 1),
	UnitY = _makevec4(0, 1, 0, 1),
	UnitZ = _makevec4(0, 0, 1, 1),
	
	NegUnitX = _makevec4(-1, 0, 0, 1),
	NegUnitY = _makevec4(0, -1, 0, 1),
	NegUnitZ = _makevec4(0, 0, -1, 1),
	
	Zero = _makevec4(0, 0, 0, 0),
	One = _makevec4(1, 1, 1, 1),

	copy = function(a)
		return Vec4(a.x, a.y, a.z, a.w)
	end,
	
	normalize = function(a)
		local f = 1.0/a:length()
		a.x = a.x*f
		a.y = a.y*f
		a.z = a.z*f
		a.w = a.w*f
		return a
	end,
	
	normalized = function(a)
		return a:copy():normalize()
	end,
	
	normalize3 = function(a)
		local f = 1.0/a:length3()
		a.x = a.x*f
		a.y = a.y*f
		a.z = a.z*f
		return a
	end,
	
	normalized3 = function(a)
		return a:copy():normalize3()
	end,
	
	components = function(a)
		return a.x, a.y, a.z, a.w
	end,
	
	toArray = function(a)
		return {a.x, a.y, a.z, a.w}
	end,
	
	lengthSq = function(a)
		return a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w
	end,
	
	length = function(a)
		return math.sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w)
	end,
	
	length3Sq = function(a)
		return a.x*a.x + a.y*a.y + a.z*a.z
	end,
	
	length3 = function(a)
		return math.sqrt(a.x*a.x + a.y*a.y + a.z*a.z)
	end,
	
	dot = function(a, b)
		return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w
	end
}

ffi.metatype(Vec4, {
	__new = function(ct, x, y, z, w)
		local v = ffi.new(Vec4)
		v.x = x
		v.y = y
		v.z = z
		v.w = w or 1
		return v
	end,
	__add = function(a, b)
		return Vec4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w)
	end,
	__sub = function(a, b)
		return Vec4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w)
	end,
	__mul = function(a, b)
		if type(b) == 'number' then
			return Vec4(a.x*b, a.y*b, a.z*b, a.w*b)
		elseif type(a) == 'number' then
			return Vec4(a*b.x, a*b.y, a*b.z, a*b.w)
		else
			return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w
		end
	end,
	__div = function(a, b)
		b = 1.0/b
		return Vec4(a.x*b, a.y*b, a.z*b, a.w*b)
	end,
	__unm = function(a)
		return Vec4(-a.x, -a.y, -a.z, -a.w)
	end,
	__len = function(a)
		return math.sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w)
	end,
	__tostring = function(v)
		return "(" .. tostring(v.x) .. ", " .. tostring(v.y) .. ", " .. tostring(v.z) .. ", " .. tostring(v.w) .. ")"
	end,
	
	__index = function(v, k)
		local f = _Vec4Members[k]
		if f == nil then
			local swzf = _Vec4SwizzleTable[k]
			if swzf then return swzf(ffi.cast(_FloatPtrType, v)) end
		end
		return f
	end
})





----------------------------------------------
--											--
--					Vec3					--
--											--
----------------------------------------------

local function _makevec3(x, y, z)
	-- Internal function, used only to create vectors while the constructor isn't set up yet
	
	local v = ffi.new(Vec3)
	v.x = x
	v.y = y
	v.z = z
	return v
end

local _Vec3Members = {
	UnitX = _makevec3(1, 0, 0),
	UnitY = _makevec3(0, 1, 0),
	UnitZ = _makevec3(0, 0, 1),
	
	NegUnitX = _makevec3(-1, 0, 0),
	NegUnitY = _makevec3(0, -1, 0),
	NegUnitZ = _makevec3(0, 0, -1),
	
	Zero = _makevec3(0, 0, 0),
	One = _makevec3(1, 1, 1),

	copy = function(a)
		return Vec3(a.x, a.y, a.z)
	end,
	
	normalize = function(a)
		local f = 1.0/a:length()
		a.x = a.x*f
		a.y = a.y*f
		a.z = a.z*f
		return a
	end,
	
	normalized = function(a)
		return a:copy():normalize()
	end,
	
	components = function(a)
		return a.x, a.y, a.z
	end,
	
	toArray = function(a)
		return {a.x, a.y, a.z}
	end,
	
	lengthSq = function(a)
		return a.x*a.x + a.y*a.y + a.z*a.z
	end,
	
	length = function(a)
		return math.sqrt(a.x*a.x + a.y*a.y + a.z*a.z)
	end,
	
	dot = function(a, b)
		return a.x*b.x + a.y*b.y + a.z*b.z
	end,
	
	cross = function(a, b)
		return Vec3 (
			a.y*b.z - a.z*b.y,
			a.z*b.x - a.x*b.z,
			a.x*b.y - a.y*b.x
		)
	end,
	
	angle = function(a, b)
		return math.acos(a:dot(b) / (a:length()*b:length()))
	end,
	
	rotateThrough = function(a, b, angle)
		local axis = a:cross(b)
		return Mat3.rotation(angle, axis) * a
	end
}

ffi.metatype(Vec3, {
	__call = function(x, y, z)
		local v = ffi.new(Vec3)
		v.x = x
		v.y = y
		v.z = z
		return v
	end,
	__add = function(a, b)
		return Vec3(a.x+b.x, a.y+b.y, a.z+b.z)
	end,
	__sub = function(a, b)
		return Vec3(a.x-b.x, a.y-b.y, a.z-b.z)
	end,
	__mul = function(a, b)
		if type(b) == 'number' then
			return Vec3(a.x*b, a.y*b, a.z*b)
		elseif type(a) == 'number' then
			return Vec3(a*b.x, a*b.y, a*b.z)
		else
			return a.x*b.x + a.y*b.y + a.z*b.z
		end
	end,
	__div = function(a, b)
		b = 1.0/b
		return Vec3(a.x*b, a.y*b, a.z*b)
	end,
	__unm = function(a)
		return Vec3(-a.x, -a.y, -a.z)
	end,
	__len = function(a)
		return math.sqrt(a.x*a.x + a.y*a.y + a.z*a.z)
	end,
	__tostring = function(v)
		return "(" .. tostring(v.x) .. ", " .. tostring(v.y) .. ", " .. tostring(v.z) .. ")"
	end,
	
	__index = function(v, k)
		local f = _Vec3Members[k]
		if f == nil then
			local swzf = _Vec3SwizzleTable[k]
			if swzf then return swzf(ffi.cast(_FloatPtrType, v)) end
		end
		return f
	end
})





----------------------------------------------
--											--
--					Vec2					--
--											--
----------------------------------------------

local function _makevec2(x, y)
	-- Internal function, used only to create vectors while the constructor isn't set up yet
	
	local v = ffi.new(Vec2)
	v.x = x
	v.y = y
	return v
end

local _Vec2Members = {
	UnitX = _makevec2(1, 0),
	UnitY = _makevec2(0, 1),
	
	NegUnitX = _makevec2(-1, 0),
	NegUnitY = _makevec2(0, -1),
	
	Zero = _makevec2(0, 0),
	One = _makevec2(1, 1),

	copy = function(a)
		return Vec2(a.x, a.y)
	end,
	
	fromPolar = function(d, angle)
		return Vec2(d*math.cos(angle), d*math.sin(angle))
	end,
	
	normalize = function(a)
		local f = 1.0/a:length()
		a.x = a.x*f
		a.y = a.y*f
		return a
	end,
	
	normalized = function(a)
		return a:copy():normalize()
	end,
	
	components = function(a)
		return a.x, a.y
	end,
	
	toArray = function(a)
		return {a.x, a.y}
	end,
	
	lengthSq = function(a)
		return a.x*a.x + a.y*a.y
	end,
	
	length = function(a)
		return math.sqrt(a.x*a.x + a.y*a.y)
	end,
	
	dot = function(a, b)
		return a.x*b.x + a.y*b.y
	end
}

ffi.metatype(Vec2, {
	__call = function(x, y)
		local v = ffi.new(Vec2)
		v.x = x
		v.y = y
		return v
	end,
	__add = function(a, b)
		return Vec2(a.x+b.x, a.y+b.y)
	end,
	__sub = function(a, b)
		return Vec2(a.x-b.x, a.y-b.y)
	end,
	__mul = function(a, b)
		if type(b) == 'number' then
			return Vec2(a.x*b, a.y*b)
		elseif type(a) == 'number' then
			return Vec3(a*b.x, a*b.y)
		else
			return a.x*b.x + a.y*b.y
		end
	end,
	__div = function(a, b)
		b = 1.0/b
		return Vec2(a.x*b, a.y*b)
	end,
	__unm = function(a)
		return Vec2(-a.x, -a.y)
	end,
	__len = function(a)
		return math.sqrt(a.x*a.x + a.y*a.y)
	end,
	__tostring = function(v)
		return "(" .. tostring(v.x) .. ", " .. tostring(v.y) .. ")"
	end,
	
	__index = function(v, k)
		local f = _Vec2Members[k]
		if f == nil then
			local swzf = _Vec2SwizzleTable[k]
			if swzf then return swzf(ffi.cast(_FloatPtrType, v)) end
		end
		return f
	end
})

