//
// Created by Daniel Adámek - daniel@nullptr.cz on 14.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <opencv2/core.hpp>
#include <glm/glm.hpp>

#include "Map.hpp"

class Collision {
public:
    Collision(const std::shared_ptr<Map> map, float playerRadius = 0.25f);

    bool isPositionBlocked(const glm::vec3& pos) const;

    glm::vec3 movement(const glm::vec3& currentPos, const glm::vec3& desiredMove) const;

private:
    bool isCellBlocked(int cx, int cz) const;

    const std::shared_ptr<Map> m_map;
    float m_radius;
};



#endif //COLLISIONS_HPP
