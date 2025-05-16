//
// Created by Daniel Adámek - daniel@nullptr.cz on 9.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#include "Map.hpp"
#include <algorithm>
#include <cassert>
#include <stdexcept>

Map::Map(const size_t w, const size_t h, const value_type fill)
    : m_width(w), m_height(h), m_data(static_cast<size_t>(w) * h, fill) {
    if (w <= 0 || h <= 0)
        throw std::invalid_argument("Map: invalid size");
}

Map::value_type& Map::operator()(const size_t x, const size_t y) {
    assert(x < m_width && y < m_height);
    return m_data[static_cast<size_t>(y) * m_width + x];
}

Map::value_type Map::operator()(const size_t x, const size_t y) const {
    assert(x < m_width && y < m_height);
    return m_data[y * m_width + x];
}

Map::value_type Map::at(const size_t x, const size_t y) const {
    return this->operator()(x, y);
}

Map::value_type Map::get(int x, int y, value_type fallback) const noexcept {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height)
        return fallback;
    return m_data[static_cast<std::size_t>(y) * m_width + x];
}

void Map::set(int x, int y, value_type v) noexcept {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height)
        return;
    (*this)(x, y) = v;
}


void Map::fill(const value_type v) {
    std::ranges::fill(m_data, v);
}

size_t Map::width() const noexcept  { return m_width; }
size_t Map::height() const noexcept { return m_height; }