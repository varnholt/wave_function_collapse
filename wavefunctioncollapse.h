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


struct Slot
{
    std::vector<bool> _tiles;
    int32_t _entropy{};
};


struct Model
{
    Model(int32_t width, int32_t height);

    int getTileIndex(Vector2D coord);

    Vector2D findMinEntropyPos() const;
    bool isFullyCollapsed() const;
    void collapse(const Vector2D& coord, int32_t tileIndex);
    void collapse(const Vector2D& coord);
    std::vector<int> getPossibleTiles(const Vector2D& coord);
    std::vector<Vector2D> getDirections(const Vector2D& coord);
    void constrain(const Vector2D& coord, int32_t tileIndex);
    void propagate(const Vector2D& coord);
    void run();
    void dump();

    Vector2D _size;
    std::vector<Slot> _slots;
    int32_t _collapsed_count{};
};


