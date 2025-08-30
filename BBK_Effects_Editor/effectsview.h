#ifndef EFFECTSVIEW_H
#define EFFECTSVIEW_H

#include <QDebug>
#include <QWidget>
#include <QPainter>
#include <QApplication>
#include <QContextMenuEvent>
#include <QVector>
#include <QTimer>
#include <QMenu>
#include <QTime>



typedef struct {
    uint32_t showTick;
    uint32_t hideTick;
} TickSH;

class EffectsView : public QWidget
{
    Q_OBJECT
public:
    explicit EffectsView(QWidget *parent = nullptr);
    ~EffectsView();
    void ClearView(void);
    void UpdateView(uint8_t *data, uint32_t len);
    void LoadEffectsOffsetVec(void);
    void ReArrangeEffectsIconIndex(void);
    void LoadEffectsIconOffsetVec(void);
    void SetTickMs(uint32_t tickMs);
    bool IsPlaying(void);
    void PrevTick(void);
    void PlayStop(bool play);
    void NextTick(void);
    void ShowCurrFrame(uint8_t currFrame);
    void ShowCurrTick(uint32_t currTick);
    void ResetView(void);
    QVector<uint16_t> GetEffectsOffsetVec();
    QVector<uint16_t> GetEffectsIconOffsetVec();

protected:
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    uint8_t     *_EffectsData = nullptr;
    int32_t     _EffectsDataSize = 0;

    QMenu       *evMenu;
    QAction     *showBG;

    bool        _showBG = false;
    QImage      bgImage = QImage(":/images/bgImage.png");

    QTimer      _EffectsTimer;
    int32_t     _TickMs = 25;

    QVector<uint16_t>   _EffectsOffsetVec;
    QVector<uint16_t>   _EffectsIconOffsetVec;

    enum {
        Single = 0,
        Overlay
    } _fixMode = Overlay;

    uint8_t _currFrame = 0;

    bool        _isPlaying = true;

    uint32_t    _currTick = 0;
    uint32_t    _TotalTicks = 0;

    QVector<TickSH> InitEffectsTickSHVec(uint32_t *totalTicks);

    void drawIcon(QPainter &painter, int sx, int sy, int iconIdx);

    void Delay_ms(uint32_t ms) {
        QTime dstTime = QTime::currentTime().addMSecs(ms);
        while(QTime::currentTime() < dstTime) {
            QApplication::processEvents(QEventLoop::AllEvents);
        }
    }

private slots:
    void on_showBG(bool checked);
    void on_EffectsTimer_timeout();

signals:
    void playStopChanged(bool play);
    void currFrameChanged(uint8_t currFrame);
    void currTickChanged(uint32_t currTick);

};

#endif // ICONVIEWEDIT_H
