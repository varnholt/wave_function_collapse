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
        return t._tileset_index == tile_index;
    });

    if (it != _tiles.end())
    {
        return;
    }

    Tile t{tile_index, static_cast<int32_t>(_tiles.size())};
    _tiles.push_back(t);
}


Tile& Config::getTile(int32_t tile_index)
{
    auto it = std::find_if(_tiles.begin(), _tiles.end(), [tile_index](const Tile& t) {
        return t._tileset_index == tile_index;
    });

    return *it;
}


void Config::load(const std::string& file_path)
{
    _project_file_path = file_path;

    std::ifstream f(file_path);
    const nlohmann::json config = nlohmann::json::parse(f);
    const nlohmann::json texture = config["texture"];
    const nlohmann::json use_bias = config["use_bias"];
    const nlohmann::json tile_size = config["tile_size"];
    const nlohmann::json grid_size = config["grid_size"];

    _texture_path = texture;
    _use_bias = use_bias;
    _tile_size = tile_size;
    _grid_size = Vector2D{grid_size[0], grid_size[1]};

    _tiles.clear();
    const nlohmann::json tiles = config["tiles"];
    for (auto& [key, value] : tiles.items())
    {
        std::cout << key << " : " << value << "\n";

        const nlohmann::json json_tile(value["index"]);
        const nlohmann::json json_tile_bias(value["bias"]);
        const nlohmann::json json_tiles(value["compatible_tiles"]);

        Tile tile{json_tile, static_cast<int32_t>(_tiles.size())};
        tile._bias = json_tile_bias;

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

    config["texture"] = _texture_path;
    config["use_bias"] = _use_bias;
    config["tile_size"] = _tile_size;
    config["grid_size"] = std::array<int32_t, 2>{_grid_size._x, _grid_size._y};

    nlohmann::json tiles;
    for (auto& tile : _tiles)
    {
        nlohmann::json json_tile;
        nlohmann::json arr(tile._compatible_tiles);

        json_tile["index"] = tile._tileset_index;
        json_tile["bias"] = tile._bias;
        json_tile["compatible_tiles"] = arr;

        tiles.push_back(json_tile);
    }

    config["tiles"] = tiles;

    std::ofstream out(_project_file_path);
    out << std::setw(4) << config << std::endl;
}
