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

    if (cosi < -1)
        cosi = -1;
    else if (cosi > 1)
        cosi = 1;

    double etai = 1, etat = index; 
    vec3f n = N; 
    if (cosi < 0)
        cosi = -cosi;
    else
    {
        std::swap(etai, etat);
        n= -N;
    } 
    
    double eta = etai / etat; 
    double k = 1 - eta * eta * (1 - cosi * cosi); 
    return (k < 0) ? vec3f(0, 0, 0) : (eta * I + (eta * cosi - sqrtf(k)) * n).normalize(); 
}

// vec3f ray::refract(const vec3f &I, const vec3f &N, const double index)
// {
//     double ratio = 1 / index;



//     if (pow(N.dot(I), 2) <= 1 - pow(1 / ratio, 2))
//         return vec3f(0, 0, 0);

//     return (ratio * ((sqrt(pow(N.dot(I), 2) + pow((1 / ratio), 2) - 1) - N.dot(I)) * N + I)).normalize();
// }
