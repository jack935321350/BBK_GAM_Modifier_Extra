#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "gam_type.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    _buildDateTimeStr = QLocale(QLocale::English).toDateTime(QString(__DATE__ " " __TIME__).replace("  ", " 0"), "MMM dd yyyy hh:mm:ss").toString("yyyyMMdd_hhmm");

    setWindowTitle(tr("BBK_Chain_Editor_") + _buildDateTimeStr);

    QSettings settings("QtProject", "BBK_Chain_Editor");

    _MlrData = (uint8_t *)malloc(MLR_DATA_MAX_SIZE);
    _TmpData = (uint8_t *)malloc(MLR_DATA_MAX_SIZE);
    if(_MlrData == nullptr || _TmpData == nullptr) {
        if(_MlrData != nullptr) {
            free(_MlrData);
        }
        if(_TmpData != nullptr) {
            free(_TmpData);
        }
        exit(-1);
    }

    itemsListMenu = new QMenu(this);

    addItem = new QAction(tr("Add Item"), this);
    itemsListMenu->addAction(addItem);
    connect(addItem, &QAction::triggered, this, &MainWidget::on_addItem);

    insItem = new QAction(tr("Insert Item"), this);
    itemsListMenu->addAction(insItem);
    connect(insItem, &QAction::triggered, this, &MainWidget::on_insItem);

    delItem = new QAction(tr("Delete Frame"), this);
    itemsListMenu->addAction(delItem);
    connect(delItem, &QAction::triggered, this, &MainWidget::on_delItem);

    ui->magicCfgGbx->setEnabled(false);
    ui->levelCfgGbx->setEnabled(false);
}

MainWidget::~MainWidget()
{
    if(_MlrData != nullptr) {
        free(_MlrData);
    }
    if(_TmpData != nullptr) {
        free(_TmpData);
    }

    mrsLocalSocket.disconnectFromServer();

    delete ui;
}

void MainWidget::on_loadMlr_clicked()
{
    QSettings settings("QtProject", "BBK_Chain_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_mlr_dir").toString() + '/' + settings.value("last_mlr_file").toString(), "MLR(*.mlr)");
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        settings.setValue("last_mlr_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.setValue("last_mlr_file", QFileInfo(fileName).fileName());
        settings.sync();

        QFile mlrFile(fileName);
        if(!mlrFile.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray mlrBA = mlrFile.readAll();
        mlrFile.close();

        memset(_MlrData, 0, MLR_DATA_MAX_SIZE);
        memcpy_s(_MlrData, mlrBA.size(), mlrBA.data(), mlrBA.size());
        _MlrDataSize = mlrBA.size();

        on_mlrData_changed();
    }
}

void MainWidget::on_createMlr_clicked()
{
    if(_MlrDataSize > 0) {
        return;
    }

    memset(_MlrData, 0, MLR_DATA_MAX_SIZE);
    _MlrData[0] = 1;
    _MlrData[1] = 1;
    _MlrData[2] = 1;
    _MlrData[3] = 1;
    _MlrData[4] = 1;

    _MlrDataSize = 5;

    on_mlrData_changed();
}

void MainWidget::on_saveMlr_clicked()
{
    QSettings settings("QtProject", "BBK_Chain_Editor");
    QFile mlrFile(QFileDialog::getSaveFileName(this, tr("Save"), settings.value("last_mlr_dir").toString() + '/' + settings.value("last_mlr_file").toString(), "MLR(*.mlr)"));
    if(!mlrFile.open(QFile::WriteOnly)) {
        return;
    }

    QDataStream mlrStream(&mlrFile);
    mlrStream.writeRawData((const char *)_MlrData, _MlrDataSize);

    mlrFile.close();
}

void MainWidget::on_mlrData_changed()
{
    disconnect(ui->chainType,       QOverload<int>::of(&QComboBox::currentIndexChanged),    this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->chainIdx,        QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);

    magic_list_head_t *magic_list_head = reinterpret_cast<magic_list_head_t *>(_MlrData);
    ui->chainType->setCurrentIndex(magic_list_head->type - 1);
    ui->chainIdx->setValue(magic_list_head->idx);
    ui->chainCount->setValue(magic_list_head->count);

    ui->itemsList->clear();
    for(int i = 0; i < magic_list_head->count; i++) {
        ui->itemsList->addItem(QString("%1").arg(i + 1, 3, 10, QLatin1Char('0')));
    }

    connect(ui->chainType,      QOverload<int>::of(&QComboBox::currentIndexChanged),    this,       &MainWidget::on_mlrUI_changed);
    connect(ui->chainIdx,       QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
}

void MainWidget::on_mlrUI_changed()
{
    magic_list_head_t *magic_list_head = reinterpret_cast<magic_list_head_t *>(_MlrData);
    magic_list_head->type = ui->chainType->currentIndex() + 1;
    magic_list_head->idx = ui->chainIdx->value();
    magic_list_head->count = ui->chainCount->value();

    QList<QListWidgetItem *> items = ui->itemsList->selectedItems();

    if(items.isEmpty()) {
        return;
    }

    if(ui->chainType->currentIndex() == 0) {

        size_t offset = sizeof(magic_list_head_t) + sizeof(magic_list_block_t) * (items.front()->text().toInt() - 1);
        magic_list_block_t *magic_list_block = reinterpret_cast<magic_list_block_t *>(_MlrData + offset);

        magic_list_block->type = ui->magicType->currentIndex() + 1;
        magic_list_block->idx = ui->magicIndex->value();

        if(sender() == ui->magicType || sender() == ui->magicIndex) {
            if(mrsLocalSocket.state() != QLocalSocket::ConnectedState) {
                mrsLocalSocket.connectToServer("BBK_Magic_Editor");
                mrsLocalSocket.waitForConnected();
            }
            if(mrsLocalSocket.state() == QLocalSocket::ConnectedState) {
                QDataStream dataStream(&mrsLocalSocket);
                dataStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                dataStream << QString("%1,%2;").arg(ui->magicType->currentIndex() + 1).arg(ui->magicIndex->value());
                mrsLocalSocket.waitForBytesWritten();
            }
        }
    }
    else if(ui->chainType->currentIndex() == 1) {

        size_t offset = sizeof(level_list_head_t) + sizeof(level_list_block_t) * (items.front()->text().toInt() - 1);
        level_list_block_t *level_list_block = reinterpret_cast<level_list_block_t *>(_MlrData + offset);

        level_list_block->hp = ui->hpMax->value();
        level_list_block->hp_max = ui->hpMax->value();
        level_list_block->mp = ui->mpMax->value();
        level_list_block->mp_max = ui->mpMax->value();
        level_list_block->attack_power = ui->attackPower->value();
        level_list_block->defense_power = ui->defensePower->value();
        level_list_block->exp = 1;
        level_list_block->exp_level_up = ui->expLevelUp->value();
        level_list_block->body_movement = ui->bodyMovement->value();
        level_list_block->spirit_power = ui->spiritPower->value();
        level_list_block->lucky_value = ui->luckyValue->value();
        level_list_block->learned = ui->learned->value();
    }

    ui->magicCfgGbx->setEnabled(ui->chainType->currentIndex() == 0);
    ui->levelCfgGbx->setEnabled(ui->chainType->currentIndex() == 1);
}

void MainWidget::on_itemsList_itemSelectionChanged()
{
    disconnect(ui->magicType,       QOverload<int>::of(&QComboBox::currentIndexChanged),    this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->magicIndex,      QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);

    disconnect(ui->hpMax,           QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->mpMax,           QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->attackPower,     QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->defensePower,    QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->expLevelUp,      QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->bodyMovement,    QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->spiritPower,     QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->luckyValue,      QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    disconnect(ui->learned,         QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);

    QList<QListWidgetItem *> items = ui->itemsList->selectedItems();

    if(items.count() == 0) {

        ui->magicCfgGbx->setEnabled(false);
        ui->levelCfgGbx->setEnabled(false);
    }
    else if(items.count() == 1) {

        ui->magicCfgGbx->setEnabled(ui->chainType->currentIndex() == 0);
        ui->levelCfgGbx->setEnabled(ui->chainType->currentIndex() == 1);

        if(ui->chainType->currentIndex() == 0) {

            size_t offset = sizeof(magic_list_head_t) + sizeof(magic_list_block_t) * (items.front()->text().toInt() - 1);
            magic_list_block_t *magic_list_block = reinterpret_cast<magic_list_block_t *>(_MlrData + offset);

            ui->magicType->setCurrentIndex(magic_list_block->type - 1);
            ui->magicIndex->setValue(magic_list_block->idx);

            if(mrsLocalSocket.state() != QLocalSocket::ConnectedState) {
                mrsLocalSocket.connectToServer("BBK_Magic_Editor");
                mrsLocalSocket.waitForConnected();
            }
            if(mrsLocalSocket.state() == QLocalSocket::ConnectedState) {
                QDataStream dataStream(&mrsLocalSocket);
                dataStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                dataStream << QString("%1,%2;").arg(ui->magicType->currentIndex() + 1).arg(ui->magicIndex->value());
                mrsLocalSocket.waitForBytesWritten();
            }
        }
        else if(ui->chainType->currentIndex() == 1) {

            size_t offset = sizeof(level_list_head_t) + sizeof(level_list_block_t) * (items.front()->text().toInt() - 1);
            level_list_block_t *level_list_block = reinterpret_cast<level_list_block_t *>(_MlrData + offset);

            ui->hpMax->setValue(level_list_block->hp_max);
            ui->mpMax->setValue(level_list_block->mp_max);
            ui->attackPower->setValue(level_list_block->attack_power);
            ui->defensePower->setValue(level_list_block->defense_power);
            ui->expLevelUp->setValue(level_list_block->exp_level_up);
            ui->bodyMovement->setValue(level_list_block->body_movement);
            ui->spiritPower->setValue(level_list_block->spirit_power);
            ui->luckyValue->setValue(level_list_block->lucky_value);
            ui->learned->setValue(level_list_block->learned);
        }
    }

    if(ui->chainType->currentIndex() == 0) {

        connect(ui->magicType,      QOverload<int>::of(&QComboBox::currentIndexChanged),    this,       &MainWidget::on_mlrUI_changed);
        connect(ui->magicIndex,     QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    }
    else if(ui->chainType->currentIndex() == 1) {

        connect(ui->hpMax,          QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->mpMax,          QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->attackPower,    QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->defensePower,   QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->expLevelUp,     QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->bodyMovement,   QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->spiritPower,    QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->luckyValue,     QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
        connect(ui->learned,        QOverload<int>::of(&QSpinBox::valueChanged),            this,       &MainWidget::on_mlrUI_changed);
    }
}

void MainWidget::on_itemsList_customContextMenuRequested(const QPoint &pos)
{
    widgetItem = ui->itemsList->itemAt(pos);
    insItem->setEnabled(widgetItem != nullptr);
    delItem->setEnabled(widgetItem != nullptr);
    itemsListMenu->exec(QCursor::pos());
}

void MainWidget::on_addItem()
{
    if(_MlrDataSize < 3) {
        return;
    }

    if(ui->chainType->currentIndex() == 0) {

        magic_list_head_t *magic_list_head = reinterpret_cast<magic_list_head_t *>(_MlrData);

        uint16_t offset = sizeof(magic_list_head_t) + sizeof(magic_list_block_t) * magic_list_head->count;

        memcpy_s(_TmpData, _MlrDataSize - offset, _MlrData + offset, _MlrDataSize - offset);

        magic_list_block_t *magic_list_block = reinterpret_cast<magic_list_block_t *>(_MlrData + offset);
        magic_list_block->type = 1;
        magic_list_block->idx = 1;

        _MlrDataSize += sizeof(magic_list_block_t);

        magic_list_head->count++;
    }
    else if(ui->chainType->currentIndex() == 1) {

        level_list_head_t *level_list_head = reinterpret_cast<level_list_head_t *>(_MlrData);

        uint16_t offset = sizeof(level_list_head_t) + sizeof(level_list_block_t) * level_list_head->level_max;

        if(level_list_head->level_max > 1) {

            memcpy_s(_MlrData + offset, sizeof(level_list_block_t), _MlrData + offset - sizeof(level_list_block_t), sizeof(level_list_block_t));
        }

        _MlrDataSize += sizeof(level_list_block_t);

        level_list_head->level_max++;
    }

    on_mlrData_changed();
}

void MainWidget::on_insItem()
{
    if(_MlrDataSize < 3) {
        return;
    }

    if(widgetItem == nullptr) {
        return;
    }

    if(ui->chainType->currentIndex() == 0) {

        size_t offset = sizeof(magic_list_head_t) + sizeof(magic_list_block_t) * (widgetItem->text().toInt() - 1);

        memcpy_s(_TmpData, _MlrDataSize - offset, _MlrData + offset, _MlrDataSize - offset);
        magic_list_block_t *magic_list_block = reinterpret_cast<magic_list_block_t *>(_MlrData + offset);
        magic_list_block->type = 1;
        magic_list_block->idx = 1;

        memcpy_s(_MlrData + offset + sizeof(magic_list_block_t), _MlrDataSize - offset, _TmpData, _MlrDataSize - offset);

        _MlrDataSize += sizeof(magic_list_block_t);

        magic_list_head_t *magic_list_head = reinterpret_cast<magic_list_head_t *>(_MlrData);
        magic_list_head->count++;

    }
    else if(ui->chainType->currentIndex() == 1) {

        size_t offset = sizeof(level_list_head_t) + sizeof(level_list_block_t) * (widgetItem->text().toInt() - 1);

        memcpy_s(_TmpData, _MlrDataSize - offset, _MlrData + offset, _MlrDataSize - offset);

        if(widgetItem->text().toInt() > 1) {

            memcpy_s(_MlrData + offset, sizeof(level_list_block_t), _MlrData + offset - sizeof(level_list_block_t), sizeof(level_list_block_t));
        }

        memcpy_s(_MlrData + offset + sizeof(level_list_block_t), _MlrDataSize - offset, _TmpData, _MlrDataSize - offset);

        _MlrDataSize += sizeof(level_list_block_t);

        level_list_head_t *level_list_head = reinterpret_cast<level_list_head_t *>(_MlrData);
        level_list_head->level_max++;
    }

    on_mlrData_changed();
}

void MainWidget::on_delItem()
{
    if(_MlrDataSize < 3) {
        return;
    }

    if(widgetItem == nullptr) {
        return;
    }

    if(ui->chainType->currentIndex() == 0) {

        size_t offset = sizeof(magic_list_head_t) + sizeof(magic_list_block_t) * (widgetItem->text().toInt() - 1);

        memcpy_s(_TmpData, _MlrDataSize - offset - sizeof(magic_list_block_t), _MlrData + offset + sizeof(magic_list_block_t), _MlrDataSize - offset - sizeof(magic_list_block_t));

        memcpy_s(_MlrData + offset, _MlrDataSize - offset - sizeof(magic_list_block_t), _TmpData, _MlrDataSize - offset - sizeof(magic_list_block_t));

        _MlrDataSize -= sizeof(magic_list_block_t);

        magic_list_head_t *magic_list_head = reinterpret_cast<magic_list_head_t *>(_MlrData);
        magic_list_head->count--;

    }
    else if(ui->chainType->currentIndex() == 1) {

        size_t offset = sizeof(level_list_head_t) + sizeof(level_list_block_t) * (widgetItem->text().toInt() - 1);

        memcpy_s(_TmpData, _MlrDataSize - offset - sizeof(level_list_block_t), _MlrData + offset + sizeof(level_list_block_t), _MlrDataSize - offset - sizeof(level_list_block_t));

        memcpy_s(_MlrData + offset, _MlrDataSize - offset - sizeof(level_list_block_t), _TmpData, _MlrDataSize - offset - sizeof(level_list_block_t));

        _MlrDataSize -= sizeof(level_list_block_t);

        level_list_head_t *level_list_head = reinterpret_cast<level_list_head_t *>(_MlrData);
        level_list_head->level_max--;
    }

    on_mlrData_changed();
}
