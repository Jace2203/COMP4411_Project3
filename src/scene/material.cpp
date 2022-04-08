#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	vec3f N(i.N.normalize());
	vec3f L(r.getPosition() - r.at(i.t).normalize());

	float lambertian = max(N.dot(L), 0.0);
	float specular;

	if (lambertian > 0.0)
	{
		vec3f R(ray::reflect(-L, N));      // Reflected light vector
		vec3f V((-r.at(i.t)).normalize()); // Vector to viewer

		// Compute the specular term
		float specAngle = max(R.dot(V), 0.0);
		specular = pow(specAngle, shininess);
	}

	vec3f color(0, 0, 0);

	for(list<Light*>::const_iterator i = scene->beginLights(); i != scene->endLights(); ++i)
		color += ((*i)->getColor(vec3f(0, 0, 0)) + lambertian * kd + specular * ks);

	return color;

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
}
