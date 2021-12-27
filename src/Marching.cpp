#include"Marching.h"

#include<cmath>

size_t MarchRay(float x, float y, float z, float dx, float dy, float dz, DE shape, float &total_dist, float &dist)
{
    float min_threshold = 0.01f;
    float max_threshold = 100.0f;
    total_dist = 0.0f;
    size_t num_steps = 0;

    while (total_dist < max_threshold)
    {
        float rx = x + dx * total_dist;
        float ry = y + dy * total_dist;
        float rz = z + dz * total_dist;
        dist = shape(rx, ry, rz);

        if (dist < min_threshold) return num_steps;

        total_dist += dist;

        ++num_steps;
    }

    return -1;
}