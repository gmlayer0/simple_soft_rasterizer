//
// Created by dofingert on 2023/6/16.
//

#ifndef SIMPLE_SOFT_RASTERIZER_VERTEX_H
#define SIMPLE_SOFT_RASTERIZER_VERTEX_H

#include <glm/glm.hpp>

class Vertex {
public:
    glm::vec4 position;
    glm::vec2 texcoord;

    Vertex() = default;

    ~Vertex() = default;

    Vertex(
            const glm::vec4 &_pos,
            const glm::vec2 &_tex
    ) :
            position(_pos), texcoord(_tex) {}

    Vertex(
            const glm::vec3 &_pos,
            const glm::vec2 &_tex = glm::vec2(0, 0)
    ) :
            position(_pos, 1.0f), texcoord(_tex) {}

    Vertex(const Vertex &v) : position(v.position), texcoord(v.texcoord) {}
};

#endif //SIMPLE_SOFT_RASTERIZER_VERTEX_H
