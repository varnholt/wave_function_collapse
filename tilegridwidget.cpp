#include "tilegridwidget.h"

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
}


void TileGridWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    int32_t x_offset = 0;
    int32_t y_offset = 0;

    const auto cols = 5;
    const auto tile_width = 16;

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
}


const QImage& TileGridWidget::texture() const
{
    return _texture;
}


void TileGridWidget::setGrid(const Vector2D& size, const std::vector<int32_t>& newGrid)
{
    _size = size;
    _grid = newGrid;

    // setMinimumSize(size._x * 16, size._y * 16);
    // setMaximumSize(size._x * 16, size._y * 16);

    update();
}
