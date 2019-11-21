#include "Camera_lua.h"



extern "C"
{

using nxcommon::Camera;

Camera* Camera_Camera() { return new Camera(); }

float Camera_getPositionX(Camera* cam) { return cam->getPosition().getX(); }
float Camera_getPositionY(Camera* cam) { return cam->getPosition().getY(); }
float Camera_getPositionZ(Camera* cam) { return cam->getPosition().getZ(); }

float Camera_getTargetX(Camera* cam) { return cam->getTarget().getX(); }
float Camera_getTargetY(Camera* cam) { return cam->getTarget().getY(); }
float Camera_getTargetZ(Camera* cam) { return cam->getTarget().getZ(); }

float Camera_getUpX(Camera* cam) { return cam->getUp().getX(); }
float Camera_getUpY(Camera* cam) { return cam->getUp().getY(); }
float Camera_getUpZ(Camera* cam) { return cam->getUp().getZ(); }

void Camera_setPosition(Camera* cam, float x, float y, float z) { cam->setPosition(x, y, z); }

void Camera_lookAt(Camera* cam, float tx, float ty, float tz, float ux, float uy, float uz)
		{ cam->lookAt(Vector3(tx, ty, tz), Vector3(ux, uy, uz)); }

void Camera_rotateHorizontal(Camera* cam, float a) { cam->rotateHorizontal(a); }
void Camera_rotateVertical(Camera* cam, float a) { cam->rotateVertical(a); }

void Camera_move(Camera* cam, float l) { cam->move(l); }
void Camera_moveSideways(Camera* cam, float l) { cam->moveSideways(l); }
void Camera_moveUp(Camera* cam, float l) { cam->moveUp(l); }

void Camera_setFrustumDistances(Camera* cam, float l, float r, float t, float b, float n, float f)
		{ cam->getFrustum().setDistances(l, r, t, b, n, f); }

void Camera_getProjectionMatrix(Camera* cam, float prjMat[16])
		{ Matrix4 pm = cam->getFrustum().getProjectionMatrix(); memcpy(prjMat, pm.toArray(), 16*sizeof(float)); }

}
