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
    const QImage& texture() const;

    void setGrid(const Vector2D& size, const std::vector<int32_t>& newGrid);
    void addPositionedTile(const Vector2D& pos, int32_t tile_index);
    void clearPositionedTiles();

protected:
    void paintEvent(QPaintEvent*);

private:

    struct PositionedTile
    {
        Vector2D _pos;
        int32_t _tile_index{};
    };

    QImage _texture;
    Vector2D _size;
    std::vector<int32_t> _grid;
    std::vector<PositionedTile> _positioned_tiles;
};

#endif // TILEGRIDWIDGET_H
