#pragma once  
#include <iostream>  
#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>  
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>  
#include <vector>  

std::vector<GLfloat> generate_plane(int div, float width) {
    std::vector<GLfloat> plane;
    float triangleSide = width / div;

    for (int row = 0; row < div; ++row) {
        for (int col = 0; col < div; ++col) {
            // Calculate the coordinates of the four corners of the current square
            glm::vec3 v0 = glm::vec3(col * triangleSide, 0.0f, row * -triangleSide);
            glm::vec3 v1 = glm::vec3((col + 1) * triangleSide, 0.0f, row * -triangleSide);
            glm::vec3 v2 = glm::vec3(col * triangleSide, 0.0f, (row + 1) * -triangleSide);
            glm::vec3 v3 = glm::vec3((col + 1) * triangleSide, 0.0f, (row + 1) * -triangleSide);

            // Calculate the texture coordinates for each vertex
            glm::vec2 uv0 = glm::vec2(static_cast<float>(col) / div, static_cast<float>(row) / div);
            glm::vec2 uv1 = glm::vec2(static_cast<float>(col + 1) / div, static_cast<float>(row) / div);
            glm::vec2 uv2 = glm::vec2(static_cast<float>(col) / div, static_cast<float>(row + 1) / div);
            glm::vec2 uv3 = glm::vec2(static_cast<float>(col + 1) / div, static_cast<float>(row + 1) / div);

            // First triangle in square
            plane.push_back(v0.x); plane.push_back(v0.y); plane.push_back(v0.z);
            plane.push_back(uv0.x); plane.push_back(uv0.y);

            plane.push_back(v1.x); plane.push_back(v1.y); plane.push_back(v1.z);
            plane.push_back(uv1.x); plane.push_back(uv1.y);

            plane.push_back(v2.x); plane.push_back(v2.y); plane.push_back(v2.z);
            plane.push_back(uv2.x); plane.push_back(uv2.y);

            // Second triangle in square
            plane.push_back(v1.x); plane.push_back(v1.y); plane.push_back(v1.z);
            plane.push_back(uv1.x); plane.push_back(uv1.y);

            plane.push_back(v3.x); plane.push_back(v3.y); plane.push_back(v3.z);
            plane.push_back(uv3.x); plane.push_back(uv3.y);

            plane.push_back(v2.x); plane.push_back(v2.y); plane.push_back(v2.z);
            plane.push_back(uv2.x); plane.push_back(uv2.y);
        }
    }
    return plane;
}