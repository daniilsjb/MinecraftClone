#include "BlockMesh.hpp"
#include "ChunkMesh.hpp"
#include "Block.hpp"

#include <array>

constexpr std::array<float, 8 * 3> CUBE_COORDINATES = {
    0.0f, 0.0f, 0.0f, // 0
    1.0f, 0.0f, 0.0f, // 1
    1.0f, 1.0f, 0.0f, // 2
    0.0f, 1.0f, 0.0f, // 3

    0.0f, 0.0f, 1.0f, // 4
    1.0f, 0.0f, 1.0f, // 5
    1.0f, 1.0f, 1.0f, // 6
    0.0f, 1.0f, 1.0f, // 7
};

constexpr std::array<float, 6 * 3> FACE_CENTERS = {
    0.5f, 0.5f, 0.0f, // North
    0.5f, 0.5f, 1.0f, // South
    1.0f, 0.5f, 0.5f, // East
    0.0f, 0.5f, 0.5f, // West
    0.5f, 1.0f, 0.5f, // Top
    0.5f, 0.0f, 0.5f, // Bottom
};

constexpr std::array<unsigned int, 6> FACE_INDICES = {
    0, 1, 2, 2, 3, 0
};

constexpr std::array<unsigned int, 4> UNIQUE_INDICES = {
    0, 1, 2, 4,
};

constexpr std::array<unsigned int, 6 * 6> CUBE_INDICES = {
    1, 0, 3, 3, 2, 1, // North
    4, 5, 6, 6, 7, 4, // South
    5, 1, 2, 2, 6, 5, // East
    0, 4, 7, 7, 3, 0, // West
    2, 3, 7, 7, 6, 2, // Up
    0, 1, 5, 5, 4, 0, // Down
};

constexpr std::array<unsigned int, 2 * 6> SPRITE_INDICES = {
    5, 0, 3, 3, 6, 5,
    // 3, 0, 5, 3, 5, 6,

    1, 4, 7, 7, 2, 1,
    // 4, 2, 1, 4, 2, 7,
};

constexpr std::array<float, 4 * 2> CUBE_UVS = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
};

void BlockMeshParams::AppendFace(ChunkMesh& target) {
    if (transparent) {
        glm::vec3 face_position = chunk_position + position + glm::vec3 {
            FACE_CENTERS[(direction * 3) + 0],
            FACE_CENTERS[(direction * 3) + 1],
            FACE_CENTERS[(direction * 3) + 2],
        };
        size_t index_start = target.m_index_count;
        target.m_faces.push_back({ face_position, index_start, 0.0f });
    }

    // Emit vertices
    for (int i = 0; i < 4; i++) {
        const float* coords = &CUBE_COORDINATES[CUBE_INDICES[(direction * 6) + UNIQUE_INDICES[i]] * 3];
        const float* uv = &CUBE_UVS[i * 2];
        
        glm::vec3 vertex_position = {
            position.x + coords[0],
            position.y + coords[1] * (liquid ? 0.9f : 1.0f),
            position.z + coords[2],
        };

        glm::vec2 vertex_tex_coords = {
            uv[0] == 1.0f ? uv_max.x : uv_min.x,
            uv[1] == 1.0f ? uv_max.y : uv_min.y,
        };

        glm::vec3 color = { 1.0f, 1.0f, 1.0f };
        switch (direction) {
            case UP: color = { 1.0f, 1.0f, 1.0f }; break;

            case NORTH:
            case SOUTH: color = { 0.86f, 0.86f, 0.86f }; break;

            case WEST:
            case EAST: color = { 0.8f, 0.8f, 0.8f }; break;

            case DOWN: color = { 0.6f, 0.6f, 0.6f }; break;
        }

        target.m_vertices.push_back({ vertex_position, vertex_tex_coords, color });
    }

    // Emit indices
    for (unsigned int i = 0; i < 6; i++) {
        target.m_indices.push_back({ target.m_vertex_count + FACE_INDICES[i] });
    }

    target.m_vertex_count += 4;
    target.m_index_count += 6;
}

void BlockMeshParams::AppendSprite(ChunkMesh& target) {
    for (size_t i = 0; i < 2; i++) {
        glm::vec3 face_position = chunk_position + position;
        size_t index_start = target.m_index_count + (i * 6);
        target.m_faces.push_back({ face_position, index_start, 0.0f });
    }

    for (int i = 0; i < 8; i++) {
        const float* coords = &CUBE_COORDINATES[i * 3];
        const float* uv = &CUBE_UVS[(i % 4) * 2];

        glm::vec3 vertex_position = {
            position.x + coords[0],
            position.y + coords[1],
            position.z + coords[2],
        };

        glm::vec2 vertex_tex_coords = {
            uv[0] == 1.0f ? uv_max.x : uv_min.x,
            uv[1] == 1.0f ? uv_max.y : uv_min.y,
        };

        target.m_vertices.push_back({ vertex_position, vertex_tex_coords, glm::vec3(1.0f) });
    }

    for (int i = 0; i < 12; i++) {
        target.m_indices.push_back({ target.m_vertex_count + SPRITE_INDICES[i] });
    }

    target.m_vertex_count += 8;
    target.m_index_count += 12;
}
