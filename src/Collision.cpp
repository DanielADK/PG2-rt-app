#include "Collision.hpp"
#include <cmath>
#include <iostream>

Collision::Collision(const std::shared_ptr<Map> map, float playerRadius)
    : m_map(map),
      m_radius(playerRadius)
{}

bool Collision::isCellBlocked(int cx, int cz) const {
    return m_map->get(cx, cz, CELL_EMPTY) == CELL_WALL;
}

bool Collision::isPositionBlocked(const glm::vec3& pos) const {
    const int xL = static_cast<int>(std::floor(pos.x - m_radius));
    const int xR = static_cast<int>(std::floor(pos.x + m_radius));
    const int zT = static_cast<int>(std::floor(pos.z - m_radius));
    const int zB = static_cast<int>(std::floor(pos.z + m_radius));

    // test 4 cells, short-circuit on first hit
    return isCellBlocked(xL, zT) ||
           isCellBlocked(xL, zB) ||
           isCellBlocked(xR, zT) ||
           isCellBlocked(xR, zB);
}

glm::vec3 Collision::movement(const glm::vec3& currentPos, const glm::vec3& desiredMove) const {
    glm::vec3 newPos = currentPos;

    const glm::vec3 moveX(desiredMove.x, 0.0f, 0.0f);
    if (!isPositionBlocked(newPos + moveX))
        newPos += moveX;

    const glm::vec3 moveZ(0.0f, 0.0f, desiredMove.z);
    if (!isPositionBlocked(newPos + moveZ))
        newPos += moveZ;

    newPos.y += desiredMove.y;

    return newPos;
}
