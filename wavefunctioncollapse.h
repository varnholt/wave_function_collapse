#pragma once

#include <array>
#include <cstdint>
#include <vector>


struct Vector2D
{
    int32_t x;
    int32_t y;
};


struct Tile
{
    char mBitmap;
    char mKeys[4];
};



struct Model
{
    Model(int width, int height);

    int getTileIndex(Vector2D coord);

    Vector2D findMinEntropyPos();
    bool isFullyCollapsed();
    void collapse(const Vector2D& coord, int tileIndex);
    void collapse(const Vector2D& coord);
    std::vector<int> GetPossibleTiles(const Vector2D& coord);
    std::vector<Vector2D> GetValidDirs(const Vector2D& coord);
    void Constrain(const Vector2D& coord, int tileIndex);
    void propagate(const Vector2D& coord);
    void run();
    void dump();

    Vector2D _dimensions;
    std::vector<bool> mCoef;
    std::vector<unsigned short> mSumCoef;
    unsigned int _total_sum;
};


