#include "envmap.h"

#include <iostream>
#include <stdexcept>

#include "stb_image.h"

EnvMap::EnvMap(std::string filepath, float yScale, float xStride, float zStride)
    : yScale(yScale), xStride(xStride), zStride(zStride)
{
    // Wczytanie obrazu heightmapy
    int width, height, channels;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 1);

    if (!data)
    {
        throw std::runtime_error("Failed to load heightmap: " + filepath);
    }

    xSize = width;
    zSize = height;

    // Tworzenie wierzchołków
    vertices.resize(xSize * zSize);

    // Generowanie pozycji wierzchołków
    for (int z = 0; z < zSize; z++)
    {
        for (int x = 0; x < xSize; x++)
        {
            int index = z * xSize + x;
            float height = static_cast<float>(data[index]) / 255.0f * yScale;

            vertices[index].position = glm::vec3(x * xStride, height, z * zStride);
            vertices[index].texCoords = glm::vec2(
            static_cast<float>(x) / (xSize - 1),
            static_cast<float>(z) / (zSize - 1)
        );
        }
    }

    // Generowanie indeksów
    for (int z = 0; z < zSize - 1; z++)
    {
        for (int x = 0; x < xSize - 1; x++)
        {
            int topLeft = z * xSize + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * xSize + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // Obliczanie normalnych
    calculateNormals();

    stbi_image_free(data);
}

EnvMap::~EnvMap() {}

float EnvMap::getHeightAt(int x, int z)
{
    if (x < 0 || x >= xSize || z < 0 || z >= zSize)
    {
        return 0.0f;
    }
    return vertices[z * xSize + x].position.y;
}

glm::vec3 EnvMap::calculateNormalAt(int x, int z)
{
    // Używamy metody różnic skończonych do obliczenia normalnej
    float heightL = getHeightAt(x - 1, z);
    float heightR = getHeightAt(x + 1, z);
    float heightD = getHeightAt(x, z - 1);
    float heightU = getHeightAt(x, z + 1);

    // Obliczanie wektora normalnego
    glm::vec3 normal(heightL - heightR,
                     2.0f,  // Skalowanie składowej Y dla lepszego efektu
                     heightD - heightU);

    return glm::normalize(normal);
}

void EnvMap::calculateNormals()
{
    // Obliczanie normalnych dla każdego wierzchołka
    for (int z = 0; z < zSize; z++)
    {
        for (int x = 0; x < xSize; x++)
        {
            vertices[z * xSize + x].normal = calculateNormalAt(x, z);
        }
    }
}
