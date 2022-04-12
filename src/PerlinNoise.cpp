#include "PerlinNoise.h"
#include <random>
#include <time.h>

#include "vecmath/vecmath.h"

#define M_PI 3.1415926535897932384

PerlinNoise* PerlinNoise::instance = nullptr;

PerlinNoise::PerlinNoise()
{
    srand(time(NULL));
}

PerlinNoise::~PerlinNoise()
{
}

PerlinNoise* PerlinNoise::GetInstance()
{
    if (instance == nullptr)
        instance = new PerlinNoise();

    return instance;
}

double* PerlinNoise::GenerateNoise2D(int size, int octave, int scale)
{
    scale += 1;
    vec3f* grad = new vec3f[scale * scale];
    for (int i = 0; i < scale * scale; i++)
    {
        double theta = double(rand()) / RAND_MAX * M_PI * 2;
        grad[i] = vec3f(cos(theta) * 1, sin(theta) * 1, 0.0);
    }

    double* result = new double[size * size];

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            double xx = (double)x / size * scale;
            double yy = (double)y / size * scale;

            int x0 = floor(xx);
            int x1 = ceil(xx);
            int y0 = floor(yy);
            int y1 = ceil(yy);

            double A = vec3f(xx - x0, yy - y0, 0.0).dot(grad[y0 * scale + x0]);
            double B = vec3f(xx - x1, yy - y0, 0.0).dot(grad[y0 * scale + x1]);
            double C = vec3f(xx - x0, yy - y1, 0.0).dot(grad[y1 * scale + x0]);
            double D = vec3f(xx - x1, yy - y1, 0.0).dot(grad[y1 * scale + x1]);

            double AB = A + (xx - x0) * (B - A);
            double CD = C + (xx - x0) * (D - C);

            double v = AB + (yy - y0) * (CD - AB);
            result[y * size + x] = fade((v + 1)/2);
        }
    }

    return result;
}

double PerlinNoise::fade(double x)
{
    return 6 * pow(x, 5) - 15 * pow(x, 4) + 10 * pow(x, 3);
}