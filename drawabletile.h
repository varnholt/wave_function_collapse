#ifndef DRAWABLETILE_H
#define DRAWABLETILE_H

#include "tile.h"

#include <QImage>

struct DrawableTile : public Tile
{
    DrawableTile(int32_t tile_index);
    QImage _image;
};

#endif // DRAWABLETILE_H
