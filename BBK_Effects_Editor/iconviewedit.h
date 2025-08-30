#ifndef ICONVIEWEDIT_H
#define ICONVIEWEDIT_H

#include <QDebug>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

#define ICON_DATA_MAX_SIZE  1048576

class IconViewEdit : public QWidget
{
    Q_OBJECT
public:
    explicit IconViewEdit(QWidget *parent = nullptr);
    void ClearView(void);
    void UpdateView(uint8_t *data, uint32_t len);
    void SetIconIndex(int iconIndex);
    void ResetView(void);
    void WriteBack(uint8_t *data, uint32_t len);

    int32_t GetIconWidth(void) {
        return _IconW;
    }

    int32_t GetIconHeight(void) {
        return _IconH;
    }

    int32_t GetIconCount(void) {
        return _IconCnt;
    }

    int32_t GetIconTransparency(void) {
        return _IconTransparency;
    }

    void SetTransColor(QColor transColor);

    void SetHeader(int32_t IconW, int32_t IconH, int32_t IconHorCnt, int32_t IconVerCnt, int32_t IconCnt, int32_t IconTransparency) {
        _IconW = IconW;
        if(_IconW % 8 > 0) {
            _IconW += (8 - _IconW % 8);
        }
        _IconH = IconH;
        _IconHorCnt = IconHorCnt;
        _IconVerCnt = IconVerCnt;
        _IconCnt = IconCnt;
        _IconTransparency = IconTransparency;
        _DataSize = 6 + _IconW * _IconH * _IconCnt * _IconTransparency / 8;
        double horScale = (width()  - 50) / (double)(_IconW * 9);
        double verScale = (height() - 50) / (double)(_IconH * 9);
        _scale = qMin(horScale, verScale);
        _currOrigin.setX((width()  / _scale - _IconW * 3) / 2);
        _currOrigin.setY((height() / _scale - _IconH * 3) / 2);
    }

    enum BrushType {
        BrushType_Black = 0,
        BrushType_White,
        BrushType_Transparent
    };
    void SetBrushType(BrushType brushType);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    int32_t _IconW;
    int32_t _IconH;
    int32_t _IconHorCnt;
    int32_t _IconVerCnt;
    int32_t _IconCnt;
    int32_t _IconTransparency;
    uint8_t _IconData[ICON_DATA_MAX_SIZE];
    uint8_t _IconSrcData[ICON_DATA_MAX_SIZE];
    uint8_t _IconIndex;
    int32_t _DataSize;

    qreal       _scale;
    BrushType   _brushType = BrushType_Black;
    QColor      _transColor = QColor(255, 0, 0);
    QPoint      _prevOrigin = QPoint(0, 0);
    QPoint      _currOrigin = QPoint(0, 0);
    QPoint      _startPoint;
    bool        _mousePressed = false;

private slots:
    void on_posBlack(QPoint pos);
    void on_posWhite(QPoint pos);
    void on_posTransparent(QPoint pos);

signals:
    void posBlack(QPoint pos);
    void posWhite(QPoint pos);
    void posTransparent(QPoint pos);
    void brushTypeChanged(BrushType brushType);

};

#endif // ICONVIEWEDIT_H
