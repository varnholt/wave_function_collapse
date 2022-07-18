#ifndef DRAWABLETILE_H
#define DRAWABLETILE_H

#include "tile.h"

#include <QImage>

struct DrawableTile : public Tile
{
    DrawableTile(int32_t tile_index, const std::array<std::set<int32_t>, 4>& compatible_tiles);
    QImage _image;
};

#endif // DRAWABLETILE_H
