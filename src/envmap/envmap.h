#ifndef ENVMAP_H
#define ENVMAP_H
#include <string>

class EnvMap
{
public:
    EnvMap(std::string filepath, float yScale, float xStride, float zStride);
    ~EnvMap();
    int xSize;
    int zSize;
    float xStride;
    float zStride;
    float yScale;
    float* vertices;
    unsigned int* indices;
};

#endif