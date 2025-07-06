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

    return {center, bbox_min, bbox_max, false};
}

// Criacao de uma bounding-sphere baseado no metodo de Ritter (escolhido por ser mais preciso)
ColliderSphere createBoundingSphereRitter(const tinyobj::attrib_t& attrib) {
    if (attrib.vertices.empty()) {
        return {glm::vec3(0.0f), 0.0f};
    }

    // Encontra os pontos mais extremos
    glm::vec3 x_min_pt, x_max_pt, y_min_pt, y_max_pt, z_min_pt, z_max_pt;
    x_min_pt.x = y_min_pt.y = z_min_pt.z = std::numeric_limits<float>::max();
    x_max_pt.x = y_max_pt.y = z_max_pt.z = std::numeric_limits<float>::lowest();

    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        glm::vec3 pt(attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]);
        if (pt.x < x_min_pt.x) x_min_pt = pt;
        if (pt.x > x_max_pt.x) x_max_pt = pt;
        if (pt.y < y_min_pt.y) y_min_pt = pt;
        if (pt.y > y_max_pt.y) y_max_pt = pt;
        if (pt.z < z_min_pt.z) z_min_pt = pt;
        if (pt.z > z_max_pt.z) z_max_pt = pt;
    }

    // Monta o par mais distante e calcula sua distancia
    float dist_sq_x = glm::distance(x_max_pt, x_min_pt);
    float dist_sq_y = glm::distance(y_max_pt, y_min_pt);
    float dist_sq_z = glm::distance(z_max_pt, z_min_pt);

    glm::vec3 p1 = x_min_pt, p2 = x_max_pt;
    if (dist_sq_y > dist_sq_x && dist_sq_y > dist_sq_z) {
        p1 = y_min_pt; p2 = y_max_pt;
    } else if (dist_sq_z > dist_sq_x && dist_sq_z > dist_sq_y) {
        p1 = z_min_pt; p2 = z_max_pt;
    }

    // Calcula centro e raio da esfera
    glm::vec3 center = (p1 + p2) * 0.5f;
    float radius = glm::distance(p1, center);

    // Verifica mais uma vez para confirmar se os pontos eram os mais distante
    // Se isto acabar atrapalhando muito o desempenho, talvez seja melhor tirar
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        glm::vec3 pt(attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]);
        float dist_sq = glm::distance(pt, center);
        if (dist_sq > (radius * radius)) {
            float dist = std::sqrt(dist_sq);
            glm::vec3 direction = (pt - center) / dist;
            glm::vec3 opposite_pt = center - direction * radius;
            center = (pt + opposite_pt) * 0.5f;
            radius = glm::distance(pt, center);
        }
    }

    return {center, radius};
}