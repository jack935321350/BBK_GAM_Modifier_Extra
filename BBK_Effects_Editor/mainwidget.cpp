#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "iconvieweditdlg.h"
#include "gam_type.h"



MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    //adjustSize();

    _buildDateTimeStr = QLocale(QLocale::English).toDateTime(QString(__DATE__ " " __TIME__).replace("  ", " 0"), "MMM dd yyyy hh:mm:ss").toString("yyyyMMdd_hhmm");

    setWindowTitle(tr("BBK_Effects_Editor_") + _buildDateTimeStr);

    QSettings settings("QtProject", "BBK_Effects_Editor");

    _EffectsData = (uint8_t *)malloc(EFFECTS_DATA_MAX_SIZE);
    _EffectsSrcData = (uint8_t *)malloc(EFFECTS_DATA_MAX_SIZE);
    _PicData = (uint8_t *)malloc(EFFECTS_DATA_MAX_SIZE);
    if(_EffectsData == nullptr || _EffectsSrcData == nullptr || _PicData == nullptr) {
        if(_EffectsData != nullptr) {
            free(_EffectsData);
        }
        if(_EffectsSrcData != nullptr) {
            free(_EffectsSrcData);
        }
        if(_PicData != nullptr) {
            free(_PicData);
        }
        exit(-1);
    }

    framesListMenu = new QMenu(this);

    addFrame = new QAction(tr("Add Frame"), this);
    framesListMenu->addAction(addFrame);
    connect(addFrame, &QAction::triggered, this, &MainWidget::on_addFrame);

    insFrame = new QAction(tr("Insert Frame"), this);
    framesListMenu->addAction(insFrame);
    connect(insFrame, &QAction::triggered, this, &MainWidget::on_insFrame);

    delFrame = new QAction(tr("Delete Frame"), this);
    framesListMenu->addAction(delFrame);
    connect(delFrame, &QAction::triggered, this, &MainWidget::on_delFrame);

    iconsListMenu = new QMenu(this);

    edtIcon = new QAction(tr("Edit Icon"), this);
    iconsListMenu->addAction(edtIcon);
    connect(edtIcon, &QAction::triggered, this, &MainWidget::on_edtIcon);

    addIcon = new QAction(tr("Add Icon"), this);
    iconsListMenu->addAction(addIcon);
    connect(addIcon, &QAction::triggered, this, &MainWidget::on_addIcon);

    insIcon = new QAction(tr("Insert Icon"), this);
    iconsListMenu->addAction(insIcon);
    connect(insIcon, &QAction::triggered, this, &MainWidget::on_insIcon);

    delIcon = new QAction(tr("Delete Icon"), this);
    iconsListMenu->addAction(delIcon);
    connect(delIcon, &QAction::triggered, this, &MainWidget::on_delIcon);
}

MainWidget::~MainWidget()
{
    if(_EffectsData != nullptr) {
        free(_EffectsData);
    }
    if(_EffectsSrcData != nullptr) {
        free(_EffectsSrcData);
    }
    if(_PicData != nullptr) {
        free(_PicData);
    }

    QSettings settings("QtProject", "BBK_Effects_Editor");

    delete ui;
}

void MainWidget::on_srsData_changed()
{
    disconnect(ui->effectsType,     &QComboBox::currentTextChanged,                 this,   &MainWidget::on_srsUI_changed);
    disconnect(ui->effectsIdx,      QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    disconnect(ui->totalFrames,     QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    disconnect(ui->beginFrame,      QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    disconnect(ui->endFrame,        QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    disconnect(ui->maxIconIdx,      QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);

    ui->effectsType->setCurrentIndex(effects_head->type - 1);
    ui->effectsIdx->setValue(effects_head->idx);
    ui->totalFrames->setValue(effects_head->frames);
    ui->beginFrame->setValue(effects_head->s_frame);
    ui->endFrame->setValue(effects_head->e_frame);
    ui->maxIconIdx->setValue(effects_head->icon_max);

    ui->effectsView->ClearView();
    ui->effectsView->UpdateView(_EffectsData, _EffectsDataSize);
    ui->effectsView->ResetView();

    ui->playTick->setText(tr("Play"));

    ui->framesList->clear();
    for(int i = 0; i < ui->effectsView->GetEffectsOffsetVec().size(); i++) {
        ui->framesList->addItem(QString("%1").arg(i, 3, 10, QLatin1Char('0')));
    }

    ui->iconIdx->setRange(0, effects_head->icon_max - 1);

    ui->iconsList->clear();
    for(int i = 0; i < ui->effectsView->GetEffectsIconOffsetVec().size(); i++) {
        ui->iconsList->addItem(QString("%1").arg(i, 3, 10, QLatin1Char('0')));
    }

    connect(ui->effectsType,    &QComboBox::currentTextChanged,                 this,   &MainWidget::on_srsUI_changed);
    connect(ui->effectsIdx,     QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    connect(ui->totalFrames,    QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    connect(ui->beginFrame,     QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    connect(ui->endFrame,       QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
    connect(ui->maxIconIdx,     QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_srsUI_changed);
}

void MainWidget::on_srsUI_changed()
{
    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);

    effects_head->type = ui->effectsType->currentIndex() + 1;
    effects_head->idx = ui->effectsIdx->value();
    effects_head->frames = ui->totalFrames->value();
    effects_head->s_frame = ui->beginFrame->value();
    effects_head->e_frame = ui->endFrame->value();
    effects_head->icon_max = ui->maxIconIdx->value();
}

void MainWidget::on_createSrs_clicked()
{
    if(_EffectsDataSize > 0) {
        return;
    }

    memset(_EffectsData, 0, EFFECTS_DATA_MAX_SIZE);
    _EffectsData[0] = 1;
    _EffectsData[1] = 1;
    _EffectsData[2] = 1;
    _EffectsData[3] = 0;
    _EffectsData[4] = 0;
    _EffectsData[5] = 0;
    _EffectsDataSize = 11;

    memset(_EffectsSrcData, 0, EFFECTS_DATA_MAX_SIZE);
    _EffectsSrcData[0] = 1;
    _EffectsSrcData[1] = 1;
    _EffectsSrcData[2] = 1;
    _EffectsSrcData[3] = 0;
    _EffectsSrcData[4] = 0;
    _EffectsSrcData[5] = 0;
    _EffectsSrcDataSize = 11;

    on_srsData_changed();
}

void MainWidget::on_loadSrs_clicked()
{
    QSettings settings("QtProject", "BBK_Effects_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_srs_dir").toString() + '/' + settings.value("last_srs_file").toString(), "SRS(*.srs)");
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        settings.setValue("last_srs_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.setValue("last_srs_file", QFileInfo(fileName).fileName());
        settings.sync();

        QFile srsFile(fileName);
        if(!srsFile.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray srsBA = srsFile.readAll();
        srsFile.close();

        memset(_EffectsData, 0, EFFECTS_DATA_MAX_SIZE);
        memcpy_s(_EffectsData, srsBA.size(), srsBA.data(), srsBA.size());
        _EffectsDataSize = srsBA.size();

        memset(_EffectsSrcData, 0, EFFECTS_DATA_MAX_SIZE);
        memcpy_s(_EffectsSrcData, srsBA.size(), srsBA.data(), srsBA.size());
        _EffectsSrcDataSize = srsBA.size();

        on_srsData_changed();
    }
}

void MainWidget::on_resetSrs_clicked()
{
    memcpy_s(_EffectsData, _EffectsSrcDataSize, _EffectsSrcData, _EffectsSrcDataSize);
    _EffectsDataSize = _EffectsSrcDataSize;

    on_srsData_changed();
}

void MainWidget::on_saveSrs_clicked()
{
    QSettings settings("QtProject", "BBK_Effects_Editor");
    QFile srsFile(QFileDialog::getSaveFileName(this, tr("Save"), settings.value("last_srs_dir").toString() + '/' + settings.value("last_srs_file").toString(), "SRS(*.srs)"));
    if(!srsFile.open(QFile::WriteOnly)) {
        return;
    }

    QDataStream srsStream(&srsFile);
    srsStream.writeRawData((const char *)_EffectsData, _EffectsDataSize);

    srsFile.close();
}

void MainWidget::on_effectsView_playStopChanged(bool play)
{
    if(play) {
        ui->playTick->setText(tr("Stop"));
    } else {
        ui->playTick->setText(tr("Play"));
    }
}

void MainWidget::on_effectsView_currFrameChanged(uint8_t currFrame)
{
    ui->currFrame->setValue(currFrame);
}

void MainWidget::on_effectsView_currTickChanged(uint32_t currTick)
{
    ui->currTick->setValue(currTick);
}

void MainWidget::on_tickTimer_valueChanged(int ms)
{
    ui->effectsView->SetTickMs(ms);
}

void MainWidget::on_prevTick_clicked()
{
    ui->effectsView->PrevTick();
}

void MainWidget::on_playTick_clicked()
{
    ui->effectsView->PlayStop(!ui->effectsView->IsPlaying());
}

void MainWidget::on_nextTick_clicked()
{
    ui->effectsView->NextTick();
}

void MainWidget::on_framesList_itemSelectionChanged()
{
    disconnect(ui->argX,        QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->argY,        QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->argShow,     QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->argNShow,    QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->iconIdx,     QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->subX,        &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->addX,        &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->subY,        &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->addY,        &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->subShow,     &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->addShow,     &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->subNShow,    &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    disconnect(ui->addNShow,    &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);

    QList<QListWidgetItem *> items = ui->framesList->selectedItems();

    if(items.count() == 0) {

        ui->argX->setEnabled(false); ui->argY->setEnabled(false);
        ui->argShow->setEnabled(false); ui->argNShow->setEnabled(false);
        ui->iconIdx->setEnabled(false);

        ui->subX->setEnabled(false); ui->addX->setEnabled(false);
        ui->subY->setEnabled(false); ui->addY->setEnabled(false);
        ui->subShow->setEnabled(false); ui->addShow->setEnabled(false);
        ui->subNShow->setEnabled(false); ui->addNShow->setEnabled(false);
    }
    else if(items.count() == 1) {

        ui->effectsView->ShowCurrFrame(items.front()->text().toInt());
        QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsOffsetVec();
        uint16_t offset = offsetVec[items.front()->text().toInt()];
        effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + offset);
        ui->argX->setValue(effects_block->x);
        ui->argY->setValue(effects_block->y);
        ui->argShow->setValue(effects_block->show);
        ui->argNShow->setValue(effects_block->n_show);
        ui->iconIdx->setValue(effects_block->icon_idx);

        ui->argX->setEnabled(true); ui->argY->setEnabled(true);
        ui->argShow->setEnabled(true); ui->argNShow->setEnabled(true);
        ui->iconIdx->setEnabled(true);

        ui->subX->setEnabled(false); ui->addX->setEnabled(false);
        ui->subY->setEnabled(false); ui->addY->setEnabled(false);
        ui->subShow->setEnabled(false); ui->addShow->setEnabled(false);
        ui->subNShow->setEnabled(false); ui->addNShow->setEnabled(false);
    }
    else {

        ui->argX->setEnabled(false); ui->argY->setEnabled(false);
        ui->argShow->setEnabled(false); ui->argNShow->setEnabled(false);
        ui->iconIdx->setEnabled(false);

        ui->subX->setEnabled(true); ui->addX->setEnabled(true);
        ui->subY->setEnabled(true); ui->addY->setEnabled(true);
        ui->subShow->setEnabled(true); ui->addShow->setEnabled(true);
        ui->subNShow->setEnabled(true); ui->addNShow->setEnabled(true);
    }

    connect(ui->argX,       QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->argY,       QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->argShow,    QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->argNShow,   QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->iconIdx,    QOverload<int>::of(&QSpinBox::valueChanged),    this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->subX,       &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->addX,       &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->subY,       &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->addY,       &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->subShow,    &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->addShow,    &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->subNShow,   &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
    connect(ui->addNShow,   &QPushButton::clicked,                          this,   &MainWidget::on_frameArgsUI_changed);
}

void MainWidget::on_frameArgsUI_changed()
{
    QList<QListWidgetItem *> items = ui->framesList->selectedItems();

    QVector<QString> strVec;
    for(const QListWidgetItem *item : qAsConst(items)) {
        strVec.append(item->text());
    }

    QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsOffsetVec();

    if(items.count() == 1) {

        uint16_t offset = offsetVec[items.front()->text().toInt()];
        effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + offset);
        effects_block->x = ui->argX->value();
        effects_block->y = ui->argY->value();
        effects_block->show = ui->argShow->value();
        effects_block->n_show = ui->argNShow->value();
        effects_block->icon_idx = ui->iconIdx->value();

        on_srsData_changed();
    }
    else if(items.count() > 1) {

        for(const QListWidgetItem *item : qAsConst(items)) {

            uint16_t offset = offsetVec[item->text().toInt()];
            effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + offset);
            if(sender() == ui->subX) {
                if(effects_block->x > 0) {
                    effects_block->x--;
                }
            }
            else if(sender() == ui->addX) {
                if(effects_block->x < 255) {
                    effects_block->x++;
                }
            }
            else if(sender() == ui->subY) {
                if(effects_block->y > 0) {
                    effects_block->y--;
                }
            }
            else if(sender() == ui->addY) {
                if(effects_block->y < 255) {
                    effects_block->y++;
                }
            }
            else if(sender() == ui->subShow) {
                if(effects_block->show > 0) {
                    effects_block->show--;
                }
            }
            else if(sender() == ui->addShow) {
                if(effects_block->show < 255) {
                    effects_block->show++;
                }
            }
            else if(sender() == ui->subNShow) {
                if(effects_block->n_show > 0) {
                    effects_block->n_show--;
                }
            }
            else if(sender() == ui->addNShow) {
                if(effects_block->n_show < 255) {
                    effects_block->n_show++;
                }
            }
        }
    }

    for(int i = 0; i < ui->framesList->count(); i++) {
        if(strVec.contains(ui->framesList->item(i)->text())) {
            ui->framesList->item(i)->setSelected(true);
        }
    }
}

void MainWidget::on_framesList_customContextMenuRequested(const QPoint &pos)
{
    frameWidgetItem = ui->framesList->itemAt(pos);
    insFrame->setEnabled(frameWidgetItem != nullptr);
    delFrame->setEnabled(frameWidgetItem != nullptr);
    framesListMenu->exec(QCursor::pos());
}

void MainWidget::on_addFrame()
{
    if(_EffectsDataSize < 6) {
        return;
    }

    QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsOffsetVec();
    uint16_t offset = offsetVec.last() + sizeof(effects_block_t);

    memcpy_s(_PicData, _EffectsDataSize - offset, _EffectsData + offset, _EffectsDataSize - offset);
    effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + offset);
    effects_block->x = 0;
    effects_block->y = 0;
    effects_block->show = 1;
    effects_block->n_show = 1;
    effects_block->icon_idx = 0;

    memcpy_s(_EffectsData + offset + sizeof(effects_block_t), _EffectsDataSize - offset, _PicData, _EffectsDataSize - offset);
    _EffectsDataSize += sizeof(effects_block_t);

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    effects_head->frames++;
    effects_head->e_frame++;

    on_srsData_changed();
}

void MainWidget::on_insFrame()
{
    if(_EffectsDataSize < 6) {
        return;
    }

    if(frameWidgetItem == nullptr) {
        return;
    }

    QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsOffsetVec();
    uint16_t offset = offsetVec[frameWidgetItem->text().toInt()];

    memcpy_s(_PicData, _EffectsDataSize - offset, _EffectsData + offset, _EffectsDataSize - offset);
    effects_block_t *effects_block = reinterpret_cast<effects_block_t *>(_EffectsData + offset);
    effects_block->x = 0;
    effects_block->y = 0;
    effects_block->show = 2;
    effects_block->n_show = 1;
    effects_block->icon_idx = 0;

    memcpy_s(_EffectsData + offset + sizeof(effects_block_t), _EffectsDataSize - offset, _PicData, _EffectsDataSize - offset);
    _EffectsDataSize += sizeof(effects_block_t);

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    effects_head->frames++;
    effects_head->e_frame++;

    on_srsData_changed();
}

void MainWidget::on_delFrame()
{
    if(_EffectsDataSize < 6) {
        return;
    }

    if(frameWidgetItem == nullptr) {
        return;
    }

    QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsOffsetVec();
    uint16_t offset = offsetVec[frameWidgetItem->text().toInt()];

    memcpy_s(_PicData, _EffectsDataSize - offset - sizeof(effects_block_t), _EffectsData + offset + sizeof(effects_block_t), _EffectsDataSize - offset - sizeof(effects_block_t));

    memcpy_s(_EffectsData + offset, _EffectsDataSize - offset - sizeof(effects_block_t), _PicData, _EffectsDataSize - offset - sizeof(effects_block_t));
    _EffectsDataSize -= sizeof(effects_block_t);

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    effects_head->frames--;
    effects_head->e_frame--;

    on_srsData_changed();
}

void MainWidget::on_iconsList_customContextMenuRequested(const QPoint &pos)
{
    iconWidgetItem = ui->iconsList->itemAt(pos);
    edtIcon->setEnabled(iconWidgetItem != nullptr);
    insIcon->setEnabled(iconWidgetItem != nullptr);
    delIcon->setEnabled(iconWidgetItem != nullptr);
    iconsListMenu->exec(QCursor::pos());
}

void MainWidget::on_edtIcon()
{
    if(_EffectsDataSize < 6) {
        return;
    }

    QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsIconOffsetVec();
    uint16_t offset = offsetVec[iconWidgetItem->text().toInt()];
    effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(_EffectsData + offset);
    IconViewEditDlg iconViewEditDlg(this);
    iconViewEditDlg.UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
    if(iconViewEditDlg.exec() == IconViewEditDlg::Accepted) {
        iconViewEditDlg.WriteBack((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
    }
}

void MainWidget::on_addIcon()
{
    if(_EffectsDataSize < 6) {
        return;
    }

    QSettings settings("QtProject", "BBK_Effects_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_pic_dir").toString() + '/' + settings.value("last_pic_file").toString(), "PIC(*.pic)");
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        settings.setValue("last_pic_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.setValue("last_pic_file", QFileInfo(fileName).fileName());
        settings.sync();
        QFile picFile(fileName);
        if(!picFile.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray picBA = picFile.readAll();
        picFile.close();

        memcpy_s(_PicData, picBA.size(), picBA.data(), picBA.size());

        effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(_PicData);
        effects_icon_head->count = 1;
        uint16_t icon_size = calc_effects_icon_size(effects_icon_head);

        effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
        effects_icon_head->idx = effects_head->icon_max;

        memcpy_s(_EffectsData + _EffectsDataSize, EFFECTS_DATA_MAX_SIZE - _EffectsDataSize, _PicData, icon_size);
        _EffectsDataSize += icon_size;

        effects_head->icon_max++;

        on_srsData_changed();
    }
}

void MainWidget::on_insIcon()
{
    if(_EffectsDataSize < 6) {
        return;
    }

    if(iconWidgetItem == nullptr) {
        return;
    }

    QSettings settings("QtProject", "BBK_Effects_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_pic_dir").toString() + '/' + settings.value("last_pic_file").toString(), "PIC(*.pic)");
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        settings.setValue("last_pic_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.setValue("last_pic_file", QFileInfo(fileName).fileName());
        settings.sync();
        QFile picFile(fileName);
        if(!picFile.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray picBA = picFile.readAll();
        picFile.close();

        memcpy_s(_PicData, picBA.size(), picBA.data(), picBA.size());

        effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(_PicData);
        effects_icon_head->count = 1;
        uint16_t icon_size = calc_effects_icon_size(effects_icon_head);

        effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
        effects_icon_head->idx = iconWidgetItem->text().toInt();

        QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsIconOffsetVec();
        uint16_t offset = offsetVec[iconWidgetItem->text().toInt()];

        memcpy_s(_PicData + icon_size, EFFECTS_DATA_MAX_SIZE - icon_size, _EffectsData + offset, _EffectsDataSize - offset);
        memcpy_s(_EffectsData + offset, _EffectsDataSize + icon_size, _PicData, icon_size + _EffectsDataSize - offset);
        _EffectsDataSize += icon_size;

        effects_head->icon_max++;

        ui->effectsView->ReArrangeEffectsIconIndex();

        on_srsData_changed();
    }
}

void MainWidget::on_delIcon()
{
    if(_EffectsDataSize < 6) {
        return;
    }

    if(iconWidgetItem == nullptr) {
        return;
    }

    QVector<uint16_t> offsetVec = ui->effectsView->GetEffectsIconOffsetVec();
    uint16_t offset = offsetVec[iconWidgetItem->text().toInt()];

    effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(_EffectsData + offset);
    uint16_t icon_size = calc_effects_icon_size(effects_icon_head);

    memcpy_s(_PicData, EFFECTS_DATA_MAX_SIZE, _EffectsData + offset + icon_size, _EffectsDataSize - offset - icon_size);
    memcpy_s(_EffectsData + offset, _EffectsDataSize - icon_size, _PicData, _EffectsDataSize - offset - icon_size);
    _EffectsDataSize -= icon_size;

    effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(_EffectsData);
    effects_head->icon_max--;

    ui->effectsView->ReArrangeEffectsIconIndex();

    on_srsData_changed();
}
