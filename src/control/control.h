#ifndef CONTROL_H
#define CONTROL_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Control
{

public:

    Control(
        float minMotionSpeed,
        float maxMotionSpeed,
        float acceleration,
        float angleSpeed,
        float phiEps
    );

    const glm::vec3& getCameraPosition() const;
    const glm::vec3& getCameraDirection() const;
    const glm::vec3& getCameraUp() const;
    void processInput(GLFWwindow *window);

private:


    glm::vec3 cameraPosition;
    glm::vec3 cameraDirection;
    glm::vec3 cameraUp;

    float minMotionSpeed;
    float maxMotionSpeed;
    float acceleration;
    float xSpeed;
    float ySpeed;
    float zSpeed;
    float angleSpeed;
    float phiEps;

    float lastTime;
    float deltaTime;

    bool pressedKeys[6];

};


#endif