#include "wavefunctioncollapse.h"

#include <chrono>
#include <iostream>

std::vector<Tile> mTiles = {
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


bool TileCompatible(int32_t index_1, int32_t index_2, Vector2D dir)
{
    int32_t key_1 = mTiles[index_1].mKeys[GetAngle(dir)];
    int32_t key_2 = mTiles[index_2].mKeys[GetHook(dir)];
    return (key_1 & key_2);
}


Model::Model(int32_t width, int32_t height)
    : _dimensions{width, height}
{
    mCoef.resize(width*height * mTiles.size(), true);
    mSumCoef.resize(width*height, mTiles.size());
    _total_sum = width * height * mTiles.size();
}


int32_t Model::getTileIndex(Vector2D pos)
{
    int32_t idx = (pos.y * _dimensions.x + pos.x) * mTiles.size();
    int32_t res = -1;
    for (int32_t i = 0; i < mTiles.size(); i++)
    {
        if (mCoef[idx + i])
        {
            res = i;
        }
    }
    return res;
}

Vector2D Model::findMinEntropyPos()
{
    auto entropy_min = INT_MAX;
    std::vector<Vector2D> entropy_min_positions;

    for (auto y = 0; y < _dimensions.y; y++)
    {
        for (auto x = 0; x < _dimensions.x; x++)
        {
            const auto coefficient = mSumCoef[y * _dimensions.x + x];

            if (coefficient == 1)
            {
                continue;
            }

            if (coefficient < entropy_min)
            {
                entropy_min = coefficient;
                entropy_min_positions.clear();
            }

            if (coefficient == entropy_min)
            {
                entropy_min_positions.push_back({x, y});
            }
        }
    }

    return entropy_min_positions[rand() % entropy_min_positions.size()];
}


bool Model::isFullyCollapsed()
{
    return _total_sum == (_dimensions.x * _dimensions.y);
}


void Model::collapse(const Vector2D& coord, int32_t tileIndex)
{
    int32_t idx = (coord.y * _dimensions.x + coord.x)*mTiles.size();
    for (auto i = 0; i < mTiles.size(); i++)
    {
        if (mCoef[idx + i])
            _total_sum--;
        mCoef[idx + i] = 0;
    }
    mCoef[idx + tileIndex] = 1;
    _total_sum++;
    mSumCoef[coord.y * _dimensions.x + coord.x] = 1;
}

void Model::collapse(const Vector2D& coord)
{
    std::vector<int> potentials(mTiles.size());
    int32_t potentialIndex = 0;
    int32_t idx = (coord.y * _dimensions.x + coord.x)*mTiles.size();

    for (auto i = 0; i < mTiles.size(); i++)
    {
        if (mCoef[idx + i])
        {
            potentials[potentialIndex++] = i;
        }
    }

    int32_t selected = potentials[rand() % potentialIndex];
    collapse(coord, selected);
}

std::vector<int> Model::GetPossibleTiles(const Vector2D& coord)
{
    std::vector<int> tiles;
    int32_t idx = (coord.y * _dimensions.x + coord.x) * mTiles.size();

    for (auto i = 0u; i < mTiles.size(); i++)
    {
        if (mCoef[idx + i])
        {
            tiles.push_back(i);
        }
    }
    return tiles;
}

std::vector<Vector2D> Model::GetValidDirs(const Vector2D& pos)
{
    std::vector<Vector2D> dirs;

    if (pos.x < (_dimensions.x - 1))
    {
        dirs.push_back({1, 0 });
    }
    if (pos.y < (_dimensions.y - 1))
    {
        dirs.push_back({ 0, 1 });
    }
    if (pos.x > 0)
    {
        dirs.push_back({ -1, 0 });
    }
    if (pos.y > 0)
    {
        dirs.push_back({0, -1});
    }

    return dirs;
}

void Model::Constrain(const Vector2D& pos, int32_t tileIndex)
{
    int32_t idx = (pos.y * _dimensions.x + pos.x)*mTiles.size();

    mCoef[idx + tileIndex] = 0;
    mSumCoef[pos.y * _dimensions.x + pos.x] --;
    _total_sum--;
}


void Model::propagate(const Vector2D& pos)
{
    std::vector<Vector2D> positions;
    positions.push_back(pos);

    while (!positions.empty())
    {
        const auto current_pos = positions.back();
        positions.pop_back();

        const auto curPossibleTiles = GetPossibleTiles(current_pos);
        const auto validDirs = GetValidDirs(current_pos);

        for (int32_t d = 0 ; d < validDirs.size(); d++)
        {
            const auto dir = validDirs[d];
            const Vector2D otherCoord = { current_pos.x + dir.x, current_pos.y + dir.y };
            const auto otherPossibleTiles = GetPossibleTiles(otherCoord);

            for (int32_t otherTileIndex = 0; otherTileIndex < otherPossibleTiles.size(); otherTileIndex++)
            {
                int32_t otherTile = otherPossibleTiles[otherTileIndex];
                bool tileCompatible = false;

                for (int32_t curTileIndex = 0; curTileIndex < curPossibleTiles.size(); curTileIndex++)
                {
                    int32_t curTile = curPossibleTiles[curTileIndex];
                    tileCompatible |= TileCompatible(curTile, otherTile, dir);
                }

                if (!tileCompatible)
                {
                    Constrain(otherCoord, otherTile);
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
    for (int32_t y = 0; y < _dimensions.y; y++)
    {
        for (int32_t x = 0; x < _dimensions.x; x++)
        {
            const auto index = getTileIndex(Vector2D{ x,y });
            printf("%c", mTiles[index].mBitmap);
        }
        printf("\n");
    }
}
