#include "config.h"


Config& Config::instance()
{
    static Config config;
    return config;
}


int32_t Config::textureColumnCount() const
{
    return _texture_size._x / _tile_size;
}


int32_t Config::textureRowCount() const
{
    return _texture_size._y / _tile_size;
}


void Config::addTile(int32_t tile_index)
{
    const auto& it = std::find_if(_tiles.begin(), _tiles.end(), [tile_index](const Tile& t) {
        return t._tile_index == tile_index;
    });

    if (it != _tiles.end())
    {
        return;
    }

    _tiles.push_back({tile_index});
}


Tile& Config::getTile(int32_t tile_index)
{
    auto it = std::find_if(_tiles.begin(), _tiles.end(), [tile_index](const Tile& t) {
        return t._tile_index == tile_index;
    });

    return *it;
}
