#include <cmath>

#include "Sphere.h"

#define M_PI            3.1415926535897932384

bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	vec3f v = -r.getPosition();
	double b = v.dot(r.getDirection());
	double discriminant = b*b - v.dot(v) + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	double t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;

	double t1 = b - discriminant;

	vec3f P;
	if( t1 > RAY_EPSILON ) {
		i.t = t1;
		P = r.at(t1);
		i.N = P.normalize();
	} else {
		i.t = t2;
		P = r.at(t2);
		i.N = P.normalize();
	}
	double alpha = atan2(P[2], P[0]) + M_PI;
	double beta = asin(P[1]/1.0) + M_PI / 2;

	i.u = alpha / (2 * M_PI);
	i.v = beta / M_PI;
	if (v.length_squared() < 3.0)
		i.N *= -1;

	return true;
}

