#pragma once

class Camera
{
public:
    float x, y, z;
    float pitch, yaw;
    float fov;

    void GenerateRay(float tx, float ty, float &dx, float &dy, float &dz) const;
};