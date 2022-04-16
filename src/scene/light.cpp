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

	double a_factor = traceUI->getConstant();
	double b_factor = traceUI->getLinear();
	double c_factor = traceUI->getQuadric();

	return min(1.0, 1.0 / (a * a_factor + b * b_factor * d + c * 1000 * c_factor * d * d));
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
	isect i;

	if (traceUI->getSoftShadow())
	{
		vec3f u(getDirection(P).cross(vec3f(1, 0, 0)).normalize());
		if (u.iszero()) u = (getDirection(P).cross(vec3f(0, 1, 0)).normalize());
		vec3f v(getDirection(P).cross(u).normalize());

		vec3f res(1, 1, 1);

		for(int y = 0; y < 4; ++y)
			for(int x = 0; x < 4; ++x)
			{
				vec3f offset(vec3f(rand() % 11 * 0.1 + x, rand() % 11 * 0.1 + y, 0) / 2);
				vec3f pt = position + prod(offset - vec3f(1, 1, 0), (u + v).normalize()) * traceUI->getApertureSize() * 0.1;
				ray shadow(P + RAY_EPSILON * (pt - P).normalize(), (pt - P).normalize());
				if (scene->intersect(shadow, i))
					res -= res / 16;
			}
		
		return res;
	}
	else
	{
		ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));

		if (scene->intersect(shadow, i))
			return i.getMaterial().kt;;

		return vec3f(1, 1, 1);
	}
}

double SpotLight::distanceAttenuation( const vec3f& P ) const
{
	if (direction.dot(-getDirection(P)) < cutoff)
		return 0.0;

	return pow( direction.dot(-getDirection(P)), 2);
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

	if (scene->intersect(shadow, i))
		return i.getMaterial().kt;;

    return vec3f(1, 1, 1);
}

double WarnLight::distanceAttenuation( const vec3f& P ) const
{
	if (shape == 0)
	{
		if (flap_0[0] <= P[0] && P[0] <= flap_1[0])
			if (flap_0[1] <= P[1] && P[1] <= flap_1[1])
				if (flap_0[2] <= P[2] && P[2] <= flap_1[2])
					if (direction.dot(-getDirection(P)) >= cutoff)
					{
						std::cout << '1' << endl;
						return pow( direction.dot(-getDirection(P)), 8);
					}
	}
	else if (shape == 1)
	{
		double delta_x = flap_1[0] - flap_0[0];
		double delta_z = flap_0[2] - flap_1[2];
		double m = delta_x / delta_z;
		vec3f p( flap_1[2], flap_0[0], 0);
		vec3f q( flap_0[2], flap_1[0], 0);
		double intercept = p[1] / m + p[0];

		if (flap_0[0] <= P[0] && P[0] <= flap_1[0])
			if (flap_0[1] <= P[1] && P[1] <= flap_1[1])
				if (flap_0[2] <= P[2] && P[2] <= flap_1[2])
					if (direction.dot(-getDirection(P)) >= cutoff)
						if (P[0] - p[0] <= m * (P[2] - p[2]))
							return pow( direction.dot(-getDirection(P)), 8);
	}
	else if (shape == 2)
	{
		vec3f c( vec3f( flap_0 + flap_1 ) / 2 );

		if (flap_0[1] <= P[1] && P[1] <= flap_1[1]) // trim made by this boundury
			if (direction.dot(-getDirection(P)) >= cutoff)
				if ( prod(c - flap_0, vec3f(1, 0, 1)).length() >= prod(c - P, vec3f(1, 0, 1)).length() )
					return pow( direction.dot(-getDirection(P)), 8);
	}

	return 0.0;
}

vec3f WarnLight::getColor( const vec3f& P ) const
{
	return color;
}

vec3f WarnLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}

vec3f WarnLight::shadowAttenuation(const vec3f& P) const
{
	ray shadow(P + RAY_EPSILON * this->getDirection(P), this->getDirection(P));
	isect i;

	if (scene->intersect(shadow, i))
		return i.getMaterial().kt;;

    return vec3f(1, 1, 1);
}