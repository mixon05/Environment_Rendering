#include "envmap.h"
#include "stb_image.h"
#include <iostream>


EnvMap::EnvMap(std::string filepath, float yScale, float xStride, float zStride)
    : vertices(nullptr), indices(nullptr), xSize(0), zSize(0) {
    this->yScale = yScale;
    this->xStride = xStride;
    this->zStride = zStride;
    int channels;
    int xSize;
    int zSize;
    unsigned char* data = stbi_load(filepath.c_str(), &xSize, &zSize, &channels, 0); // 0 – automatyczne dopasowanie liczby kanałów
    if(!data){
        std::cerr << "Błąd podczas wczytywania pliku: " << filepath << std::endl;
        return;
    }
    this->xSize = xSize;
    this->zSize = zSize;


    unsigned int vertices_size = 3*xSize*zSize;
    this->vertices = new float [vertices_size];
    for(int i = 0; i < zSize; i++){
        for(int j = 0; j< xSize; j++){
            
            vertices[3 * (i * xSize + j)] = xStride * (j - (xSize -1) / 2.0);
            vertices[3 * (i * xSize + j) + 1] = data[(i * xSize + j) * channels] / yScale;
            vertices[3 * (i * xSize + j) + 2] = zStride * (i - (zSize - 1) / 2.0);

        }
    }

    stbi_image_free(data);

    unsigned int indices_size = 6*(xSize-1)*(zSize-1);
    this->indices = new unsigned int [indices_size];
    for(int i = 0; i < (zSize-1); i++) {
        for(int j = 0; j < (xSize-1); j++) {

            indices[6*(i*(xSize-1) + j)] = i*(xSize) + j;
            indices[6*(i*(xSize-1) + j) + 1] = i*(xSize) + j + 1;
            indices[6*(i*(xSize-1) + j) + 2] = (i+1)*(xSize) + j + 1;
            indices[6*(i*(xSize-1) + j) + 3] = i*(xSize) + j;
            indices[6*(i*(xSize-1) + j) + 4] = (i+1)*(xSize) + j;
            indices[6*(i*(xSize-1) + j) + 5] = (i+1)*(xSize) + j + 1;
        }
    }



}

EnvMap::~EnvMap() {
    delete[] this->vertices; // Usuwanie dynamicznej tablicy wierzchołków
    delete[] this->indices;  // Usuwanie dynamicznej tablicy indeksów
}