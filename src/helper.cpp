#include "helper.h"

bool RayIn(const vec3f& r, const vec3f& b_min, const vec3f& b_max)
{
    if (r[0] > b_max[0] || r[0] < b_min[0])
        return false;
    if (r[1] > b_max[1] || r[1] < b_min[1])
        return false;
    if (r[2] > b_max[2] || r[2] < b_min[2])
        return false;

    return true;
}