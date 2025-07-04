#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "collisions.hpp" // Include your collisions header

void drawBoundingBox(const ColliderBox& box, const glm::mat4& transform, GLuint model_uniform, GLuint object_id_uniform);

#endif
