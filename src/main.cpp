#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>

#include "control/control.h"
#include "envmap/envmap.h"
#include "light/light.h"
#include "nlohmann/json.hpp"
#include "stb_image.h"

using JsonValue = std::variant<std::string, int, float>;
using JsonMap = std::map<std::string, JsonValue>;

JsonMap parseJsonToMap(const std::string &filePath)
{
    // Wczytaj JSON-a z pliku
    std::ifstream file(filePath);
    if (!file)
    {
        throw std::runtime_error("Nie można otworzyć pliku JSON: " + filePath);
    }

    nlohmann::json json;
    file >> json;

    // Przekształć JSON-a na mapę
    JsonMap result;
    for (auto &[key, value] : json.items())
    {
        if (value.is_string())
        {
            result[key] = value.get<std::string>();
        }
        else if (value.is_number_integer())
        {
            result[key] = value.get<int>();
        }
        else if (value.is_number_float())
        {
            result[key] = value.get<float>();
        }
        else
        {
            std::cerr << "Pominięto nieobsługiwany typ dla klucza: " << key << std::endl;
        }
    }
    return result;
}

JsonMap config = parseJsonToMap("../config.json");

const unsigned int SCR_WIDTH = std::get<int>(config.at("SRC_WIDTH"));
const unsigned int SCR_HEIGHT = std::get<int>(config.at("SRC_HEIGHT"));

// Dzięki temu obrazek rozciąga się wraz z okienkiem
void framebuffer_size_callback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

// Deklaracja/definicja funkcji loadShaderSource
std::string loadShaderSource(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Nie można otworzyć pliku: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int loadTexture(std::string path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


int main()
{
    GLFWwindow *window;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GKOM environment Rendering", NULL, NULL);
    if (!window)
    {
        std::cerr << "Nie udało się utworzyć okna GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewInit();

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    EnvMap envMap("../" + std::get<std::string>(config.at("bitmapPath")), std::get<float>(config.at("yScale")),
                  std::get<float>(config.at("xStride")), std::get<float>(config.at("zStride")),
                  std::get<float>(config.at("waterHeightLevel")));

    // Załaduj shadery
    std::string vertexShaderSourceStr = loadShaderSource("../src/shaders/vertex_shader.vs");
    std::string fragmentShaderSourceStr = loadShaderSource("../src/shaders/fragment_shader.fs");

    const char *vertexShaderSource = vertexShaderSourceStr.c_str();
    const char *fragmentShaderSource = fragmentShaderSourceStr.c_str();

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
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Błąd kompilacji vertex shadera: " << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
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

    Light light(glm::vec3(envMap.xSize * envMap.xStride / 2.0f, std::get<float>(config.at("lightYCoord")), envMap.zSize * envMap.zStride / 2.0f),
                glm::vec3(1.0f, 1.0f, 1.0f), std::get<float>(config.at("lightIntensity")));

    GLuint viewPosLoc = glGetUniformLocation(program, "viewPos");

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
    glBufferData(GL_ARRAY_BUFFER, envMap.vertices.size() * sizeof(Vertex), envMap.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, envMap.indices.size() * sizeof(unsigned int), envMap.indices.data(),
                 GL_STATIC_DRAW);

    // Atrybuty wierzchołków
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, isWater));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    unsigned int textureZero = loadTexture("../" + std::get<std::string>(config.at("textureZeroPath")));
    unsigned int textureOne = loadTexture("../" + std::get<std::string>(config.at("textureOnePath")));
    unsigned int textureTwo = loadTexture("../" + std::get<std::string>(config.at("textureTwoPath")));
    unsigned int waterTexture = loadTexture("../" + std::get<std::string>(config.at("waterTexturePath")));
    unsigned int waterTextureNormal = loadTexture("../" + std::get<std::string>(config.at("waterTextureNormalPath")));

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "textureZero"), 0);
    glUniform1i(glGetUniformLocation(program, "textureOne"), 1);
    glUniform1i(glGetUniformLocation(program, "textureTwo"), 2);
    glUniform1i(glGetUniformLocation(program, "waterTexture"), 3);
    glUniform1i(glGetUniformLocation(program, "waterTextureNormal"), 4);
    glUniform1f(glGetUniformLocation(program, "waterTextureNormalFactor"), std::get<float>(config.at("waterTextureNormalFactor")));

    glUniform1f(glGetUniformLocation(program, "textureThresholdZeroOne"), std::get<float>(config.at("textureThresholdZeroOne")));
    glUniform1f(glGetUniformLocation(program, "textureThresholdOneTwo"), std::get<float>(config.at("textureThresholdOneTwo")));
    
    glm::vec2 waterSpeed(std::get<float>(config.at("waterSpeedU")), std::get<float>(config.at("waterSpeedV")));
    glUniform2fv(glGetUniformLocation(program, "waterSpeed"), 1, glm::value_ptr(waterSpeed));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureZero);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureOne);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureTwo);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, waterTexture);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, waterTextureNormal);

    // Włączamy Z-buffer
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::vec3 cameraStartPosition = glm::vec3(std::get<float>(config.at("cameraStartPositionX")),
                                              std::get<float>(config.at("cameraStartPositionY")),
                                              std::get<float>(config.at("cameraStartPositionZ"))
                                             );
    glm::vec3 cameraStartDirection = glm::vec3(std::get<float>(config.at("cameraStartDirectionX")),
                                              std::get<float>(config.at("cameraStartDirectionY")),
                                              std::get<float>(config.at("cameraStartDirectionZ"))
                                             );
    cameraStartDirection = glm::normalize(cameraStartDirection);


    Control controller(std::get<float>(config.at("minMotionSpeed")), std::get<float>(config.at("maxMotionSpeed")),
                       std::get<float>(config.at("acceleration")), std::get<float>(config.at("angleSpeed")),
                       std::get<float>(config.at("phiEps")), cameraStartPosition, cameraStartDirection);

    // Główna pętla renderowania
    while (!glfwWindowShouldClose(window))
    {
        controller.processInput(window);
        // render
        glClearColor(
            std::get<float>(config.at("backgroundRed")),
            std::get<float>(config.at("backgroundGreen")),
            std::get<float>(config.at("backgroundBlue")),
            1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view =
            glm::lookAt(controller.getCameraPosition(),
                        controller.getCameraPosition() + controller.getCameraDirection(), controller.getCameraUp());
        
        //std::cout << controller.getCameraDirection().x << " " << controller.getCameraDirection().y << " " <<controller.getCameraDirection().z <<"\n";

        projection = glm::perspective(glm::radians(45.0f), ((float)SCR_WIDTH / (float)SCR_HEIGHT), 0.1f,
                                      std::get<float>(config.at("sightRange")));

        // Przesyłanie macierzy do shaderów
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        float time = glfwGetTime();  // Czas od uruchomienia aplikacji (w sekundach)
        glUniform1f(glGetUniformLocation(program, "time"), time);

        light.setUniforms(program);

        glUniform3fv(viewPosLoc, 1, glm::value_ptr(controller.getCameraPosition()));

        glDrawElements(GL_TRIANGLES, envMap.indices.size(), GL_UNSIGNED_INT, 0);

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
