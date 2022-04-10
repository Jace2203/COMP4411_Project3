#include "ray.h"
#include "material.h"
#include "light.h"

#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

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
	vec3f N = i.N;

	// emissive
	vec3f result = ke + prod(ka, scene->ambient_Light);

	// diffuse
	vec3f diffuse;
	if (map == nullptr)
	{
		diffuse = kd;
	}
	else
	{
		int v = min(int(round(i.v * height)), height - 1), u = min(int(round(i.u * width)), width - 1);
		int a = (v * width + u) * 3;
		unsigned char* d = &map[a];
		diffuse = vec3f(d[0]/255.0, d[1]/255.0, d[2]/255.0);
		if (traceUI->getBumpMapping())
		{
			int uu = min(u + 1, width - 1), vv = min(v + 1, height - 1);
			unsigned char* r_map = &map[(v * width + uu) * 3];
			unsigned char* g_map = &map[(vv * width + u) * 3];
			vec3f NN = (N + vec3f(0.0, 1.0, 0.0)).normalize();
			if (abs((NN - N).length_squared()) < NORMAL_EPSILON)
			{
				NN = (N + vec3f(1.0, 0.0, 0.0)).normalize();
			}

			vec3f U = NN.cross(N).normalize() * 50.0;
			vec3f V = N.cross(U).normalize() * 50.0;
			double dr = (d[0] - r_map[0]) / 255.0, dg = (d[1] - g_map[1]) / 255.0;
			N = (N + U * dr + V * dg).normalize();
		}
	}

	// ambient
	for (auto& it = scene->beginLights(); it != scene->endLights(); it++)
	{
		vec3f I((*it)->getColor(P));
		vec3f L((*it)->getDirection(P));
		vec3f R(ray::reflect(L, N).normalize());

		vec3f atten((*it)->shadowAttenuation(P) * (*it)->distanceAttenuation(P));

		// diffuse
		vec3f d = diffuse * max(N.normalize() * L.normalize(), 0.0);

		// specular
		vec3f specular(ks * pow(max(R.dot(V), 0.0), shininess * 128));

		/*
`		//reflect
		for(int time = 0; time < traceUI-)
		vec3f reflect(ks * )
		*/

		result += prod(prod(d + specular, atten), I);
	}

	return result.clamp();
}
