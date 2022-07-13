#include "wavefunctioncollapse.h"

#include <chrono>
#include <iostream>

std::vector<Tile> _tiles = {
    {' ',{2,2,2,2}},
    {'X',{1,1,1,1}},
    {'-',{1,2,1,1}},
    {'-',{1,1,1,2}},
    {'|',{2,1,1,1}},
    {'|',{1,1,2,1}},
};



int32_t GetAngle(Vector2D dir)
{
    if (dir.x == 1)
        return 0;
    if (dir.y == 1)
        return 1;
    if (dir.x == -1)
        return 2;
    if (dir.y == -1)
        return 3;

    return -1;
}

int32_t GetHook(Vector2D dir)
{
    if (dir.x == 1)
        return 2;
    if (dir.y == 1)
        return 3;
    if (dir.x == -1)
        return 0;
    if (dir.y == -1)
        return 1;

    return -1;
}


bool isTileCompatible(int32_t index_1, int32_t index_2, Vector2D dir)
{
    int32_t key_1 = _tiles[index_1].mKeys[GetAngle(dir)];
    int32_t key_2 = _tiles[index_2].mKeys[GetHook(dir)];
    return (key_1 & key_2);
}


Model::Model(int32_t width, int32_t height)
    : _size{width, height}
{
    _slots.resize(width * height);

    for (auto& slot : _slots)
    {
        slot._tiles.resize(_tiles.size(), true);
    }

    _collapsed_count = width * height * static_cast<int32_t>(_tiles.size());
}


int32_t Model::getTileIndex(Vector2D pos)
{
    int32_t index = (pos.y * _size.x + pos.x);
    const auto& tiles = _slots[index]._tiles;

    int32_t res = -1;
    for (int32_t i = 0; i < _tiles.size(); i++)
    {
        if (tiles[i])
        {
            res = i;
        }
    }
    return res;
}


Vector2D Model::findMinEntropyPos() const
{
    auto entropy_min = INT_MAX;
    std::vector<Vector2D> entropy_min_positions;

    for (auto y = 0; y < _size.y; y++)
    {
        for (auto x = 0; x < _size.x; x++)
        {
            const auto entropy = _slots[y * _size.x + x]._entropy;

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


bool Model::isFullyCollapsed() const
{
    return _collapsed_count == (_size.x * _size.y);
}


void Model::collapse(const Vector2D& coord, int32_t tile_index)
{
    auto& tiles = _slots[(coord.y * _size.x + coord.x)]._tiles;

    for (auto i = 0; i < _tiles.size(); i++)
    {
        if (tiles[i])
        {
            _collapsed_count--;
        }

        tiles[i] = false;
    }

    tiles[tile_index] = 1;

    _collapsed_count++;

    _slots[coord.y * _size.x + coord.x]._entropy = 1;
}


void Model::collapse(const Vector2D& coord)
{
    std::vector<int32_t> potential_indices;
    const auto& tiles = _slots[coord.y * _size.x + coord.x]._tiles;

    for (auto i = 0; i < _tiles.size(); i++)
    {
        if (tiles[i])
        {
            potential_indices.push_back(i);
        }
    }

    const auto selected = potential_indices[rand() % potential_indices.size()];
    collapse(coord, selected);
}


std::vector<int> Model::getPossibleTiles(const Vector2D& coord)
{
    std::vector<int> tile_indices;
    const auto& tiles = _slots[coord.y * _size.x + coord.x]._tiles;

    for (auto i = 0u; i < _tiles.size(); i++)
    {
        if (tiles[i])
        {
            tile_indices.push_back(i);
        }
    }

    return tile_indices;
}


std::vector<Vector2D> Model::getDirections(const Vector2D& pos)
{
    std::vector<Vector2D> directions;

    static constexpr Vector2D left{1, 0};
    static constexpr Vector2D right{-1, 0};
    static constexpr Vector2D up{0, -1};
    static constexpr Vector2D down{0, 1};

    if (pos.x < (_size.x - 1))
    {
        directions.push_back(left);
    }

    if (pos.y < (_size.y - 1))
    {
        directions.push_back(down);
    }

    if (pos.x > 0)
    {
        directions.push_back(right);
    }

    if (pos.y > 0)
    {
        directions.push_back(up);
    }

    return directions;
}


void Model::constrain(const Vector2D& pos, int32_t tile_index)
{
    auto& tiles = _slots[pos.y * _size.x + pos.x]._tiles;
    tiles[tile_index] = 0;

    _slots[pos.y * _size.x + pos.x]._entropy--;
    _collapsed_count--;
}


void Model::propagate(const Vector2D& pos)
{
    std::vector<Vector2D> positions;
    positions.push_back(pos);

    while (!positions.empty())
    {
        const auto current_pos = positions.back();
        positions.pop_back();

        const auto curPossibleTiles = getPossibleTiles(current_pos);
        const auto directions = getDirections(current_pos);

        for (int32_t d = 0 ; d < directions.size(); d++)
        {
            const auto dir = directions[d];
            const Vector2D otherCoord = {current_pos.x + dir.x, current_pos.y + dir.y};
            const auto otherPossibleTiles = getPossibleTiles(otherCoord);

            for (int32_t otherTileIndex = 0; otherTileIndex < otherPossibleTiles.size(); otherTileIndex++)
            {
                const auto otherTile = otherPossibleTiles[otherTileIndex];

                auto tileCompatible = false;
                for (auto curTileIndex = 0; curTileIndex < curPossibleTiles.size(); curTileIndex++)
                {
                    auto curTile = curPossibleTiles[curTileIndex];
                    tileCompatible |= isTileCompatible(curTile, otherTile, dir);
                }

                if (!tileCompatible)
                {
                    constrain(otherCoord, otherTile);
                    positions.push_back(otherCoord);
                }
            }
        }
    }
}


void Model::run()
{
    while (!isFullyCollapsed())
    {
        const auto pos = findMinEntropyPos();
        collapse(pos);
        propagate(pos);
    }
}


void Model::dump()
{
    for (auto y = 0; y < _size.y; y++)
    {
        for (auto x = 0; x < _size.x; x++)
        {
            const auto index = getTileIndex(Vector2D{x, y});
            std::cout << _tiles[index].mBitmap;
        }
        std::cout << std::endl;
    }
}
