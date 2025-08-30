#ifndef TILEVIEWSELECT_H
#define TILEVIEWSELECT_H

#include <QDebug>
#include <QWidget>
#include <QMenu>
#include <QPainter>
#include <QMouseEvent>
#include <QImage>

class TileViewSelect : public QWidget
{
    Q_OBJECT

public:
    explicit TileViewSelect(QWidget *parent = nullptr);
    void SetTile(QImage Tile);
    void SetCurrTile(uint8_t CurrTile);
    void Rescale();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QImage      _Tile;
    uint8_t     _TileSize = 32;
    uint8_t     _TileCntX = 1;
    uint8_t     _TileCntY = 1;

    uint8_t     _CurrTile = 1;
    uint8_t     _TileMax = 1;

    qreal       _scale = 1.0;
    QPoint      _prevOrigin = QPoint(0, 0);
    QPoint      _currOrigin = QPoint(0, 0);
    QPoint      _startPoint;
    bool        _mousePressed = false;

    QMenu       *contextMenu;

    QAction     *tileConfig;
    QAction     *is16x16;

private slots:
    void on_is16x16(bool checked);
    void on_tileConfig(bool checked);

signals:
    void tileArgsChanged(uint8_t TileSize, uint8_t TileCntX, uint8_t TileCntY);
    void currTileChanged(uint8_t CurrTile);

};

#endif // MAPVIEWEDIT_H
