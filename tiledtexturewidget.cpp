#include "tiledtexturewidget.h"

#include <QColor>
#include <QMouseEvent>
#include <QPainter>

#include <iostream>

TiledTextureWidget::TiledTextureWidget(QWidget* parent)
    : QWidget{parent}
{
    if (!_image.load("texture.png"))
    {
        std::cout << "can't load image" << std::endl;
    }

    setMinimumSize(_image.size());
    setMaximumSize(_image.size());
    setMouseTracking(true);
}

void TiledTextureWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    auto r = _image.rect();
    p.drawImage(r, _image);

    auto tile_count_x = r.width() / _tile_size;
    auto tile_count_y = r.height() / _tile_size;

    p.setPen(QColor(0, 0, 0, 50));

    for (auto tx = 0; tx < tile_count_x; tx++)
    {
        p.drawLine(tx * _tile_size, 0, tx * _tile_size, r.height());
    }

    for (auto ty = 0; ty < tile_count_y; ty++)
    {
        p.drawLine(0, ty * _tile_size, r.width(), ty * _tile_size);
    }

    int32_t selected_x = _mouse_pos.x() / _tile_size;
    int32_t selected_y = _mouse_pos.y() / _tile_size;
    QRect selected_rect;
    selected_rect.setRect(selected_x * _tile_size, selected_y * _tile_size, _tile_size, _tile_size);
    p.fillRect(selected_rect, QColor(255, 0, 0, 50));

    std::cout << "press " << selected_rect.x() << ", " << selected_rect.y() << std::endl;
}


void TiledTextureWidget::mousePressEvent(QMouseEvent *event)
{
    // std::cout << "press " << _mouse_pos.x() << ", " << _mouse_pos.y() << std::endl;

    _selected_tile_index = (event->pos().y() / _tile_size) * (_image.width() / _tile_size) + (event->pos().x() / _tile_size);
    std::cout << _selected_tile_index << std::endl;
}


void TiledTextureWidget::mouseMoveEvent(QMouseEvent *event)
{
    _mouse_pos = event->pos();
    update();
    // std::cout << "move " << _mouse_pos.x() << ", " << _mouse_pos.y() << std::endl;
}
