#pragma once  
#include <iostream>  
#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>  
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>  
#include <vector>  


// Add a vertex to the plane array
void add_vertex(std::vector<GLfloat>& plane, float x, float y, float z, float r, float g, float b, float u, float v, float nx, float ny, float nz) {
    plane.push_back(x);
    plane.push_back(y);
    plane.push_back(z);
    plane.push_back(r);
    plane.push_back(g);
    plane.push_back(b);
    plane.push_back(u);
    plane.push_back(v);
    plane.push_back(nx);
    plane.push_back(ny);
    plane.push_back(nz);
}

// Calculate the height of the dune at a given pos
float get_dune_height(float x, float z, float maxHeight, float frequency, float sharpness, float roughness) {
    // Use a combination of sine waves to create dune patterns
    // Primary dune pattern
    float height = maxHeight * 0.7f * pow(
        (sin(x * frequency) * 0.5f + 0.5f) *
        (sin(z * frequency * 1.5f) * 0.5f + 0.5f),
        sharpness
    );
    // Rotated 45 degrees
    float deg = glm::radians(45.f);
    float x2 = x * cos(deg) - z * sin(deg);
    float z2 = x * sin(deg) + z * cos(deg);
    height += maxHeight * 0.3f * pow(
        (sin(x2 * frequency * 0.7f) * 0.5f + 0.5f) *
        (sin(z2 * frequency * 0.7f) * 0.5f + 0.5f),
        sharpness
    );

    // Calculate final height
    height += maxHeight * roughness * ((sin(x * frequency * 5.0f) * cos(z * frequency * 7.0f)) * 0.5f + 0.5f);

    return height;
}

// Calculate normal for a triangle
glm::vec3 calculate_triangle_normal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 v1 = p2 - p1;
    glm::vec3 v2 = p3 - p1;
    glm::vec3 normal = glm::cross(v1, v2);
    return glm::normalize(normal);
}

std::vector<GLfloat> generate_plane(int div, float width, glm::vec3 colour) {
    std::vector<GLfloat> plane;
    float triangleSide = width / div;
    float halfWidth = width / 2.0f;
    // Maximum height of dunes
    float duneHeight = 5.0f;  
    // Frequency of dunes
    float duneFrequency = 0.05f;   
	// Dune sharpness
    float duneSharpness = 2.0f;  
	// Roughness 
    float roughness = 0.3f;         

    // For each cell in our grid
    for (int row = 0; row < div; row++) {
        for (int col = 0; col < div; col++) {
            // Calculate the four corners of square
            float x1 = col * triangleSide - halfWidth;
            float z1 = row * -triangleSide + halfWidth;

            float x2 = (col + 1) * triangleSide - halfWidth;
            float z2 = row * -triangleSide + halfWidth;

            float x3 = col * triangleSide - halfWidth;
            float z3 = (row + 1) * -triangleSide + halfWidth;

            float x4 = (col + 1) * triangleSide - halfWidth;
            float z4 = (row + 1) * -triangleSide + halfWidth;

            // Calculate height at each corner of square
            float y1 = get_dune_height(x1, z1, duneHeight, duneFrequency, duneSharpness, roughness);
            float y2 = get_dune_height(x2, z2, duneHeight, duneFrequency, duneSharpness, roughness);
            float y3 = get_dune_height(x3, z3, duneHeight, duneFrequency, duneSharpness, roughness);
            float y4 = get_dune_height(x4, z4, duneHeight, duneFrequency, duneSharpness, roughness);


            // Num times texture will repeat
            float textureRepeat = 6.0f;

            // First triangle
            // Calculate normal 
            glm::vec3 normal1 = calculate_triangle_normal(
                glm::vec3(x1, y1, z1),
                glm::vec3(x2, y2, z2),
                glm::vec3(x3, y3, z3)
            );

            // Vertex 1
            add_vertex(plane, x1, y1, z1, colour.r, colour.g, colour.b,
                static_cast<float>(col) / div * textureRepeat,
                static_cast<float>(row) / div * textureRepeat,
                normal1.x, normal1.y, normal1.z);

            // Vertex 2
            add_vertex(plane, x2, y2, z2, colour.r, colour.g, colour.b,
                static_cast<float>(col + 1) / div * textureRepeat,
                static_cast<float>(row) / div * textureRepeat,
                normal1.x, normal1.y, normal1.z);

            // Vertex 3
            add_vertex(plane, x3, y3, z3, colour.r, colour.g, colour.b,
                static_cast<float>(col) / div * textureRepeat,
                static_cast<float>(row + 1) / div * textureRepeat,
                normal1.x, normal1.y, normal1.z);

            // Triangle 2
      
            // Calculate normal
            glm::vec3 normal2 = calculate_triangle_normal(
                glm::vec3(x2, y2, z2),
                glm::vec3(x4, y4, z4),
                glm::vec3(x3, y3, z3)
            );

            // Vertex 4
            add_vertex(plane, x2, y2, z2, colour.r, colour.g, colour.b,
                static_cast<float>(col + 1) / div * textureRepeat,
                static_cast<float>(row) / div * textureRepeat,
                normal2.x, normal2.y, normal2.z);

            // Vertex 5
            add_vertex(plane, x4, y4, z4, colour.r, colour.g, colour.b,
                static_cast<float>(col + 1) / div * textureRepeat,
                static_cast<float>(row + 1) / div * textureRepeat,
                normal2.x, normal2.y, normal2.z);

            // Vertex 6
            add_vertex(plane, x3, y3, z3, colour.r, colour.g, colour.b,
                static_cast<float>(col) / div * textureRepeat,
                static_cast<float>(row + 1) / div * textureRepeat,
                normal2.x, normal2.y, normal2.z);
        }
        
    }
    // Return array of vertices
    return plane;
}
