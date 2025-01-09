#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "stb_image.h"
#include "envmap/envmap.h"
#include "control/control.h"
#include "nlohmann/json.hpp"
#include <variant>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

using JsonValue = std::variant<std::string, int, float>;
using JsonMap = std::map<std::string, JsonValue>;

JsonMap parseJsonToMap(const std::string& filePath) {
    // Wczytaj JSON-a z pliku
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Nie można otworzyć pliku JSON: " + filePath);
    }

    nlohmann::json json;
    file >> json;

    // Przekształć JSON-a na mapę
    JsonMap result;
    for (auto& [key, value] : json.items()) {
        if (value.is_string()) {
            result[key] = value.get<std::string>();
        } else if (value.is_number_integer()) {
            result[key] = value.get<int>();
        } else if (value.is_number_float()) {
            result[key] = value.get<float>();
        } else {
            std::cerr << "Pominięto nieobsługiwany typ dla klucza: " << key << std::endl;
        }
    }
    return result;
}


JsonMap config = parseJsonToMap("../config.json");

const unsigned int SCR_WIDTH = std::get<int>(config.at("SRC_WIDTH"));
const unsigned int SCR_HEIGHT = std::get<int>(config.at("SRC_HEIGHT"));

// Dzięki temu obrazek rozciąga się wraz z okienkiem
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Deklaracja/definicja funkcji loadShaderSource
std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    GLFWwindow* window;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GKOM Enviroment Rendering", NULL, NULL);
    if (!window) {
        std::cerr << "Nie udało się utworzyć okna GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewInit();
    EnvMap envMap("../" + std::get<std::string>(config.at("bitmapPath")),
                    std::get<float>(config.at("yScale")),
                    std::get<float>(config.at("xStride")),
                    std::get<float>(config.at("zStride")));

    int vertices_size = 3*envMap.xSize*envMap.zSize;
    int indices_size = 6*(envMap.xSize-1)*(envMap.zSize-1);


    // Załaduj shadery
    std::string vertexShaderSourceStr = loadShaderSource("../src/shaders/vertex_shader.vs");
    std::string fragmentShaderSourceStr = loadShaderSource("../src/shaders/fragment_shader.fs");

    const char* vertexShaderSource = vertexShaderSourceStr.c_str();
    const char* fragmentShaderSource = fragmentShaderSourceStr.c_str();

    // Tworzenie shaderów
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Sprawdzenie błędów kompilacji shaderów
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Błąd kompilacji vertex shadera: " << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Błąd kompilacji fragment shadera: " << infoLog << std::endl;
    }

    // Tworzenie programu shaderowego
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    // Ustalenie identyfikatorów uniformów
    GLuint modelLoc = glGetUniformLocation(program, "model");
    GLuint viewLoc = glGetUniformLocation(program, "view");
    GLuint projectionLoc = glGetUniformLocation(program, "projection");

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size * sizeof(float), envMap.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(int), envMap.indices, GL_STATIC_DRAW);

    // Atrybuty wierzchołków
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Włączamy Z-buffer
    glEnable(GL_DEPTH_TEST);  
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Control controller(std::get<float>(config.at("minMotionSpeed")),
                        std::get<float>(config.at("maxMotionSpeed")),
                        std::get<float>(config.at("acceleration")),
                        std::get<float>(config.at("angleSpeed")),
                        std::get<float>(config.at("phiEps")));

    // Główna pętla renderowania
    while (!glfwWindowShouldClose(window)) {

        controller.processInput(window);
        // render
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(controller.getCameraPosition(),
                                    controller.getCameraPosition()+controller.getCameraDirection(),
                                    controller.getCameraUp());

        projection = glm::perspective(glm::radians(45.0f), ((float)SCR_WIDTH / (float)SCR_HEIGHT), 0.1f, 100.0f);

        // Przesyłanie macierzy do shaderów
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glDrawElements(GL_TRIANGLES, vertices_size * sizeof(float), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Usuwanie zasobów
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
