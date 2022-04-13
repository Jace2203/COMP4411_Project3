#ifndef TORUS_H
#define TORUS_H

#include "../scene/scene.h"

class Torus
    : public MaterialSceneObject
{
public:
    Torus( Scene *scene, Material *mat,
            double R, double r)
        : MaterialSceneObject( scene, mat), m_R(R), m_r(r)
    {
    }

	virtual bool intersectLocal( const ray& r, isect& i ) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
    
    virtual BoundingBox ComputeLocalBoundingBox() const
    {
        BoundingBox localbounds;
		localbounds.min = vec3f(-(m_R + m_r), -m_r, -(m_R + m_r));
		localbounds.max = vec3f(m_R + m_r, m_r, m_R + m_r);
        return localbounds;
    }

protected:
    double m_R;
    double m_r;
};

#endif