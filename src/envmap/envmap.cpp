#include "envmap.h"

#include <iostream>
#include <stdexcept>

#include "stb_image.h"

EnvMap::EnvMap(std::string filepath, float yScale, float xStride, float zStride, float waterHeightLevel)
    : yScale(yScale), xStride(xStride), zStride(zStride), waterHeightLevel(waterHeightLevel)
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
    vertices.resize(xSize * zSize + 4);

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

    // Wierzchołki wody

    vertices[xSize * zSize].position = glm::vec3(0.0f, waterHeightLevel, 0.0f);
    vertices[xSize * zSize].texCoords = glm::vec2(0.0f, 0.0f);

    vertices[xSize * zSize + 1].position = glm::vec3((xSize-1) * xStride, waterHeightLevel, 0.0f);
    vertices[xSize * zSize + 1].texCoords = glm::vec2(1.0f, 0.0f);

    vertices[xSize * zSize+ 2].position = glm::vec3(0.0f, waterHeightLevel, (zSize - 1) * zStride);
    vertices[xSize * zSize+ 2].texCoords = glm::vec2(0.0f, 1.0f);

    vertices[xSize * zSize + 3].position = glm::vec3((xSize-1) * xStride, waterHeightLevel, (zSize - 1) * zStride);
    vertices[xSize * zSize + 3].texCoords = glm::vec2(1.0f, 1.0f);

    // Generowanie indeksów
    indices.reserve(6*xSize*zSize + 6);

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
    // Indeksy wody

    indices.push_back(xSize * zSize);
    indices.push_back(xSize * zSize + 2);
    indices.push_back(xSize * zSize + 1);

    indices.push_back(xSize * zSize + 1);
    indices.push_back(xSize * zSize + 2);
    indices.push_back(xSize * zSize + 3);


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

    vertices[xSize * zSize].normal = glm::normalize(glm::vec3(0, 1.0, 0));
    vertices[xSize * zSize + 1].normal = glm::normalize(glm::vec3(0, 1.0, 0));
    vertices[xSize * zSize + 2].normal = glm::normalize(glm::vec3(0, 1.0, 0));
    vertices[xSize * zSize + 3].normal = glm::normalize(glm::vec3(0, 1.0, 0));

}
