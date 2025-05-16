//
// Created by Daniel Adámek - daniel@nullptr.cz on 14.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#include "MazeGenerator.hpp"

#include <iostream>
#include <glm/vec2.hpp>

#include "Map.hpp"

MazeGenerator::MazeGenerator(int rows, int cols, int corridorWidth, unsigned seed)
    : m_rows(rows),
      m_cols(cols),
      m_corridor(std::max(1, corridorWidth)),
      m_rng(seed) {
    if (m_rows % 2 == 0) ++m_rows;
    if (m_cols % 2 == 0) ++m_cols;
    if (m_corridor < 1) m_corridor = 1;
}

void MazeGenerator::generate(const std::shared_ptr<Map>& outMap, glm::vec2& outStart, glm::vec2& outEnd) {
    // clear whole map
    outMap->fill(CELL_EMPTY);

    // walls along border
    for (int x = 0; x < m_cols; ++x) {
        outMap->set(x, 0,            CELL_WALL);
        outMap->set(x, m_rows - 1,   CELL_WALL);
    }
    for (int y = 0; y < m_rows; ++y) {
        outMap->set(0,          y,   CELL_WALL);
        outMap->set(m_cols - 1, y,   CELL_WALL);
    }

    // recursive division
    divide(1, 1,
           m_cols - 2, m_rows - 2,
           chooseOrientation(m_cols - 2, m_rows - 2),
           outMap);

    // pick start/end on empty cells
    std::uniform_int_distribution<int> pickRow(1, m_rows - 2);
    std::uniform_int_distribution<int> pickCol(1, m_cols - 2);

    do {
        outStart = { pickCol(m_rng), pickRow(m_rng) };
    } while (outMap->get(outStart.x, outStart.y, CELL_WALL) == CELL_WALL);

    do {
        outEnd = { pickCol(m_rng), pickRow(m_rng) };
    } while ((outEnd == outStart) ||
             outMap->get(outEnd.x, outEnd.y, CELL_WALL) == CELL_WALL);

    // mark end cell
    outMap->set(outEnd.x, outEnd.y, CELL_END);
    // mark start cell
    outMap->set(outStart.x, outStart.y, CELL_START);
}

void MazeGenerator::divide(int x, int y, int width, int height, Orientation orientation, const std::shared_ptr<Map> map) {
    // stop if the area is too small
    if (width < (m_corridor * 2 + 1) || height < (m_corridor * 2 + 1))
        return;

    std::uniform_int_distribution<int> wallDist;
    std::uniform_int_distribution<int> passageDist;

    if (orientation == Orientation::Vertical) {
        std::uniform_int_distribution<int> wallDist(
            x + m_corridor,
            x + width - m_corridor - 1);
        int wallX = wallDist(m_rng);

        for (int row = y; row < y + height; ++row)
            (*map)(wallX, row) = '#';

        std::uniform_int_distribution<int> passageDist(
        y,
        y + height - m_corridor);
        int gapY = passageDist(m_rng);
        for (int dy = 0; dy < m_corridor; ++dy)
            (*map)(wallX, gapY + dy) = '.';

        // recurse left / right
        divide(x,          y,             wallX - x,               height,
               chooseOrientation(wallX - x, height),               map);
        divide(wallX + 1,  y,             x + width - wallX - 1,   height,
               chooseOrientation(x + width - wallX - 1, height),   map);
    }
    else { // Horizontal
        std::uniform_int_distribution<int> wallDist(
        y + m_corridor,
        y + height - m_corridor - 1);
        int wallY = wallDist(m_rng);

        for (int col = x; col < x + width; ++col)
            (*map)(col, wallY) = '#';

        std::uniform_int_distribution<int> passageDist(
        x,
        x + width - m_corridor);
        int gapX = passageDist(m_rng);
        for (int dx = 0; dx < m_corridor; ++dx)
            (*map)(gapX + dx, wallY) = '.';

        // recurse top / bottom
        divide(x,y, width, wallY - y, chooseOrientation(width, wallY - y), map);
        divide(x, wallY + 1, width, y + height - wallY - 1, chooseOrientation(width, y + height - wallY - 1),   map);
    }
}


MazeGenerator::Orientation MazeGenerator::chooseOrientation(int width, int height) {
    if (width < height)          return Orientation::Horizontal;
    else if (height < width)     return Orientation::Vertical;
    else {
        std::uniform_int_distribution<int> pick(0, 1);
        return pick(m_rng) ? Orientation::Horizontal : Orientation::Vertical;
    }
}