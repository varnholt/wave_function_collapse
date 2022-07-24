#include "tilegridwidget.h"

#include "config.h"

#include <QPaintEvent>
#include <QPainter>

#include <iostream>

TileGridWidget::TileGridWidget(QWidget* parent)
 : QWidget(parent)
{
}


void TileGridWidget::setTexture(const QImage& newTexture)
{
    _texture = newTexture;
    update();
}


void TileGridWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    int32_t x_offset = 0;
    int32_t y_offset = 0;

    const auto cols = Config::instance().textureColumnCount();
    const auto tile_width = Config::instance()._tile_size;

    for (auto y = 0; y < _size._y; y++)
    {
        for (auto x = 0; x < _size._x; x++)
        {
            const auto index = _grid[y * _size._x + x];

            const auto source_x = (index % cols) * tile_width;
            const auto source_y = (index / cols) * tile_width;

            p.drawImage(x_offset, y_offset, _texture, source_x, source_y, tile_width, tile_width);
            x_offset += tile_width;
        }
        x_offset = 0;
        y_offset += tile_width;
    }

    for (const auto& pt : _positioned_tiles)
    {
        const auto& tile = Config::instance().getTile(pt._tile_index);
        const auto source_x = (tile._tileset_index % cols) * tile_width;
        const auto source_y = (tile._tileset_index / cols) * tile_width;
        p.drawImage(pt._pos._x, pt._pos._y, _texture, source_x, source_y, tile_width, tile_width);
    }
}


const QImage& TileGridWidget::texture() const
{
    return _texture;
}


void TileGridWidget::setGrid(const Vector2D& size, const std::vector<int32_t>& newGrid)
{
    _size = size;
    _grid = newGrid;

    update();
}


void TileGridWidget::addPositionedTile(const Vector2D& pos, int32_t tile_index)
{
    PositionedTile pt;
    pt._tile_index = Config::instance()._tiles[tile_index]._tileset_index;
    pt._pos._x = pos._x * Config::instance()._tile_size;
    pt._pos._y = pos._y * Config::instance()._tile_size;
    _positioned_tiles.push_back(pt);
    update();
}


void TileGridWidget::clearPositionedTiles()
{
    _positioned_tiles.clear();
    update();
}
