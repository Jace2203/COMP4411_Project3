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

unsigned char* PerlinNoise::GenerateNoise2D(int size, int octave, int scale)
{
    vec3f* grad = new vec3f[(size + 1) * (size + 1)];
    for (int i = 0; i < (size + 1) * (size + 1); i++)
    {
        double theta = double(rand()) / RAND_MAX * M_PI * 2;
        grad[i] = vec3f(cos(theta) * 1, sin(theta) * 1, 0.0);
    }

    unsigned char* result = new unsigned char[size * size * scale * scale];

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            for (int j = 0; j < scale; j++)
            {
                for (int i = 0; i < scale; i++)
                {
                    vec3f pos(float(i)/scale, float(j)/scale, 0.0);
                    double A = (pos - vec3f(0.0, 0.0, 0.0)).dot(grad[y * size + x]);
                    double B = (pos - vec3f(1.0, 0.0, 0.0)).dot(grad[y * size + x + 1]);
                    double C = (pos - vec3f(0.0, 1.0, 0.0)).dot(grad[(y + 1) * size + x]);
                    double D = (pos - vec3f(1.0, 1.0, 0.0)).dot(grad[(y + 1) * size + x + 1]);

                    double AB = A + pos[0] * (B - A);
                    double CD = C + pos[0] * (D - C);

                    double v = AB + pos[1] * (CD - AB);
                    result[(y * scale + j) * scale * size + (x * scale + i)] = fade((v + 1)/2) * 255;
                }
            }
        }
    }

    return result;
}

double PerlinNoise::fade(double x)
{
    return 6 * pow(x, 5) - 15 * pow(x, 4) + 10 * pow(x, 3);
}