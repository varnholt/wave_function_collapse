#ifndef TILEDTEXTUREWIDGET_H
#define TILEDTEXTUREWIDGET_H

#include <QWidget>

#include <set>

class TiledTextureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TiledTextureWidget(QWidget* parent = nullptr);

signals:

protected:

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    QImage _image;
    int32_t _tile_size = 16;
    QPoint _mouse_pos;
    int32_t _selected_tile_index = 0;
    std::set<int32_t> _selected_tile_indices;
};

#endif // TILEDTEXTUREWIDGET_H
