#ifndef TILE_H
#define TILE_H

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include "vector2d.h"

struct Tile
{
   enum class Direction
   {
      North = 0,
      East = 1,
      South = 2,
      West = 3
   };

   // Tile(int32_t tile_index);
   Tile(int32_t tile_index, int32_t array_index);
   Tile(int32_t tile_index, const std::array<std::set<int32_t>, 4>& compatible_tiles);

   void addCompatibleTile(Direction dir, int32_t other);
   void removeCompatibleTile(Direction dir, int32_t other);
   void setCompatibleTiles(Direction dir, const std::set<int32_t>& indices);

   virtual bool isCompatibleWith(const Tile& another, const Vector2D& dir) const;

   bool operator<(const Tile& other);

   int32_t _tileset_index{0};
   int32_t _instance_index{0};
   int32_t _array_index{0};
   std::array<std::set<int32_t>, 4> _compatible_tiles;
   float _bias{0.2f};
   static int32_t instance_counter;
};

#endif  // TILE_H
