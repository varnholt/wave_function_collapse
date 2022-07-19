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
