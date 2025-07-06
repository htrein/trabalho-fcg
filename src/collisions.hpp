#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "colliders.hpp"
#include "matrices.h"

bool AABBCollision(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2, const glm::vec3& max2);
bool SphereCollision(const glm::vec3& center1, float radius1, const glm::vec3& center2, float radius2);
bool SphereBoxCollision(glm::vec3& center, float radius, ColliderBox box, glm::vec3* intersection_point);
void collisionTreatmentAABB(glm::vec3* g_BunnyPosition, ColliderBox bunny_limits, glm::vec3 obj_min, glm::vec3 obj_max, glm::vec3 previous_bunny_position, float folga, bool* jumping, bool* on_top, float* jump_velocity);
std::vector<glm::vec4> cornersOfBox(ColliderBox box);
std::pair<float, float> projectionLimits(const std::vector<glm::vec4>& vertices, glm::vec4 axis);
bool overlapHappend(std::pair<float, float> limits1, std::pair<float, float> limits2);
std::vector<glm::vec4> getBoxNormals();
bool BoxBoxCollision(ColliderBox box1, ColliderBox box2, glm::mat4 col1_transform, glm::mat4 col2_transform);
bool BoxPlaneCollision(ColliderBox box, std::pair<glm::vec4, glm::vec4> limits_line, glm::mat4 col_transform, glm::mat4 line_transform);
#endif
