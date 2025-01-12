#include "light.h"
#include <glm/gtc/type_ptr.hpp>

Light::Light(const glm::vec3 &position, const glm::vec3 &color, float intensity)
    : position(position), color(color), intensity(intensity)
{
}

void Light::setUniforms(GLuint shaderProgram) const
{
  GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
  GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
  GLuint lightIntensityLoc = glGetUniformLocation(shaderProgram, "lightIntensity");

  glUniform3fv(lightPosLoc, 1, glm::value_ptr(position));
  glUniform3fv(lightColorLoc, 1, glm::value_ptr(color));
  glUniform1f(lightIntensityLoc, intensity);
}