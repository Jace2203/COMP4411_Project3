#ifndef PERLINNOISE_H
#define PERLINNOISE_H

class PerlinNoise
{
public:
    static PerlinNoise* GetInstance();

    static unsigned char* GenerateNoise2D(int size, int octave, int scale);

private:
    PerlinNoise();
    ~PerlinNoise();

    static double fade(double x);

    static PerlinNoise* instance;
};

#endif