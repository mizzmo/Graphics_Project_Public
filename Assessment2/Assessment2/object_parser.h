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
};

struct triangle
{
public:
	vertex v1, v2, v3;
	bool reflect;
	int primID;
	int texID;
};

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

	// Load materials/textures if textures map is provided
	std::map<int, GLuint> mat_to_tex_map; // Maps material index to texture ID

	if (textures != nullptr && !materials.empty()) {
		for (size_t m = 0; m < materials.size(); m++) {
			const tinyobj::material_t& mat = materials[m];

			// Check for diffuse texture
			if (!mat.diffuse_texname.empty()) {
				std::string tex_path = std::string(base_folder) + "/" + mat.diffuse_texname;

				// Check if we've already loaded this texture
				if (textures->find(tex_path) == textures->end()) {
					// Load the texture using your setup_texture function
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
					vert.nor = glm::vec3(0.0f); // fallback normal
				}

				// Textures
				if (idx.texcoord_index >= 0) {
					vert.tex = {
						attrib.texcoords[2 * idx.texcoord_index + 0],
						attrib.texcoords[2 * idx.texcoord_index + 1]
					};
				}
				else {
					vert.tex = glm::vec2(0.0f); // fallback tex coords
				}

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
					vert.col = glm::vec3(1.0f); // default white
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