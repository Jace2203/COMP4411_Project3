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

extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
	ray r( vec3f(0,0,0), vec3f(0,0,0) );
	scene->getCamera()->rayThrough( x,y,r );

	if (!traceUI->getDOF() && !traceUI->getMotion())
		return traceRay( scene, r, vec3f(1.0,1.0,1.0) * traceUI->getThreshhold(), traceUI->getDepth() ).clamp();

	vec3f res(0, 0, 0);

	vec3f dir(r.getDirection());
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
			// double offx = rand() % 11;
			// double offy = rand() % 11;
			double offx = 0.5;
			double offy = 0.5;		

			if (i < a-1)
			{
				for(int y = 0; y < 4; ++y)
					for(int x = 0; x < 4; ++x)
					{
						vec3f offset(vec3f(vec3f(offx * 0.1 + x, offy * 0.1 + y, 0) / 2 - vec3f(1, 1, 0)));
						//vec3f new_eye = eye + prod(offset, (u + v).normalize()) + i * u * 1 / a;
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
		int a = 5;

		vec3f focus_pt = eye + dir * traceUI->getFocalLength();

		for(int i = 0; i < a; ++i)
		{
			srand(time(NULL) + i);

			for(int y = 0; y < 4; ++y)
				for(int x = 0; x < 4; ++x)
				{
					vec3f offset(vec3f(rand() % 11 * 0.1 + x, rand() % 11 * 0.1 + y, 0) / 2);
					vec3f new_eye = eye + prod(offset - vec3f(1, 1, 0), (u + v).normalize()) * traceUI->getApertureSize() * 0.1;
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

		//std::cout << thresh << endl;

		const Material& m = i.getMaterial();

		if (!depth)
			return m.shade(scene, r, i);

		vec3f refl(0, 0, 0);
		vec3f refr(0, 0, 0);

		if (!m.kr.iszero() && m.kr.length() >= thresh.length())
		{
			vec3f reflect(ray::reflect(r.getDirection(), i.N).normalize());

			if (!traceUI->getGlossy())
				refl = traceRay(scene, ray(r.at(i.t) + RAY_EPSILON * reflect, reflect), thresh, depth - 1).clamp();
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
						vec3f new_reflect = reflect + prod(offset - vec3f(1, 1, 0), (u + v).normalize()) * 0.05;
						refl += traceRay(scene, ray(r.at(i.t) + RAY_EPSILON * new_reflect, new_reflect), thresh, depth - 1).clamp();
					}

				refl /= 16;
			}
		}

		if (m.index != 1 && m.kt.length() >= thresh.length())
		{
			vec3f refract(ray::refract(r.getDirection(), i.N, m.index).normalize());
			refr = traceRay(scene, ray(r.at(i.t) + RAY_EPSILON * refract, refract), thresh, depth - 1);
		}
		
		return m.shade(scene, r, i) + prod(m.kr, refl) + prod(m.kt, refr);
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}

RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
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
	// std::cout << sceneLoaded() << endl;

	// std::cout << '1' << endl;

	// std::cout << width << endl;
	// std::cout << height << endl;

	Material *mat = new Material();
	mat->kd = vec3f(1, 0, 0);

 	Trimesh *tmesh = new Trimesh( scene, mat, &scene->transformRoot);

    // tmesh->addVertex( vec3f(0,0,0)); 
    // tmesh->addVertex( vec3f(0,1,0)); 
    // tmesh->addVertex( vec3f(0,1,1)); 
    // tmesh->addVertex( vec3f(0,0,1));
    // tmesh->addVertex( vec3f(1,0,0)); 
    // tmesh->addVertex( vec3f(1,1,0)); 
    // tmesh->addVertex( vec3f(1,1,1)); 
    // tmesh->addVertex( vec3f(1,0,1));
	
    // tmesh->addFace(2,3,7); 
    // tmesh->addFace(1,5,4); 
    // tmesh->addFace(2,1,0);
    // tmesh->addFace(6,7,4); 
    // tmesh->addFace(2,6,5); 
    // tmesh->addFace(3,0,4);

	// Material *mt = new Material();
    // mt->kd = vec3f(1,0,0);
	// tmesh->addMaterial( mt );
	// mt = new Material();
    // mt->kd = vec3f(0,1,0);
	// tmesh->addMaterial( mt );
	// mt = new Material();
    // mt->kd = vec3f(0.3,0.2,0); 
	// tmesh->addMaterial( mt );
	// mt = new Material();
    // mt->kd = vec3f(0,1,0);
	// tmesh->addMaterial( mt );
	// mt = new Material();
    // mt->kd = vec3f(1,0,0);
	// tmesh->addMaterial( mt );
	// mt = new Material();
    // mt->kd = vec3f(1,1,0);
	// tmesh->addMaterial( mt );
	// mt = new Material();
    // mt->kd = vec3f(0.4,0.3,0.3);
	// tmesh->addMaterial( mt );
	// mt = new Material();
    // mt->kd = vec3f(1,0,1);
	// tmesh->addMaterial( mt );
	
	for(int j = 0; j < height; ++j)
		for(int i = 0; i < width; ++i)
		{
			// std::cout << (vec3f( grey[(i + j * width) * 3], grey[(i + j * width) * 3 + 1], grey[(i + j * width) * 3 + 2]) / 255) << endl;
			tmesh->addVertex( vec3f( j, -(vec3f( grey[(i + j * width) * 3], grey[(i + j * width) * 3 + 1], grey[(i + j * width) * 3 + 2]) / 255).length() * 2, i) / 4);
		}
			// tmesh->addVertex( vec3f( j, 0, i) / 4);

	std::cout << '2' << endl;

	for(int j = 0; j < height - 1; ++j)
	{
		for(int i = 0; i < width - 1; ++i)
		{
			tmesh->addFace(i + 1 + (j + 1) * width, i + (j + 1) * width, i + j * width);
			tmesh->addFace(i + 1 + (j + 1) * width, i + j * width, i + 1 + j * width);
		}
		std::cout << j << endl;
	}

	std::cout << '3' << endl;

	for(int j = 0; j < height; ++j)
		for(int i = 0; i < width; ++i)
		{
			Material *mt = new Material();
			mt->kd = vec3f(data[(i + j * width) * 3], data[(i + j * width) * 3 + 1], data[(i + j * width) * 3 + 2]) / 255;
			tmesh->addMaterial( mt );
		}

	std::cout << '4' << endl;

	// scene->add(tmesh);
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

