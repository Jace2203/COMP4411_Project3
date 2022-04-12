#include <cmath>

#include "Torus.h"

#include <limits>
#include "../vendor/Quartic/quartic.h"

#define M_PI 3.14159265358979323846

bool Torus::intersectLocal( const ray& r, isect& i ) const
{
    BoundingBox box = ComputeLocalBoundingBox();
    isect j;
    double tMin, tMax;
    if (!box.intersect(r, tMin, tMax))
        return false;

    vec3f P = r.getPosition(), D = r.getDirection();

    double t = std::numeric_limits<double>::max();
    double b, c, d, e, sigma;
    sigma = m_r * m_r + m_R * m_R;

    b = 4 * P.dot(D);
    c = 2 * (P.length_squared() - sigma) + 4 * pow(P.dot(D), 2) + 4 * m_R * m_R * D[1] * D[1];
    d = 4 * (P.length_squared() - sigma) * P.dot(D) + 8 * m_R * m_R * P[1] * D[1];
    e = pow(P.length_squared() - sigma, 2) - 4 * m_R * m_R * (m_r * m_r - P[1] * P[1]);

    DComplex* solutions = solve_quartic(b, c, d, e);
    bool has_one = false;
    for (int i = 0; i < 4; i++)
    {
        if (abs(solutions[i].imag() - 0.0) < RAY_EPSILON)
        {
            if (solutions[i].real() < t)
            {
                has_one = true;
                t = solutions[i].real();
            }
        }
    }

    delete[] solutions;

    if (!has_one)
        return false;

    i.obj = this;
    vec3f p = r.at(t);
    vec3f normal = vec3f(4.0 * p[0] * p.length_squared() - sigma,
                         4.0 * p[1] * p.length_squared() - sigma,
                         4.0 * p[2] * p.length_squared() - sigma + 8 * m_R * m_R * p[2]);

    i.N = normal.normalize();

    return true;
}
