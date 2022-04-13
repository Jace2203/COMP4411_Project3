#ifndef PARABOLOID_H
#define PARABOLOID_H

#include "../scene/scene.h"

class Paraboloid
    : public MaterialSceneObject
{
public:
	Paraboloid( Scene *scene, Material *mat, 
			double a, double b, double lower_bound, double upper_bound)
		: MaterialSceneObject( scene, mat ),
		  m_A(a), m_B(b), m_lb(lower_bound), m_ub(upper_bound)
	{
		if (m_lb > m_ub)
		{
			double temp = m_lb;
			m_lb = m_ub;
			m_lb = temp;
		}
	}
	
	virtual bool intersectLocal( const ray& r, isect& i ) const;
	virtual bool hasBoundingBoxCapability() const { return true; }

    virtual BoundingBox ComputeLocalBoundingBox()
    {
        BoundingBox localbounds;

		double sqrt_y = sqrt(m_ub);

		localbounds.min = vec3f(-m_A * sqrt_y, m_lb, -m_B * sqrt_y);
		localbounds.max = vec3f( m_A * sqrt_y, m_ub,  m_B * sqrt_y);
        return localbounds;
    }


protected:
	double m_A;
	double m_B;

	double m_lb;
	double m_ub;
};

#endif