#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <wingdi.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


using namespace std;

#define PIXEL_W 1920
#define PIXEL_H 1080

float colour_buffer[PIXEL_W * PIXEL_H * 3];
float depth_buffer[PIXEL_W * PIXEL_H];

struct vertex
{
	glm::vec4 pos;
	glm::vec3 col;
	glm::vec3 nor;
	glm::vec2 tex;
	glm::vec3 tangent;    // Added for normal mapping
	glm::vec3 bitangent;  // Added for normal mapping
};

struct triangle
{
public:
	vertex v1, v2, v3;
	bool reflect;
	int primID;
	int texID;
};

// Function to calculate tangent and bitangent vectors
void calculateTangentSpace(vertex& v1, vertex& v2, vertex& v3)
{
	// Positions
	glm::vec3 pos1(v1.pos.x, v1.pos.y, v1.pos.z);
	glm::vec3 pos2(v2.pos.x, v2.pos.y, v2.pos.z);
	glm::vec3 pos3(v3.pos.x, v3.pos.y, v3.pos.z);

	// Texture coordinates
	glm::vec2 uv1 = v1.tex;
	glm::vec2 uv2 = v2.tex;
	glm::vec2 uv3 = v3.tex;

	// Edges of the triangle in 3D space
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;

	// Texture space differences
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	// Avoid division by zero
	float denominator = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	if (abs(denominator) < 0.0001f) {
		v1.tangent = v2.tangent = v3.tangent = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		v1.bitangent = v2.bitangent = v3.bitangent = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
		return;
	}

	float f = 1.0f / denominator;

	// Calculate tangent and bitangent vectors
	// tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y)
	// bitangent = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y)
	glm::vec3 tangent, bitangent;
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent.x = f * (deltaUV1.x * edge2.x - deltaUV2.x * edge1.x);
	bitangent.y = f * (deltaUV1.x * edge2.y - deltaUV2.x * edge1.y);
	bitangent.z = f * (deltaUV1.x * edge2.z - deltaUV2.x * edge1.z);

	// Normalize
	tangent = glm::normalize(tangent);
	bitangent = glm::normalize(bitangent);

	glm::vec3 normal1 = glm::vec3(v1.nor);
	glm::vec3 normal2 = glm::vec3(v2.nor);
	glm::vec3 normal3 = glm::vec3(v3.nor);

	tangent = glm::normalize(tangent - normal1 * glm::dot(normal1, tangent));

	bitangent = glm::normalize(bitangent - normal1 * glm::dot(normal1, bitangent) - tangent * glm::dot(tangent, bitangent));

	v1.tangent = v2.tangent = v3.tangent = tangent;
	v1.bitangent = v2.bitangent = v3.bitangent = bitangent;
}

int obj_parse(const char* filename, std::vector<triangle>* io_tris, const char* base_folder, std::map<std::string, GLuint>* textures = nullptr)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, base_folder))
	{
		throw std::runtime_error(warn + err);
	}

	// Load materials/textures
	std::map<int, GLuint> mat_to_tex_map; 

	if (textures != nullptr && !materials.empty()) {
		for (size_t m = 0; m < materials.size(); m++) {
			const tinyobj::material_t& mat = materials[m];

			// Check for diffuse texture
			if (!mat.diffuse_texname.empty()) {
				std::string tex_path = std::string(base_folder) + "/" + mat.diffuse_texname;

				// Check if already loaded texture
				if (textures->find(tex_path) == textures->end()) {
					// Load the texture using setup_texture function
					GLuint tex_id = setup_texture(tex_path.c_str());
					(*textures)[tex_path] = tex_id;
				}

				// Store the mapping from material index to texture ID
				mat_to_tex_map[m] = (*textures)[tex_path];
				printf("Material %zu uses texture: %s\n", m, tex_path.c_str());
			}
		}
	}


	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	for (const auto& shape : shapes)
	{
		size_t index_offset = 0;

		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			int fv = shape.mesh.num_face_vertices[f];
			int mat_id = shape.mesh.material_ids[f];

			std::vector<vertex> face_verts;

			for (size_t v = 0; v < fv; v++)
			{
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
				vertex vert{};
				// Positions
				vert.pos = {
					attrib.vertices[3 * idx.vertex_index + 0],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2],
					1.f
				};
				// Normals
				if (idx.normal_index >= 0) {
					vert.nor = {
						attrib.normals[3 * idx.normal_index + 0],
						attrib.normals[3 * idx.normal_index + 1],
						attrib.normals[3 * idx.normal_index + 2]
					};
				}
				else {
					// Fallback normal
					vert.nor = glm::vec3(0.0f); 
				}

				// Textures
				if (idx.texcoord_index >= 0) {
					vert.tex = {
						attrib.texcoords[2 * idx.texcoord_index + 0],
						attrib.texcoords[2 * idx.texcoord_index + 1]
					};
				}
				else {
					// Fallback tex coords
					vert.tex = glm::vec2(0.0f); 
				}

				// Initialize tangent and bitangent to zero 
				vert.tangent = glm::vec3(0.0f);
				vert.bitangent = glm::vec3(0.0f);

				// Materials
				if (mat_id >= 0 && mat_id < static_cast<int>(materials.size())) {
					const auto& mat = materials[mat_id];
					vert.col = {
						mat.diffuse[0],
						mat.diffuse[1],
						mat.diffuse[2]
					};
				}
				else {
					// Default white
					vert.col = glm::vec3(1.0f); 
				}

				face_verts.push_back(vert);
			}

			index_offset += fv;

			// Only support triangles
			if (face_verts.size() == 3)
			{
				triangle tri;
				tri.v1 = face_verts[0];
				tri.v2 = face_verts[1];
				tri.v3 = face_verts[2];

				// Calculate tangent and bitangent for triangle
				calculateTangentSpace(tri.v1, tri.v2, tri.v3);

				tri.reflect = false;
				tri.primID = io_tris->size();

				// Assign texture ID if available
				if (mat_id >= 0 && mat_to_tex_map.find(mat_id) != mat_to_tex_map.end()) {
					tri.texID = mat_to_tex_map[mat_id];
				}
				else {
					// No texture or default texture
					tri.texID = 0;
				}

				io_tris->push_back(tri);
			}
		}
	}

	printf("successfully parsed %s and read %d triangles \n", filename, io_tris->size());
	return 0;
}

vector<GLfloat> tri_to_fl_array(const std::vector<triangle>& triangles, bool uses_normal) {
	// Each triangle has 3 vertices
	// Each vertex has 17 floats: (x,y,z,r,g,b,s,t,nx,ny,nz,tx,ty,tz,bx,by,bz)
	std::vector<GLfloat> tri_array;
	tri_array.reserve(triangles.size() * 3 * 17); 

	for (const triangle& tri : triangles) {
		const vertex* verts[3] = { &tri.v1, &tri.v2, &tri.v3 };

		for (int i = 0; i < 3; ++i) {
			const vertex& v = *verts[i];
			// Position
			tri_array.push_back(v.pos.x);
			tri_array.push_back(v.pos.y);
			tri_array.push_back(v.pos.z);

			// Color
			tri_array.push_back(v.col.r);
			tri_array.push_back(v.col.g);
			tri_array.push_back(v.col.b);

			// Texture coordinates
			tri_array.push_back(v.tex.x);
			tri_array.push_back(v.tex.y);

			// Normal
			tri_array.push_back(v.nor.x);
			tri_array.push_back(v.nor.y);
			tri_array.push_back(v.nor.z);
			// For objects that use normal mapping.
			if (uses_normal) {
				// Tangent
				tri_array.push_back(v.tangent.x);
				tri_array.push_back(v.tangent.y);
				tri_array.push_back(v.tangent.z);

				// Bitangent
				tri_array.push_back(v.bitangent.x);
				tri_array.push_back(v.bitangent.y);
				tri_array.push_back(v.bitangent.z);
				
			}
		}
	}
	
	

	return tri_array;
}