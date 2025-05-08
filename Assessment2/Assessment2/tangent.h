#pragma once

const int floatsPerInputVertex = 11;
const int floatsPerOutputVertex = 17;

// Function to generate tangents and bitangents for a mesh
void generateTangentsAndBitangents(const GLfloat* inputVertices, size_t numVertices, std::vector<GLfloat>& outputVertices) {
    for (size_t i = 0; i < numVertices; i += 3) {
        // Load input triangle vertices
        const GLfloat* v0 = &inputVertices[(i + 0) * floatsPerInputVertex];
        const GLfloat* v1 = &inputVertices[(i + 1) * floatsPerInputVertex];
        const GLfloat* v2 = &inputVertices[(i + 2) * floatsPerInputVertex];

        // Positions
        glm::vec3 pos1(v0[0], v0[1], v0[2]);
        glm::vec3 pos2(v1[0], v1[1], v1[2]);
        glm::vec3 pos3(v2[0], v2[1], v2[2]);

        // UVs
        glm::vec2 uv1(v0[6], v0[7]);
        glm::vec2 uv2(v1[6], v1[7]);
        glm::vec2 uv3(v2[6], v2[7]);

        // Edge vectors
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent, bitangent;

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);

        // Append each vertex with full attributes (17 floats)
        for (int j = 0; j < 3; ++j) {
            const GLfloat* vin = &inputVertices[(i + j) * floatsPerInputVertex];
            outputVertices.insert(outputVertices.end(), vin, vin + floatsPerInputVertex); 
            outputVertices.push_back(tangent.x);
            outputVertices.push_back(tangent.y);
            outputVertices.push_back(tangent.z);
            outputVertices.push_back(bitangent.x);
            outputVertices.push_back(bitangent.y);
            outputVertices.push_back(bitangent.z);
        }
    }
}