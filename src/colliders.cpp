#include "colliders.hpp"

ColliderBox createBoundingBox(const tinyobj::attrib_t& atrib){
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();
    float max_z = std::numeric_limits<float>::lowest();
    for (size_t i = 0; i < atrib.vertices.size(); i += 3)
    {
        float vx = atrib.vertices[i + 0];
        float vy = atrib.vertices[i + 1];
        float vz = atrib.vertices[i + 2];

        if (vx < min_x) min_x = vx;
        if (vy < min_y) min_y = vy;
        if (vz < min_z) min_z = vz;
        if (vx > max_x) max_x = vx;
        if (vy > max_y) max_y = vy;
        if (vz > max_z) max_z = vz;
    }
    glm::vec3 bbox_min = glm::vec3(min_x, min_y, min_z);
    glm::vec3 bbox_max = glm::vec3(max_x, max_y, max_z);
    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f); //(bbox_min + bbox_max) * 0.5f;

    return {center, bbox_min, bbox_max};
}

// Criacao de uma bounding-sphere baseado no metodo de Ritter (escolhido por ser mais preciso)
ColliderSphere createBoundingSphereRitter(const tinyobj::attrib_t& attrib) {
    if (attrib.vertices.empty()) {
        return {glm::vec3(0.0f), 0.0f};
    }

    glm::vec3 first_pt(attrib.vertices[0], attrib.vertices[1], attrib.vertices[2]);

    glm::vec3 x_min = first_pt, x_max = first_pt;
    glm::vec3 y_min = first_pt, y_max = first_pt;
    glm::vec3 z_min = first_pt, z_max = first_pt;

    // Procura os pontos extremos
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        glm::vec3 pt(attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]);

        if (pt.x < x_min.x) x_min = pt;
        if (pt.x > x_max.x) x_max = pt;

        if (pt.y < y_min.y) y_min = pt;
        if (pt.y > y_max.y) y_max = pt;

        if (pt.z < z_min.z) z_min = pt;
        if (pt.z > z_max.z) z_max = pt;
    }

    // Encontra o maior eixo
    float dx = glm::distance(x_min, x_max);
    float dy = glm::distance(y_min, y_max);
    float dz = glm::distance(z_min, z_max);

    glm::vec3 p1 = x_min, p2 = x_max;
    if (dy > dx && dy > dz){
        p1 = y_min;
        p2 = y_max;
    } else if (dz > dx && dz > dy){
        p1 = z_min;
        p2 = z_max;
    }

    glm::vec3 center = (p1 + p2) * 0.5f;
    float radius = glm::distance(p1, center);

    // Passa mais uma vez para refinar
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        glm::vec3 pt(attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]);
        float dist_sq = glm::distance(pt, center);
        if (dist_sq > radius * radius) {
            float dist = std::sqrt(dist_sq);
            glm::vec3 direction = (pt - center) / dist;
            glm::vec3 opposite = center - direction * radius;
            center = (pt + opposite) * 0.5f;
            radius = glm::distance(pt, center);
        }
    }

    return {center, radius};
}

ColliderPlane createTopPlane(const tinyobj::attrib_t& atrib, glm::mat4 plane_transform){
    float min_x = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();
    float max_z = std::numeric_limits<float>::lowest();
    for (size_t i = 0; i < atrib.vertices.size(); i += 3)
    {
        float vx = atrib.vertices[i + 0];
        float vy = atrib.vertices[i + 1];
        float vz = atrib.vertices[i + 2];

        if (vx < min_x) min_x = vx;
        if (vz < min_z) min_z = vz;
        if (vx > max_x) max_x = vx;
        if (vy > max_y) max_y = vy;
        if (vz > max_z) max_z = vz;
    }
    glm::vec4 min_point = glm::vec4(min_x, max_y, min_z, 1.0f);
    glm::vec4 max_point = glm::vec4(max_x, max_y, max_z, 1.0f);

    std::pair<glm::vec4, glm::vec4> plane_limits_local = std::make_pair(min_point, max_point);
    

    return {plane_limits_local, plane_transform};
}