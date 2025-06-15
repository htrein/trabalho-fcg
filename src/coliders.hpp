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
    glm::vec3 pos;
    float radius;
};

ColliderBox createBoundingBox(const tinyobj::attrib_t& atrib);
ColliderSphere createBoundingSphere(const tinyobj::attrib_t& atrib);

#endif