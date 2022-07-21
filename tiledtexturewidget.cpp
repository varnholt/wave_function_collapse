#include "tiledtexturewidget.h"

#include <QColor>
#include <QMouseEvent>
#include <QPainter>

#include <iostream>

TiledTextureWidget::TiledTextureWidget(QWidget* parent)
    : QWidget{parent}
{
    if (!_texture.load("texture.png"))
    {
        std::cout << "can't load image" << std::endl;
    }

    setMinimumSize(_texture.size());
    setMaximumSize(_texture.size());
    setMouseTracking(true);
}


void TiledTextureWidget::setSelectMultiple(bool enabled)
{
    _select_multiple = enabled;
}


void TiledTextureWidget::setSelectedTiles(const std::set<int32_t>& selection)
{
    _selected_tile_indices = selection;
    update();
}


void TiledTextureWidget::clearSelection()
{
    _selected_tile_indices.clear();
    update();
}


void TiledTextureWidget::highlightIndex(int32_t index, QPainter& painter, const QColor& color)
{
    const auto cols = 5;
    const auto tile_width = 16;

    const auto source_x = (index % cols) * tile_width;
    const auto source_y = (index / cols) * tile_width;

    QRect selected_rect;
    selected_rect.setRect(source_x, source_y, _tile_size, _tile_size);
    painter.fillRect(selected_rect, color);
}


const QImage& TiledTextureWidget::texture() const
{
    return _texture;
}


void TiledTextureWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    auto r = _texture.rect();
    painter.drawImage(r, _texture);

    auto tile_count_x = r.width() / _tile_size;
    auto tile_count_y = r.height() / _tile_size;

    // draw grid
    painter.setPen(QColor(0, 0, 0, 50));
    for (auto tx = 0; tx < tile_count_x; tx++)
    {
        painter.drawLine(tx * _tile_size, 0, tx * _tile_size, r.height());
    }

    for (auto ty = 0; ty < tile_count_y; ty++)
    {
        painter.drawLine(0, ty * _tile_size, r.width(), ty * _tile_size);
    }

    // highlight tile under mouse cursor
    int32_t selected_x = _mouse_pos.x() / _tile_size;
    int32_t selected_y = _mouse_pos.y() / _tile_size;
    QRect selected_rect;
    selected_rect.setRect(selected_x * _tile_size, selected_y * _tile_size, _tile_size, _tile_size);
    painter.fillRect(selected_rect, QColor(255, 255, 255, 50));

    // highlight selected
    if (!_select_multiple)
    {
        highlightIndex(_selected_tile_index, painter, QColor(255, 255, 255, 100));
    }

    // highlight set of indices
    for (auto index : _selected_tile_indices)
    {
        highlightIndex(index, painter, QColor(255, 0, 0, 100));
    }

    // std::cout << "press " << selected_rect.x() << ", " << selected_rect.y() << std::endl;
}


void TiledTextureWidget::updateSelection()
{
    if (_select_multiple)
    {
        const auto it = _selected_tile_indices.find(_selected_tile_index);
        if (it != _selected_tile_indices.end())
        {
            _selected_tile_indices.erase(it);
        }
        else
        {
            _selected_tile_indices.insert(_selected_tile_index);
        }

        emit tileSelectionUpdated(_selected_tile_indices);
    }
}


void TiledTextureWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout << _selected_tile_index << std::endl;

    _selected_tile_index = (event->pos().y() / _tile_size) * (_texture.width() / _tile_size) + (event->pos().x() / _tile_size);
    updateSelection();
    emit tileSelected(_selected_tile_index);
    update();
}


void TiledTextureWidget::mouseMoveEvent(QMouseEvent *event)
{
    _mouse_pos = event->pos();
    update();
}
