//
// Created by Daniel Adámek - daniel@nullptr.cz on 9.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <cstdint>

const char CELL_WALL = '#';
const char CELL_EMPTY = 0;
const char CELL_END = 'e';
const char CELL_START = 's';

class Map {
public:
    using value_type = uint8_t;

    Map(const size_t w, const size_t h, value_type fill = '.');

    value_type& operator()(const size_t x, const size_t y);
    value_type  operator()(const size_t x, const size_t y) const;

    value_type at(const size_t x, const size_t y) const;
    value_type get(int x, int y, value_type fallback) const noexcept;
    void set(int x, int y, value_type v) noexcept;

    void fill(value_type v);

    size_t width()  const noexcept;
    size_t height() const noexcept;

private:
    size_t m_width = 0;
    size_t m_height = 0;
    std::vector<value_type> m_data;
};

#endif //MAP_HPP
