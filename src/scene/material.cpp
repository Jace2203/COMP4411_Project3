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

	vec3f V = r.getDirection().normalize();
	vec3f P = r.at(i.t);
	vec3f diffuse, specular;

	vec3f result = ke + prod(ka, scene->ambient_Light) * traceUI->getAmbient();

	double shadow_buffer = new double[traceUI->getSize() * (int)(width / pUI->raytracer->aspectRatio() + 0.5)] = {};

	for (auto& it = scene->beginLights(); it != scene->endLights(); it++)
	{

	}

	for (auto& it = scene->beginLights(); it != scene->endLights(); it++)
	{
		vec3f I((*it)->getColor(P));
		vec3f L((*it)->getDirection(P));
		vec3f R(ray::reflect(L, i.N).normalize());

		vec3f atten((*it)->shadowAttenuation(P) * (*it)->distanceAttenuation(P));

		// diffuse
		diffuse = (kd * max(i.N.normalize() * L.normalize(), 0.0));

		// specular
		specular = (ks * pow(max(R.dot(V), 0.0), shininess * 128));

		result += prod(prod(diffuse + specular, atten), I);
	}

	return result.clamp();
}
