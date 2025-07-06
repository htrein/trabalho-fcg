//aplicar 3 tipos de teste de intersecção
//provavelmente, 1. entre os obstáculos que se movem
//               2. entre o personagem e os obstáculos
//               3. Colisao com Esferas

#include "collisions.hpp"

bool AABBCollision(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2, const glm::vec3& max2) {
    bool overlapX = max1.x >= min2.x && min1.x <= max2.x;
    bool overlapY = max1.y >= min2.y && min1.y <= max2.y;
    bool overlapZ = max1.z >= min2.z && min1.z <= max2.z;
    return overlapX && overlapY && overlapZ;
}

bool SphereCollision(glm::vec3& center1, float radius1, glm::vec3& center2, float radius2){
    float distance;
    float sumRadius;
    sumRadius = radius1 + radius2;
    // Distancia entre os centros das esferas
    distance = glm::distance(center1, center2);
    // Se a distancia for menor, as cascas das esferas estao interceptando
    return distance <= sumRadius;
}

bool SphereBoxCollision(glm::vec3& center, float radius, ColliderBox box, glm::vec3* intersection_point){
     // Encontra o ponto da caixa mais próximo do centro da esfera
    float closest_x = glm::clamp(center.x, box.bbox_min.x, box.bbox_max.x);
    float closest_y = glm::clamp(center.y, box.bbox_min.y, box.bbox_max.y);
    float closest_z = glm::clamp(center.z, box.bbox_min.z, box.bbox_max.z);

    glm::vec3 closest_point(closest_x, closest_y, closest_z);

    // Calcula o quadrado da distancia
    glm::vec3 vector_to_closest = closest_point - center;
    float distance_squared = glm::dot(vector_to_closest, vector_to_closest);
    *intersection_point = closest_point;

    // Se o quadrado for menor do que o raio ao quadrado
    // Colidiu
    return distance_squared < (radius * radius);
}

bool BoxBoxCollision(ColliderBox box1, ColliderBox box2, glm::mat4 col1_transform, glm::mat4 col2_transform){
    std::vector<glm::vec4> box_corners1 = cornersOfBox(box1);
    std::vector<glm::vec4> box_corners2 = cornersOfBox(box2);
    std::vector<glm::vec4> localNormals = getBoxNormals();

    glm::mat4 normalMatrix1 = glm::transpose(glm::inverse(col1_transform));
    glm::mat4 normalMatrix2 = glm::transpose(glm::inverse(col2_transform));

    // Convertendo tudo para as coordenadas do mundo
    for(long unsigned int i = 0; i < box_corners1.size(); i++){
        box_corners1[i] = col1_transform * box_corners1[i];
    }
    for(long unsigned int i = 0; i < box_corners2.size(); i++){
        box_corners2[i] = col2_transform * box_corners2[i];

    }    

    std::vector<glm::vec4> world_normals1, world_normals2;    
    for(long unsigned int i = 0; i < localNormals.size(); i++){
        world_normals1.push_back(normalMatrix1 * localNormals[i]);
        world_normals2.push_back(normalMatrix2 * localNormals[i]);
    } 

    // 3 normais das caixas
    for(int i = 0; i < 3; i++){
        // Caixa 1
        std::pair<float, float> limits1 = projectionLimits(box_corners1, world_normals1[i]);
        std::pair<float, float> limits2 = projectionLimits(box_corners2, world_normals1[i]);
        if(!overlapHappend(limits1, limits2)){
            return false;
        }

        // Caixa 2
        limits1 = projectionLimits(box_corners1, world_normals2[i]);
        limits2 = projectionLimits(box_corners2, world_normals2[i]);
        if(!overlapHappend(limits1, limits2)){
            return false;
        }
    }

    // 9 normais do produto vetorial entre as normais das caixas
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            glm::vec4 normal = crossproduct(world_normals1[i], world_normals2[j]);
            //printf("normal[%d%d]: x = %f, y = %f, z = %f\n", i,j, normal.x, normal.y, normal.z);
            if(norm(normal) < 0.001f) continue; // Evitar vetores nulos
            std::pair<float, float> limits1 = projectionLimits(box_corners1, normal);
            std::pair<float, float> limits2 = projectionLimits(box_corners2, normal);
            if(!overlapHappend(limits1, limits2)){
                return false;
            }
        }   
    }
    return true;
}

std::vector<glm::vec4> cornersOfBox(ColliderBox box){
    std::vector<glm::vec4> vertices;
    vertices.reserve(8); // Reserva espaço para 8 vértices
    vertices.push_back(glm::vec4(box.bbox_min.x, box.bbox_min.y, box.bbox_min.z, 1.0f));
    vertices.push_back(glm::vec4(box.bbox_max.x, box.bbox_min.y, box.bbox_min.z, 1.0f));
    vertices.push_back(glm::vec4(box.bbox_min.x, box.bbox_max.y, box.bbox_min.z, 1.0f));
    vertices.push_back(glm::vec4(box.bbox_min.x, box.bbox_min.y, box.bbox_max.z, 1.0f));
    vertices.push_back(glm::vec4(box.bbox_max.x, box.bbox_max.y, box.bbox_min.z, 1.0f));
    vertices.push_back(glm::vec4(box.bbox_max.x, box.bbox_min.y, box.bbox_max.z, 1.0f));
    vertices.push_back(glm::vec4(box.bbox_min.x, box.bbox_max.y, box.bbox_max.z, 1.0f));
    vertices.push_back(glm::vec4(box.bbox_max.x, box.bbox_max.y, box.bbox_max.z, 1.0f));
    return vertices;
}

std::pair<float, float> projectionLimits(const std::vector<glm::vec4>& vertices, glm::vec4 axis){
    axis = glm::normalize(axis);

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
    for (const auto& vertex : vertices){
        float projection = glm::dot(vertex, axis);

        if (projection < min) {
            min = projection;
        }
        if (projection > max) {
            max = projection;
        }
    }
    return std::pair<float, float>(min, max);
}

bool overlapHappend(std::pair<float, float> limits1, std::pair<float, float> limits2){
    return limits1.second >= limits2.first && limits2.second >= limits1.first;
}

std::vector<glm::vec4> getBoxNormals() {
    std::vector<glm::vec4> normals;
    normals.reserve(3);
    normals.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)); // Local X-axis
    normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)); // Local Y-axis
    normals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)); // Local Z-axis
    return normals;
}

void collisionTreatmentAABB(glm::vec3* g_BunnyPosition, ColliderBox bunny_limits, glm::vec3 obj_min, glm::vec3 obj_max, glm::vec3 previous_bunny_position, float folga, bool* jumping, bool* on_top, float* jump_velocity){
    glm::vec3 bunny_min = *g_BunnyPosition + bunny_limits.bbox_min;
    glm::vec3 bunny_max = *g_BunnyPosition + bunny_limits.bbox_max;
    
    // Se o coelho estava acima do objeto, mas seu limite inferior já estava dentro
    if (previous_bunny_position.y >= obj_max.y - folga && bunny_min.y < obj_max.y + folga) {
        //y
        (*g_BunnyPosition).y = obj_max.y - bunny_limits.bbox_min.y;
        if (*jump_velocity <= 0.0f) {
            *jumping = false;
            *jump_velocity = 0.0f;
            *on_top = true;
        }
    } else {
        //x
        glm::vec3 try_pos = *g_BunnyPosition;
        try_pos.x = previous_bunny_position.x;
        glm::vec3 try_min = try_pos + bunny_limits.bbox_min;
        glm::vec3 try_max = try_pos + bunny_limits.bbox_max;
        if (!AABBCollision(try_min, try_max, obj_min, obj_max)) {
            (*g_BunnyPosition).x = previous_bunny_position.x;
            return;
        }
        //z
        try_pos = *g_BunnyPosition;
        try_pos.z = previous_bunny_position.z;
        try_min = try_pos + bunny_limits.bbox_min;
        try_max = try_pos + bunny_limits.bbox_max;
        if (!AABBCollision(try_min, try_max, obj_min, obj_max)) {
            (*g_BunnyPosition).z = previous_bunny_position.z;
            return;
        }
        *g_BunnyPosition = previous_bunny_position;
    }
}

bool BoxPlaneCollision(const ColliderBox& box, const std::pair<glm::vec4, glm::vec4>& plane_limits_local, const glm::mat4& box_transform, const glm::mat4& plane_transform){
    // Matriz para coloca o collider no espaco da linha
    glm::mat4 box_to_plane_space = glm::inverse(plane_transform) * box_transform;

    std::vector<glm::vec4> box_corners = cornersOfBox(box);
    float box_min_y = std::numeric_limits<float>::max();
    float box_max_y = std::numeric_limits<float>::lowest();

    // Encontra o maior e menor y
    for (auto& corner : box_corners) {
        corner = box_to_plane_space * corner;
        box_min_y = std::min(box_min_y, corner.y);
        box_max_y = std::max(box_max_y, corner.y);
    }

    // Se o ponto mais baixo estiver acima
    // Ou o ponto mais alto estiver abaixo
    const float plane_y = plane_limits_local.first.y;
    if (box_max_y < plane_y || box_min_y > plane_y) {
        return false;
    }

    // Constroi uma AABB para o collider
    glm::vec3 box_min_aabb(box_corners[0]);
    glm::vec3 box_max_aabb(box_corners[0]);
    for (const auto& corner : box_corners) {
        box_min_aabb.x = std::min(box_min_aabb.x, corner.x);
        box_min_aabb.z = std::min(box_min_aabb.z, corner.z);
        box_max_aabb.x = std::max(box_max_aabb.x, corner.x);
        box_max_aabb.z = std::max(box_max_aabb.z, corner.z);
    }

    const float plane_min_x = std::min(plane_limits_local.first.x, plane_limits_local.second.x);
    const float plane_max_x = std::max(plane_limits_local.first.x, plane_limits_local.second.x);
    const float plane_min_z = std::min(plane_limits_local.first.z, plane_limits_local.second.z);
    const float plane_max_z = std::max(plane_limits_local.first.z, plane_limits_local.second.z);

    // Se houve overlap no x e z
    const bool overlap_x = box_max_aabb.x >= plane_min_x && box_min_aabb.x <= plane_max_x;
    const bool overlap_z = box_max_aabb.z >= plane_min_z && box_min_aabb.z <= plane_max_z;

    return overlap_x && overlap_z;
}
void collisionTreatmentBozPlane(glm::vec3* g_BunnyPosition, ColliderBox bunny_limits, glm::vec3 obj_min, glm::vec3 obj_max, glm::vec3 previous_bunny_position, float folga, bool* jumping, bool* on_top, float* jump_velocity){
    glm::vec3 bunny_min = *g_BunnyPosition + bunny_limits.bbox_min;
    glm::vec3 bunny_max = *g_BunnyPosition + bunny_limits.bbox_max;

    // Se o coelho estava acima do objeto, mas seu limite inferior já estava dentro
    if (previous_bunny_position.y >= obj_max.y - folga && bunny_min.y < obj_max.y + folga) {
        //y
        (*g_BunnyPosition).y = obj_max.y - bunny_limits.bbox_min.y;
        if (*jump_velocity <= 0.0f) {
            *jumping = false;
            *jump_velocity = 0.0f;
            *on_top = true;
        }
    } else {
        //x
        glm::vec3 try_pos = *g_BunnyPosition;
        try_pos.x = previous_bunny_position.x;
        glm::vec3 try_min = try_pos + bunny_limits.bbox_min;
        glm::vec3 try_max = try_pos + bunny_limits.bbox_max;
        if (!AABBCollision(try_min, try_max, obj_min, obj_max)) {
            (*g_BunnyPosition).x = previous_bunny_position.x;
            return;
        }
        //z
        try_pos = *g_BunnyPosition;
        try_pos.z = previous_bunny_position.z;
        try_min = try_pos + bunny_limits.bbox_min;
        try_max = try_pos + bunny_limits.bbox_max;
        if (!AABBCollision(try_min, try_max, obj_min, obj_max)) {
            (*g_BunnyPosition).z = previous_bunny_position.z;
            return;
        }
        *g_BunnyPosition = previous_bunny_position;
    }
}

