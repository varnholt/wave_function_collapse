#include "tile.h"

#include <iostream>

int32_t Tile::instance_counter = 0;

namespace
{

Tile::Direction getDirection(const Vector2D& dir)
{
    if (dir._y == -1)
    {
        return Tile::Direction::North;
    }
    if (dir._x == 1)
    {
        return Tile::Direction::East;
    }
    if (dir._y == 1)
    {
        return Tile::Direction::South;
    }
    return Tile::Direction::West;
}

}

Tile::Tile(int32_t tile_index)
 : _tile_index(tile_index),
   _instance_index(instance_counter++)
{
}


bool Tile::isCompatibleWith(const Tile& another, const Vector2D& dir_vector) const
{
    return true;

    const auto& tile_set = _compatible_tiles[static_cast<int32_t>(getDirection(dir_vector))];
    const auto compatible = tile_set.find(another._tile_index) != tile_set.end();
    return compatible;
}

