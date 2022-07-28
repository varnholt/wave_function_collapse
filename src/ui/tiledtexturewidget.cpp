#include "tiledtexturewidget.h"

#include "config.h"

#include <QColor>
#include <QMouseEvent>
#include <QPainter>

#include <iostream>

TiledTextureWidget::TiledTextureWidget(QWidget* parent) : QWidget{parent}
{
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

void TiledTextureWidget::setTexture(const QImage& texture)
{
   _texture = texture;
   setMinimumSize(_texture.size());
   setMaximumSize(_texture.size());
   update();
}

void TiledTextureWidget::highlightIndex(int32_t index, QPainter& painter, const QColor& color)
{
   const auto cols = Config::instance().textureColumnCount();

   if (cols == 0)
   {
      return;
   }

   const auto tile_size = Config::instance()._tile_size;

   const auto source_x = (index % cols) * tile_size;
   const auto source_y = (index / cols) * tile_size;

   QRect selected_rect;
   selected_rect.setRect(source_x, source_y, tile_size, tile_size);
   painter.fillRect(selected_rect, color);
}

const QImage& TiledTextureWidget::texture() const
{
   return _texture;
}

void TiledTextureWidget::paintEvent(QPaintEvent* /*event*/)
{
   QPainter painter(this);
   // painter.scale(2, 2);

   const auto tile_size = Config::instance()._tile_size;

   auto r = _texture.rect();
   painter.drawImage(r, _texture);

   auto tile_count_x = r.width() / tile_size;
   auto tile_count_y = r.height() / tile_size;

   // draw grid
   painter.setPen(QColor(0, 0, 0, 50));
   for (auto tx = 0; tx < tile_count_x; tx++)
   {
      painter.drawLine(tx * tile_size, 0, tx * tile_size, r.height());
   }

   for (auto ty = 0; ty < tile_count_y; ty++)
   {
      painter.drawLine(0, ty * tile_size, r.width(), ty * tile_size);
   }

   // highlight tile under mouse cursor
   int32_t selected_x = _mouse_pos.x() / tile_size;
   int32_t selected_y = _mouse_pos.y() / tile_size;
   QRect selected_rect;
   selected_rect.setRect(selected_x * tile_size, selected_y * tile_size, tile_size, tile_size);
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

void TiledTextureWidget::mousePressEvent(QMouseEvent* event)
{
   std::cout << _selected_tile_index << std::endl;

   const auto tile_size = Config::instance()._tile_size;

   _selected_tile_index = (event->pos().y() / tile_size) * (_texture.width() / tile_size) + (event->pos().x() / tile_size);
   updateSelection();
   emit tileSelected(_selected_tile_index);
   update();
}

void TiledTextureWidget::mouseMoveEvent(QMouseEvent* event)
{
   _mouse_pos = event->pos();
   update();
}
