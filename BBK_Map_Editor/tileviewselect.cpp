#include "tileviewselect.h"
#include "tilecfgdlg.h"

TileViewSelect::TileViewSelect(QWidget *parent) : QWidget{parent}
{
    contextMenu = new QMenu(this);

    is16x16 = new QAction("16 x 16", this);
    is16x16->setCheckable(true);
    is16x16->setChecked(false);
    contextMenu->addAction(is16x16);
    connect(is16x16, &QAction::triggered, this, &TileViewSelect::on_is16x16);

    contextMenu->addSeparator();

    tileConfig = new QAction(tr("Tile Config"), this);
    contextMenu->addAction(tileConfig);
    connect(tileConfig, &QAction::triggered, this, &TileViewSelect::on_tileConfig);
}

void TileViewSelect::SetTile(QImage Tile)
{
    if(Tile.width() < 16) {
        return;
    }
    else if(Tile.width() < 32) {
        is16x16->setChecked(true);
        _TileSize = 16;
    }

    _Tile = Tile;

    _TileCntX = _Tile.width() / _TileSize;
    _TileCntY = _Tile.height() / _TileSize;
    _TileMax = _TileCntX * _TileCntY - 1;

    for(uint8_t y = _TileCntY - 1; y > 1; y--) {
        for(uint8_t x = _TileCntX - 1; x > 1; x--) {
            QImage tileSeg;
            tileSeg = _Tile.copy(x * _TileSize, y * _TileSize, _TileSize, _TileSize);
            bool allWhite = true;
            for(uint8_t yy = 0; yy < _TileSize; yy++) {
                for(uint8_t xx = 0; xx < _TileSize; xx++) {
                    if(tileSeg.pixel(xx, yy) != 0xFFFFFFFF) {
                        allWhite = false;
                    }
                }
            }
            if(!allWhite) {
                break;
            }
            _TileMax--;
        }
    }

    if(_CurrTile > _TileMax) {
        _CurrTile = _TileMax;
    }

    emit tileArgsChanged(_TileSize, _TileCntX, _TileCntY);

    Rescale();
}

void TileViewSelect::SetCurrTile(uint8_t CurrTile)
{
    _CurrTile = CurrTile;

    update();
}

void TileViewSelect::Rescale(void)
{
    double horScale = (width() - 50) / (double)(_TileCntX * (_TileSize + 1));
    double verScale = (height() - 50) / (double)(_TileCntY * (_TileSize + 1));
    _scale = qMin(horScale, verScale);

    _currOrigin.setX((width() / _scale - _TileCntX * (_TileSize + 1)) / 2);
    _currOrigin.setY((height() / _scale - _TileCntY * (_TileSize + 1)) / 2);

    _prevOrigin = _currOrigin;

    update();
}

void TileViewSelect::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkGray);
    painter.drawRect(rect());

    painter.scale(_scale, _scale);
    painter.translate(_currOrigin);

    painter.setPen(QPen(Qt::darkMagenta));
    painter.setBrush(Qt::NoBrush);

    if(_TileSize == 32) {
        painter.setFont(QFont("Consolas", 9));
    }
    else if(_TileSize == 16) {
        painter.setFont(QFont("Consolas", 5));
    }

    for(uint8_t y = 0; y < _TileCntY; y++) {
        QString numStr = QString::number(y);
        painter.drawText(- painter.fontMetrics().horizontalAdvance(numStr) - 4,
                         y * (_TileSize + 1) + (_TileSize + 1) / 2 + painter.fontMetrics().height() / 4,
                         numStr);
    }

    for(uint8_t x = 0; x < _TileCntX; x ++) {
        QString numStr = QString::number(x);
        painter.drawText(x * (_TileSize + 1) + (_TileSize + 1) / 2 - painter.fontMetrics().horizontalAdvance(numStr) / 2,
                         - painter.fontMetrics().height() / 4,
                         numStr);
    }

    for(uint8_t y = 0; y < _TileCntY; y++) {
        for(uint8_t x = 0; x < _TileCntX; x++) {
            QImage tileSeg;
            tileSeg = _Tile.copy(x * _TileSize, y * _TileSize, _TileSize, _TileSize);
            painter.drawImage(x * (_TileSize + 1), y * (_TileSize + 1), tileSeg);
        }
    }

    if(!_Tile.isNull() && _TileCntX > 0) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::NoBrush);
        int currX = _CurrTile % _TileCntX;
        int currY = _CurrTile / _TileCntX;
        painter.drawRect(currX * (_TileSize + 1) - 1, currY * (_TileSize + 1) - 1, _TileSize + 2, _TileSize + 2);
    }
}

void TileViewSelect::mousePressEvent(QMouseEvent *event)
{
    QPoint pressedPos(event->pos().x() / _scale - _currOrigin.x(), event->pos().y() / _scale - _currOrigin.y());

    if(event->button() == Qt::LeftButton) {

        if(!QRect(0, 0, _TileCntX * (_TileSize + 1), _TileCntY * (_TileSize + 1)).contains(pressedPos)) {
            return;
        }

        int currX = pressedPos.x() / (_TileSize + 1);
        int currY = pressedPos.y() / (_TileSize + 1);

        _CurrTile = currY * _TileCntX + currX;

        if(_CurrTile < 1) {
            _CurrTile = 1;
        }

        if(_CurrTile > _TileMax) {
            _CurrTile = _TileMax;
        }

        emit currTileChanged(_CurrTile);

    }
    else if(event->button() == Qt::RightButton) {

    }
    else if(event->button() == Qt::MiddleButton) {

        _startPoint = event->pos();

    }

    update();
}

void TileViewSelect::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pressedPos(event->pos().x() / _scale - _currOrigin.x(), event->pos().y() / _scale - _currOrigin.y());

    if(event->buttons() & Qt::MiddleButton) {

        QPoint endPoint = event->pos();
        _currOrigin = (endPoint - _startPoint) / _scale + _prevOrigin;

    }

    update();
}

void TileViewSelect::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton) {

        _prevOrigin = _currOrigin;

    }

    update();
}

void TileViewSelect::wheelEvent(QWheelEvent *event)
{
    QPoint prevMousePos = event->position().toPoint() / _scale - _currOrigin;
    if(event->angleDelta().y() > 0) {

        _scale += _scale / 10;
        _currOrigin = event->position().toPoint() / _scale - prevMousePos;
        _prevOrigin = _currOrigin;

    }
    else if(_scale > 0.005) {

        _scale -= _scale / 10;
        _currOrigin = event->position().toPoint() / _scale - prevMousePos;
        _prevOrigin = _currOrigin;

    }

    update();
}

void TileViewSelect::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
}

void TileViewSelect::keyReleaseEvent(QKeyEvent *event)
{
    int currX = _CurrTile % _TileCntX;
    int currY = _CurrTile / _TileCntX;
    int TileIndex = _CurrTile;

    switch(event->key())
    {
    case Qt::Key_Up:
    case Qt::Key_W:
        if(currY > 0) {
            currY--;
            TileIndex = currY * _TileCntX + currX;
        }
        break;

    case Qt::Key_Down:
    case Qt::Key_S:
        if(currY < (_TileCntY - 1)) {
            currY++;
            TileIndex = currY * _TileCntX + currX;
        }
        break;

    case Qt::Key_Left:
    case Qt::Key_A:
        TileIndex--;
        break;

    case Qt::Key_Right:
    case Qt::Key_D:
        TileIndex++;
        break;

    default:
        break;
    }

    if(TileIndex > 0 && TileIndex <= _TileMax) {
        _CurrTile = TileIndex;
        emit currTileChanged(_CurrTile);
        update();
    }

    QWidget::keyReleaseEvent(event);
}

void TileViewSelect::contextMenuEvent(QContextMenuEvent *event)
{
    if(_Tile.isNull() || _TileCntX < 1) {
        return;
    }

    contextMenu->exec(event->globalPos());
}

void TileViewSelect::on_is16x16(bool checked)
{
    if(_Tile.width() < 32) {
        is16x16->setChecked(true);
        return;
    }

    _TileSize = checked ? 16 : 32;
    _TileCntX = _Tile.width() / _TileSize;
    _TileCntY = _Tile.height() / _TileSize;
    _TileMax = _TileCntX * _TileCntY - 1;

    for(uint8_t y = _TileCntY - 1; y > 1; y--) {
        for(uint8_t x = _TileCntX - 1; x > 1; x--) {
            QImage tileSeg;
            tileSeg = _Tile.copy(x * _TileSize, y * _TileSize, _TileSize, _TileSize);
            bool allWhite = true;
            for(uint8_t yy = 0; yy < _TileSize; yy++) {
                for(uint8_t xx = 0; xx < _TileSize; xx++) {
                    if(tileSeg.pixel(xx, yy) != 0xFFFFFFFF) {
                        allWhite = false;
                    }
                }
            }
            if(!allWhite) {
                break;
            }
            _TileMax--;
        }
    }

    if(_CurrTile > _TileMax) {
        _CurrTile = _TileMax;
    }

    emit tileArgsChanged(_TileSize, _TileCntX, _TileCntY);

    Rescale();
}

void TileViewSelect::on_tileConfig(bool checked)
{
    Q_UNUSED(checked)

    TileCfgDlg tileCfgDlg(this, _TileMax);
    if(tileCfgDlg.exec() == TileCfgDlg::Accepted) {
        _TileMax = tileCfgDlg.TileMax;
        if(_CurrTile > _TileMax) {
            _CurrTile = _TileMax;
            update();
        }
    }
}
