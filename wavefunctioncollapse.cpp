#include "wavefunctioncollapse.h"

#include <chrono>
#include <iostream>


bool Grid::isTileCompatible(int32_t index_1, int32_t index_2, const Vector2D& dir)
{
    auto& t1 = _tiles[index_1];
    auto& t2 = _tiles[index_2];

    return t1.isCompatibleWith(t2, dir);
}


Grid::Grid(int32_t width, int32_t height)
    : _size{width, height}
{
    // _tiles = {
    //     {' ',{2,2,2,2}},
    //     {'X',{1,1,1,1}},
    //     {'-',{1,2,1,1}},
    //     {'-',{1,1,1,2}},
    //     {'|',{2,1,1,1}},
    //     {'|',{1,1,2,1}},
    // };

    _slots.resize(width * height);

    for (auto& slot : _slots)
    {
        slot._tile_states.resize(_tiles.size(), Slot::TileState::Probable);
    }

    _collapsed_count = width * height * static_cast<int32_t>(_tiles.size());
}


int32_t Grid::getProbableTileIndex(const Vector2D& pos)
{
    const auto index = (pos._y * _size._x + pos._x);
    const auto& states = _slots[index]._tile_states;

    const auto it = std::find_if(states.begin(), states.end(), [](const auto& s)
    {
        return s == Slot::TileState::Probable;
    });

    return static_cast<int32_t>(std::distance(states.begin(), it));
}


Vector2D Grid::findMinEntropyPos() const
{
    auto entropy_min = INT_MAX;
    std::vector<Vector2D> entropy_min_positions;

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
                entropy_min_positions.clear();
            }

            if (entropy == entropy_min)
            {
                entropy_min_positions.push_back({x, y});
            }
        }
    }

    return entropy_min_positions[rand() % entropy_min_positions.size()];
}


bool Grid::isFullyCollapsed() const
{
    return _collapsed_count == (_size._x * _size._y);
}


void Grid::collapse(const Vector2D& pos, int32_t tile_index)
{
    auto& slot = _slots[(pos._y * _size._x + pos._x)];

    for (auto i = 0; i < slot._tile_states.size(); i++)
    {
        if (slot._tile_states[i] == Slot::TileState::Probable)
        {
            _collapsed_count--;
        }

        slot._tile_states[i] = Slot::TileState::RuledOut;
    }

    slot._tile_states[tile_index] = Slot::TileState::Probable;
    slot._entropy = 1;

    _collapsed_count++;
}


void Grid::collapse(const Vector2D& pos)
{
    const auto& tiles = _slots[pos._y * _size._x + pos._x]._tile_states;

    std::vector<int32_t> tile_indices;
    for (auto i = 0; i < _tiles.size(); i++)
    {
        if (tiles[i] == Slot::TileState::Probable)
        {
            tile_indices.push_back(i);
        }
    }

    const auto selected = tile_indices[rand() % tile_indices.size()];
    collapse(pos, selected);
}


std::vector<int> Grid::getProbableTiles(const Vector2D& pos)
{
    const auto& tiles = _slots[pos._y * _size._x + pos._x]._tile_states;

    std::vector<int32_t> tile_indices;
    for (auto i = 0; i < _tiles.size(); i++)
    {
        if (tiles[i] == Slot::TileState::Probable)
        {
            tile_indices.push_back(i);
        }
    }

    return tile_indices;
}


std::vector<Vector2D> Grid::getDirections(const Vector2D& pos)
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
    _collapsed_count--;
}


void Grid::propagate(const Vector2D& start_pos)
{
    std::vector<Vector2D> positions;
    positions.push_back(start_pos);

    while (!positions.empty())
    {
        const auto current_pos = positions.back();
        positions.pop_back();

        const auto probable_tiles = getProbableTiles(current_pos);

        const auto directions = getDirections(current_pos);
        for (const auto& direction : directions)
        {
            const Vector2D next_pos = {current_pos._x + direction._x, current_pos._y + direction._y};
            const auto next_probable_tiles = getProbableTiles(next_pos);

            for (auto next_tile : next_probable_tiles)
            {
                auto compatible = false;
                for (auto current_tile : probable_tiles)
                {
                    compatible |= isTileCompatible(current_tile, next_tile, direction);
                }

                if (!compatible)
                {
                    constrain(next_pos, next_tile);
                    positions.push_back(next_pos);
                }
            }
        }
    }
}


void Grid::run()
{
    while (!isFullyCollapsed())
    {
        const auto pos = findMinEntropyPos();
        collapse(pos);
        propagate(pos);
    }
}


void Grid::dump()
{
    for (auto y = 0; y < _size._y; y++)
    {
        for (auto x = 0; x < _size._x; x++)
        {
            const auto index = getProbableTileIndex(Vector2D{x, y});
            std::cout << _tiles[index].instance_counter;
        }
        std::cout << std::endl;
    }
}
