//
// Created by dofingert on 2023/6/16.
//

#ifndef SIMPLE_SOFT_RASTERIZER_UTILS_H
#define SIMPLE_SOFT_RASTERIZER_UTILS_H

#include <glm/glm.hpp>
static inline glm::vec4 lerp(const glm::vec4 &v1,const glm::vec4 &v2,float factor) {
    return (1.0f - factor) * v1 + factor * v2;
}
static inline glm::vec3 lerp(const glm::vec3 &v1, const glm::vec3 &v2, float factor) {
    return (1.0f - factor)*v1 + factor * v2;
}
static inline glm::vec2 lerp(const glm::vec2 &v1, const glm::vec2 &v2, float factor) {
    return (1.0f - factor)*v1 + factor * v2;
}

#endif //SIMPLE_SOFT_RASTERIZER_UTILS_H
