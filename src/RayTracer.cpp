// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "SceneObjects/trimesh.h"

#include "ui/TraceUI.h"

#include "PerlinNoise.h"

extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
	this->x = x;
	this->y = y;
	ray r( vec3f(0,0,0), vec3f(0,0,0) );
	scene->getCamera()->rayThrough( x,y,r );

	if (!traceUI->getDOF() && !traceUI->getMotion())
		return traceRay( scene, r, vec3f(1.0,1.0,1.0) * traceUI->getThreshhold(), traceUI->getDepth() ).clamp();

	vec3f res(0, 0, 0);

	vec3f dir(r.getDirection().normalize());
	vec3f u = dir.cross(vec3f(1, 0, 0));
	if (u.iszero()) u = dir.cross(vec3f(0, 1, 0));
	vec3f v = dir.cross(u);

	vec3f eye(r.getPosition());

	if (traceUI->getMotion())
	{
		int a = 10;

		for(int i = 0; i < a; ++i)
		{
			srand(time(NULL) + i);
			double offx = 0.5;
			double offy = 0.5;		

			if (i < a-1)
			{
				for(int y = 0; y < 4; ++y)
					for(int x = 0; x < 4; ++x)
					{
						vec3f offset(vec3f(vec3f(offx * 0.1 + x, offy * 0.1 + y, 0) / 2 - vec3f(1, 1, 0)));
						vec3f new_eye = eye + prod(offset, (u + v).normalize()) * 0.07 + (-u - v) * i * 0.7 / a;
						r = ray( new_eye, dir );
						res += traceRay( scene, r, vec3f(1.0,1.0,1.0) * traceUI->getThreshhold(), traceUI->getDepth() ).clamp();
					}
			}
			else
			{
				vec3f new_eye = eye + (-u - v) * i * 0.7 / a;
				r = ray( eye, dir );
				res += traceRay( scene, r, vec3f(1.0,1.0,1.0) * traceUI->getThreshhold(), traceUI->getDepth() ).clamp();
			}
		}

		return res / 16 / a;
	}

	if (traceUI->getDOF())
	{
		int a = 10;

		vec3f focus_pt = eye + dir * traceUI->getFocalLength();

		for(int i = 0; i < a; ++i)
		{
			srand(time(NULL) + i);

			for(int y = 0; y < 4; ++y)
				for(int x = 0; x < 4; ++x)
				{
					vec3f offset(vec3f(rand() % 11 * 0.1 + x, rand() % 11 * 0.1 + y, 0) / 2);
					vec3f new_eye = eye + prod(offset - vec3f(1, 1, 0), (u + v).normalize()) * traceUI->getApertureSize() * 0.135;
					r = ray( new_eye, (focus_pt - new_eye).normalize() );
					res += traceRay( scene, r, vec3f(1.0,1.0,1.0) * traceUI->getThreshhold(), traceUI->getDepth() ).clamp();
				}
		}

		return res / 16 / a;
	}
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();

		vec3f shade = prod( (vec3f(1, 1, 1) - m.kt), m.shade(scene, r, i) ).clamp();

		if (!depth)
			return shade;

		vec3f refl(0, 0, 0);
		vec3f refr(0, 0, 0);

		if (!m.kr.iszero() && m.kr.length() >= thresh.length())
		{
			vec3f reflect(ray::reflect(r.getDirection(), i.N).normalize());

			vec3f new_thresh = thresh;
			if (!thresh.iszero())
			{
				vec3f kr( (m.kr[0]) ? m.kr[0] : 1, (m.kr[1]) ? m.kr[1] : 1, (m.kr[2]) ? m.kr[2] : 1 );
				new_thresh = prod( thresh, vec3f(1 / kr[0], 1/kr[1], 1/kr[2]) );
			}

			if (!traceUI->getGlossy())
				refl = traceRay(scene, ray(r.at(i.t) + RAY_EPSILON * reflect, reflect), new_thresh, depth - 1).clamp();
			else
			{
				vec3f u(reflect.cross(vec3f(1, 0, 0)).normalize());
				if (u.iszero()) u = reflect.cross(vec3f(0, 1, 0)).normalize();
				vec3f v(reflect.cross(u).normalize());

				for(int y = 0; y < 4; ++y)
					for(int x = 0; x < 4; ++x)
					{
						srand(time(NULL) + x + y);
						vec3f offset(vec3f(rand() % 11 * 0.1 + x, rand() % 11 * 0.1 + y, 0) / 2);
						vec3f new_reflect = reflect + prod(offset - vec3f(1, 1, 0), (u + v).normalize()) * 0.06;
						refl += traceRay(scene, ray(r.at(i.t) + RAY_EPSILON * new_reflect, new_reflect), new_thresh, depth - 1).clamp();
					}
				refl /= 16;
			}
		}

		if (!m.kt.iszero() && m.kt.length() >= thresh.length())
		{
			vec3f new_thresh = thresh;
			if (!thresh.iszero())
			{
				vec3f kt( (m.kt[0]) ? m.kt[0] : 1, (m.kt[1]) ? m.kt[1] : 1, (m.kt[2]) ? m.kt[2] : 1 );
				new_thresh = prod( thresh, vec3f(1 / kt[0], 1/kt[1], 1/kt[2]) );
			}
			vec3f refract(ray::refract(r.getDirection(), i.N, m.index).normalize());
			refr = traceRay(scene, ray(r.at(i.t) + RAY_EPSILON * refract, refract), new_thresh, depth - 1).clamp();
		}	
		
		return shade + prod(m.kr, refl) + prod(m.kt, refr);	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		if (bgloaded)
		{

			int i = x  * bgwidth;
			int j = y  * bgheight;

			// std::cout << i << ' ' << j << endl;
			// std::cout << buffer_width << ' ' << buffer_height << endl;
			// std::cout << bgwidth << ' ' << bgwidth << endl;
			//std::cout << i << ' ' << j << endl;

			return vec3f( background[(i + j * bgwidth) * 3] / 255.0, background[(i + j * bgwidth) * 3 + 1] / 255.0, background[(i + j * bgwidth) * 3 + 2] / 255.0 );
		}
		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;

	m_nNoiseSize = 0;
	m_dNoiseTexture = nullptr;
}

RayTracer::~RayTracer()
{
	bgloaded = 0;
	delete [] buffer;
	delete scene;
	delete [] m_dNoiseTexture;
	delete []background;
	background = nullptr;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::loadHField(unsigned char *data, unsigned char *grey, int width, int height)
{
	Material *mat = new Material();
	mat->kd = vec3f(1, 0, 0);

 	Trimesh *tmesh = new Trimesh( scene, mat, &scene->transformRoot);
	
	for(int j = 0; j < height; ++j)
		for(int i = 0; i < width; ++i)
			tmesh->addVertex( vec3f( j, -(vec3f( grey[(i + j * width) * 3], grey[(i + j * width) * 3 + 1], grey[(i + j * width) * 3 + 2]) / 255).length() * width / 8, i) / width * 4);

	for(int j = 0; j < height - 1; ++j)
	{
		for(int i = 0; i < width - 1; ++i)
		{
			tmesh->addFace(i + 1 + (j + 1) * width, i + (j + 1) * width, i + j * width);
			tmesh->addFace(i + 1 + (j + 1) * width, i + j * width, i + 1 + j * width);
		}
	}

	for(int j = 0; j < height; ++j)
		for(int i = 0; i < width; ++i)
		{
			Material *mt = new Material();
			mt->kd = vec3f(data[(i + j * width) * 3], data[(i + j * width) * 3 + 1], data[(i + j * width) * 3 + 2]) / 255;
			tmesh->addMaterial( mt );
		}

	scene->initScene();
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

void RayTracer::setNoiseTexture()
{
	if (m_dNoiseTexture)
		delete[] m_dNoiseTexture;

	int size = pow(2, traceUI->getNoiseInput());
	int scale = traceUI->getNoiseScale();
	m_nNoiseSize = size * scale;
	m_dNoiseTexture = PerlinNoise::GenerateNoise2D(size, 1, scale);
}

unsigned char* RayTracer::getNoiseTexture()
{
	return m_dNoiseTexture;
}

int RayTracer::getNoiseSize()
{
	return m_nNoiseSize;
}

vec3f RayTracer::AdaptSampling(double x, double y, int depth)
{
	int size = pow(2, depth) + 1;
	vec3f*** cache = new vec3f**[size];
	for (int i = 0; i < size; i++)
	{
		cache[i] = new vec3f*[size];
		for (int j = 0; j < size; j++)
		{
			cache[i][j] = nullptr;
		}
	}

	vec3f result = AdaptSampling(x, y, depth, 0, size - 1, 0, size - 1, 1, cache);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			delete cache[i][j];
		}
		delete[] cache[i];
	}
	delete[] cache;

	return result;
}

vec3f RayTracer::AdaptSampling(double x, double y, int depth, int a, int b, int c, int d, int division, vec3f*** cache)
{
	vec3f result = vec3f(0.0, 0.0, 0.0);
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
	scene->getCamera()->rayThrough( x,y,r );
	if (cache[(b - a) / 2][(d - c) / 2] == nullptr)
		cache[(b - a) / 2][(d - c) / 2] = new vec3f(traceRay( scene, r, vec3f(1.0, 1.0, 1.0), traceUI->getDepth()));

	double sub_size = 1.0 / division;
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			double xx = x + (i * sub_size - sub_size / 2) / buffer_width;
			double yy = y + (j * sub_size - sub_size / 2) / buffer_height;

			if (cache[(i ? b : a)][(j ? d : c)] == nullptr)
			{
    			scene->getCamera()->rayThrough( xx,yy,r );
				cache[(i ? b : a)][(j ? d : c)] = new vec3f(traceRay(scene, r, vec3f(1.0, 1.0, 1.0), traceUI->getDepth()));
			}

			vec3f diff = *cache[(i ? b : a)][(j ? d : c)] - *cache[(b - a) / 2][(d - c) / 2];
			if (diff.length_squared() > 0.001 && depth > 1)
				result += AdaptSampling((xx + x)/2, (yy + y)/2, depth - 1, 
				(i ? (a + b)/2 : a), (i ? b : (a + b)/2),
				(j ? (c + d)/2 : c), (j ? d : (c + d)/2),
				division * 2, cache);
			else
				result += *cache[(i ? b : a)][(j ? d : c)];
		}
	}
	result /= 4;

	return result;
}

