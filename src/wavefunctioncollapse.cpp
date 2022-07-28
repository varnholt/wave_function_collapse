#include "wavefunctioncollapse.h"

#include <algorithm>
#include <iostream>
#include <optional>

Grid::Grid(int32_t width, int32_t height, const std::vector<Tile>& tiles)
    : _size{width, height}, _tiles(tiles), _collapsed_remaining_count(width * height * static_cast<int32_t>(tiles.size()))
{
   _slots.resize(width * height);

   for (auto& slot : _slots)
   {
      slot._tile_states.resize(_tiles.size(), Slot::TileState::Probable);
      slot._entropy = static_cast<int32_t>(_tiles.size());
   }
}

int32_t Grid::getProbableTileIndex(const Vector2D& pos) const
{
   const auto index = (pos._y * _size._x + pos._x);
   const auto& states = _slots[index]._tile_states;

   const auto it = std::find_if(states.begin(), states.end(), [](const auto& s) { return s == Slot::TileState::Probable; });

   return static_cast<int32_t>(std::distance(states.begin(), it));
}

Vector2D Grid::findMinEntropyPos() const
{
   auto entropy_min = INT_MAX;
   std::vector<Vector2D> slot_with_min_entropy_pos;

   for (auto y = 0; y < _size._y; y++)
   {
      for (auto x = 0; x < _size._x; x++)
      {
         const auto entropy = _slots[y * _size._x + x]._entropy;

         // already collapsed
         if (entropy == 1)
         {
            continue;
         }

         if (entropy < entropy_min)
         {
            entropy_min = entropy;
            slot_with_min_entropy_pos.clear();
         }

         if (entropy == entropy_min)
         {
            slot_with_min_entropy_pos.push_back({x, y});
         }
      }
   }

   return slot_with_min_entropy_pos[std::rand() % slot_with_min_entropy_pos.size()];
}

bool Grid::isFullyCollapsed() const
{
   return _collapsed_remaining_count == (_size._x * _size._y);
}

void Grid::collapseTile(Slot& slot, int32_t tile_index)
{
   for (auto i = 0; i < slot._tile_states.size(); i++)
   {
      if (slot._tile_states[i] == Slot::TileState::Probable)
      {
         _collapsed_remaining_count--;
      }

      slot._tile_states[i] = Slot::TileState::RuledOut;
   }

   slot._tile_states[tile_index] = Slot::TileState::Probable;
   slot._entropy = 1;

   _collapsed_remaining_count++;
}

void Grid::collapseSlot(const Vector2D& pos)
{
   auto& slot = _slots[pos._y * _size._x + pos._x];
   const auto& tile_states = slot._tile_states;

   std::vector<int32_t> tile_indices;
   for (auto i = 0; i < tile_states.size(); i++)
   {
      if (tile_states[i] == Slot::TileState::Probable)
      {
         tile_indices.push_back(i);
      }
   }

   // for debugging only
   if (tile_indices.empty())
   {
      _given_up = true;
      // std::cerr << "giving up, slots left: " << _collapsed_remaining_count << " pos: " << pos._x << ", " << pos._y << std::endl;
      return;
   }

   std::optional<int32_t> selected_tile_index;

   if (_use_bias)
   {
      // prefer tiles with heigher weight
      std::vector<Tile> all_tiles;
      std::vector<Tile> tiles_filtered_by_bias;
      for (auto index : tile_indices)
      {
         all_tiles.push_back(_tiles[index]);
      }
      std::sort(all_tiles.begin(), all_tiles.end());
      const auto random_bias = (std::rand() % 100) * 0.01f;
      std::copy_if(
         all_tiles.begin(),
         all_tiles.end(),
         std::back_inserter(tiles_filtered_by_bias),
         [random_bias](const Tile& t) { return t._bias >= random_bias; }
      );

      if (!tiles_filtered_by_bias.empty())
      {
         selected_tile_index = tiles_filtered_by_bias[std::rand() % tiles_filtered_by_bias.size()]._array_index;
      }
   }

   if (!selected_tile_index.has_value())
   {
      selected_tile_index = tile_indices[std::rand() % tile_indices.size()];
   }

   collapseTile(slot, selected_tile_index.value());

   if (_tile_collapsed_callback)
   {
      _tile_collapsed_callback(pos, selected_tile_index.value());
   }

   // std::cout << "collapsed slot at pos: " << pos._x << ", " << pos._y << ": " << _tiles[selected_tile_index]._tile_index << std::endl;
   _collapsed_slot_count++;
}

std::vector<int32_t> Grid::getProbableTileIndices(const Vector2D& pos) const
{
   const auto& tile_states = _slots[pos._y * _size._x + pos._x]._tile_states;

   std::vector<int32_t> tile_indices;
   for (auto i = 0; i < tile_states.size(); i++)
   {
      if (tile_states[i] == Slot::TileState::Probable)
      {
         tile_indices.push_back(i);
      }
   }

   return tile_indices;
}

std::vector<Vector2D> Grid::getDirections(const Vector2D& pos) const
{
   std::vector<Vector2D> directions;

   static constexpr Vector2D left{1, 0};
   static constexpr Vector2D right{-1, 0};
   static constexpr Vector2D up{0, -1};
   static constexpr Vector2D down{0, 1};

   if (pos._x < (_size._x - 1))
   {
      directions.push_back(left);
   }

   if (pos._y < (_size._y - 1))
   {
      directions.push_back(down);
   }

   if (pos._x > 0)
   {
      directions.push_back(right);
   }

   if (pos._y > 0)
   {
      directions.push_back(up);
   }

   return directions;
}

void Grid::constrain(const Vector2D& pos, int32_t tile_index)
{
   auto& slot = _slots[pos._y * _size._x + pos._x];
   slot._tile_states[tile_index] = Slot::TileState::RuledOut;
   slot._entropy--;
   _collapsed_remaining_count--;

   // for debugging only
   //
   // if (slot._entropy == 0)
   // {
   //     _terminate = true;
   //     std::cerr << "entropy set to 0: " << _collapsed_remaining_count << " tile index: " << _tiles[tile_index]._tile_index << " pos: "
   //     << pos._x << ", " << pos._y << std::endl;
   // }
}

void Grid::propagate(const Vector2D& start_pos)
{
   std::vector<Vector2D> positions{start_pos};

   while (!positions.empty())
   {
      const auto current_pos = positions.back();
      positions.pop_back();

      const auto current_probable_tile_indices = getProbableTileIndices(current_pos);

      const auto directions = getDirections(current_pos);
      for (const auto& direction : directions)
      {
         const Vector2D next_pos{current_pos._x + direction._x, current_pos._y + direction._y};
         const auto next_probable_tile_indices = getProbableTileIndices(next_pos);

         for (auto next_tile_index : next_probable_tile_indices)
         {
            auto compatible = false;
            for (auto current_tile_index : current_probable_tile_indices)
            {
               const auto& tile_a = _tiles[current_tile_index];
               const auto& tile_b = _tiles[next_tile_index];

               compatible |= tile_a.isCompatibleWith(tile_b, direction);
            }

            if (!compatible)
            {
               constrain(next_pos, next_tile_index);
               positions.push_back(next_pos);
            }
         }
      }
   }
}

void Grid::run()
{
   while (!isFullyCollapsed() && !_given_up)
   {
      const auto slot_pos = findMinEntropyPos();
      collapseSlot(slot_pos);

      if (!_given_up)
      {
         propagate(slot_pos);
      }
   }
}

std::vector<int32_t> Grid::readGrid() const
{
   std::vector<int32_t> grid;
   grid.resize(_size._x * _size._y);

   for (auto y = 0; y < _size._y; y++)
   {
      for (auto x = 0; x < _size._x; x++)
      {
         const auto index = getProbableTileIndex(Vector2D{x, y});
         if (index < _tiles.size())
         {
            grid[y * _size._x + x] = _tiles[index]._tileset_index;
         }
      }
   }

   return grid;
}

void Grid::debug() const
{
   for (auto y = 0; y < _size._y; y++)
   {
      for (auto x = 0; x < _size._x; x++)
      {
         const auto index = getProbableTileIndex(Vector2D{x, y});

         if (index < _tiles.size())
         {
            std::cout << _tiles[index]._instance_index;
         }
         else
         {
            std::cout << "x";
         }
      }
      std::cout << std::endl;
   }
}
