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
    std::vector<glm::vec4> normalsBox1 = getBoxNormals(box1, box_corners1);
    std::vector<glm::vec4> normalsBox2 = getBoxNormals(box2, box_corners2);

    // Convertendo tudo para as coordenadas do mundo
    for(long unsigned int i = 0; i < box_corners1.size(); i++){
        box_corners1[i] = glm::inverse(col1_transform) * box_corners1[i];
    }

    for(long unsigned int i = 0; i < box_corners2.size(); i++){
        box_corners2[i] = glm::inverse(col2_transform) * box_corners2[i];
    }

    for(long unsigned int i = 0; i < normalsBox1.size(); i++){
        normalsBox1[i] = glm::inverse(col1_transform) * normalsBox1[i];
    }

    for(long unsigned int i = 0; i < normalsBox2.size(); i++){
        normalsBox2[i] = glm::inverse(col2_transform) * normalsBox2[i];
    }
    
    // 3 normais da caixa 1
    for(int i = 0; i < 3; i++){
        std::pair<float, float> limits1 = projectionLimits(box_corners1, normalsBox1[i]);
        std::pair<float, float> limits2 = projectionLimits(box_corners2, normalsBox1[i]);
        if(!overlapHappend(limits1, limits2)){
            return false;
        }
    }

    // 3 normais da caixa 2
    for(int i = 0; i < 3; i++){
        std::pair<float, float> limits1 = projectionLimits(box_corners1, normalsBox2[i]);
        std::pair<float, float> limits2 = projectionLimits(box_corners2, normalsBox2[i]);
        if(!overlapHappend(limits1, limits2)){
            return false;
        }
    }

    // 9 normais de cross product entre as normais das caixas

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            glm::vec4 normal = crossproduct(normalsBox1[i], normalsBox2[j]);
            std::pair<float, float> limits1 = projectionLimits(box_corners1, normal);
            std::pair<float, float> limits2 = projectionLimits(box_corners2, normal);
            if(!overlapHappend(limits1, limits2)){
                return false;
            }
        }   
    }
    return true;
}

void collisionTreatmentAABB(glm::vec3* g_BunnyPosition, ColliderBox bunny_limits, glm::vec3 obj_min, glm::vec3 obj_max, glm::vec3 previous_bunny_position, float folga, bool* jumping, bool* on_top, float* jump_velocity){
    glm::vec3 bunny_min = *g_BunnyPosition + bunny_limits.bbox_min;
    glm::vec3 bunny_max = *g_BunnyPosition + bunny_limits.bbox_max;
    
    if (AABBCollision(bunny_min, bunny_max, obj_min, obj_max)) {

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
}

std::vector<glm::vec4> cornersOfBox(ColliderBox box){
    std::vector<glm::vec4> vertices;
    vertices.reserve(8);
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
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
    axis = axis / norm(axis);

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

    if(limits1.first <= limits2.first && limits1.second >= limits2.first){
        return true;
    }
    if(limits2.first <= limits1.first && limits2.second >= limits1.first){
        return true;
    }
    return false;
}

std::vector<glm::vec4> getBoxNormals(ColliderBox box, std::vector<glm::vec4> box_corners){
    std::vector<glm::vec4> normals;
    normals.reserve(3);
    glm::vec4 origin = box_corners[0];
    glm::vec4 vec2 = box_corners[1] - origin;
    glm::vec4 vec3 = box_corners[2] - origin;
    glm::vec4 vec4 = box_corners[3] - origin;
    glm::vec4 vec5 = box_corners[4] - origin;
    normals.push_back(crossproduct(vec2, vec3));
    normals.push_back(crossproduct(vec2, vec4));
    normals.push_back(crossproduct(vec2, vec5));

    for(int i = 0; i < 3; i++){
        normals[i] = normals[i] / norm(normals[i]);
    }
    return normals;
}