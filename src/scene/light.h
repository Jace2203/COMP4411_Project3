#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

	double a = 0;
	double b = 0;
	double c = 0;

protected:
	vec3f position;
};

class SpotLight
	: public Light
{
public:
	SpotLight( Scene *scene, const vec3f& pos, const vec3f& color, const vec3f& dir, const vec3f& edgeplace)
		: Light( scene, color ), position( pos ), direction(dir.normalize()), cutoff( cos(edgeplace[0] * 3.14159265 / 180) ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f position;
	vec3f direction;
	double cutoff;
};

class WarnLight
	: public Light
{
public:
	WarnLight( Scene *scene, const vec3f& pos, const vec3f& color, const vec3f& dir, const vec3f& edgeplace, const vec3f& min_flap, const vec3f& max_flap)
		: Light( scene, color ), position( pos ), direction(dir.normalize()), min_flap(min_flap), max_flap(max_flap), cutoff(cos(edgeplace[0] * 3.1415926535 / 180)) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f position;
	vec3f direction;
	vec3f min_flap;
	vec3f max_flap;
	double cutoff;
};

#endif // __LIGHT_H__