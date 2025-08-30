#include "effectsview.h"

#include "gam_type.h"

EffectsView::EffectsView(QWidget *parent) : QWidget{parent}
{
    evMenu = new QMenu(this);

    showBG = new QAction(tr("Show Background"), this);
    showBG->setCheckable(true);
    evMenu->addAction(showBG);
    connect(showBG, &QAction::triggered, this, &EffectsView::on_showBG);

    ClearView();

    connect(&_EffectsTimer, &QTimer::timeout, this, &EffectsView::on_EffectsTimer_timeout);
}

EffectsView::~EffectsView()
{

}

void EffectsView::ClearView(void)
{
    _isPlaying = false;
    _EffectsTimer.stop();

    _EffectsData = nullptr;
    _EffectsDataSize = 0;

    _EffectsOffsetVec.clear();
    _EffectsIconOffsetVec.clear();

    emit playStopChanged(_isPlaying);

    update();
}

void EffectsView::UpdateView(uint8_t *data, uint32_t len)
{
    _EffectsData = data;
    _EffectsDataSize = len;

    LoadEffectsOffsetVec();
    LoadEffectsIconOffsetVec();

    update();
}

void EffectsView::LoadEffectsOffsetVec(void)
{
    _EffectsOffsetVec.clear();

    if(_EffectsData == nullptr) {
        return;
    }

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    uint16_t effectsOffset = sizeof(effects_head_t);

    for(uint8_t i = 0; i < effects_head->frames; i++) {
        _EffectsOffsetVec.append(effectsOffset + sizeof(effects_block_t) * i);
    }
}

void EffectsView::ReArrangeEffectsIconIndex(void)
{
    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    uint16_t effectsIconOffset = sizeof(effects_head_t) + sizeof(effects_block_t) * effects_head->frames;

    for(uint8_t i = 0; i < effects_head->icon_max; i++) {
        reinterpret_cast<effects_icon_head_t *>(_EffectsData + effectsIconOffset)->idx = i + 1;
        effectsIconOffset += calc_effects_icon_size(reinterpret_cast<effects_icon_head_t *>(_EffectsData + effectsIconOffset));
    }
}

void EffectsView::LoadEffectsIconOffsetVec(void)
{
    _EffectsIconOffsetVec.clear();

    if(_EffectsData == nullptr) {
        return;
    }

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    uint16_t effectsIconOffset = sizeof(effects_head_t) + sizeof(effects_block_t) * effects_head->frames;

    for(uint8_t i = 0; i < effects_head->icon_max; i++) {
        _EffectsIconOffsetVec.append(effectsIconOffset);
        effectsIconOffset += calc_effects_icon_size(reinterpret_cast<effects_icon_head_t *>(_EffectsData + effectsIconOffset));
    }
}

void EffectsView::SetTickMs(uint32_t tickMs)
{
    _TickMs = tickMs;
    if(_isPlaying) {
        _EffectsTimer.stop();
        _EffectsTimer.start(_TickMs);
    }
}

bool EffectsView::IsPlaying(void)
{
    return _isPlaying;
}

void EffectsView::PrevTick(void)
{
    _fixMode = Overlay;
    _isPlaying = false;
    _EffectsTimer.stop();
    if(_currTick > 0) {
        _currTick--;
        emit currTickChanged(_currTick);
    }
    emit playStopChanged(_isPlaying);
    update();
}

void EffectsView::PlayStop(bool play)
{
    _fixMode = Overlay;
    _EffectsTimer.stop();
    _isPlaying = play;
    if(_isPlaying) {
        _EffectsTimer.start(_TickMs);
    }
    emit playStopChanged(_isPlaying);
}

void EffectsView::NextTick(void)
{
    _fixMode = Overlay;
    _isPlaying = false;
    _EffectsTimer.stop();
    if(_currTick < _TotalTicks) {
        _currTick++;
        emit currTickChanged(_currTick);
    }
    emit playStopChanged(_isPlaying);
    update();
}

void EffectsView::ShowCurrFrame(uint8_t currFrame)
{
    _fixMode = Single;
    _isPlaying = false;
    _currFrame = currFrame;
    emit currFrameChanged(_currFrame);
    emit playStopChanged(_isPlaying);
    update();
}

void EffectsView::ShowCurrTick(uint32_t currTick)
{
    _fixMode = Overlay;
    _currTick = currTick;
    update();
}

void EffectsView::ResetView(void)
{
    _fixMode = Single;
    _currFrame = 0;
    _currTick = 0;
    emit currFrameChanged(_currFrame);
    emit currTickChanged(_currTick);
    update();
}

QVector<uint16_t> EffectsView::GetEffectsOffsetVec()
{
    return _EffectsOffsetVec;
}

QVector<uint16_t> EffectsView::GetEffectsIconOffsetVec()
{
    return _EffectsIconOffsetVec;
}

void EffectsView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    if(_showBG) {

        painter.drawImage(rect(), bgImage);
    } else {

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::darkGray);
        painter.drawRect(rect());
    }

    if(_EffectsData == nullptr) {
        return;
    }

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    if(effects_head->frames < 1 || effects_head->icon_max < 1) {
        return;
    }

    if(_fixMode == Single) {

        effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + _EffectsOffsetVec[_currFrame]);
        drawIcon(painter, effects_block->x, effects_block->y, effects_block->icon_idx);
    }
    else if(_fixMode == Overlay) {

        bool currFramValid = false;
        QVector<TickSH> EffectsTickSHVec = InitEffectsTickSHVec(&_TotalTicks);
        for(uint8_t i = 0; i < EffectsTickSHVec.size(); i++) {
            if(EffectsTickSHVec[i].showTick <= _currTick && EffectsTickSHVec[i].hideTick >= _currTick) {
                if(!currFramValid) {
                    currFramValid = true;
                    _currFrame = i;
                    emit currFrameChanged(_currFrame);
                }
                effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + _EffectsOffsetVec[i]);
                drawIcon(painter, effects_block->x, effects_block->y, effects_block->icon_idx);
            }
        }

    }
}

void EffectsView::contextMenuEvent(QContextMenuEvent *event)
{
    evMenu->exec(event->globalPos());
}

QVector<TickSH> EffectsView::InitEffectsTickSHVec(uint32_t *totalTicks)
{
    QVector<TickSH> effectsTickSHVec;

    if(_EffectsData == nullptr) {
        *totalTicks = 0;
        return effectsTickSHVec;
    }

    QVector<effects_block_t> EffectsBlockVec;
    for(uint8_t i = 0; i < _EffectsOffsetVec.size(); i++) {
        effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + _EffectsOffsetVec[i]);
        EffectsBlockVec.append(*effects_block);
    }

    int32_t showTick = 0;
    for(uint8_t i = 0; i < EffectsBlockVec.size(); i++) {
        if(i > 0) {
            showTick += EffectsBlockVec[i - 1].n_show;
        }
        TickSH tickSH;
        tickSH.showTick = showTick;
        tickSH.hideTick = showTick + EffectsBlockVec[i].show;
        effectsTickSHVec.append(tickSH);
    }

    *totalTicks = effectsTickSHVec.last().hideTick;

    return effectsTickSHVec;
}

void EffectsView::drawIcon(QPainter &painter, int sx, int sy, int iconIdx)
{
    if(_EffectsData == nullptr) {
        return;
    }

    effects_icon_head_t *effects_icon_head = nullptr;
    for(int i = 0; i < _EffectsIconOffsetVec.size(); i++) {
        effects_icon_head = reinterpret_cast<effects_icon_head_t *>(_EffectsData + _EffectsIconOffsetVec[iconIdx]);
        if(effects_icon_head->idx == iconIdx) {
            break;
        }
    }

    if(effects_icon_head == nullptr) {
        return;
    }

    int width = effects_icon_head->width;
    if(width % 8 > 0) {
        width += (8 - width % 8);
    }

    painter.setPen(Qt::NoPen);

    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(effects_icon_head) + sizeof(effects_icon_head_t);
    if(effects_icon_head->transparency < 2) {
        for(int y = 0; y < effects_icon_head->height; y++) {
            for(int i = 0; i < (width / 8); i++) {
                uint8_t data = dataPtr[y * width / 8 + i];
                for(int j = 0; j < 8; j++) {
                    if(data & (1 << (7 - j))) {
                        painter.setBrush(Qt::black);
                    } else {
                        painter.setBrush(Qt::white);
                    }
                    if((i * 8 + j) < effects_icon_head->width) {
                        painter.drawRect((sx + i * 8 + j) * 2, (sy + y) * 2, 2, 2);
                    }
                }
            }
        }
    }
    else if(effects_icon_head->transparency < 3) {
        for(int y = 0; y < effects_icon_head->height; y++) {
            for(int i = 0; i < (width / 4); i++) {
                uint8_t data = dataPtr[y * width / 4 + i];
                for(int j = 0; j < 4; j++) {
                    if((data & (1 << ((7 - j * 2)))) == 0) {
                        if(data & (1 << ((6 - j * 2)))) {
                            painter.setBrush(Qt::black);
                        } else {
                            painter.setBrush(Qt::white);
                        }
                        if((i * 4 + j) < effects_icon_head->width) {
                            painter.drawRect((sx + i * 4 + j) * 2, (sy + y) * 2, 2, 2);
                        }
                    }
                }
            }
        }
    }
}

void EffectsView::on_showBG(bool checked)
{
    _showBG = checked;
    update();
}

void EffectsView::on_EffectsTimer_timeout()
{
    if(_TotalTicks > 0) {
        if(!_isPlaying) {
            _EffectsTimer.stop();
            return;
        }
        _currTick = (_currTick + 1) % (_TotalTicks + 1);
        if(_currTick == 0) {
            _EffectsTimer.stop();
            Delay_ms(1000);
            if(_isPlaying) {
                _EffectsTimer.start(_TickMs);
            }
        }
        emit currTickChanged(_currTick);
    }
    update();
}
