#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QTranslator>
#include <QLocale>
#include <QImage>
#include <QDate>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    QString _buildDateTimeStr, _windowTitle;
    QImage  _Tile;
    QString _MapName;
    uint8_t _TileIndex = 0;

private slots:
    void on_mapViewEdit_mapArgsChanged(QString MapName, uint8_t TileIndex);
    void on_mapViewEdit_currTileChanged(uint8_t CurrTile);
    void on_mapViewEdit_pointPosChanged(int x, int y);
    void on_tileViewSelect_tileArgsChanged(uint8_t TileSize, uint8_t TileCntX, uint8_t TileCntY);
    void on_tileViewSelect_currTileChanged(uint8_t CurrTile);

    void on_createMapBtn_clicked();
    void on_loadMapBtn_clicked();
    void on_saveMapBtn_clicked();
    void on_loadTileBtn_clicked();
    void on_setTileBtn_clicked();
    void on_setFeasibleBtn_clicked();
    void on_setEventBtn_clicked();

};

#endif // MAINWIDGET_H
