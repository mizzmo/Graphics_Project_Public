#pragma once
#define _USE_MATH_DEFINES 
#include "math.h"

#define DEG2RAD(n)  ((n) * (M_PI / 180.0f))

std::vector<GLfloat> form_cylinder(int num_segments, float radius, float height, int& out_vertex_count) {
    const float half_height = height / 2.0f;
    const float offset = 360.0f / num_segments;

    // 3 floats for position (x, y, z)
    // 3 floats for color (r, g, b)
    // 2 floats for texture coordinates (u, v)
    // 3 floats for normal (nx, ny, nz)
    // 11 floats per vertex

    // Calculate correct number of vertices
    // 3 for top, 3 for bottom, 6 for sides
    const int vertices_per_segment = 12; 
    const int total_vertices = vertices_per_segment * num_segments;

    // 11 floats per vertex (pos + color + tex + normal)
    std::vector<GLfloat> vert_array;
    vert_array.reserve(total_vertices * 11);

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
        float x1 = radius * cos(angle_rad1);
        float y1 = radius * sin(angle_rad1);
        float x2 = radius * cos(angle_rad2);
        float y2 = radius * sin(angle_rad2);

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
        // Texture coordinates (center of circle)
        vert_array.push_back(0.5f);
        vert_array.push_back(0.5f);
        // Normal (pointing up)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(1.0f);

        // First edge point
        // Position
        vert_array.push_back(x1);
        vert_array.push_back(y1);
        vert_array.push_back(half_height);
        // Color (top face - light blue)
        vert_array.push_back(0.7f);
        vert_array.push_back(0.7f);
        vert_array.push_back(1.0f);
        // Texture coordinates
        vert_array.push_back(tex_u1);
        vert_array.push_back(tex_v1);
        // Normal (pointing up)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(1.0f);

        // Second edge point
        // Position
        vert_array.push_back(x2);
        vert_array.push_back(y2);
        vert_array.push_back(half_height);
        // Color (top face - light blue)
        vert_array.push_back(0.7f);
        vert_array.push_back(0.7f);
        vert_array.push_back(1.0f);
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
        // Texture coordinates (center of circle)
        vert_array.push_back(0.5f);
        vert_array.push_back(0.5f);
        // Normal (pointing down)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(-1.0f);

        // Second edge point (swapped for correct winding)
        // Position
        vert_array.push_back(x2);
        vert_array.push_back(y2);
        vert_array.push_back(-half_height);
        // Color (bottom face - darker blue)
        vert_array.push_back(0.3f);
        vert_array.push_back(0.3f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_u2);
        vert_array.push_back(tex_v2);
        // Normal (pointing down)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(-1.0f);

        // First edge point (swapped for correct winding)
        // Position
        vert_array.push_back(x1);
        vert_array.push_back(y1);
        vert_array.push_back(-half_height);
        // Color (bottom face - darker blue)
        vert_array.push_back(0.3f);
        vert_array.push_back(0.3f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_u1);
        vert_array.push_back(tex_v1);
        // Normal (pointing down)
        vert_array.push_back(0.0f);
        vert_array.push_back(0.0f);
        vert_array.push_back(-1.0f);

        // --- Side quad (2 triangles) ---
        // First triangle
        // Bottom-left vertex
        // Position
        vert_array.push_back(x1);
        vert_array.push_back(y1);
        vert_array.push_back(-half_height);
        // Color (side faces - gradient based on position)
        vert_array.push_back(0.5f + nx1 * 0.5f);
        vert_array.push_back(0.5f + ny1 * 0.5f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_side_u1);
        vert_array.push_back(0.0f);
        // Normal (pointing outward)
        vert_array.push_back(nx1);
        vert_array.push_back(ny1);
        vert_array.push_back(0.0f);

        // Top-left vertex
        // Position
        vert_array.push_back(x1);
        vert_array.push_back(y1);
        vert_array.push_back(half_height);
        // Color (side faces - gradient based on position)
        vert_array.push_back(0.5f + nx1 * 0.5f);
        vert_array.push_back(0.5f + ny1 * 0.5f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_side_u1);
        vert_array.push_back(1.0f);
        // Normal (pointing outward)
        vert_array.push_back(nx1);
        vert_array.push_back(ny1);
        vert_array.push_back(0.0f);

        // Top-right vertex
        // Position
        vert_array.push_back(x2);
        vert_array.push_back(y2);
        vert_array.push_back(half_height);
        // Color (side faces - gradient based on position)
        vert_array.push_back(0.5f + nx2 * 0.5f);
        vert_array.push_back(0.5f + ny2 * 0.5f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_side_u2);
        vert_array.push_back(1.0f);
        // Normal (pointing outward)
        vert_array.push_back(nx2);
        vert_array.push_back(ny2);
        vert_array.push_back(0.0f);

        // Second triangle
        // Bottom-left vertex
        // Position
        vert_array.push_back(x1);
        vert_array.push_back(y1);
        vert_array.push_back(-half_height);
        // Color (side faces - gradient based on position)
        vert_array.push_back(0.5f + nx1 * 0.5f);
        vert_array.push_back(0.5f + ny1 * 0.5f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_side_u1);
        vert_array.push_back(0.0f);
        // Normal (pointing outward)
        vert_array.push_back(nx1);
        vert_array.push_back(ny1);
        vert_array.push_back(0.0f);

        // Top-right vertex
        // Position
        vert_array.push_back(x2);
        vert_array.push_back(y2);
        vert_array.push_back(half_height);
        // Color (side faces - gradient based on position)
        vert_array.push_back(0.5f + nx2 * 0.5f);
        vert_array.push_back(0.5f + ny2 * 0.5f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_side_u2);
        vert_array.push_back(1.0f);
        // Normal (pointing outward)
        vert_array.push_back(nx2);
        vert_array.push_back(ny2);
        vert_array.push_back(0.0f);

        // Bottom-right vertex
        // Position
        vert_array.push_back(x2);
        vert_array.push_back(y2);
        vert_array.push_back(-half_height);
        // Color (side faces - gradient based on position)
        vert_array.push_back(0.5f + nx2 * 0.5f);
        vert_array.push_back(0.5f + ny2 * 0.5f);
        vert_array.push_back(0.7f);
        // Texture coordinates
        vert_array.push_back(tex_side_u2);
        vert_array.push_back(0.0f);
        // Normal (pointing outward)
        vert_array.push_back(nx2);
        vert_array.push_back(ny2);
        vert_array.push_back(0.0f);

        angle += offset;
    }

    out_vertex_count = total_vertices;
    return vert_array;
}