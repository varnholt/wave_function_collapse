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
 : _tile_index(tile_index)
{
    instance_counter++;
}


void Tile::setCompatibleTiles(const std::array<std::set<int32_t>, 4>& arr)
{
    _compatible_tiles[Tile::Direction::North] = arr[0];
    _compatible_tiles[Tile::Direction::East] = arr[1];
    _compatible_tiles[Tile::Direction::South] = arr[2];
    _compatible_tiles[Tile::Direction::West] = arr[3];
}


bool Tile::isCompatibleWith(const Tile& another, const Vector2D& dir_vector) const
{
    const auto dir = getDirection(dir_vector);
    const auto set_it = _compatible_tiles.find(dir);
    if (set_it == _compatible_tiles.end())
    {
        return false;
    }

    const auto& tile_set = set_it->second;
    const auto compatible = tile_set.find(another._tile_index) != tile_set.end();

    // std::cout << compatible << std::endl;

    return compatible;
}

