#include <cmath>
#include <limits>

#include "Paraboloid.h"

#define M_PI 3.1415926535897932384

bool Paraboloid::intersectLocal( const ray& r, isect& i ) const
{
    vec3f P = r.getPosition();
    vec3f D = r.getDirection();

    double a2 = m_A * m_A, b2 = m_B * m_B;

    double a = b2 * D[0] * D[0] + a2 * D[2] * D[2];
    double b = 2 * (b2 * P[0] * D[0] + a2 * P[2] * D[2]) - a2 * b2 * D[1];
    double c = b2 * P[0] * P[0] + a2 * P[2] * P[2] - a2 * b2 * P[1];

    double d = b * b - 4 * a * c;
    if (d < 0)
        return false;
    
    double sqrt_d = sqrt(d);
    double t[2] = { (-b - sqrt_d) / (2 * a), (-b + sqrt_d) / (2 * a) };
    if (t[0] > t[1])
    {
        double temp = t[0];
        t[0] = t[1];
        t[1] = temp;
    }
    
    bool has_one = false;
    double tMin = std::numeric_limits<double>::max();
    for (int i = 0; i < 2; i++)
    {
        vec3f pos = r.at(t[i]);
        if (pos[1] < m_ub && pos[1] > m_lb)
        {
            if (t[i] > 0.0 && tMin > t[i])
            {
                tMin = t[i];
                has_one = true;
            }
        }
    }

    if (!has_one)
        return false;

    vec3f p = r.at(tMin);
    i.obj = this;
    i.t = tMin;

	double sqrt_y = sqrt(m_ub);
    double aa = m_A * sqrt_y;
    double bb = m_B * sqrt_y;

    i.u = p[0] / (2 * aa) + 0.5;
    i.v = p[2] / (2 * bb) + 0.5;

    i.N = vec3f(2.0 * p[0] / a2, -1, 2.0 * p[2] / b2).normalize();

    if (abs(i.t - t[1]) < RAY_EPSILON)
        i.N *= -1;

    return true;
}