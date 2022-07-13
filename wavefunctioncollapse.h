#pragma once

#include <array>
#include <cstdint>
#include <vector>


struct Vector2D
{
    int32_t _x{};
    int32_t _y{};
};


struct Tile
{
    char _ascii{};
    char _bitmask[4];
};


struct Slot
{
    enum class TileState
    {
        Probable = 1,
        RuledOut = 0
    };

    std::vector<TileState> _tiles;
    int32_t _entropy{};
};


struct Grid
{
    Grid(int32_t width, int32_t height);

    int getTileIndex(Vector2D coord);

    Vector2D findMinEntropyPos() const;
    bool isFullyCollapsed() const;
    void collapse(const Vector2D& coord, int32_t tileIndex);
    void collapse(const Vector2D& coord);
    std::vector<int> getProbableTiles(const Vector2D& coord);
    std::vector<Vector2D> getDirections(const Vector2D& coord);
    void constrain(const Vector2D& coord, int32_t tileIndex);
    void propagate(const Vector2D& coord);
    void run();
    void dump();

    Vector2D _size;
    std::vector<Slot> _slots;
    int32_t _collapsed_count{};
};


