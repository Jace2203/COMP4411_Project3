#include "ray.h"
#include "material.h"
#include "scene.h"

const Material &
isect::getMaterial() const
{
    return material ? *material : obj->getMaterial();
}


vec3f ray::refract(const vec3f &I, const vec3f &N, const double index)
{
    double cosi = I.dot(N);
    double ratio = 1 / index;

    vec3f normal = N;

    if (cosi < -1)
        cosi = -1;
    else if (cosi > 1)
        cosi = 1;

    if (cosi < 0)
        cosi = -cosi;
    else
    {
        ratio  = 1/ ratio;
        normal *= -1;
    }
    
    double k = 1 - ratio * ratio * (1 - cosi * cosi); 
    return (k < 0) ? vec3f(0, 0, 0) : (ratio * I + (ratio * cosi - sqrtf(k)) * normal).normalize(); 
}