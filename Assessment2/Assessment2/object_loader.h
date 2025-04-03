#include <iostream>
#include <vector>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "error.h"
#include "shader.h"

struct Mesh {
    GLuint vao;
    GLuint vbo;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices; // Add indices for EBO
    GLuint ebo; //Element Buffer Object
};

Mesh loadOBJ(const std::string& filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    std::string mtl_base_path = filename.substr(0, filename.find_last_of("/\\") + 1); //extract path of obj file.

    std::cout << "mtl file path being checked: " << mtl_base_path << std::endl;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), mtl_base_path.c_str());


    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        throw std::runtime_error("Failed to load OBJ file!");
    }

    Mesh mesh;

    // Iterate over shapes
    for (const auto& shape : shapes) {
        // Iterate over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];

            // Iterate over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

                mesh.vertices.emplace_back(vx, vy, vz);

                // Check if `normal_index` is zero or positive.
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                    mesh.normals.emplace_back(nx, ny, nz);
                }

                mesh.indices.push_back(mesh.vertices.size()-1); //add index.
            }
            index_offset += fv;
        }
    }

    // Create VAO and VBO
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    std::vector<float> vertexData;
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        vertexData.push_back(mesh.vertices[i].x);
        vertexData.push_back(mesh.vertices[i].y);
        vertexData.push_back(mesh.vertices[i].z);
        if (mesh.normals.size() > 0) {
            vertexData.push_back(mesh.normals[i].x);
            vertexData.push_back(mesh.normals[i].y);
            vertexData.push_back(mesh.normals[i].z);
        }
    }

    glNamedBufferStorage(mesh.vbo, vertexData.size() * sizeof(float), vertexData.data(), 0);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (mesh.normals.size() > 0 ? 6 : 3) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if (mesh.normals.size() > 0) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    //Create EBO
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glNamedBufferStorage(mesh.ebo, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), 0);

    glBindVertexArray(0);
    return mesh;
}