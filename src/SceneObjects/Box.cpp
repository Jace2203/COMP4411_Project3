#include <cmath>
#include <assert.h>

#include <limits>

#include "Box.h"

#include "../scene/scene.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.

	// Smit's method
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	double tNear, tFar, tyNear, tyFar, tzNear, tzFar;
	int axis = 0;

	// X
	if (d[0] >= 0) {
		tNear = (-0.5 - p[0]) / d[0];
		tFar =  ( 0.5 - p[0]) / d[0];
	}
	else
	{
		tNear = ( 0.5 - p[0]) / d[0];
		tFar =  (-0.5 - p[0]) / d[0];
	}

	// Y
	if (d[1] >= 0)
	{
		tyNear = (-0.5 - p[1]) / d[1];
		tyFar =  ( 0.5 - p[1]) / d[1];
	}
	else
	{
		tyNear = ( 0.5 - p[1]) / d[1];
		tyFar =  (-0.5 - p[1]) / d[1];
	}

	if ((tNear > tyFar) || (tyNear > tFar))
		return false;
	if (tyNear > tNear)
	{
		tNear = tyNear;
		axis = 1;
	}
	if (tyFar < tFar)
		tFar = tyFar;

	// Z
	if (d[2] >= 0)
	{
		tzNear = (-0.5 - p[2]) / d[2];
		tzFar =  ( 0.5 - p[2]) / d[2];
	}
	else
	{
		tzNear = ( 0.5 - p[2]) / d[2];
		tzFar =  (-0.5 - p[2]) / d[2];
	}
	
	if ((tNear > tzFar) || (tzNear > tFar))
		return false;
	if (tzNear > tNear)
	{
		tNear = tzNear;
		axis = 2;
	}
	if (tzFar < tFar)
		tFar = tzFar;

	i.obj = this;
	i.t = tNear;

	switch (axis)
	{
	case 0: i.N = vec3f(((p[0] < 0.0) ? -1.0f : 1.0f), 0.0f, 0.0f); break;
	case 1: i.N = vec3f(0.0f, ((p[1] < 0.0) ? -1.0f : 1.0f), 0.0f); break;
	case 2: i.N = vec3f(0.0f, 0.0f, ((p[2] < 0.0) ? -1.0f : 1.0f)); break;
	}

	return true;
}
