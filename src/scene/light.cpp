#include <cmath>

#include "light.h"

#include "vector"

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
		return i.getMaterial().kt;

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

	// double a_factor = traceUI->getConstant();
	// double b_factor = traceUI->getLinear();
	// double c_factor = traceUI->getQuadric();

	double a_factor = 1;
	double b_factor = 1;
	double c_factor = 1;

	return min(1.0, 1 / (a * a_factor + b * b_factor * d + c * c_factor * d * d));
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

	double occlusion = 1;

	if (scene->intersect(shadow, i))
		occlusion = (shadow.at(i.t) - P).length() / (position - P).length();

	if (occlusion == 1)
		return vec3f(1, 1, 1);

	return i.getMaterial().kt; 
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
	ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));
	isect i;

	// if (scene->intersect(shadow, i))
	// 	return i.getMaterial().kt;

    return vec3f(1, 1, 1);
}