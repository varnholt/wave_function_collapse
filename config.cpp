#include "config.h"

#include <iostream>
#include <fstream>


Config& Config::instance()
{
    static Config config;
    return config;
}


int32_t Config::textureColumnCount() const
{
    if (_tile_size == 0)
    {
        return 0;
    }

    return _texture_size._x / _tile_size;
}


int32_t Config::textureRowCount() const
{
    if (_tile_size == 0)
    {
        return 0;
    }

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


void Config::load()
{
    _tiles.clear();

    std::ifstream f("config.json");
    const nlohmann::json data = nlohmann::json::parse(f);

    for (auto& [key, value] : data.items())
    {
        std::cout << key << " : " << value << "\n";

        const nlohmann::json json_tile(value["index"]);
        const nlohmann::json json_tiles(value["tiles"]);

        Tile tile{json_tile};

        for (auto i = 0; i < 4; i++)
        {
            std::set<int32_t> indices;
            for (auto& direction : json_tiles[i].items())
            {
                const int32_t index = direction.value();
                indices.insert(index);
            }

            tile._compatible_tiles[i] = indices;
        }

        _tiles.push_back(tile);
    }
}


void Config::save()
{
    nlohmann::json config;

    for (auto& tile : _tiles)
    {
        nlohmann::json json_tile;
        nlohmann::json arr(tile._compatible_tiles);

        json_tile["index"] = tile._tile_index;
        json_tile["tiles"] = arr;

        config.push_back(json_tile);
    }

    std::ofstream out("config.json");
    out << std::setw(4) << config << std::endl;
}
