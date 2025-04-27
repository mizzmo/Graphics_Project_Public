#include "Object.h"
#include "texture.h" 

Object::Object(const std::vector<Vertex>& vertices, const std::string& texturePath)
    : vertices(vertices)
{
	// Generate the VAO and VBO for Object
    create_object();
    textureID = setup_texture(texturePath.c_str());
}

Object::~Object() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &textureID);
}

void Object::create_object() {
    // Create a VAO
    glGenVertexArrays(1, &vao);
    // Create a VBO
    glGenBuffers(1, &vbo);
	// Bind the VAO
    glBindVertexArray(vao);
    // Bind to VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Vertex Attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Colour
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Colour));
    glEnableVertexAttribArray(1);

    // TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // Normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void Object::draw() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    // Unbind
    glBindVertexArray(0);
}