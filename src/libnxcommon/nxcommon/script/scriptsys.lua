--[[
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
]]

function CreateClass(base)
    local cls = {}
    
    -- Do this FIRST, because this does copy not only methods and fields, but also things like _basetable and
    -- metamethods, which we want to override later on.
    if base then
        for name, method in pairs(base) do
            cls[name] = method
        end
    end
    
    cls.__index = cls
    
    if base then
    	cls._basetable = {[base] = true}
    	
    	for baseBase, _ in pairs(base._basetable) do
    		cls._basetable[baseBase] = true
    	end
    else
    	cls._basetable = {}
    end
    
    setmetatable(cls, {
        __call = function(c, ...)
            local inst = setmetatable({}, c)
            rawset(inst, "_props", {})
            local init = inst._init
            
            if init then
                init(inst, ...)
            end
            
            return inst
        end
    })
    
    function cls:__index(key)
    	local getterName = "_get" .. string.upper(string.sub(key, 1, 1)) ..string.sub(key, 2)
    	local getter = cls[getterName]
    	
    	if getter ~= nil then
    		return getter(self)
    	else
    		local val = self._props[key]
    		
    		if val ~= nil then
    			return val
    		end
    	end
    	
    	return cls[key]
    end
    
    
    function cls:__newindex(key, value)
    	local setterName = "_set" .. string.upper(string.sub(key, 1, 1)) ..string.sub(key, 2)
    	local setter = cls[setterName]
    	
    	if setter ~= nil then
    		setter(self, value)
    	else
    		self._props[key] = value
    	end
    end
    
    return cls
end


function instanceof(self, othercls)
	if type(self) ~= "table" then
		return false
	end
	
	local cls = getmetatable(self)
    return cls  and  (othercls == cls  or  cls._basetable[othercls] ~= nil)
end



