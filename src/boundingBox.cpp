#include "boundingBox.hpp"
#include <vector>

void drawBoundingBox(const ColliderBox& box, const glm::mat4& transform, GLuint model_uniform, GLuint object_id_uniform) {
    std::vector<glm::vec4> corners = cornersOfBox(box); // Assuming cornersOfBox exists
    for (auto& corner : corners) {
        corner = transform * corner;
    }

    // Define indices for the lines of the bounding box
    std::vector<unsigned int> indices = {
        0, 1, 1, 5, 5, 3, 3, 0, // Bottom face
        2, 4, 4, 7, 7, 6, 6, 2, // Top face
        0, 2, 1, 4, 5, 7, 3, 6  // Connecting lines
    };

    // Create VAO and VBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, corners.size() * sizeof(glm::vec4), corners.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set model matrix and object ID (assuming these are needed for your shader)
    // Draw lines
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Identity for now, adjust if needed
    glUniform1i(object_id_uniform, 8); // Assuming a distinct object ID for the bounding box (e.g., 8)
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
