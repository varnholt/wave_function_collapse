#ifndef TILE_H
#define TILE_H

#include "vector2d.h"
#include <cstdint>
#include <map>
#include <set>

struct Tile
{
    enum class Direction
    {
        North,
        East,
        South,
        West
    };

    Tile();
    static int32_t instance_counter;
    std::map<Direction, std::set<int32_t>> _compatible_tiles;
    int32_t _tile_index = 0;
    virtual bool isCompatibleWith(const Tile& another, const Vector2D &dir) const;
};

#endif // TILE_H
