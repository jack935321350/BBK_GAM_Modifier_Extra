#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "mapcfgdlg.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    //adjustSize();

    _buildDateTimeStr = QLocale(QLocale::English).toDateTime(QString(__DATE__ " " __TIME__).replace("  ", " 0"), "MMM dd yyyy hh:mm:ss").toString("yyyyMMdd_hhmm");

    setWindowTitle(tr("BBK_Map_Editor_") + _buildDateTimeStr);

    _windowTitle = windowTitle();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_mapViewEdit_mapArgsChanged(QString MapName, uint8_t TileIndex)
{
    _MapName = MapName;
    _TileIndex = TileIndex;
    setWindowTitle(_windowTitle + QString(" - %1 - %2").arg(MapName).arg(_TileIndex));
}

void MainWidget::on_mapViewEdit_currTileChanged(uint8_t CurrTile)
{
    ui->tileViewSelect->SetCurrTile(CurrTile);
}

void MainWidget::on_mapViewEdit_pointPosChanged(int x, int y)
{
    if(x < 0 && y < 0) {
        setWindowTitle(_windowTitle + QString(" - %1 - %2").arg(_MapName).arg(_TileIndex));
    } else {
        setWindowTitle(_windowTitle + QString(" - %1 - %2 - Pos (%3, %4) - Load(%5, %6)").arg(_MapName).arg(_TileIndex).arg(x).arg(y).arg(x - 4).arg(y - 3));
    }
}

void MainWidget::on_tileViewSelect_tileArgsChanged(uint8_t TileSize, uint8_t TileCntX, uint8_t TileCntY)
{
    ui->mapViewEdit->SetTileArgs(TileSize, TileCntX, TileCntY);
    ui->mapViewEdit->Rescale();
}

void MainWidget::on_tileViewSelect_currTileChanged(uint8_t CurrTile)
{
    ui->mapViewEdit->SetCurrTile(CurrTile);
}

void MainWidget::on_createMapBtn_clicked()
{
    MapCfgDlg mapCfgDlg(this);
    if(mapCfgDlg.exec() == MapCfgDlg::Accepted) {
        uint8_t *MapData = (uint8_t *)malloc(MAP_DATA_MAX_SIZE);
        if(MapData == nullptr) {
            return;
        }
        memcpy_s(MapData, sizeof(map_head_t), &mapCfgDlg.Map_Head, sizeof(map_head_t));
        map_block_t *map_block = reinterpret_cast<map_block_t *>(MapData + sizeof(map_head_t));
        for(int y = 0; y < MapData[0x11]; y++) {
            for(int x = 0; x < MapData[0x10]; x++) {
                map_block[y * MapData[0x10] + x].tile = 0x81;
                map_block[y * MapData[0x10] + x].event = 0;
            }
        }
        ui->mapViewEdit->ClearView();
        ui->mapViewEdit->UpdateView(MapData, sizeof(map_head_t) + sizeof(map_block_t) * MapData[0x10] * MapData[0x11]);
        free(MapData);
    }
}

void MainWidget::on_loadMapBtn_clicked()
{
    QSettings settings("QtProject", "BBK_Map_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_map_dir").toString(), "MAP(*.map)");
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        settings.setValue("last_map_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.setValue("last_map_file", QFileInfo(fileName).fileName());
        settings.sync();
        QFile mapFile(fileName);
        if(!mapFile.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray mapBA = mapFile.readAll();
        mapFile.close();
        ui->mapViewEdit->ClearView();
        ui->mapViewEdit->UpdateView((uint8_t *)mapBA.data(), mapBA.size());
    }
}

void MainWidget::on_saveMapBtn_clicked()
{
    QSettings settings("QtProject", "BBK_Map_Editor");
    QFile mapFile(QFileDialog::getSaveFileName(this, tr("Save"), settings.value("last_map_dir").toString() + '/' + settings.value("last_map_file").toString(), "MAP(*.map)"));
    if(!mapFile.open(QFile::WriteOnly)) {
        return;
    }
    uint8_t *MapData = (uint8_t *)malloc(MAP_DATA_MAX_SIZE);
    if(MapData == nullptr) {
        return;
    }
    uint32_t len = 0;
    ui->mapViewEdit->WriteBack(MapData, &len);
    QDataStream mapStream(&mapFile);
    mapStream.writeRawData((const char *)MapData, len);
    mapFile.close();
    free(MapData);
}

void MainWidget::on_loadTileBtn_clicked()
{
    QSettings settings("QtProject", "BBK_Map_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_tile_dir").toString(), "BMP(*.bmp)");
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        settings.setValue("last_tile_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.sync();
        _Tile.load(fileName);
        ui->mapViewEdit->SetTile(_Tile);
        ui->tileViewSelect->SetTile(_Tile);
    }
}

void MainWidget::on_setTileBtn_clicked()
{
    ui->mapViewEdit->SetBrushType(MapViewEdit::BrushType_Tile);
    ui->setTileBtn->setEnabled(false);
    ui->setFeasibleBtn->setEnabled(true);
    ui->setEventBtn->setEnabled(true);
}

void MainWidget::on_setFeasibleBtn_clicked()
{
    ui->mapViewEdit->SetBrushType(MapViewEdit::BrushType_Feasible);
    ui->setTileBtn->setEnabled(true);
    ui->setFeasibleBtn->setEnabled(false);
    ui->setEventBtn->setEnabled(true);
}

void MainWidget::on_setEventBtn_clicked()
{
    ui->mapViewEdit->SetBrushType(MapViewEdit::BrushType_Event);
    ui->setTileBtn->setEnabled(true);
    ui->setFeasibleBtn->setEnabled(true);
    ui->setEventBtn->setEnabled(false);
}
