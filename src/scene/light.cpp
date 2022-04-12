#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}

vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.

	ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));
	isect i;

	if (scene->intersect(shadow, i))
		return vec3f(0, 0 ,0);

    return vec3f(1, 1, 1);
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation.normalize();
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE
	
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double d = (position - P).length();

	return min(1.0, 1 / (a + b * d + c * d * d));
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}

vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
	ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));
	isect i;

	if (scene->intersect(shadow, i))
		return vec3f(0, 0 ,0);

    return vec3f(1, 1, 1);
}

double SpotLight::distanceAttenuation( const vec3f& P ) const
{
	if (direction.dot(-getDirection(P)) < cutoff)
		return 0.0;

	return 1.0;
}

vec3f SpotLight::getColor( const vec3f& P ) const
{
	return color;
}

vec3f SpotLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}

vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
    return vec3f(1,1,1);
}