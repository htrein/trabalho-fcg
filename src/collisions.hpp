#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

bool AABBCollision(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2, const glm::vec3& max2);
bool SphereCollision(const glm::vec3& center1, float radius1, const glm::vec3& center2, float radius2);
#endif
