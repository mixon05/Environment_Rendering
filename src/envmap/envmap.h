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
};

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

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

   private:
    void calculateNormals();
    float getHeightAt(int x, int z);
    glm::vec3 calculateNormalAt(int x, int z);
};
#endif