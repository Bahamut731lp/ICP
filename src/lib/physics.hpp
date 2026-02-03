#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    bool intersects(const AABB &other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    bool contains(const glm::vec3 &point) const {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }
};