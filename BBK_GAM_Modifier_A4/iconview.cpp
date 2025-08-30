#include "iconview.h"

IconView::IconView(QWidget *parent) : QWidget{parent}
{

}

void IconView::ClearView(void)
{
    _scale = 1.0;
    _IconW = 0;
    _IconH = 0;
    _IconCnt = 0;
    _IconTransparency = 0;
    memset(_IconData, 0, ICON_DATA_MAX_SIZE);
    _IconIndex = 0;
    update();
}

void IconView::UpdateView(uint8_t *data, uint32_t len)
{
    int32_t iconSrcW    = data[2];
    _IconW              = data[2];
    if(_IconW % 4 > 0) {
        _IconW += (4 - _IconW % 4);
    }
    _IconH              = data[3];
    _IconCnt            = data[4];
    _IconTransparency   = data[5];
    uint8_t transparency = _IconTransparency;
    if(transparency == 3) {
        transparency = 2;
    }
    _DataSize = 6 + _IconW * _IconH * _IconCnt * transparency / 8;
    if(_DataSize != len) {
        _DataSize = 0;
        return;
    }
    memcpy_s(_IconData, ICON_DATA_MAX_SIZE, data + 6, len - 6);
    if(iconSrcW < _IconW) {
        if(_IconTransparency == 1) {
            int bytesPerRow = _IconW / 8;
            for(int y = 0; y < _IconH; y++) {
                for(int x = iconSrcW; x < _IconW; x++) {
                    int index = y * bytesPerRow + x / 8;
                    _IconData[index] = _IconData[index] & ~(1 << (7 - (x % 8)));
                }
            }
        }
        else if(_IconTransparency == 2) {
            int bytesPerRow = _IconW / 4;
            for(int y = 0; y < _IconH; y++) {
                for(int x = iconSrcW; x < _IconW; x++) {
                    int index = y * bytesPerRow + x / 4;
                    _IconData[index] = _IconData[index] & ~(1 << (6 - (x % 4) * 2));
                }
            }
        }
        else if(_IconTransparency == 3) {
            int bytesPerRow = _IconW / 4;
            for(int y = 0; y < _IconH; y++) {
                for(int x = iconSrcW; x < _IconW; x++) {
                    int index = y * bytesPerRow + x / 4;
                    _IconData[index] = _IconData[index] |  (1 << (7 - (x % 4) * 2));
                    _IconData[index] = _IconData[index] & ~(1 << (6 - (x % 4) * 2));
                }
            }
        }
        else if(_IconTransparency == 4) {
            int bytesPerRow = _IconW / 4;
            for(int y = 0; y < _IconH; y++) {
                for(int x = iconSrcW; x < _IconW; x++) {
                    int index = y * bytesPerRow + x / 4;
                    _IconData[index] = _IconData[index] & ~(1 << (7 - (x % 4) * 2));
                    _IconData[index] = _IconData[index] |  (1 << (6 - (x % 4) * 2));
                }
            }
        }
    }
    double horScale = (double)width() / (double)(_IconW + 8);
    double verScale = (double)height() / (double)(_IconH + 8);
    _scale = qMin(horScale, verScale);
    update();
}

void IconView::SetIconIndex(int iconIndex)
{
    if(iconIndex < _IconCnt) {
        _IconIndex = iconIndex;
        update();
    }
}

void IconView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    painter.scale(_scale, _scale);
    painter.translate(4, 4);

    uint8_t *dataPtr = _IconData;
    if(_IconIndex < _IconCnt) {
        uint8_t transparency = _IconTransparency;
        if(transparency == 3) {
            transparency = 2;
        }
        dataPtr += _IconW * _IconH * transparency / 8 * _IconIndex;
    }

    if(_IconTransparency == 1) {
        for(int y = 0; y < _IconH; y++) {
            for(int i = 0; i < (_IconW / 8); i++) {
                uint8_t data = dataPtr[y * _IconW / 8 + i];
                for(int j = 0; j < 8; j++) {
                    if(data & (1 << (7 - j))) {
                        painter.drawRect((i * 8 + j), y, 1, 1);
                    }
                }
            }
        }
    }
    else if(_IconTransparency == 2) {
        for(int y = 0; y < _IconH; y++) {
            for(int i = 0; i < (_IconW / 4); i++) {
                uint8_t data = dataPtr[y * _IconW / 4 + i];
                for(int j = 0; j < 4; j++) {
                    if(data & (1 << ((6 - j * 2)))) {
                        painter.drawRect((i * 4 + j), y, 1, 1);
                    }
                }
            }
        }
    }
    else if(_IconTransparency == 3) {
        for(int y = 0; y < _IconH; y++) {
            for(int i = 0; i < (_IconW / 4); i++) {
                uint8_t data = dataPtr[y * _IconW / 4 + i];
                for(int j = 0; j < 4; j++) {
                    if((data & (1 << ((7 - j * 2)))) == 0) {
                        if(data & (1 << ((6 - j * 2)))) {
                            painter.setBrush(Qt::white);
                        } else {
                            painter.setBrush(Qt::black);
                        }
                        painter.drawRect((i * 4 + j), y, 1, 1);
                    }
                }
            }
        }
    }
    else if(_IconTransparency == 4) {
        for(int y = 0; y < _IconH; y++) {
            for(int i = 0; i < (_IconW / 4); i++) {
                uint8_t dataF = dataPtr[y * _IconW / 4 + i];
                uint8_t dataR = dataPtr[_IconW * _IconH / 4 + y * _IconW / 4 + i];
                for(int j = 0; j < 4; j++) {
                    if(dataF & (1 << ((7 - j * 2)))) {
                        painter.setBrush(Qt::white);
                        painter.drawRect((i * 4 + j), y, 1, 1);
                    }
                    if((dataR & (1 << ((7 - j * 2)))) == 0) {
                        painter.setBrush(Qt::black);
                        painter.drawRect((i * 4 + j), y, 1, 1);
                    }
                }
            }
        }
    }
}
