#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <GL/glew.h>

class Light
{
public:
  Light(const glm::vec3 &position, const glm::vec3 &color, float intensity);

  void setUniforms(GLuint shaderProgram) const;

  // Gettery
  glm::vec3 getPosition() const { return position; }
  glm::vec3 getColor() const { return color; }
  float getIntensity() const { return intensity; }

  // Settery
  void setPosition(const glm::vec3 &newPosition) { position = newPosition; }
  void setColor(const glm::vec3 &newColor) { color = newColor; }
  void setIntensity(float newIntensity) { intensity = newIntensity; }

private:
  glm::vec3 position;
  glm::vec3 color;
  float intensity;
};

#endif