#include "ray.h"
#include "material.h"
#include "light.h"

#include "../RayTracer.h"

extern RayTracer* theRayTracer;

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

	// emissive
	vec3f result = ke + prod(ka, scene->ambient_Light);

	// diffuse
	vec3f diffuse;
	if (map == nullptr)
	{
		diffuse = kd;
	}
	else if (traceUI->getNoiseTexture())
	{
		int size = theRayTracer->getNoiseSize();
		double u = i.u * size, v = i.v * size;
		int u0 = floor(u), u1 = ceil(u), v0 = floor(v), v1 = ceil(v);

		unsigned char* noise = theRayTracer->getNoiseTexture();
		int A = noise[v0 * size + u0];
		int B = noise[v0 * size + u1];
		int C = noise[v1 * size + u0];
		int D = noise[v1 * size + u1];

		double AB = A + (u - u0) * (B - A);
		double CD = C + (u - u0) * (D - C);

		double d = AB + (v - v0) * (CD - AB);
		d /= 255.0;
		diffuse = vec3f(d, d, d);
	}
	else
	{
		double u = i.u * width, v = i.v * height;
		int u0 = min(floor(u), width - 1), u1 = min(ceil(u), width - 1);
		int v0 = min(floor(v), height - 1), v1 = min(ceil(v), height - 1);

		int a = (v0 * width + u0) * 3;
		int b = (v0 * width + u1) * 3;
		int c = (v1 * width + u0) * 3;
		int d = (v1 * width + u1) * 3;

		unsigned char* aa = &map[a];
		unsigned char* bb = &map[b];
		unsigned char* cc = &map[c];
		unsigned char* dd = &map[d];

		vec3f A(aa[0] / 255.0, aa[1] / 255.0, aa[2] / 255.0);
		vec3f B(bb[0] / 255.0, bb[1] / 255.0, bb[2] / 255.0);
		vec3f C(cc[0] / 255.0, cc[1] / 255.0, cc[2] / 255.0);
		vec3f D(dd[0] / 255.0, dd[1] / 255.0, dd[2] / 255.0);

		vec3f AB = A + (u - u0) * (B - A);
		vec3f CD = C + (u - u0) * (D - C);

		diffuse = AB + (v - v0) * (CD - AB);
	}

	// ambient
	for (auto& it = scene->beginLights(); it != scene->endLights(); it++)
	{
		vec3f I((*it)->getColor(P));
		vec3f L((*it)->getDirection(P));
		vec3f R(ray::reflect(L, i.N).normalize());

		vec3f atten((*it)->shadowAttenuation(P) * (*it)->distanceAttenuation(P));

		// diffuse
		vec3f d = diffuse * max(i.N.normalize() * L.normalize(), 0.0);

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
