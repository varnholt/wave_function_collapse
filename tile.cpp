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


//Tile::Tile(int32_t tile_index)
//    : _tileset_index(tile_index),
//      _instance_index(instance_counter++)
//{
//}

Tile::Tile(int32_t tile_index, int32_t array_index)
    : _tileset_index(tile_index),
      _instance_index(instance_counter++),
      _array_index(array_index)
{
}


Tile::Tile(int32_t tile_index, const std::array<std::set<int32_t>, 4>& compatible_tiles)
 : _tileset_index(tile_index),
   _instance_index(instance_counter++),
   _compatible_tiles(compatible_tiles)
{
}


void Tile::addCompatibleTile(Direction dir, int32_t other)
{
    _compatible_tiles[static_cast<int32_t>(dir)].insert(other);
}


void Tile::removeCompatibleTile(Direction dir, int32_t other)
{
    _compatible_tiles[static_cast<int32_t>(dir)].erase(other);
}


void Tile::setCompatibleTiles(Direction dir, const std::set<int32_t> &indices)
{
    _compatible_tiles[static_cast<int32_t>(dir)] = indices;
}


bool Tile::isCompatibleWith(const Tile& another, const Vector2D& dir_vector) const
{
    const auto& tile_set = _compatible_tiles[static_cast<int32_t>(getDirection(dir_vector))];
    const auto compatible = tile_set.find(another._tileset_index) != tile_set.end();
    return compatible;
}

bool Tile::operator <(const Tile &other)
{
    return _bias < other._bias;
}



