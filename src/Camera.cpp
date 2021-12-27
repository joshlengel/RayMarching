#include"Camera.h"

#include<cmath>

void Camera::GenerateRay(float tx, float ty, float &dx, float &dy, float &dz) const
{
    float p0 = pitch - 0.5f * fov;
    float y0 = yaw - 0.5f * fov;

    float p = p0 + ty * fov;
    float y = y0 + tx * fov;

    float cp = cos(p);

    dx = sin(y) * cp;
    dy = sin(p);
    dz = cos(y) * cp;
}