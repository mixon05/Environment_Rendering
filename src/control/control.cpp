#include "control.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

Control::Control(
    float minMotionSpeed,
    float maxMotionSpeed,
    float acceleration,
    float angleSpeed,
    float phiEps
) : minMotionSpeed(minMotionSpeed),
    maxMotionSpeed(maxMotionSpeed),
    acceleration(acceleration),
    angleSpeed(angleSpeed),
    phiEps(phiEps)
{
    xSpeed = minMotionSpeed;
    ySpeed = minMotionSpeed;
    zSpeed = minMotionSpeed;
    lastTime = 0.0f;
    deltaTime = 0.0f;

    for (int i = 0; i < 6; ++i) {
        pressedKeys[i] = false;
    }

    cameraPosition = glm::vec3(0, 0, 20);
    cameraDirection = glm::vec3(0, 0, -1);
    cameraUp = glm::vec3(0, 1, 0);
}

const glm::vec3& Control::getCameraPosition() const {
    return this->cameraPosition;
}

const glm::vec3& Control::getCameraDirection() const {
    return this->cameraDirection;
}

const glm::vec3& Control::getCameraUp() const {
    return this->cameraUp;
}

void Control::processInput(GLFWwindow *window) {
    glm::vec3 cameraRight = glm::cross(cameraDirection, cameraUp);

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

        cameraPosition -= cameraRight * xSpeed;

        pressedKeys[0] = true;
        pressedKeys[1] = false;
    }
    else if(glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (pressedKeys[1])
            xSpeed += acceleration * deltaTime;
        else
            xSpeed = minMotionSpeed;
        if (xSpeed > maxMotionSpeed) xSpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości

        cameraPosition += cameraRight * xSpeed;

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

        cameraPosition += cameraUp * zSpeed;


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
        
        cameraPosition -= cameraUp * zSpeed;

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

        cameraPosition += cameraDirection * zSpeed;

        pressedKeys[4] = true;
        pressedKeys[5] = false;
    }
    else if(glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (pressedKeys[5])
            zSpeed += acceleration * deltaTime;
        else
            zSpeed = minMotionSpeed;        
        if (zSpeed > maxMotionSpeed) zSpeed = maxMotionSpeed;  // Ograniczenie maksymalnej prędkości

        cameraPosition -= cameraDirection * zSpeed;

        pressedKeys[4] = false;
        pressedKeys[5] = true;
    }
    else{
        zSpeed = minMotionSpeed;
        pressedKeys[4] = false;
        pressedKeys[5] = false;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS) {
        float r = 1.0f;
        float phi = atan2(cameraDirection.z, cameraDirection.x);
        float theta = acos(cameraDirection.y);

        theta -= angleSpeed * deltaTime;
        theta = glm::max(theta, 0.0f + phiEps);

        cameraDirection.x = r * sin(theta) * cos(phi);
        cameraDirection.z = r * sin(theta) * sin(phi);
        cameraDirection.y = r * cos(theta);
    }

    else if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        float r = 1.0f;
        float phi = atan2(cameraDirection.z, cameraDirection.x);
        float theta = acos(cameraDirection.y);

        theta += angleSpeed * deltaTime;
        theta = glm::min(theta, glm::pi<float>() - phiEps);

        cameraDirection.x = r * sin(theta) * cos(phi);
        cameraDirection.z = r * sin(theta) * sin(phi);
        cameraDirection.y = r * cos(theta);

    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS) {
        float r = 1.0f;
        float phi = atan2(cameraDirection.z, cameraDirection.x);
        float theta = acos(cameraDirection.y);

        phi -= angleSpeed * deltaTime;

        cameraDirection.x = r * sin(theta) * cos(phi);
        cameraDirection.z = r * sin(theta) * sin(phi);
        cameraDirection.y = r * cos(theta);
    }

    else if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        float r = 1.0f;
        float phi = atan2(cameraDirection.z, cameraDirection.x);
        float theta = acos(cameraDirection.y);

        phi += angleSpeed * deltaTime;

        cameraDirection.x = r * sin(theta) * cos(phi);
        cameraDirection.z = r * sin(theta) * sin(phi);
        cameraDirection.y = r * cos(theta);
    }
}