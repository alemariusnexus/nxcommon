local ffi = require("ffi")

local util = {}


function util.GetTickcount()
    return ffi.C.GetTickcount()
end

function util.GetTickcountMicroseconds()
    return ffi.C.GetTickcountMicroseconds()
end

function util.GetTickcountSeconds()
    return util.GetTickcount() / 1000
end


return util
