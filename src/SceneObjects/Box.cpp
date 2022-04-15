#include <cmath>
#include <assert.h>

#include <limits>

#include "Box.h"

#include "../scene/scene.h"
#include "../helper.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	double tMin, tMax;
	BoundingBox bound = BoundingBox();
    bound.max = vec3f(0.5, 0.5, 0.5);
	bound.min = vec3f(-0.5, -0.5, -0.5);

	if (!bound.intersect(r, tMin, tMax))
		return false;
	
	if (tMin < 0.0)
		i.t = tMax;
	else
		i.t = tMin;
	i.obj = this;
	
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();
	
	double tyMin, tzMin;
	int axis = 0;
	
	if (d[1] > 0.0)
		tyMin = (-0.5 - p[1]) / d[1];
	else
		tyMin = ( 0.5 - p[1]) / d[1];
	if (abs(tyMin - tMin) < RAY_EPSILON)
		axis = 1;

	if (d[2] > 0.0)
		tzMin = (-0.5 - p[2]) / d[2];
	else
		tzMin = ( 0.5 - p[2]) / d[2];
	if (abs(tzMin - tMin) < RAY_EPSILON)
		axis = 2;

	vec3f P = r.at(tMin);

	switch (axis)
	{
	case 0:
		i.N = vec3f((r.getDirection()[0] < 0.0) ? 1.0 : -1.0, 0.0, 0.0);
		if (i.N[0] == -1.0)
		{
			i.u = P[2] + 0.5;
			i.v = P[1] + 0.5;
		}
		else
		{
			i.u = -P[2] + 0.5;
			i.v = P[1] + 0.5;
		}
		break;
	case 1:
		i.N = vec3f(0.0, (r.getDirection()[1] < 0.0) ? 1.0 : -1.0, 0.0);
		if (i.N[1] == -1.0)
		{
			i.u = P[0] + 0.5;
			i.v = P[2] + 0.5;
		}
		else
		{
			i.u = -P[0] + 0.5;
			i.v = -P[2] + 0.5;
		}
		break;
	case 2:
		i.N = vec3f(0.0, 0.0, (r.getDirection()[2] < 0.0) ? 1.0 : -1.0);
		if (i.N[2] == -1.0)
		{
			i.u = P[0] + 0.5;
			i.v = P[1] + 0.5;
		}
		else
		{
			i.u = -P[0] + 0.5;
			i.v = -P[1] + 0.5;
		}
		break;
	}

	if (RayIn(p, bound.min, bound.max))
		i.N *= -1;
		
	return true;
}