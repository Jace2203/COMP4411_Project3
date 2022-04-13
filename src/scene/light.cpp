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
	/*
	double radius = 0.05;

	ray shadow[7] = {
		ray(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P)),
		ray(P + RAY_EPSILON * this->getDirection(P) + vec3f( 1,  0,  0) * radius, this->getDirection(P)),
		ray(P + RAY_EPSILON * this->getDirection(P) + vec3f(-1,  0,  0) * radius, this->getDirection(P)),
		ray(P + RAY_EPSILON * this->getDirection(P) + vec3f( 0,  1,  0) * radius, this->getDirection(P)),
		ray(P + RAY_EPSILON * this->getDirection(P) + vec3f( 0, -1,  0) * radius, this->getDirection(P)),
		ray(P + RAY_EPSILON * this->getDirection(P) + vec3f( 0,  0,  1) * radius, this->getDirection(P)),
		ray(P + RAY_EPSILON * this->getDirection(P) + vec3f( 0,  0, -1) * radius, this->getDirection(P))
	};
	*/

	/*
	double lower = -1,
		   upper = 1,
		   interval = 0.5;

	int part = 0;

	if (traceUI->getSoftShadow())
	{
		std::vector<ray> lights;
		for(double x = lower; x <= upper; x += interval)
			for(double y = lower; y <= upper; y += interval)
				for(double z = lower; z <= upper; z += interval)
				{
					// std::cout << -getDirection(P).dot(vec3f(x, y, z) - position) << endl;
					if (getDirection(P).dot((vec3f(x, y, z)).normalize()) <= 0.4)
					{
						// std::cout << 'y' << endl;
						++part;
						vec3f pt(position + vec3f(x, y, z).normalize() * 0.13);
						ray shadows(P + RAY_EPSILON * (pt - P).normalize(), (pt - P).normalize());
						lights.push_back(shadows);
					}
				}


		// ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));
		isect i;

		vec3f res(1, 1, 1);

		// std::cout << lights.size() << endl;

		for(ray shadow : lights)
			if (rand() % 100 <= 70 &&  scene->intersect(shadow, i))
				res -= res / part;

		return res;
	}
	else
	{
		ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));
		isect i;

		if (scene->intersect(shadow, i))
			return i.getMaterial().kt;

		return vec3f(1, 1, 1);
	}
	*/

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
	//std::cout << (position - P).normalize() << endl;
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

double WarnLight::distanceAttenuation( const vec3f& P ) const
{
	// std::cout << P << endl;

	if (min_flap[0] <= P[0] && P[0] <= max_flap[0])
		if (min_flap[1] <= P[1] && P[1] <= max_flap[1])
			if (min_flap[2] <= P[2] && P[2] <= max_flap[2])
				if (direction.dot(-getDirection(P)) >= cutoff)
					return 1.0;

	return 0.0;
}

vec3f WarnLight::getColor( const vec3f& P ) const
{
	return color;
}

vec3f WarnLight::getDirection( const vec3f& P ) const
{
	//std::cout << (position - P).normalize() << endl;
	return (position - P).normalize();
}

vec3f WarnLight::shadowAttenuation(const vec3f& P) const
{
	ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));
	isect i;

	if (scene->intersect(shadow, i))
		return i.getMaterial().kt;

    return vec3f(1, 1, 1);
}