#ifndef TILEDTEXTUREWIDGET_H
#define TILEDTEXTUREWIDGET_H

#include <QWidget>

#include <set>

class TiledTextureWidget : public QWidget
{
   Q_OBJECT
public:
   explicit TiledTextureWidget(QWidget* parent = nullptr);

   void setSelectMultiple(bool enabled);
   void setSelectedTiles(const std::set<int32_t>& selection);
   void clearSelection();

   void setTexture(const QImage& texture);
   const QImage& texture() const;

signals:

   void tileSelected(int32_t index);
   void tileSelectionUpdated(const std::set<int32_t>& selection);

protected:
   void paintEvent(QPaintEvent* event) override;
   void mousePressEvent(QMouseEvent* event) override;
   void mouseMoveEvent(QMouseEvent* event) override;

private:
   void updateSelection();
   void highlightIndex(int32_t index, QPainter& painter, const QColor& color);

   QImage _texture;
   QPoint _mouse_pos;
   int32_t _selected_tile_index = 0;
   std::set<int32_t> _selected_tile_indices;
   bool _select_multiple = false;
};

#endif  // TILEDTEXTUREWIDGET_H
