#ifndef CONFIG_H
#define CONFIG_H

#include "json.hpp"
#include "tile.h"
#include "vector2d.h"

#include <vector>

struct Config
{
   static Config& instance();

   int32_t textureColumnCount() const;
   int32_t textureRowCount() const;

   void addTile(int32_t tile_index);
   Tile& getTile(int32_t tile_index);

   std::vector<Tile> _tiles;
   Vector2D _texture_size;
   Vector2D _grid_size;
   int32_t _tile_size{0};
   std::string _texture_path;
   std::string _project_file_path;

   bool _use_bias = false;

   void load(const std::string& file_path);
   void save();
};

#endif  // CONFIG_H
