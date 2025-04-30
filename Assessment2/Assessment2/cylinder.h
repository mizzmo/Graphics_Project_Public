#pragma once
#define _USE_MATH_DEFINES 
#include "math.h"

#define DEG2RAD(n)  ((n) * (M_PI / 180.0f))

std::vector<GLfloat> form_cylinder(int num_segments, float top_radius, float bottom_radius, float height, float opacity, int& out_vertex_count) {
    const float half_height = height / 2.0f;
    const float offset = 360.0f / num_segments;

    // 3 floats for position (x, y, z)
    // 4 floats for color (r, g, b, a)
    // 2 floats for texture coordinates (u, v)
    // 3 floats for normal (nx, ny, nz)
    // 12 floats per vertex

    // Calculate correct number of vertices
    // 3 for top, 3 for bottom, 6 for sides
    const int vertices_per_segment = 12; 
    const int total_vertices = vertices_per_segment * num_segments;

    // 12 floats per vertex (pos + color + tex + normal)
    std::vector<GLfloat> vert_array;
    vert_array.reserve(total_vertices * 12);

    // Pre-calculate center points
    const float center_top_x = 0.0f;
    const float center_top_y = 0.0f;
    const float center_top_z = half_height;

    const float center_bottom_x = 0.0f;
    const float center_bottom_y = 0.0f;
    const float center_bottom_z = -half_height;

    float angle = 0.0f;
    for (int i = 0; i < num_segments; i++) {
        float angle_rad1 = DEG2RAD(angle);
        float angle_rad2 = DEG2RAD(angle + offset);

        // Calculate points for current segment
        float x1_top = top_radius * cos(angle_rad1);
        float y1_top = top_radius * sin(angle_rad1);
        float x2_top = top_radius * cos(angle_rad2);
        float y2_top = top_radius * sin(angle_rad2);

        float x1_bottom = bottom_radius * cos(angle_rad1);
        float y1_bottom = bottom_radius * sin(angle_rad1);
        float x2_bottom = bottom_radius * cos(angle_rad2);
        float y2_bottom = bottom_radius * sin(angle_rad2);


        // Calculate normals for side vertices
        // Normals point outward from center
        float nx1 = cos(angle_rad1);  
        float ny1 = sin(angle_rad1);
        float nx2 = cos(angle_rad2);
        float ny2 = sin(angle_rad2);

        // Calculate texture coordinates
        // For top and bottom circles
        // Map [-1,1] to [0,1]
        float tex_u1 = (cos(angle_rad1) * 0.5f) + 0.5f;  
        float tex_v1 = (sin(angle_rad1) * 0.5f) + 0.5f;
        float tex_u2 = (cos(angle_rad2) * 0.5f) + 0.5f;
        float tex_v2 = (sin(angle_rad2) * 0.5f) + 0.5f;

        // For side quads - Map based on angle/height
        float tex_side_u1 = static_cast<float>(i) / num_segments;
        float tex_side_u2 = static_cast<float>(i + 1) / num_segments;

        // --- Top circle triangle ---
        // Center point
        // Position
        vert_array.push_back(center_top_x);
        vert_array.push_back(center_top_y);
        vert_array.push_back(center_top_z);
        // Color (top face - light blue)
        vert_array.push_back(0.7f);
        vert_array.push_back(0.7f);
        vert_array.push_back(1.0f);
        vert_array.push_back(opacity);
        // Texture coordinates (center of circle)
        vert_array.push_back(0.5f);
        vert_array.push_back(0.5f);
        // Normal (pointing up)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(1.0f);

        // First edge point
        // Position
        vert_array.push_back(x1_top);
        vert_array.push_back(y1_top);
        vert_array.push_back(half_height);
        // Color (top face - light blue)
        vert_array.push_back(0.7f);
        vert_array.push_back(0.7f);
        vert_array.push_back(1.0f);
        vert_array.push_back(opacity);
        // Texture coordinates
        vert_array.push_back(tex_u1);
        vert_array.push_back(tex_v1);
        // Normal (pointing up)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(1.0f);

        // Second edge point
        // Position
        vert_array.push_back(x2_top);
        vert_array.push_back(y2_top);
        vert_array.push_back(half_height);
        // Color (top face - light blue)
        vert_array.push_back(0.7f);
        vert_array.push_back(0.7f);
        vert_array.push_back(1.0f);
        vert_array.push_back(opacity);
        // Texture coordinates
        vert_array.push_back(tex_u2);
        vert_array.push_back(tex_v2);
        // Normal (pointing up)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(1.0f);

        // --- Bottom circle triangle ---
        // Center point
        // Position
        vert_array.push_back(center_bottom_x);
        vert_array.push_back(center_bottom_y);
        vert_array.push_back(center_bottom_z);
        // Color (bottom face - darker blue)
        vert_array.push_back(0.3f);
        vert_array.push_back(0.3f);
        vert_array.push_back(0.7f);
        vert_array.push_back(opacity);
        // Texture coordinates (center of circle)
        vert_array.push_back(0.5f);
        vert_array.push_back(0.5f);
        // Normal (pointing down)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(-1.0f);

        // Second edge point
        // Position
        vert_array.push_back(x2_bottom);
        vert_array.push_back(y2_bottom);
        vert_array.push_back(-half_height);
        // Color (bottom face - darker blue)
        vert_array.push_back(0.3f);
        vert_array.push_back(0.3f);
        vert_array.push_back(0.7f);
        vert_array.push_back(opacity);
        // Texture coordinates
        vert_array.push_back(tex_u2);
        vert_array.push_back(tex_v2);
        // Normal (pointing down)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(-1.0f);

        // First edge point
        // Position
        vert_array.push_back(x1_bottom);
        vert_array.push_back(y1_bottom);
        vert_array.push_back(-half_height);
        // Color (bottom face - darker blue)
        vert_array.push_back(0.3f);
        vert_array.push_back(0.3f);
        vert_array.push_back(0.7f);
        vert_array.push_back(opacity);
        // Texture coordinates
        vert_array.push_back(tex_u1);
        vert_array.push_back(tex_v1);
        // Normal (pointing down)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(-1.0f);

        // --- Side quad (2 triangles) ---
        // First triangle
        // Define the four vertices of the quad
        glm::vec3 pos1_bottom(x1_bottom, y1_bottom, -half_height);
        glm::vec3 pos1_top(x1_top, y1_top, half_height);
        glm::vec3 pos2_top(x2_top, y2_top, half_height);
        glm::vec3 pos2_bottom(x2_bottom, y2_bottom, -half_height);

        // Calculate two edge vectors that span the quad
        glm::vec3 edge_vertical1 = pos1_top - pos1_bottom;
        glm::vec3 edge_horizontal = pos2_bottom - pos1_bottom;
        glm::vec3 edge_vertical2 = pos2_top - pos2_bottom;

        // Calculate the normal using cross product
        glm::vec3 normal = glm::normalize(glm::cross(edge_horizontal, edge_vertical1));

        // ---- Colour Gradient ----
        // Define start and end gradient colours
        glm::vec3 bottomColor = glm::vec3(0.0f, 0.2f, 0.4f); 
        glm::vec3 topColor = glm::vec3(0.4f, 0.8f, 0.6f);    

        // Interpolate colour
        // For each vertex, calculate a t value based on its z coordinate
        // // Use t to linearly interpolate a new gradient colour between the set bounds
		// Normalise to be between 0 and 1 and apply
        auto interpolateColor = [&](float t) {
            return bottomColor + t * (topColor - bottomColor);
        };

        // ---- Opacity Gradient ----
        auto calculateOpacity = [&](float z_pos) {
            // Normalize z from [-half_height, half_height] to [0, 1]
            float normalized_z = (z_pos + half_height) / (2.0f * half_height);
            // Interpolate opacity from 0.1 (at z=0) to 0.5 (at z=1)
            return 0.1f + normalized_z * (0.8f - 0.f);
        };


        // Position
        vert_array.push_back(pos1_bottom.x);
        vert_array.push_back(pos1_bottom.y);
        vert_array.push_back(pos1_bottom.z);
        // Color 
        float t_bottom = (pos1_bottom.z + half_height) / (2.0f * half_height);
        glm::vec3 color_bottom = interpolateColor(t_bottom);
        vert_array.push_back(color_bottom.r);
        vert_array.push_back(color_bottom.g);
        vert_array.push_back(color_bottom.b);
        vert_array.push_back(calculateOpacity(pos1_bottom.z));
        // Texture coordinates
        vert_array.push_back(tex_side_u1);
        vert_array.push_back(0.0f);
        // Normal 
        vert_array.push_back(normal.x);
        vert_array.push_back(normal.y);
        vert_array.push_back(normal.z);

        // Position
        vert_array.push_back(pos1_top.x);
        vert_array.push_back(pos1_top.y);
        vert_array.push_back(pos1_top.z);
        // Color
        float t_top = (pos1_top.z + half_height) / (2.0f * half_height); 
        glm::vec3 color_top = interpolateColor(t_top);
        vert_array.push_back(color_top.r);
        vert_array.push_back(color_top.g);
        vert_array.push_back(color_top.b);
        vert_array.push_back(calculateOpacity(pos1_top.z));
        // Texture coordinates
        vert_array.push_back(tex_side_u1);
        vert_array.push_back(1.0f);
        // Normal (pointing outward)
        vert_array.push_back(normal.x);
        vert_array.push_back(normal.y);
        vert_array.push_back(normal.z);

        // Bottom-right vertex
        // Position
        vert_array.push_back(pos2_bottom.x);
        vert_array.push_back(pos2_bottom.y);
        vert_array.push_back(pos2_bottom.z);
        // Color 
        float t_bottom_right = (pos2_bottom.z + half_height) / (2.0f * half_height); 
        glm::vec3 color_bottom_right = interpolateColor(t_bottom_right);
        vert_array.push_back(color_bottom_right.r);
        vert_array.push_back(color_bottom_right.g);
        vert_array.push_back(color_bottom_right.b);
        vert_array.push_back(calculateOpacity(pos2_bottom.z));
        // Texture coordinates
        vert_array.push_back(tex_side_u2);
        vert_array.push_back(0.0f);
        // Normal (pointing outward)
        vert_array.push_back(normal.x);
        vert_array.push_back(normal.y);
        vert_array.push_back(normal.z);

        // Second triangle
        // Top-left vertex 
        // Position
        vert_array.push_back(pos1_top.x);
        vert_array.push_back(pos1_top.y);
        vert_array.push_back(pos1_top.z);
        // Color 
        vert_array.push_back(color_top.r);
        vert_array.push_back(color_top.g);
        vert_array.push_back(color_top.b);
        vert_array.push_back(calculateOpacity(pos1_top.z));
        // Texture coordinates
        vert_array.push_back(tex_side_u1);
        vert_array.push_back(1.0f);
        // Normal (pointing outward)
        vert_array.push_back(normal.x);
        vert_array.push_back(normal.y);
        vert_array.push_back(normal.z);

        // Bottom-right vertex 
        // Position
        vert_array.push_back(pos2_bottom.x);
        vert_array.push_back(pos2_bottom.y);
        vert_array.push_back(pos2_bottom.z);
        // Color (side faces - gradient based on position)
        vert_array.push_back(color_bottom_right.r);
        vert_array.push_back(color_bottom_right.g);
        vert_array.push_back(color_bottom_right.b);
        vert_array.push_back(calculateOpacity(pos2_bottom.z));
        // Texture coordinates
        vert_array.push_back(tex_side_u2);
        vert_array.push_back(0.0f);
        // Normal (pointing outward)
        vert_array.push_back(normal.x);
        vert_array.push_back(normal.y);
        vert_array.push_back(normal.z);

        // Top-right vertex 
        // Position
        vert_array.push_back(pos2_top.x);
        vert_array.push_back(pos2_top.y);
        vert_array.push_back(pos2_top.z);
        // Color (side faces - gradient based on position)
        float t_top_right = (pos2_top.z + half_height) / (2.0f * half_height); 
        glm::vec3 color_top_right = interpolateColor(t_top_right);
        vert_array.push_back(color_top_right.r);
        vert_array.push_back(color_top_right.g);
        vert_array.push_back(color_top_right.b);
        vert_array.push_back(calculateOpacity(pos2_top.z));
        // Texture coordinates
        vert_array.push_back(tex_side_u2);
        vert_array.push_back(1.0f);
        // Normal (pointing outward)
        vert_array.push_back(normal.x);
        vert_array.push_back(normal.y);
        vert_array.push_back(normal.z);

        angle += offset;
    }

    out_vertex_count = total_vertices;
    return vert_array;
}