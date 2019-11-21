#ifndef LIBNXCOMMON_NXCOMMON_GL_CAMERA_LUA_H_
#define LIBNXCOMMON_NXCOMMON_GL_CAMERA_LUA_H_

#include "../script/luasysdefs.h"

#ifndef GENERATE_LUAJIT_FFI_CDEF
#include "Camera.h"
#endif



#ifndef GENERATE_LUAJIT_FFI_CDEF

extern "C"
{

using nxcommon::Camera;

#else

typedef struct Camera Camera;

#endif

LUASYS_EXPORT Camera* Camera_Camera();

LUASYS_EXPORT float Camera_getPositionX(Camera* cam);
LUASYS_EXPORT float Camera_getPositionY(Camera* cam);
LUASYS_EXPORT float Camera_getPositionZ(Camera* cam);

LUASYS_EXPORT float Camera_getTargetX(Camera* cam);
LUASYS_EXPORT float Camera_getTargetY(Camera* cam);
LUASYS_EXPORT float Camera_getTargetZ(Camera* cam);

LUASYS_EXPORT float Camera_getUpX(Camera* cam);
LUASYS_EXPORT float Camera_getUpY(Camera* cam);
LUASYS_EXPORT float Camera_getUpZ(Camera* cam);

LUASYS_EXPORT void Camera_setPosition(Camera* cam, float x, float y, float z);

LUASYS_EXPORT void Camera_lookAt(Camera* cam, float tx, float ty, float tz, float ux, float uy, float uz);

LUASYS_EXPORT void Camera_rotateHorizontal(Camera* cam, float a);
LUASYS_EXPORT void Camera_rotateVertical(Camera* cam, float a);

LUASYS_EXPORT void Camera_move(Camera* cam, float l);
LUASYS_EXPORT void Camera_moveSideways(Camera* cam, float l);
LUASYS_EXPORT void Camera_moveUp(Camera* cam, float l);

LUASYS_EXPORT void Camera_setFrustumDistances(Camera* cam, float l, float r, float t, float b, float n, float f);

LUASYS_EXPORT void Camera_getProjectionMatrix(Camera* cam, float prjMat[16]);


#ifndef GENERATE_LUAJIT_FFI_CDEF
}
#endif


#endif /* LIBNXCOMMON_NXCOMMON_GL_CAMERA_LUA_H_ */
