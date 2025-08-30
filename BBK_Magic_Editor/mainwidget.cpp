#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "gam_type.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    adjustSize();
    resize(0, 0);

    buildDateTimeStr = QLocale(QLocale::English).toDateTime(QString(__DATE__ " " __TIME__).replace("  ", " 0"), "MMM dd yyyy hh:mm:ss").toString("yyyyMMdd_hhmm");

    setWindowTitle(tr("BBK_Magic_Editor_") + buildDateTimeStr);

    _MrsData = (uint8_t *)malloc(MRS_DATA_MAX_SIZE);
    if(_MrsData == nullptr) {
        exit(-1);
    }
}

MainWidget::~MainWidget()
{
    if(_MrsData != nullptr) {
        free(_MrsData);
    }

    delete ui;
}

void MainWidget::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e)
}

QSet<QString> MainWidget::getDirFiles(const QString &path)
{
    QDir dir(path);
    dir.setNameFilters(QStringList("*.mrs"));
    QStringList entries = dir.entryList();
    QStringList fileList;
    for(const QString &entry : qAsConst(entries)) {
        fileList.append(dir.absoluteFilePath(entry));
    }
    return QSet<QString>(fileList.begin(), fileList.end());
}

void MainWidget::on_magicLocalServer_newConnection()
{
    magicLocalSocket = magicLocalServer.nextPendingConnection();
    connect(magicLocalSocket, &QLocalSocket::readyRead, this, &MainWidget::on_magicLocalSocket_readyRead);
    connect(magicLocalSocket, &QLocalSocket::disconnected, this, &MainWidget::on_magicLocalSocket_disconnected);
}

void MainWidget::on_magicLocalSocket_readyRead()
{
    QDataStream dataStream(magicLocalSocket);
    dataStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    QString str;
    dataStream >> str;
    static QRegularExpression regExp("^\\d+,\\d+;$");
    if(regExp.match(str).hasMatch()) {

        QStringList strList = str.remove(';').split(',');
        uint8_t type = strList.front().toInt();
        uint8_t idx = strList.back().toInt();

        for(const QByteArray &mrsData : qAsConst(mrsDataVec)) {

            if(mrsData.data()[0] == type && mrsData.data()[1] == idx) {

                memset(_MrsData, 0, MRS_DATA_MAX_SIZE);
                memcpy_s(_MrsData, mrsData.size(), mrsData.data(), mrsData.size());
                _MrsDataSize = mrsData.size();

                magic_t *magic = reinterpret_cast<magic_t *>(_MrsData);

                ui->magicType->setCurrentIndex(magic->type - 1);
                ui->magicIdx->setValue(magic->idx);

                break;
            }
        }
    }
}

void MainWidget::on_magicLocalSocket_disconnected()
{
    magicLocalSocket->deleteLater();
}

void MainWidget::on_loadMrs_clicked()
{
    QSettings settings("QtProject", "BBK_Magic_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_mrs_dir").toString() + '/' + settings.value("last_mrs_file").toString(), "MRS(*.mrs)");
    if(!fileName.isEmpty() && QFile(fileName).exists()) {

        settings.setValue("last_mrs_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.setValue("last_mrs_file", QFileInfo(fileName).fileName());
        settings.sync();

        QFile mrsFile(fileName);
        if(!mrsFile.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray mrsBA = mrsFile.readAll();
        mrsFile.close();

        memset(_MrsData, 0, MRS_DATA_MAX_SIZE);
        memcpy_s(_MrsData, mrsBA.size(), mrsBA.data(), mrsBA.size());
        _MrsDataSize = mrsBA.size();

        magic_t *magic = reinterpret_cast<magic_t *>(_MrsData);

        ui->magicType->setCurrentIndex(magic->type - 1);
        ui->magicIdx->setValue(magic->idx);
    }
}

void MainWidget::on_createMrs_clicked()
{
    if(_MrsDataSize > 0) {
        return;
    }

    magic_t *magic = reinterpret_cast<magic_t *>(_MrsData);

    magic->type = 1;
    magic->idx = 1;

    _MrsDataSize = sizeof(magic_t);

    ui->magicType->setCurrentIndex(magic->type - 1);
    ui->magicIdx->setValue(magic->idx);
}

void MainWidget::on_monitorDir_clicked()
{
    if(mrsPath.size() > 0) {
        return;
    }

    QSettings settings("QtProject", "BBK_Magic_Editor");
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open"), settings.value("last_mrs_dir").toString());

    if(dirName.isEmpty()) {
        return;
    }

    if(!QDir(dirName).exists()) {
        return;
    }

    settings.setValue("last_mrs_dir", QDir(dirName).absolutePath());
    settings.sync();
    mrsPath = dirName;

    mrsDataVec.clear();

    QSet<QString> mrsFileNames = getDirFiles(dirName);
    for(const QString &mrsFileName : qAsConst(mrsFileNames)) {

        QFile mrsFile(mrsFileName);
        if(!mrsFile.open(QFile::ReadOnly)) {
            continue;
        }

        mrsDataVec.append(mrsFile.readAll());

        mrsFile.close();
    }

    if(magicLocalServer.listen("BBK_Magic_Editor")) {

        connect(&magicLocalServer, &QLocalServer::newConnection, this, &MainWidget::on_magicLocalServer_newConnection);

        ui->monitorDir->setEnabled(false);
    }
}

void MainWidget::on_saveMrs_clicked()
{
    QSettings settings("QtProject", "BBK_Magic_Editor");
    QFile mrsFile(QFileDialog::getSaveFileName(this, tr("Save"), settings.value("last_mrs_dir").toString() + '/' + settings.value("last_mrs_file").toString(), "MRS(*.mrs)"));
    if(!mrsFile.open(QFile::WriteOnly)) {
        return;
    }

    QDataStream mrsStream(&mrsFile);
    mrsStream.writeRawData((const char *)_MrsData, _MrsDataSize);

    mrsFile.close();
}

void MainWidget::on_magicType_currentIndexChanged(int index)
{
    if(_MrsDataSize < sizeof(magic_t)) {
        return;
    }

    disconnect(ui->magicName,           &QLineEdit::editingFinished,                    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicHp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicMp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicAttackPower,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicDefensePower,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicBodyMovement,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicMpCost,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicEffects,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicPoisonChk,      &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicMessChk,        &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicSealChk,        &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicSleepChk,       &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicGroupEffectChk, &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicEffectiveRound, QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicDescription,    &QTextEdit::textChanged,                        this, &MainWidget::on_magicData_valueChanged);

    magic_t *magic = reinterpret_cast<magic_t *>(_MrsData);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    ui->magicName->setText(gbkCodec->toUnicode((char *)magic->name));

    switch(index)
    {
    case 0:
        ui->magicHp->setEnabled(true);
        ui->magicMp->setEnabled(true);
        ui->magicAttackPower->setEnabled(true);
        ui->magicDefensePower->setEnabled(true);
        ui->magicBodyMovement->setEnabled(true);
        ui->magicPoisonChk->setEnabled(true);
        ui->magicMessChk->setEnabled(true);
        ui->magicSealChk->setEnabled(true);
        ui->magicSleepChk->setEnabled(true);
        ui->magicGroupEffectChk->setEnabled(true);
        ui->magicEffectiveRound->setEnabled(true);

        ui->magicHp->setValue(magic->hp);
        ui->magicMp->setValue(magic->mp);
        ui->magicAttackPower->setValue(magic->attack_power);
        ui->magicDefensePower->setValue(magic->defense_power);
        ui->magicBodyMovement->setValue(magic->body_movement);
        ui->magicPoisonChk->setChecked(magic->effective & 0x08);
        ui->magicMessChk->setChecked(magic->effective & 0x04);
        ui->magicSealChk->setChecked(magic->effective & 0x02);
        ui->magicSleepChk->setChecked(magic->effective & 0x01);
        ui->magicGroupEffectChk->setChecked(magic->g_effective_round & 0x80);
        ui->magicEffectiveRound->setValue(magic->g_effective_round & 0x7f);

        break;

    case 1:
        ui->magicHp->setEnabled(false);
        ui->magicMp->setEnabled(false);
        ui->magicAttackPower->setEnabled(true);
        ui->magicDefensePower->setEnabled(true);
        ui->magicBodyMovement->setEnabled(true);
        ui->magicPoisonChk->setEnabled(false);
        ui->magicMessChk->setEnabled(false);
        ui->magicSealChk->setEnabled(false);
        ui->magicSleepChk->setEnabled(false);
        ui->magicGroupEffectChk->setEnabled(true);
        ui->magicEffectiveRound->setEnabled(true);

        ui->magicHp->setValue(0);
        ui->magicMp->setValue(0);
        ui->magicAttackPower->setValue(magic->attack_power);
        ui->magicDefensePower->setValue(magic->defense_power);
        ui->magicBodyMovement->setValue(magic->body_movement);
        ui->magicPoisonChk->setChecked(false);
        ui->magicMessChk->setChecked(false);
        ui->magicSealChk->setChecked(false);
        ui->magicSleepChk->setChecked(false);
        ui->magicGroupEffectChk->setChecked(magic->g_effective_round & 0x80);
        ui->magicEffectiveRound->setValue(magic->g_effective_round & 0x7f);

        break;

    case 2:
        ui->magicHp->setEnabled(true);
        ui->magicMp->setEnabled(false);
        ui->magicAttackPower->setEnabled(false);
        ui->magicDefensePower->setEnabled(false);
        ui->magicBodyMovement->setEnabled(false);
        ui->magicPoisonChk->setEnabled(true);
        ui->magicMessChk->setEnabled(true);
        ui->magicSealChk->setEnabled(true);
        ui->magicSleepChk->setEnabled(true);
        ui->magicGroupEffectChk->setEnabled(true);
        ui->magicEffectiveRound->setEnabled(false);

        ui->magicHp->setValue(magic->hp);
        ui->magicMp->setValue(0);
        ui->magicAttackPower->setValue(0);
        ui->magicDefensePower->setValue(0);
        ui->magicBodyMovement->setValue(0);
        ui->magicPoisonChk->setChecked(magic->effective & 0x08);
        ui->magicMessChk->setChecked(magic->effective & 0x04);
        ui->magicSealChk->setChecked(magic->effective & 0x02);
        ui->magicSleepChk->setChecked(magic->effective & 0x01);
        ui->magicGroupEffectChk->setChecked(magic->g_effective_round & 0x80);
        ui->magicEffectiveRound->setValue(0);

        break;

    case 3:
        ui->magicHp->setEnabled(true);
        ui->magicMp->setEnabled(false);
        ui->magicAttackPower->setEnabled(false);
        ui->magicDefensePower->setEnabled(false);
        ui->magicBodyMovement->setEnabled(false);
        ui->magicPoisonChk->setEnabled(false);
        ui->magicMessChk->setEnabled(false);
        ui->magicSealChk->setEnabled(false);
        ui->magicSleepChk->setEnabled(false);
        ui->magicGroupEffectChk->setEnabled(false);
        ui->magicEffectiveRound->setEnabled(false);

        ui->magicHp->setValue(magic->hp);
        ui->magicMp->setValue(0);
        ui->magicAttackPower->setValue(0);
        ui->magicDefensePower->setValue(0);
        ui->magicBodyMovement->setValue(0);
        ui->magicPoisonChk->setChecked(false);
        ui->magicMessChk->setChecked(false);
        ui->magicSealChk->setChecked(false);
        ui->magicSleepChk->setChecked(false);
        ui->magicGroupEffectChk->setChecked(false);
        ui->magicEffectiveRound->setValue(0);

        break;

    case 4:
        ui->magicHp->setEnabled(false);
        ui->magicMp->setEnabled(false);
        ui->magicAttackPower->setEnabled(false);
        ui->magicDefensePower->setEnabled(false);
        ui->magicBodyMovement->setEnabled(false);
        ui->magicPoisonChk->setEnabled(false);
        ui->magicMessChk->setEnabled(false);
        ui->magicSealChk->setEnabled(false);
        ui->magicSleepChk->setEnabled(false);
        ui->magicGroupEffectChk->setEnabled(false);
        ui->magicEffectiveRound->setEnabled(false);

        ui->magicHp->setValue(0);
        ui->magicMp->setValue(0);
        ui->magicAttackPower->setValue(0);
        ui->magicDefensePower->setValue(0);
        ui->magicBodyMovement->setValue(0);
        ui->magicPoisonChk->setChecked(false);
        ui->magicMessChk->setChecked(false);
        ui->magicSealChk->setChecked(false);
        ui->magicSleepChk->setChecked(false);
        ui->magicGroupEffectChk->setChecked(false);
        ui->magicEffectiveRound->setValue(0);

        break;

    default:

        break;
    }

    ui->magicMpCost->setValue(magic->mp_cost);
    ui->magicEffects->setValue(magic->icon);
    ui->magicDescription->setText(gbkCodec->toUnicode((char *)magic->description));

    connect(ui->magicName,           &QLineEdit::editingFinished,                   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicHp,             QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicMp,             QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicAttackPower,    QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicDefensePower,   QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicBodyMovement,   QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicMpCost,         QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicEffects,        QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicPoisonChk,      &QCheckBox::clicked,                           this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicMessChk,        &QCheckBox::clicked,                           this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicSealChk,        &QCheckBox::clicked,                           this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicSleepChk,       &QCheckBox::clicked,                           this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicGroupEffectChk, &QCheckBox::clicked,                           this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicEffectiveRound, QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicDescription,    &QTextEdit::textChanged,                       this, &MainWidget::on_magicData_valueChanged);
}

void MainWidget::on_magicIdx_valueChanged(int index)
{
    Q_UNUSED(index)
    on_magicType_currentIndexChanged(ui->magicType->currentIndex());
}

void MainWidget::on_magicData_valueChanged()
{
    if(_MrsDataSize < sizeof(magic_t)) {
        return;
    }

    magic_t *magic = reinterpret_cast<magic_t *>(_MrsData);

    magic->type = ui->magicType->currentIndex() + 1;
    magic->idx = ui->magicIdx->value();

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    QByteArray magicName = gbkCodec->fromUnicode(ui->magicName->text());
    memset(magic->name, 0xff, 12);
    memcpy_s(magic->name, 12, magicName.data(), magicName.size());
    magic->name[magicName.size()] = '\0';

    switch(ui->magicType->currentIndex())
    {
    case 0:
        magic->hp = ui->magicHp->value();
        magic->mp = ui->magicMp->value();

        magic->attack_power = ui->magicAttackPower->value();
        magic->defense_power = ui->magicDefensePower->value();
        magic->body_movement = ui->magicBodyMovement->value();

        if(ui->magicPoisonChk->isChecked()) {
            magic->effective |= 0x08;
        } else {
            magic->effective &= ~0x08;
        }

        if(ui->magicMessChk->isChecked()) {
            magic->effective |= 0x04;
        } else {
            magic->effective &= ~0x04;
        }

        if(ui->magicSealChk->isChecked()) {
            magic->effective |= 0x02;
        } else {
            magic->effective &= ~0x02;
        }

        if(ui->magicSleepChk->isChecked()) {
            magic->effective |= 0x01;
        } else {
            magic->effective &= ~0x01;
        }

        if(ui->magicGroupEffectChk->isChecked()) {
            magic->g_effective_round |= 0x80;
        } else {
            magic->g_effective_round &= ~0x80;
        }

        break;

    case 1:
        magic->attack_power = ui->magicAttackPower->value();
        magic->defense_power = ui->magicDefensePower->value();
        magic->body_movement = ui->magicBodyMovement->value();

        if(ui->magicGroupEffectChk->isChecked()) {
            magic->g_effective_round |= 0x80;
        } else {
            magic->g_effective_round &= ~0x80;
        }

        break;

    case 2:
        magic->hp = ui->magicHp->value();

        if(ui->magicPoisonChk->isChecked()) {
            magic->effective |= 0x08;
        } else {
            magic->effective &= ~0x08;
        }

        if(ui->magicMessChk->isChecked()) {
            magic->effective |= 0x04;
        } else {
            magic->effective &= ~0x04;
        }

        if(ui->magicSealChk->isChecked()) {
            magic->effective |= 0x02;
        } else {
            magic->effective &= ~0x02;
        }

        if(ui->magicSleepChk->isChecked()) {
            magic->effective |= 0x01;
        } else {
            magic->effective &= ~0x01;
        }

        if(ui->magicGroupEffectChk->isChecked()) {
            magic->g_effective_round |= 0x80;
        } else {
            magic->g_effective_round &= ~0x80;
        }

        break;

    case 3:
        magic->hp = ui->magicHp->value();

        break;

    case 4:

        break;

    default:
        break;
    }

    magic->mp_cost = ui->magicMpCost->value();
    magic->icon = ui->magicEffects->value();

    QByteArray magicDescription = gbkCodec->fromUnicode(ui->magicDescription->toPlainText());
    memset(magic->description, 0xff, 102);
    memcpy_s(magic->description, 102, magicDescription.data(), magicDescription.size());
    magic->description[magicDescription.size()] = '\0';
}

void MainWidget::on_magicDescription_textChanged()
{
    int len8Bit = ui->magicDescription->toPlainText().toLocal8Bit().length();
    if(len8Bit > 101) {
        ui->magicDescription->textCursor().movePosition(QTextCursor::End);
        if(ui->magicDescription->textCursor().hasSelection()) {
            ui->magicDescription->textCursor().clearSelection();
        }
        do {
            ui->magicDescription->textCursor().deletePreviousChar();
            len8Bit = ui->magicDescription->toPlainText().toLocal8Bit().length();
        } while(len8Bit > 101);
    }
}
