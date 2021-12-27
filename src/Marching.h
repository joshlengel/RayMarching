#pragma once

#include<functional>

typedef float(*DE)(float, float, float);

size_t MarchRay(float x, float y, float z, float dx, float dy, float dz, DE shape, float &total_dist, float &dist);