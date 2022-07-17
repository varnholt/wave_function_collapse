#ifndef TILE_H
#define TILE_H

#include "vector2d.h"
#include <array>
#include <cstdint>
#include <map>
#include <set>

struct Tile
{
    enum class Direction
    {
        North = 0,
        East = 1,
        South = 2,
        West = 3
    };

    Tile(int32_t tile_index);
    virtual bool isCompatibleWith(const Tile& another, const Vector2D &dir) const;

    std::array<std::set<int32_t>, 4> _compatible_tiles;
    int32_t _tile_index = 0;
    int32_t _instance_index = 0;
    static int32_t instance_counter;
};

#endif // TILE_H
