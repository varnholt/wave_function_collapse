#include "tile.h"

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

Tile::Tile()
{
    instance_counter++;
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
    return tile_set.find(another._tile_index) != tile_set.end();
}

