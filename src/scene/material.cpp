#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	vec3f V = -r.getDirection().normalize();
	vec3f P = r.at(i.t);
	// emissive

	vec3f result = ke;

	// ambient
	for (auto& it = scene->beginLights(); it != scene->endLights(); it++)
	{
		vec3f I = (*it)->getColor(P);
		vec3f L = (*it)->getDirection(P);
		vec3f R = (L - 2 * (L * i.N) * i.N).normalize();
		double n_dot = i.N.dot(L);

		if (n_dot <= 0.0)
			continue;

		// diffuse
		vec3f temp = kd * n_dot;
		// specular
		temp += ks * pow(max(V * R, 0.0), 15);
		temp *= (*it)->distanceAttenuation(P);
		// ambient
		temp += ka;

		result += prod(temp, I);
	}

	return result;
}
