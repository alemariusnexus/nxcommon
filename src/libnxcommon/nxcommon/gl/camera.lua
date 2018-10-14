local ffi = require("ffi")

local camera = {}


local Camera = CreateClass()
camera.Camera = Camera


function Camera:_init()
    self.cam = ffi.C.Camera_Camera()
end

function Camera:getPosition()
    return Vec3(ffi.C.Camera_getPositionX(self.cam), ffi.C.Camera_getPositionY(self.cam), ffi.C.Camera_getPositionZ(self.cam))
end

function Camera:getTarget()
    return Vec3(ffi.C.Camera_getTargetX(self.cam), ffi.C.Camera_getTargetY(self.cam), ffi.C.Camera_getTargetZ(self.cam))
end

function Camera:getUp()
    return Vec3(ffi.C.Camera_getUpX(self.cam), ffi.C.Camera_getUpY(self.cam), ffi.C.Camera_getUpZ(self.cam))
end

function Camera:setPosition(x, y, z)
    if type(x) ~= 'number' then
        x, y, z = x.x, x.y, x.z
    end
    ffi.C.Camera_setPosition(self.cam, x, y, z)
end

function Camera:lookAt(target, up)
    ffi.C.Camera_lookAt(self.cam, target.x, target.y, target.z, up.x, up.y, up.z)
end

function Camera:rotateHorizontal(angle)
    ffi.C.Camera_rotateHorizontal(self.cam, angle)
end

function Camera:rotateVertical(angle)
    ffi.C.Camera_rotateVertical(self.cam, angle)
end

function Camera:move(length)
    ffi.C.Camera_move(self.cam, length)
end

function Camera:moveSideways(length)
    ffi.C.Camera_moveSideways(self.cam, length)
end

function Camera:moveUp(length)
    ffi.C.Camera_moveUp(self.cam, length)
end

function Camera:setFrustumDistances(l, r, t, b, n, f)
    ffi.C.Camera_setFrustumDistances(self.cam, l, r, t, b, n, f)
end

function Camera:getProjectionMatrix()
    local d = ffi.new("float[16]")
    ffi.C.Camera_getProjectionMatrix(self.cam, d)
    return Mat4 (
        d[0], d[1], d[2], d[3],
        d[4], d[5], d[6], d[7],
        d[8], d[9], d[10], d[11],
        d[12], d[13], d[14], d[15]
    )
end

return camera
