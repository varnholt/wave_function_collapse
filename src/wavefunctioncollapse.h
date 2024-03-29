#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

#include "tile.h"

struct Slot
{
   enum class TileState
   {
      Probable = 1,
      RuledOut = 0
   };

   std::vector<TileState> _tile_states;
   int32_t _entropy{};
};

struct Grid
{
   Grid(int32_t width, int32_t height, const std::vector<Tile>& tiles);

   using CollapsedCallback = std::function<void(const Vector2D&, int32_t)>;

   bool isTileCompatible(int32_t index_1, int32_t index_2, const Vector2D& dir) const;
   Vector2D findMinEntropyPos() const;
   bool isFullyCollapsed() const;
   void collapseTile(Slot& slot, int32_t tile_index);
   void collapseSlot(const Vector2D& pos);
   std::vector<int32_t> getProbableTileIndices(const Vector2D& pos) const;
   int32_t getProbableTileIndex(const Vector2D& pos) const;
   std::vector<Vector2D> getDirections(const Vector2D& pos) const;

   void run();
   void propagate(const Vector2D& pos);
   void constrain(const Vector2D& pos, int32_t tile_index);
   std::vector<int32_t> readGrid() const;

   void debug() const;

   Vector2D _size;
   std::vector<Slot> _slots;
   std::vector<Tile> _tiles;
   int32_t _collapsed_remaining_count{};
   int32_t _collapsed_slot_count{};
   bool _given_up{false};
   bool _use_bias{false};
   CollapsedCallback _tile_collapsed_callback;
};
