#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku shadera: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float camX = 0.0f;
float camY = 0.0f;
float camZ = -3.0f;

float minMotionSpeed = 1.0f;
float maxMotionSpeed = 10.0f; // Maksymalna prędkość ruchu
float acceleration = 0.5f; // Wartość przyspieszenia ruchu


// Początkowa prędkość
float xSpeed = minMotionSpeed;
float ySpeed = minMotionSpeed;
float zSpeed = minMotionSpeed; 

float camPhi = 0.0;
float camTheta = 0.0;

float angleSpeed = 30.0f;

float lastTime = 0.0;  // Czas w ostatniej klatce
float deltaTime = 0.0; // Różnica czasowa między klatkami

// Dzięki temu obrazek rozciąga się wraz z okienkiem
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/*
A
D,
Space,
Shift,
W,
S
*/

bool pressedKeys[6] = {
    false,
    false,
    false,
    false,    
    false,
    false, 
};


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
        return;
    }

    float currentTime = glfwGetTime();  // Aktualny czas
    deltaTime = currentTime - lastTime;  // Obliczenie różnicy czasowej między klatkami
    lastTime = currentTime;
    // Ruch lewo-prawo
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS) {
        if (pressedKeys[0])
            xSpeed += acceleration * deltaTime;
        else
            xSpeed = minMotionSpeed;
        if (xSpeed > maxMotionSpeed) xSpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości
        camX += xSpeed * deltaTime;

        pressedKeys[0] = true;
        pressedKeys[1] = false;
    }
    else if(glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (pressedKeys[1])
            xSpeed += acceleration * deltaTime;
        else
            xSpeed = minMotionSpeed;
        if (xSpeed > maxMotionSpeed) xSpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości
        camX -= xSpeed * deltaTime;

        pressedKeys[0] = false;
        pressedKeys[1] = true;
    }
    else{
        xSpeed = minMotionSpeed;
        
        pressedKeys[0] = false;
        pressedKeys[1] = false;
    }

    // Ruch góra-doł
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && 
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) != GLFW_PRESS) {
        if (pressedKeys[2] && !pressedKeys[3])
            ySpeed += acceleration * deltaTime;
        else
            ySpeed = minMotionSpeed;
        if (ySpeed > maxMotionSpeed) ySpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości
        camY -= ySpeed * deltaTime;

        pressedKeys[2] = true;
        pressedKeys[3] = false;
    }
    else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
            glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        if (pressedKeys[2] && pressedKeys[3])
            ySpeed += acceleration * deltaTime;
        else
            ySpeed = minMotionSpeed;
        if (ySpeed > maxMotionSpeed) ySpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości
        camY += ySpeed * deltaTime;

        pressedKeys[2] = true;
        pressedKeys[3] = true;
    }
    else{
        ySpeed = minMotionSpeed;
        
        pressedKeys[2] = false;
        pressedKeys[3] = false;
    }


    // Ruch zoom in-out
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS) {
        if (pressedKeys[4])
            zSpeed += acceleration * deltaTime;
        else
            zSpeed = minMotionSpeed;
        if (zSpeed > maxMotionSpeed) zSpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości
        camZ += zSpeed * deltaTime;

        pressedKeys[4] = true;
        pressedKeys[5] = false;
    }
    else if(glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (pressedKeys[5])
            zSpeed += acceleration * deltaTime;
        else
            zSpeed = minMotionSpeed;        
        if (zSpeed > maxMotionSpeed) zSpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości
        camZ -= zSpeed * deltaTime;

        pressedKeys[4] = false;
        pressedKeys[5] = true;
    }
    else{
        zSpeed = minMotionSpeed;

        pressedKeys[4] = false;
        pressedKeys[5] = false;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS) {
        camPhi += angleSpeed * deltaTime;
        camPhi = std::min(90.0f, std::max(-90.0f, camPhi));
    }

    else if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camPhi -= angleSpeed * deltaTime;
        camPhi = std::min(90.0f, std::max(-90.0f, camPhi));
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS) {
        camTheta += angleSpeed * deltaTime;
        camTheta = std::min(90.0f, std::max(-90.0f, camTheta));
    }

    else if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camTheta -= angleSpeed * deltaTime;
        camTheta = std::min(90.0f, std::max(-90.0f, camTheta));
    }
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

    // Definicja wierzchołków i indeksów
    float vertices[] = {
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,

    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3,
        4, 5, 6,
        5, 6, 7,
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Atrybuty wierzchołków
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Włączamy Z-buffer
    glEnable(GL_DEPTH_TEST);  
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Główna pętla renderowania
    while (!glfwWindowShouldClose(window)) {

        processInput(window);
        // render
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        view  = glm::translate(view, glm::vec3(camX, camY, camZ));
        view = glm::rotate(view, glm::radians(camPhi), glm::vec3(1, 0, 0));
        view = glm::rotate(view, glm::radians(camTheta), glm::vec3(0, 1, 0));

        projection = glm::perspective(glm::radians(45.0f), ((float)SCR_WIDTH / (float)SCR_HEIGHT), 0.1f, 100.0f);

        // Przesyłanie macierzy do shaderów
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

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
