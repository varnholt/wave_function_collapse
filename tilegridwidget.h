#ifndef TILEGRIDWIDGET_H
#define TILEGRIDWIDGET_H

#include <QImage>
#include <QWidget>

#include "vector2d.h"

class QPaintEvent;

class TileGridWidget : public QWidget
{
public:
    explicit TileGridWidget(QWidget* parent = nullptr);

    void setTexture(const QImage& newTexture);
    void setGrid(const Vector2D &size, const std::vector<int32_t>& newGrid);

protected:
    void paintEvent(QPaintEvent*);

private:
    QImage _texture;
    Vector2D _size;
    std::vector<int32_t> _grid;
};

#endif // TILEGRIDWIDGET_H
