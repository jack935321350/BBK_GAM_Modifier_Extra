#ifndef MAPVIEWEDIT_H
#define MAPVIEWEDIT_H

#include <QDebug>
#include <QWidget>
#include <QMenu>
#include <QPainter>
#include <QTextCodec>
#include <QMouseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QImage>

#define MAP_DATA_MAX_SIZE  1048576

class MapViewEdit : public QWidget
{
    Q_OBJECT

public:
    explicit MapViewEdit(QWidget *parent = nullptr);
    ~MapViewEdit();
    void SetTile(QImage Tile);
    void SetTileArgs(uint8_t TileSize, uint8_t TileCntX, uint8_t TileCntY);
    void SetCurrTile(uint8_t CurrTile);
    void Rescale(void);
    void ClearView(void);
    void UpdateView(uint8_t *data, uint32_t len);
    void ResetView(void);
    void WriteBack(uint8_t *data, uint32_t *len);

    uint8_t MapWidth(void) {
        return _MapData[0x10];
    }

    uint8_t MapHeight(void) {
        return _MapData[0x11];
    }

    QString MapName(void) {
        QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
        return gbkCodec->toUnicode((char *)&_MapData[3]);
    }

    enum BrushType {
        BrushType_Tile = 0,
        BrushType_Feasible,
        BrushType_Event
    };

    void SetBrushType(BrushType brushType);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QImage      _Tile;
    uint8_t     _TileSize = 32;
    uint8_t     _TileCntX = 1;
    uint8_t     _TileCntY = 1;

    uint8_t     _CurrTile = 1;

    uint8_t     *_MapData = nullptr;
    uint8_t     *_MapSrcData = nullptr;
    int32_t     _DataSize;

    qreal       _scale = 1.0;
    BrushType   _brushType = BrushType_Tile;
    QPoint      _prevOrigin = QPoint(0, 0);
    QPoint      _currOrigin = QPoint(0, 0);
    QPoint      _startPoint;
    bool        _mousePressed = false;

    QPoint      _menuPressedPos;
    QMenu       *contextMenu;

    QAction     *mapConfig;
    QAction     *rowAdd;
    QAction     *rowInsert;
    QAction     *rowDelete;
    QAction     *colAdd;
    QAction     *colInsert;
    QAction     *colDelete;
    bool        _tileVisible = true;
    QAction     *tileVisible;
    bool        _feasibleVisible = true;
    QAction     *feasibleVisible;
    bool        _eventVisible = true;
    QAction     *eventVisible;
    QAction     *saveThumbnail;

private slots:
    void on_posTile(QPoint pos, Qt::MouseButton btn);
    void on_posFeasible(QPoint pos, Qt::MouseButton btn);
    void on_posEvent(QPoint pos, Qt::MouseButton btn);
    void on_mapConfig(bool checked);
    void on_rowAdd(bool checked);
    void on_rowInsert(bool checked);
    void on_rowDelete(bool checked);
    void on_colAdd(bool checked);
    void on_colInsert(bool checked);
    void on_colDelete(bool checked);

    void on_tileVisible(bool checked);
    void on_feasibleVisible(bool checked);
    void on_eventVisible(bool checked);

    void on_saveThumbnail(bool checked);

signals:
    void posTile(QPoint pos, Qt::MouseButton btn);
    void posFeasible(QPoint pos, Qt::MouseButton btn);
    void posEvent(QPoint pos, Qt::MouseButton btn);
    void mapArgsChanged(QString MapName, uint8_t TileIndex);
    void currTileChanged(uint8_t CurrTile);
    void pointPosChanged(int x, int y);

};

#endif // MAPVIEWEDIT_H
