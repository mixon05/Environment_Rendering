#ifndef ENVMAP_H
#define ENVMAP_H
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    float isWater;
};

class EnvMap
{
   public:
    EnvMap(std::string filepath, float yScale, float xStride, float zStride, float waterHeightLevel);
    ~EnvMap();

    int xSize;
    int zSize;
    float yScale;
    float xStride;
    float zStride;
    float waterHeightLevel;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

   private:
    void calculateNormals();
    float getHeightAt(int x, int z);
    glm::vec3 calculateNormalAt(int x, int z);
};
#endif