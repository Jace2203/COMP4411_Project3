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
    double NdotI = N.dot(I);
    vec3f n(N);

    double etai = 1,
           etat = index;

    double cosi;
    if (NdotI < -1)
        cosi = -1;
    else if (NdotI > 1)
        cosi = 1;
    else
        cosi = NdotI;

    if (cosi < 0)
        cosi *= -1;
    else
    {
        n *= -1;
        double temp = etai;
        etai = etat;
        etat = temp;
    }

    double eta = etai / etat;

    double k = 1 - eta * eta * (1 - cosi * cosi);

    return (k < 0) ? vec3f(0, 0, 0) : eta * I + (eta * cosi - sqrtf(k)) * n;
}