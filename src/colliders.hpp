#ifndef COLLIDERS_HPP
#define COLLIDERS_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>
#include <stb_image.h>

struct ColliderBox{ //estrutura para um objeto colidível cúbico
    glm::vec3 pos;
    glm::vec3 bbox_min;
    glm::vec3 bbox_max;
};

struct ColliderSphere{ //estrutura para um objeto colidível esférico
    glm::vec4 pos;
    float radius;
    glm::mat4 sphere_transform;
};

struct ColliderPlane{ //estrutura para um objeto colidível esférico
    std::pair<glm::vec4, glm::vec4> plane_limits_local;
    glm::mat4 plane_transform;
};

ColliderBox createBoundingBox(const tinyobj::attrib_t& atrib);
ColliderSphere createBoundingSphereRitter(const tinyobj::attrib_t& atrib);
ColliderPlane createTopPlane(const tinyobj::attrib_t& atrib, glm::mat4 plane_transform);

#endif