//
// Created by Daniel Adámek - daniel@nullptr.cz on 14.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#ifndef MAZEGENERATOR_HPP
#define MAZEGENERATOR_HPP

#include <opencv2/core.hpp>
#include <random>
#include <algorithm>
#include <glm/vec2.hpp>

#include "Map.hpp"

class MazeGenerator {
public:
    explicit MazeGenerator(int rows, int cols, int corridorWidth = 2, unsigned seed = std::random_device{}());

    void generate(const std::shared_ptr<Map> &outMap, glm::vec2& outStart, glm::vec2& outEnd);

    static glm::vec2 get_random_empty_location(const std::shared_ptr<Map> &map);

private:
    enum class Orientation { Horizontal, Vertical };

    void divide(int x, int y, int width, int height, Orientation orientation, std::shared_ptr<Map> map);

    Orientation chooseOrientation(int width, int height);

    int m_rows;
    int m_cols;
    int m_corridor;
    std::default_random_engine m_rng;
};


#endif //MAZEGENERATOR_HPP
