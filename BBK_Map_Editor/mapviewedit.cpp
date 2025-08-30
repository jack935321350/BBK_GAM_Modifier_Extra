#include "mapviewedit.h"
#include "eventcfgdlg.h"
#include "mapcfgdlg.h"
#include "gam_type.h"

MapViewEdit::MapViewEdit(QWidget *parent) : QWidget{parent}
{
    _MapData = (uint8_t *)malloc(MAP_DATA_MAX_SIZE);
    if(_MapData == nullptr) {
        exit(-1);
    }

    _MapSrcData = (uint8_t *)malloc(MAP_DATA_MAX_SIZE);
    if(_MapSrcData == nullptr) {
        exit(-1);
    }

    contextMenu = new QMenu(this);

    mapConfig = new QAction(tr("Map Config"), this);
    contextMenu->addAction(mapConfig);
    connect(mapConfig, &QAction::triggered, this, &MapViewEdit::on_mapConfig);

    contextMenu->addSeparator();

    rowAdd = new QAction(tr("Add Row"), this);
    contextMenu->addAction(rowAdd);
    connect(rowAdd, &QAction::triggered, this, &MapViewEdit::on_rowAdd);

    rowInsert = new QAction(tr("Insert Row"), this);
    contextMenu->addAction(rowInsert);
    connect(rowInsert, &QAction::triggered, this, &MapViewEdit::on_rowInsert);

    rowDelete = new QAction(tr("Delete Row"), this);
    contextMenu->addAction(rowDelete);
    connect(rowDelete, &QAction::triggered, this, &MapViewEdit::on_rowDelete);

    contextMenu->addSeparator();

    colAdd = new QAction(tr("Add Col"), this);
    contextMenu->addAction(colAdd);
    connect(colAdd, &QAction::triggered, this, &MapViewEdit::on_colAdd);

    colInsert = new QAction(tr("Insert Col"), this);
    contextMenu->addAction(colInsert);
    connect(colInsert, &QAction::triggered, this, &MapViewEdit::on_colInsert);

    colDelete = new QAction(tr("Delete Col"), this);
    contextMenu->addAction(colDelete);
    connect(colDelete, &QAction::triggered, this, &MapViewEdit::on_colDelete);

    contextMenu->addSeparator();

    tileVisible = new QAction(tr("Tile Layer"), this);
    tileVisible->setCheckable(true);
    tileVisible->setChecked(true);
    contextMenu->addAction(tileVisible);
    connect(tileVisible, &QAction::triggered, this, &MapViewEdit::on_tileVisible);

    feasibleVisible = new QAction(tr("Feasible Layer"), this);
    feasibleVisible->setCheckable(true);
    feasibleVisible->setChecked(true);
    contextMenu->addAction(feasibleVisible);
    connect(feasibleVisible, &QAction::triggered, this, &MapViewEdit::on_feasibleVisible);

    eventVisible = new QAction(tr("Event Layer"), this);
    eventVisible->setCheckable(true);
    eventVisible->setChecked(true);
    contextMenu->addAction(eventVisible);
    connect(eventVisible, &QAction::triggered, this, &MapViewEdit::on_eventVisible);

    contextMenu->addSeparator();

    saveThumbnail = new QAction(tr("Save Thumbnail"), this);
    contextMenu->addAction(saveThumbnail);
    connect(saveThumbnail, &QAction::triggered, this, &MapViewEdit::on_saveThumbnail);

    connect(this, &MapViewEdit::posTile, this, &MapViewEdit::on_posTile);
    connect(this, &MapViewEdit::posFeasible, this, &MapViewEdit::on_posFeasible);
    connect(this, &MapViewEdit::posEvent, this, &MapViewEdit::on_posEvent);
}

MapViewEdit::~MapViewEdit()
{
    if(_MapData != nullptr) {
        free(_MapData);
        _MapData = nullptr;
    }

    if(_MapSrcData != nullptr) {
        free(_MapSrcData);
        _MapSrcData = nullptr;
    }
}

void MapViewEdit::SetTile(QImage Tile)
{
    _Tile = Tile;

    update();
}

void MapViewEdit::SetTileArgs(uint8_t TileSize, uint8_t TileCntX, uint8_t TileCntY)
{
    _TileSize = TileSize;
    _TileCntX = TileCntX;
    _TileCntY = TileCntY;

    update();
}

void MapViewEdit::SetCurrTile(uint8_t CurrTile)
{
    _CurrTile = CurrTile;
}

void MapViewEdit::Rescale(void)
{
    double horScale = (width() - 50) / (double)(MapWidth() * _TileSize);
    double verScale = (height() - 50) / (double)(MapHeight() * _TileSize);

    _scale = qMin(horScale, verScale);
    _currOrigin.setX((width() / _scale - MapWidth() * _TileSize) / 2);
    _currOrigin.setY((height() / _scale - MapHeight() * _TileSize) / 2);

    _prevOrigin = _currOrigin;

    update();
}

void MapViewEdit::ClearView(void)
{
    _scale = 1.0;
    memset(_MapData, 0, MAP_DATA_MAX_SIZE);
    memset(_MapSrcData, 0, MAP_DATA_MAX_SIZE);

    update();
}

void MapViewEdit::UpdateView(uint8_t *data, uint32_t len)
{
    memcpy_s(_MapData, MAP_DATA_MAX_SIZE, data, len);
    memcpy_s(_MapSrcData, MAP_DATA_MAX_SIZE, data, len);

    emit emit mapArgsChanged(MapName(), _MapData[2]);

    Rescale();
}

void MapViewEdit::ResetView(void)
{
    memcpy_s(_MapData, MAP_DATA_MAX_SIZE, _MapSrcData, MAP_DATA_MAX_SIZE);

    update();
}

void MapViewEdit::WriteBack(uint8_t *data, uint32_t *len)
{
    if(data == nullptr) {
        return;
    }
    *len = sizeof(map_head_t) + sizeof(map_block_t) * MapWidth() * MapHeight();
    memcpy_s(data, *len, _MapData, *len);
}

void MapViewEdit::SetBrushType(BrushType brushType)
{
    _brushType = brushType;
    switch(_brushType)
    {
    case BrushType_Tile:
        setCursor(Qt::ArrowCursor);
        break;

    case BrushType_Feasible:
        setCursor(Qt::PointingHandCursor);
        break;

    case BrushType_Event:
        setCursor(Qt::WhatsThisCursor);
        break;

    default:
        break;
    }
}

void MapViewEdit::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkGray);
    painter.drawRect(rect());

    painter.scale(_scale, _scale);
    painter.translate(_currOrigin);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawRect(0, 0, MapWidth() * _TileSize, MapHeight() * _TileSize);

    map_head_t *map_head = reinterpret_cast<map_head_t *>(_MapData);
    map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));

    painter.setPen(QPen(Qt::darkBlue));
    painter.setBrush(Qt::NoBrush);

    if(_TileSize == 32) {
        painter.setFont(QFont("Consolas", 9));
    }
    else if(_TileSize == 16) {
        painter.setFont(QFont("Consolas", 5));
    }

    for(uint8_t y = 0; y < map_head->height; y++) {
        QString numStr = QString::number(y);
        painter.drawText(- painter.fontMetrics().horizontalAdvance(numStr) - 4,
                         y * _TileSize + _TileSize / 2 + painter.fontMetrics().height() / 4,
                         numStr);
    }

    for(uint8_t x = 0; x < map_head->width; x++) {
        QString numStr = QString::number(x);
        painter.drawText(x * _TileSize + _TileSize / 2 - painter.fontMetrics().horizontalAdvance(numStr) / 2,
                         - painter.fontMetrics().height() / 4,
                         numStr);
    }

    for(uint8_t y = 0; y < map_head->height; y++) {
        for(uint8_t x = 0; x < map_head->width; x++) {
            map_block_t curr_map_block = map_block[y * map_head->width + x];
            int currX = (curr_map_block.tile & 0x7F) % _TileCntX;
            int currY = (curr_map_block.tile & 0x7F) / _TileCntX;

            if(_tileVisible) {
                QImage tileSeg;
                tileSeg = _Tile.copy(currX * _TileSize, currY * _TileSize, _TileSize, _TileSize);
                painter.drawImage(x * _TileSize, y * _TileSize, tileSeg);
            }

            if(curr_map_block.tile & 0x80) {

            }
            else if(_feasibleVisible) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255, 0, 0, 75));
                painter.drawRect(x * _TileSize, y * _TileSize, _TileSize, _TileSize);
            }

            if(_eventVisible && curr_map_block.event > 0) {
                QString eventStr = QString::number(curr_map_block.event);
                if(_TileSize == 32) {
                    painter.setFont(QFont("Consolas", 15));
                }
                else if(_TileSize == 16) {
                    painter.setFont(QFont("Consolas", 7));
                }
                painter.setPen(QPen(Qt::green));
                painter.setBrush(Qt::NoBrush);
                painter.drawText(x * _TileSize + _TileSize / 2 - painter.fontMetrics().horizontalAdvance(eventStr) / 2,
                                 y * _TileSize + _TileSize / 2 + painter.fontMetrics().height() / 4,
                                 eventStr);
            }
        }
    }
}

void MapViewEdit::mousePressEvent(QMouseEvent *event)
{
    QPoint pressedPos(event->pos().x() / _scale - _currOrigin.x(), event->pos().y() / _scale - _currOrigin.y());
    if(event->button() == Qt::LeftButton) {
        switch(_brushType)
        {
        case BrushType_Tile:
            emit posTile(pressedPos, Qt::LeftButton);
            break;

        case BrushType_Feasible:
            emit posFeasible(pressedPos, Qt::LeftButton);
            break;

        default:
            break;
        }
    }
    else if(event->button() == Qt::RightButton) {
        switch(_brushType)
        {
        case BrushType_Tile:
            emit posTile(pressedPos, Qt::RightButton);
            break;

        case BrushType_Feasible:
            emit posFeasible(pressedPos, Qt::RightButton);
            break;

        case BrushType_Event:
            emit posEvent(pressedPos, Qt::RightButton);
            break;

        default:
            break;
        }
    }
    else if(event->button() == Qt::MiddleButton) {
        _startPoint = event->pos();
    }

    update();
}

void MapViewEdit::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pressedPos(event->pos().x() / _scale - _currOrigin.x(), event->pos().y() / _scale - _currOrigin.y());
    if(event->buttons() & Qt::LeftButton) {

        switch(_brushType)
        {
        case BrushType_Tile:
            emit posTile(pressedPos, Qt::LeftButton);
            break;

        case BrushType_Feasible:
            emit posFeasible(pressedPos, Qt::LeftButton);
            break;

        default:
            break;
        }

    }
    else if(event->buttons() & Qt::RightButton) {

        switch(_brushType)
        {
        case BrushType_Feasible:
            emit posFeasible(pressedPos, Qt::RightButton);
            break;

        default:
            break;
        }

    }
    else if(event->buttons() & Qt::MiddleButton) {

        QPoint endPoint = event->pos();
        _currOrigin = (endPoint - _startPoint) / _scale + _prevOrigin;

    }

    if(QRect(0, 0, MapWidth() * _TileSize, MapHeight() * _TileSize).contains(pressedPos)) {
        emit pointPosChanged(pressedPos.x() / _TileSize, pressedPos.y() / _TileSize);
    } else {
        emit pointPosChanged(-1, -1);
    }

    update();
}

void MapViewEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton) {
        _prevOrigin = _currOrigin;
    }

    update();
}

void MapViewEdit::wheelEvent(QWheelEvent *event)
{
    QPoint prevMousePos = event->position().toPoint() / _scale - _currOrigin;
    if(event->angleDelta().y() > 0)
    {
        _scale += _scale / 10;
        _currOrigin = event->position().toPoint() / _scale - prevMousePos;
        _prevOrigin = _currOrigin;
    }
    else if(_scale > 0.005)
    {
        _scale -= _scale / 10;
        _currOrigin = event->position().toPoint() / _scale - prevMousePos;
        _prevOrigin = _currOrigin;
    }

    update();
}

void MapViewEdit::contextMenuEvent(QContextMenuEvent *event)
{
    if(MapWidth() < 1 || MapHeight() < 1) {
        return;
    }

    QPoint pressedPos(event->pos().x() / _scale - _currOrigin.x(), event->pos().y() / _scale - _currOrigin.y());

    if(QRect(0, 0, MapWidth() * _TileSize, MapHeight() * _TileSize).contains(pressedPos)) {
        return;
    }

    if(pressedPos.x() < 0 || pressedPos.x() > MapWidth() * _TileSize) {
        colInsert->setEnabled(false);
        colDelete->setEnabled(false);
    } else {
        colInsert->setEnabled(true);
        colDelete->setEnabled(true);
    }

    if(pressedPos.y() < 0 || pressedPos.y() > MapHeight() * _TileSize) {
        rowInsert->setEnabled(false);
        rowDelete->setEnabled(false);
    } else {
        rowInsert->setEnabled(true);
        rowDelete->setEnabled(true);
    }

    _menuPressedPos = pressedPos;

    contextMenu->exec(event->globalPos());
}

void MapViewEdit::on_posTile(QPoint pos, Qt::MouseButton btn)
{
    if(!QRect(0, 0, MapWidth() * _TileSize, MapHeight() * _TileSize).contains(pos)) {
        return;
    }

    int currX = pos.x() / _TileSize;
    int currY = pos.y() / _TileSize;

    map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));

    if(btn == Qt::LeftButton) {

        map_block[currY * MapWidth() + currX].tile &= 0x80;
        map_block[currY * MapWidth() + currX].tile |= (_CurrTile & 0x7F);

    }
    else if(btn == Qt::RightButton) {

        _CurrTile = (map_block[currY * MapWidth() + currX].tile & 0x7F);
        emit currTileChanged(_CurrTile);

    }

    update();
}

void MapViewEdit::on_posFeasible(QPoint pos, Qt::MouseButton btn)
{
    if(!QRect(0, 0, MapWidth() * _TileSize, MapHeight() * _TileSize).contains(pos)) {
        return;
    }

    int currX = pos.x() / _TileSize;
    int currY = pos.y() / _TileSize;

    map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));

    if(btn == Qt::LeftButton) {

        map_block[currY * MapWidth() + currX].tile &= 0x7F;

    }
    else if(btn == Qt::RightButton) {

        map_block[currY * MapWidth() + currX].tile |= 0x80;

    }

    update();
}

void MapViewEdit::on_posEvent(QPoint pos, Qt::MouseButton btn)
{
    if(btn == Qt::RightButton) {

        int currX = pos.x() / _TileSize;
        int currY = pos.y() / _TileSize;

        map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));

        EventCfgDlg eventCfgDlg(this, map_block[currY * MapWidth() + currX].event);
        if(eventCfgDlg.exec() == EventCfgDlg::Accepted) {
            map_block[currY * MapWidth() + currX].event = eventCfgDlg.Event;
        }
    }

    update();
}

void MapViewEdit::on_mapConfig(bool checked)
{
    Q_UNUSED(checked);

    map_head_t *map_head = reinterpret_cast<map_head_t *>(_MapData);
    MapCfgDlg mapCfgDlg(this, *map_head, false);
    if(mapCfgDlg.exec() == MapCfgDlg::Accepted) {
        memcpy_s(map_head, sizeof(map_head_t), &mapCfgDlg.Map_Head, sizeof(map_head_t));
        emit mapArgsChanged(MapName(), _MapData[2]);
    }
}

void MapViewEdit::on_rowAdd(bool checked)
{
    Q_UNUSED(checked);

    int oldHeight = MapHeight();

    if(_MapData[0x11] < 127) {
        _MapData[0x11]++;
    } else {
        return;
    }

    map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));

    for(int x = 0; x < MapWidth(); x++) {
        map_block[oldHeight * MapWidth() + x].tile = 0x81;
        map_block[oldHeight * MapWidth() + x].event = 0;
    }

    update();
}

void MapViewEdit::on_rowInsert(bool checked)
{
    Q_UNUSED(checked);

    int currY = _menuPressedPos.y() / _TileSize;

    if(_MapData[0x11] < 127) {
        _MapData[0x11]++;
    } else {
        return;
    }

    map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));
    for(int y = MapHeight(); y > currY; y--) {
        memcpy_s(&map_block[y * MapWidth()], sizeof(map_block_t) * MapWidth(), &map_block[(y - 1) * MapWidth()], sizeof(map_block_t) * MapWidth());
    }

    for(int x = 0; x < MapWidth(); x++) {
        map_block[currY * MapWidth() + x].tile = 0x81;
        map_block[currY * MapWidth() + x].event = 0;
    }

    update();
}

void MapViewEdit::on_rowDelete(bool checked)
{
    Q_UNUSED(checked);

    int oldHeight = MapHeight();

    int currY = _menuPressedPos.y() / _TileSize;

    if(_MapData[0x11] > 1) {
        _MapData[0x11]--;
    } else {
        return;
    }

    map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));
    for(int y = currY; y < MapHeight(); y++) {
        memcpy_s(&map_block[y * MapWidth()], sizeof(map_block_t) * MapWidth(), &map_block[(y + 1) * MapWidth()], sizeof(map_block_t) * MapWidth());
    }

    for(int x = 0; x < MapWidth(); x++) {
        map_block[oldHeight * MapWidth() + x].tile = 0x81;
        map_block[oldHeight * MapWidth() + x].event = 0;
    }

    update();
}

void MapViewEdit::on_colAdd(bool checked)
{
    Q_UNUSED(checked);

    uint8_t *MapData = (uint8_t *)malloc(MAP_DATA_MAX_SIZE);
    if(MapData == nullptr) {
        return;
    }

    memcpy_s(MapData, sizeof(map_head_t), _MapData, sizeof(map_head_t));

    if(MapData[0x10] < 127) {
        MapData[0x10]++;
    } else {
        free(MapData);
        return;
    }

    map_block_t *map_block = reinterpret_cast<map_block_t *>(MapData + sizeof(map_head_t));
    map_block_t *_map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));
    for(int y = 0; y < MapData[0x11]; y++) {
        for(int x = 0; x < MapData[0x10]; x++) {
            if(x < (MapData[0x10] - 1)) {
                map_block[y * MapData[0x10] + x] = _map_block[y * MapWidth() + x];
            } else {
                map_block[y * MapData[0x10] + x].tile = 0x81;
                map_block[y * MapData[0x10] + x].event = 0;
            }
        }
    }

    memcpy_s(_MapData, MAP_DATA_MAX_SIZE, MapData, MAP_DATA_MAX_SIZE);

    free(MapData);

    update();
}

void MapViewEdit::on_colInsert(bool checked)
{
    Q_UNUSED(checked);

    int currX = _menuPressedPos.x() / _TileSize;

    uint8_t *MapData = (uint8_t *)malloc(MAP_DATA_MAX_SIZE);
    if(MapData == nullptr) {
        return;
    }

    memcpy_s(MapData, sizeof(map_head_t), _MapData, sizeof(map_head_t));

    if(MapData[0x10] < 127) {
        MapData[0x10]++;
    } else {
        free(MapData);
        return;
    }

    map_block_t *map_block = reinterpret_cast<map_block_t *>(MapData + sizeof(map_head_t));
    map_block_t *_map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));
    for(int y = 0; y < MapData[0x11]; y++) {
        for(int x = 0; x < MapData[0x10]; x++) {
            if(x < currX) {
                map_block[y * MapData[0x10] + x] = _map_block[y * MapWidth() + x];
            }
            else if(x > currX) {
                map_block[y * MapData[0x10] + x] = _map_block[y * MapWidth() + x - 1];
            }
            else {
                map_block[y * MapData[0x10] + x].tile = 0x81;
                map_block[y * MapData[0x10] + x].event = 0;
            }
        }
    }

    memcpy_s(_MapData, MAP_DATA_MAX_SIZE, MapData, MAP_DATA_MAX_SIZE);

    free(MapData);

    update();
}

void MapViewEdit::on_colDelete(bool checked)
{
    Q_UNUSED(checked);

    int currX = _menuPressedPos.x() / _TileSize;

    uint8_t *MapData = (uint8_t *)malloc(MAP_DATA_MAX_SIZE);
    if(MapData == nullptr) {
        return;
    }

    memcpy_s(MapData, sizeof(map_head_t), _MapData, sizeof(map_head_t));

    if(MapData[0x10] > 1) {
        MapData[0x10]--;
    } else {
        free(MapData);
        return;
    }

    map_block_t *map_block = reinterpret_cast<map_block_t *>(MapData + sizeof(map_head_t));
    map_block_t *_map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));
    for(int y = 0; y < MapData[0x11]; y++) {
        for(int x = 0; x < MapData[0x10]; x++) {
            if(x < currX) {
                map_block[y * MapData[0x10] + x] = _map_block[y * MapWidth() + x];
            } else {
                map_block[y * MapData[0x10] + x] = _map_block[y * MapWidth() + x + 1];
            }
        }
    }

    memcpy_s(_MapData, MAP_DATA_MAX_SIZE, MapData, MAP_DATA_MAX_SIZE);

    free(MapData);

    update();
}

void MapViewEdit::on_tileVisible(bool checked)
{
    _tileVisible = checked;

    update();
}

void MapViewEdit::on_feasibleVisible(bool checked)
{
    _feasibleVisible = checked;

    update();
}

void MapViewEdit::on_eventVisible(bool checked)
{
    _eventVisible = checked;

    update();
}

void MapViewEdit::on_saveThumbnail(bool checked)
{
    Q_UNUSED(checked)

    QImage image((MapWidth() + 2) * _TileSize, (MapHeight() + 2) * _TileSize, QImage::Format_ARGB32);

    QPainter painter(&image);

    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkGray);
    painter.drawRect(0, 0, (MapWidth() + 2) * _TileSize, (MapHeight() + 2) * _TileSize);

    painter.scale(1, 1);
    painter.translate(_TileSize, _TileSize);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawRect(0, 0, MapWidth() * _TileSize, MapHeight() * _TileSize);

    map_head_t *map_head = reinterpret_cast<map_head_t *>(_MapData);
    map_block_t *map_block = reinterpret_cast<map_block_t *>(_MapData + sizeof(map_head_t));

    painter.setPen(QPen(Qt::darkBlue));
    painter.setBrush(Qt::NoBrush);

    if(_TileSize == 32) {
        painter.setFont(QFont("Consolas", 9));
    }
    else if(_TileSize == 16) {
        painter.setFont(QFont("Consolas", 5));
    }

    for(uint8_t y = 0; y < map_head->height; y++) {
        QString numStr = QString::number(y);
        painter.drawText(- painter.fontMetrics().horizontalAdvance(numStr) - 4,
                         y * _TileSize + _TileSize / 2 + painter.fontMetrics().height() / 4,
                         numStr);
    }

    for(uint8_t x = 0; x < map_head->width; x++) {
        QString numStr = QString::number(x);
        painter.drawText(x * _TileSize + _TileSize / 2 - painter.fontMetrics().horizontalAdvance(numStr) / 2,
                         - painter.fontMetrics().height() / 4,
                         numStr);
    }

    for(uint8_t y = 0; y < map_head->height; y++) {
        for(uint8_t x = 0; x < map_head->width; x++) {
            map_block_t curr_map_block = map_block[y * map_head->width + x];
            int currX = (curr_map_block.tile & 0x7F) % _TileCntX;
            int currY = (curr_map_block.tile & 0x7F) / _TileCntX;

            if(_tileVisible) {
                QImage tileSeg;
                tileSeg = _Tile.copy(currX * _TileSize, currY * _TileSize, _TileSize, _TileSize);
                painter.drawImage(x * _TileSize, y * _TileSize, tileSeg);
            }

            if(curr_map_block.tile & 0x80) {

            }
            else if(_feasibleVisible) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255, 0, 0, 75));
                painter.drawRect(x * _TileSize, y * _TileSize, _TileSize, _TileSize);
            }

            if(_eventVisible && curr_map_block.event > 0) {
                QString eventStr = QString::number(curr_map_block.event);
                if(_TileSize == 32) {
                    painter.setFont(QFont("Consolas", 15));
                }
                else if(_TileSize == 16) {
                    painter.setFont(QFont("Consolas", 7));
                }
                painter.setPen(QPen(Qt::green));
                painter.setBrush(Qt::NoBrush);
                painter.drawText(x * _TileSize + _TileSize / 2 - painter.fontMetrics().horizontalAdvance(eventStr) / 2,
                                 y * _TileSize + _TileSize / 2 + painter.fontMetrics().height() / 4,
                                 eventStr);
            }
        }
    }

    painter.end();

    QSettings settings("QtProject", "BBK_Map_Editor");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save"), settings.value("last_thumb_dir").toString() + '/' + MapName() + ".bmp", "BMP(*.bmp)");
    if(!fileName.isEmpty() && image.save(fileName)) {
        settings.setValue("last_thumb_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.sync();
    }
}
