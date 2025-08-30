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

    setWindowTitle(tr("BBK_Prop_Editor_") + buildDateTimeStr);

    _GrsData = (uint8_t *)malloc(GRS_DATA_MAX_SIZE);
    if(_GrsData == nullptr) {
        exit(-1);
    }
}

MainWidget::~MainWidget()
{
    if(_GrsData != nullptr) {
        free(_GrsData);
    }

    delete ui;
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

void MainWidget::on_loadGrs_clicked()
{
    QSettings settings("QtProject", "BBK_Prop_Editor");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("last_grs_dir").toString() + '/' + settings.value("last_grs_file").toString(), "GRS(*.grs)");
    if(!fileName.isEmpty() && QFile(fileName).exists()) {

        settings.setValue("last_grs_dir", QFileInfo(fileName).absoluteDir().absolutePath());
        settings.setValue("last_grs_file", QFileInfo(fileName).fileName());
        settings.sync();

        QFile grsFile(fileName);
        if(!grsFile.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray grsBA = grsFile.readAll();
        grsFile.close();

        memset(_GrsData, 0, GRS_DATA_MAX_SIZE);
        memcpy_s(_GrsData, grsBA.size(), grsBA.data(), grsBA.size());
        _GrsDataSize = grsBA.size();

        prop_t *prop = reinterpret_cast<prop_t *>(_GrsData);

        ui->propType->setCurrentIndex(prop->type - 1);
        ui->propIdx->setValue(prop->idx);
    }
}

void MainWidget::on_createGrs_clicked()
{
    if(_GrsDataSize > 0) {
        return;
    }

    prop_t *prop = reinterpret_cast<prop_t *>(_GrsData);

    prop->type = 1;
    prop->idx = 1;

    _GrsDataSize = sizeof(prop_t);

    ui->propType->setCurrentIndex(prop->type - 1);
    ui->propIdx->setValue(prop->idx);
}

void MainWidget::on_saveGrs_clicked()
{
    QSettings settings("QtProject", "BBK_Prop_Editor");
    QFile grsFile(QFileDialog::getSaveFileName(this, tr("Save"), settings.value("last_grs_dir").toString() + '/' + settings.value("last_grs_file").toString(), "GRS(*.grs)"));
    if(!grsFile.open(QFile::WriteOnly)) {
        return;
    }

    QDataStream grsStream(&grsFile);
    grsStream.writeRawData((const char *)_GrsData, _GrsDataSize);

    grsFile.close();
}

void MainWidget::on_propType_currentIndexChanged(int index)
{
    if(_GrsDataSize < sizeof(prop_t)) {
        return;
    }

    disconnect(ui->propName,            &QLineEdit::editingFinished,                    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propIcon,            QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propEvent,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propPriceBuy,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propPriceSale,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propHp,              QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propMp,              QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propHpMax,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propMpMax,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propAttackPower,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propDefensePower,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propBodyMovement,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propSpiritPower,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propLuckyValue,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->player1Chk,          &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->player2Chk,          &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->player3Chk,          &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->player4Chk,          &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->multiplayerMagic,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->poisonChk,           &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->messChk,             &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->sealChk,             &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->sleepChk,            &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->groupEffectChk,      &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->effectiveRound,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propDescription,     &QTextEdit::textChanged,                        this, &MainWidget::on_propData_valueChanged);

    prop_t *prop = reinterpret_cast<prop_t *>(_GrsData);

    // qDebug() << getResOffset(Res_GRS, index + 1, ui->propIdx->value()) << propIndexVecVec[index][ui->propIdx->value() - 1];

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    ui->propName->setText(gbkCodec->toUnicode((char *)prop->name));

    ui->propIcon->setValue(prop->icon);

    ui->propEvent->setValue(prop->event);

    ui->propPriceBuy->setValue(prop->price_buy);
    ui->propPriceSale->setValue(prop->price_sale);

    if(index < 5) {
        ui->propHp->setEnabled(false);
        ui->propMp->setEnabled(false);

        ui->propHpMax->setEnabled(true);
        ui->propMpMax->setEnabled(true);

        ui->propAttackPower->setEnabled(true);
        ui->propDefensePower->setEnabled(true);

        ui->propBodyMovement->setEnabled(true);
        ui->propSpiritPower->setEnabled(true);
        ui->propLuckyValue->setEnabled(true);

        ui->player1Chk->setEnabled(true);
        ui->player2Chk->setEnabled(true);
        ui->player3Chk->setEnabled(true);
        ui->player4Chk->setEnabled(true);

        ui->multiplayerMagic->setEnabled(false);

        ui->poisonChk->setEnabled(true);
        ui->messChk->setEnabled(true);
        ui->sealChk->setEnabled(true);
        ui->sleepChk->setEnabled(true);

        ui->groupEffectChk->setEnabled(false);

        ui->effectiveRound->setEnabled(false);



        ui->propHp->setValue(0);
        ui->propMp->setValue(0);

        ui->propHpMax->setValue(fromNumber(prop->mp_hp_hp16.mp_hp.hpMax_hp.hpMax));
        ui->propMpMax->setValue(fromNumber(prop->mp_hp_hp16.mp_hp.mpMax_mp.mpMax));

        ui->propAttackPower->setValue(fromNumber(prop->df_at_mp16.df_at.attack_power));
        ui->propDefensePower->setValue(fromNumber(prop->df_at_mp16.df_at.defense_power));

        ui->propBodyMovement->setValue(fromNumber(prop->body_movement_animation.body_movement));
        ui->propSpiritPower->setValue(fromNumber(prop->spirit_power_index.spirit_power));
        ui->propLuckyValue->setValue(fromNumber(prop->lucky_value_unknown.lucky_value));

        ui->player1Chk->setChecked(prop->who_can_use & 0x01);
        ui->player2Chk->setChecked(prop->who_can_use & 0x02);
        ui->player3Chk->setChecked(prop->who_can_use & 0x04);
        ui->player4Chk->setChecked(prop->who_can_use & 0x08);

        ui->multiplayerMagic->setValue(0);

        ui->poisonChk->setChecked(prop->effective_multiplayer_magic.effective & 0x08);
        ui->messChk->setChecked(prop->effective_multiplayer_magic.effective & 0x04);
        ui->sealChk->setChecked(prop->effective_multiplayer_magic.effective & 0x02);
        ui->sleepChk->setChecked(prop->effective_multiplayer_magic.effective & 0x01);

        ui->groupEffectChk->setChecked(false);

        ui->effectiveRound->setValue(0);

    }
    else if(index < 6) {

        ui->propHp->setEnabled(true);
        ui->propMp->setEnabled(true);

        ui->propHpMax->setEnabled(false);
        ui->propMpMax->setEnabled(false);

        ui->propAttackPower->setEnabled(true);
        ui->propDefensePower->setEnabled(true);

        ui->propBodyMovement->setEnabled(true);
        ui->propSpiritPower->setEnabled(true);
        ui->propLuckyValue->setEnabled(true);

        ui->player1Chk->setEnabled(true);
        ui->player2Chk->setEnabled(true);
        ui->player3Chk->setEnabled(true);
        ui->player4Chk->setEnabled(true);

        ui->multiplayerMagic->setEnabled(true);

        ui->poisonChk->setEnabled(false);
        ui->messChk->setEnabled(false);
        ui->sealChk->setEnabled(false);
        ui->sleepChk->setEnabled(false);

        ui->groupEffectChk->setEnabled(false);

        ui->effectiveRound->setEnabled(false);



        ui->propHp->setValue(fromNumber(prop->mp_hp_hp16.mp_hp.hpMax_hp.hp));
        ui->propMp->setValue(fromNumber(prop->mp_hp_hp16.mp_hp.mpMax_mp.mp));

        ui->propHpMax->setValue(0);
        ui->propMpMax->setValue(0);

        ui->propAttackPower->setValue(fromNumber(prop->df_at_mp16.df_at.attack_power));
        ui->propDefensePower->setValue(fromNumber(prop->df_at_mp16.df_at.defense_power));

        ui->propBodyMovement->setValue(fromNumber(prop->body_movement_animation.body_movement));
        ui->propSpiritPower->setValue(fromNumber(prop->spirit_power_index.spirit_power));
        ui->propLuckyValue->setValue(fromNumber(prop->lucky_value_unknown.lucky_value));

        ui->player1Chk->setChecked(prop->who_can_use & 0x01);
        ui->player2Chk->setChecked(prop->who_can_use & 0x02);
        ui->player3Chk->setChecked(prop->who_can_use & 0x04);
        ui->player4Chk->setChecked(prop->who_can_use & 0x08);

        ui->multiplayerMagic->setValue(prop->effective_multiplayer_magic.multiplayer_magic);

        ui->poisonChk->setChecked(false);
        ui->messChk->setChecked(false);
        ui->sealChk->setChecked(false);
        ui->sleepChk->setChecked(false);

        ui->groupEffectChk->setChecked(false);

        ui->effectiveRound->setValue(0);

    }
    else if(index < 7) {

        ui->propHp->setEnabled(false);
        ui->propMp->setEnabled(false);

        ui->propHpMax->setEnabled(true);
        ui->propMpMax->setEnabled(true);

        ui->propAttackPower->setEnabled(true);
        ui->propDefensePower->setEnabled(true);

        ui->propBodyMovement->setEnabled(true);
        ui->propSpiritPower->setEnabled(true);
        ui->propLuckyValue->setEnabled(true);

        ui->player1Chk->setEnabled(true);
        ui->player2Chk->setEnabled(true);
        ui->player3Chk->setEnabled(true);
        ui->player4Chk->setEnabled(true);

        ui->multiplayerMagic->setEnabled(false);

        ui->poisonChk->setEnabled(true);
        ui->messChk->setEnabled(true);
        ui->sealChk->setEnabled(true);
        ui->sleepChk->setEnabled(true);

        ui->groupEffectChk->setEnabled(true);

        ui->effectiveRound->setEnabled(true);



        ui->propHp->setValue(0);
        ui->propMp->setValue(0);

        ui->propHpMax->setValue(fromNumber(prop->mp_hp_hp16.mp_hp.hpMax_hp.hpMax));
        ui->propMpMax->setValue(fromNumber(prop->mp_hp_hp16.mp_hp.mpMax_mp.mpMax));

        ui->propAttackPower->setValue(prop->df_at_mp16.df_at.attack_power);
        ui->propDefensePower->setValue(fromNumber(prop->df_at_mp16.df_at.defense_power));
        ui->propBodyMovement->setValue(fromNumber(prop->body_movement_animation.body_movement));

        ui->propSpiritPower->setValue(fromNumber(prop->spirit_power_index.spirit_power));
        ui->propLuckyValue->setValue(fromNumber(prop->lucky_value_unknown.lucky_value));

        ui->player1Chk->setChecked(prop->who_can_use & 0x01);
        ui->player2Chk->setChecked(prop->who_can_use & 0x02);
        ui->player3Chk->setChecked(prop->who_can_use & 0x04);
        ui->player4Chk->setChecked(prop->who_can_use & 0x08);

        ui->multiplayerMagic->setValue(0);

        ui->poisonChk->setChecked(prop->effective_multiplayer_magic.effective & 0x08);
        ui->messChk->setChecked(prop->effective_multiplayer_magic.effective & 0x04);
        ui->sealChk->setChecked(prop->effective_multiplayer_magic.effective & 0x02);
        ui->sleepChk->setChecked(prop->effective_multiplayer_magic.effective & 0x01);

        ui->groupEffectChk->setChecked(prop->effective_multiplayer_magic.effective & 0x10);

        ui->effectiveRound->setValue(prop->effective_round);

    }
    else if(index < 9) {

        ui->propHp->setEnabled(true);
        ui->propMp->setEnabled(true);

        ui->propHpMax->setEnabled(false);
        ui->propMpMax->setEnabled(false);

        ui->propAttackPower->setEnabled(false);
        ui->propDefensePower->setEnabled(false);

        ui->propBodyMovement->setEnabled(false);
        ui->propSpiritPower->setEnabled(false);
        ui->propLuckyValue->setEnabled(false);

        ui->player1Chk->setEnabled(false);
        ui->player2Chk->setEnabled(false);
        ui->player3Chk->setEnabled(false);
        ui->player4Chk->setEnabled(false);

        ui->multiplayerMagic->setEnabled(false);

        ui->poisonChk->setEnabled(true);
        ui->messChk->setEnabled(true);
        ui->sealChk->setEnabled(true);
        ui->sleepChk->setEnabled(true);

        ui->groupEffectChk->setEnabled(true);

        ui->effectiveRound->setEnabled(index < 8);



        ui->propHp->setValue(prop->mp_hp_hp16.hp16);
        ui->propMp->setValue(prop->df_at_mp16.mp16);

        ui->propHpMax->setValue(0);
        ui->propMpMax->setValue(0);

        ui->propAttackPower->setValue(0);
        ui->propDefensePower->setValue(0);

        ui->propBodyMovement->setValue(0);
        ui->propSpiritPower->setValue(0);
        ui->propLuckyValue->setValue(0);

        ui->player1Chk->setChecked(false);
        ui->player2Chk->setChecked(false);
        ui->player3Chk->setChecked(false);
        ui->player4Chk->setChecked(false);

        ui->multiplayerMagic->setValue(0);

        ui->poisonChk->setChecked(prop->effective_multiplayer_magic.effective & 0x08);
        ui->messChk->setChecked(prop->effective_multiplayer_magic.effective & 0x04);
        ui->sealChk->setChecked(prop->effective_multiplayer_magic.effective & 0x02);
        ui->sleepChk->setChecked(prop->effective_multiplayer_magic.effective & 0x01);

        ui->groupEffectChk->setChecked(prop->effective_multiplayer_magic.effective & 0x10);

        if(index < 8) {
            ui->effectiveRound->setValue(prop->effective_round);
        } else {
            ui->effectiveRound->setValue(0);
        }

    }
    else if(index < 10) {

        ui->propHp->setEnabled(true);
        ui->propMp->setEnabled(false);

        ui->propHpMax->setEnabled(false);
        ui->propMpMax->setEnabled(false);

        ui->propAttackPower->setEnabled(false);
        ui->propDefensePower->setEnabled(false);

        ui->propBodyMovement->setEnabled(false);
        ui->propSpiritPower->setEnabled(false);
        ui->propLuckyValue->setEnabled(false);

        ui->player1Chk->setEnabled(false);
        ui->player2Chk->setEnabled(false);
        ui->player3Chk->setEnabled(false);
        ui->player4Chk->setEnabled(false);

        ui->multiplayerMagic->setEnabled(false);

        ui->poisonChk->setEnabled(false);
        ui->messChk->setEnabled(false);
        ui->sealChk->setEnabled(false);
        ui->sleepChk->setEnabled(false);

        ui->groupEffectChk->setEnabled(false);

        ui->effectiveRound->setEnabled(false);



        ui->propHp->setValue(prop->mp_hp_hp16.mp_hp.hpMax_hp.hp);
        ui->propMp->setValue(0);

        ui->propHpMax->setValue(0);
        ui->propMpMax->setValue(0);

        ui->propAttackPower->setValue(0);
        ui->propDefensePower->setValue(0);

        ui->propBodyMovement->setValue(0);
        ui->propSpiritPower->setValue(0);
        ui->propLuckyValue->setValue(0);

        ui->player1Chk->setChecked(false);
        ui->player2Chk->setChecked(false);
        ui->player3Chk->setChecked(false);
        ui->player4Chk->setChecked(false);

        ui->multiplayerMagic->setValue(0);

        ui->poisonChk->setChecked(false);
        ui->messChk->setChecked(false);
        ui->sealChk->setChecked(false);
        ui->sleepChk->setChecked(false);

        ui->groupEffectChk->setChecked(false);

        ui->effectiveRound->setValue(0);

    }
    else if(index < 11) {

        ui->propHp->setEnabled(false);
        ui->propMp->setEnabled(false);

        ui->propHpMax->setEnabled(true);
        ui->propMpMax->setEnabled(true);

        ui->propAttackPower->setEnabled(true);
        ui->propDefensePower->setEnabled(true);

        ui->propBodyMovement->setEnabled(true);
        ui->propSpiritPower->setEnabled(true);
        ui->propLuckyValue->setEnabled(true);

        ui->player1Chk->setEnabled(false);
        ui->player2Chk->setEnabled(false);
        ui->player3Chk->setEnabled(false);
        ui->player4Chk->setEnabled(false);

        ui->multiplayerMagic->setEnabled(false);

        ui->poisonChk->setEnabled(false);
        ui->messChk->setEnabled(false);
        ui->sealChk->setEnabled(false);
        ui->sleepChk->setEnabled(false);

        ui->groupEffectChk->setEnabled(false);

        ui->effectiveRound->setEnabled(false);



        ui->propHp->setValue(0);
        ui->propMp->setValue(0);

        ui->propHpMax->setValue(prop->mp_hp_hp16.mp_hp.hpMax_hp.hpMax);
        ui->propMpMax->setValue(prop->mp_hp_hp16.mp_hp.mpMax_mp.mpMax);

        ui->propAttackPower->setValue(prop->df_at_mp16.df_at.attack_power);
        ui->propDefensePower->setValue(prop->df_at_mp16.df_at.defense_power);

        ui->propBodyMovement->setValue(prop->body_movement_animation.body_movement);
        ui->propSpiritPower->setValue(prop->spirit_power_index.spirit_power);
        ui->propLuckyValue->setValue(prop->lucky_value_unknown.lucky_value);

        ui->player1Chk->setChecked(false);
        ui->player2Chk->setChecked(false);
        ui->player3Chk->setChecked(false);
        ui->player4Chk->setChecked(false);

        ui->multiplayerMagic->setValue(0);

        ui->poisonChk->setChecked(false);
        ui->messChk->setChecked(false);
        ui->sealChk->setChecked(false);
        ui->sleepChk->setChecked(false);

        ui->groupEffectChk->setChecked(false);

        ui->effectiveRound->setValue(0);

    }
    else if(index < 12) {

        ui->propHp->setEnabled(false);
        ui->propMp->setEnabled(false);

        ui->propHpMax->setEnabled(false);
        ui->propMpMax->setEnabled(false);

        ui->propAttackPower->setEnabled(true);
        ui->propDefensePower->setEnabled(true);

        ui->propBodyMovement->setEnabled(true);
        ui->propSpiritPower->setEnabled(false);
        ui->propLuckyValue->setEnabled(false);

        ui->player1Chk->setEnabled(false);
        ui->player2Chk->setEnabled(false);
        ui->player3Chk->setEnabled(false);
        ui->player4Chk->setEnabled(false);

        ui->multiplayerMagic->setEnabled(false);

        ui->poisonChk->setEnabled(false);
        ui->messChk->setEnabled(false);
        ui->sealChk->setEnabled(false);
        ui->sleepChk->setEnabled(false);

        ui->groupEffectChk->setEnabled(true);

        ui->effectiveRound->setEnabled(true);



        ui->propHp->setValue(0);
        ui->propMp->setValue(0);

        ui->propHpMax->setValue(0);
        ui->propMpMax->setValue(0);

        ui->propAttackPower->setValue(prop->df_at_mp16.df_at.attack_power);
        ui->propDefensePower->setValue(prop->df_at_mp16.df_at.defense_power);

        ui->propBodyMovement->setValue(prop->body_movement_animation.body_movement);
        ui->propSpiritPower->setValue(0);
        ui->propLuckyValue->setValue(0);

        ui->player1Chk->setChecked(false);
        ui->player2Chk->setChecked(false);
        ui->player3Chk->setChecked(false);
        ui->player4Chk->setChecked(false);

        ui->multiplayerMagic->setValue(0);

        ui->poisonChk->setChecked(false);
        ui->messChk->setChecked(false);
        ui->sealChk->setChecked(false);
        ui->sleepChk->setChecked(false);

        ui->groupEffectChk->setChecked(prop->effective_multiplayer_magic.effective & 0x10);

        ui->effectiveRound->setValue(prop->effective_round);

    }
    else {

        ui->propHp->setEnabled(false);
        ui->propMp->setEnabled(false);

        ui->propHpMax->setEnabled(false);
        ui->propMpMax->setEnabled(false);

        ui->propAttackPower->setEnabled(false);
        ui->propDefensePower->setEnabled(false);

        ui->propBodyMovement->setEnabled(false);
        ui->propSpiritPower->setEnabled(false);
        ui->propLuckyValue->setEnabled(false);

        ui->player1Chk->setEnabled(false);
        ui->player2Chk->setEnabled(false);
        ui->player3Chk->setEnabled(false);
        ui->player4Chk->setEnabled(false);

        ui->multiplayerMagic->setEnabled(false);

        ui->poisonChk->setEnabled(false);
        ui->messChk->setEnabled(false);
        ui->sealChk->setEnabled(false);
        ui->sleepChk->setEnabled(false);

        ui->groupEffectChk->setEnabled(false);

        ui->effectiveRound->setEnabled(false);

    }

    ui->propDescription->setText(gbkCodec->toUnicode((char *)prop->description));

    connect(ui->propName,           &QLineEdit::editingFinished,                    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propIcon,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propEvent,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propPriceBuy,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propPriceSale,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propHp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propMp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propHpMax,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propMpMax,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propAttackPower,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propDefensePower,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propBodyMovement,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propSpiritPower,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propLuckyValue,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->player1Chk,         &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->player2Chk,         &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->player3Chk,         &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->player4Chk,         &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->multiplayerMagic,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->poisonChk,          &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->messChk,            &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->sealChk,            &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->sleepChk,           &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->groupEffectChk,     &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->effectiveRound,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propDescription,    &QTextEdit::textChanged,                        this, &MainWidget::on_propData_valueChanged);
}

void MainWidget::on_propIdx_valueChanged(int index)
{
    Q_UNUSED(index)
    on_propType_currentIndexChanged(ui->propType->currentIndex());
}

void MainWidget::on_propData_valueChanged()
{
    if(_GrsDataSize < sizeof(prop_t)) {
        return;
    }

    prop_t *prop = reinterpret_cast<prop_t *>(_GrsData);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    QByteArray propName = gbkCodec->fromUnicode(ui->propName->text());
    memset(prop->name, 0xff, 12);
    memcpy_s(prop->name, 12, propName.data(), propName.size());
    prop->name[propName.size()] = '\0';

    prop->icon = ui->propIcon->value();

    prop->event = ui->propEvent->value();

    prop->price_buy = ui->propPriceBuy->value();
    prop->price_sale = ui->propPriceSale->value();

    if(ui->propType->currentIndex() < 5) {

        prop->mp_hp_hp16.mp_hp.hpMax_hp.hpMax       = toNumber(ui->propHpMax->value());
        prop->mp_hp_hp16.mp_hp.mpMax_mp.mpMax       = toNumber(ui->propMpMax->value());

        prop->df_at_mp16.df_at.attack_power         = toNumber(ui->propAttackPower->value());
        prop->df_at_mp16.df_at.defense_power        = toNumber(ui->propDefensePower->value());

        prop->body_movement_animation.body_movement = toNumber(ui->propBodyMovement->value());
        prop->spirit_power_index.spirit_power       = toNumber(ui->propSpiritPower->value());
        prop->lucky_value_unknown.lucky_value       = toNumber(ui->propLuckyValue->value());

        if(ui->player1Chk->isChecked()) {
            prop->who_can_use |= 0x01;
        } else {
            prop->who_can_use &= ~0x01;
        }
        if(ui->player2Chk->isChecked()) {
            prop->who_can_use |= 0x02;
        } else {
            prop->who_can_use &= ~0x02;
        }
        if(ui->player3Chk->isChecked()) {
            prop->who_can_use |= 0x04;
        } else {
            prop->who_can_use &= ~0x04;
        }
        if(ui->player4Chk->isChecked()) {
            prop->who_can_use |= 0x08;
        } else {
            prop->who_can_use &= ~0x08;
        }

        if(ui->poisonChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x08;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x08;
        }
        if(ui->messChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x04;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x04;
        }
        if(ui->sealChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x02;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x02;
        }
        if(ui->sleepChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x01;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x01;
        }

    }
    else if(ui->propType->currentIndex() < 6) {

        prop->mp_hp_hp16.mp_hp.hpMax_hp.hp          = toNumber(ui->propHp->value());
        prop->mp_hp_hp16.mp_hp.mpMax_mp.mp          = toNumber(ui->propMp->value());

        prop->df_at_mp16.df_at.attack_power         = toNumber(ui->propAttackPower->value());
        prop->df_at_mp16.df_at.defense_power        = toNumber(ui->propDefensePower->value());

        prop->body_movement_animation.body_movement = toNumber(ui->propBodyMovement->value());
        prop->spirit_power_index.spirit_power       = toNumber(ui->propSpiritPower->value());
        prop->lucky_value_unknown.lucky_value       = toNumber(ui->propLuckyValue->value());

        if(ui->player1Chk->isChecked()) {
            prop->who_can_use |= 0x01;
        } else {
            prop->who_can_use &= ~0x01;
        }
        if(ui->player2Chk->isChecked()) {
            prop->who_can_use |= 0x02;
        } else {
            prop->who_can_use &= ~0x02;
        }
        if(ui->player3Chk->isChecked()) {
            prop->who_can_use |= 0x04;
        } else {
            prop->who_can_use &= ~0x04;
        }
        if(ui->player4Chk->isChecked()) {
            prop->who_can_use |= 0x08;
        } else {
            prop->who_can_use &= ~0x08;
        }

        prop->effective_multiplayer_magic.multiplayer_magic = ui->multiplayerMagic->value();

    }
    else if(ui->propType->currentIndex() < 7) {

        prop->mp_hp_hp16.mp_hp.hpMax_hp.hpMax       = toNumber(ui->propHpMax->value());
        prop->mp_hp_hp16.mp_hp.mpMax_mp.mpMax       = toNumber(ui->propMpMax->value());

        prop->df_at_mp16.df_at.attack_power         = ui->propAttackPower->value();
        prop->df_at_mp16.df_at.defense_power        = toNumber(ui->propDefensePower->value());

        prop->body_movement_animation.body_movement = toNumber(ui->propBodyMovement->value());
        prop->spirit_power_index.spirit_power       = toNumber(ui->propSpiritPower->value());
        prop->lucky_value_unknown.lucky_value       = toNumber(ui->propLuckyValue->value());

        if(ui->player1Chk->isChecked()) {
            prop->who_can_use |= 0x01;
        } else {
            prop->who_can_use &= ~0x01;
        }
        if(ui->player2Chk->isChecked()) {
            prop->who_can_use |= 0x02;
        } else {
            prop->who_can_use &= ~0x02;
        }
        if(ui->player3Chk->isChecked()) {
            prop->who_can_use |= 0x04;
        } else {
            prop->who_can_use &= ~0x04;
        }
        if(ui->player4Chk->isChecked()) {
            prop->who_can_use |= 0x08;
        } else {
            prop->who_can_use &= ~0x08;
        }

        if(ui->poisonChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x08;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x08;
        }
        if(ui->messChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x04;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x04;
        }
        if(ui->sealChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x02;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x02;
        }
        if(ui->sleepChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x01;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x01;
        }

        if(ui->groupEffectChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x10;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x10;
        }

        prop->effective_round = ui->effectiveRound->value();

    }
    else if(ui->propType->currentIndex() < 9) {

        prop->mp_hp_hp16.hp16 = ui->propHp->value();
        prop->df_at_mp16.mp16 = ui->propMp->value();

        if(ui->poisonChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x08;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x08;
        }
        if(ui->messChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x04;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x04;
        }
        if(ui->sealChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x02;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x02;
        }
        if(ui->sleepChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x01;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x01;
        }

        if(ui->groupEffectChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x10;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x10;
        }

        if(ui->propType->currentIndex() < 8) {
            prop->effective_round = ui->effectiveRound->value();
        }

    }
    else if(ui->propType->currentIndex() < 10) {

        prop->mp_hp_hp16.mp_hp.hpMax_hp.hp = ui->propHp->value();

    }
    else if(ui->propType->currentIndex() < 11) {

        prop->mp_hp_hp16.mp_hp.hpMax_hp.hpMax       = ui->propHpMax->value();
        prop->mp_hp_hp16.mp_hp.mpMax_mp.mpMax       = ui->propMpMax->value();

        prop->df_at_mp16.df_at.attack_power         = ui->propAttackPower->value();
        prop->df_at_mp16.df_at.defense_power        = ui->propDefensePower->value();

        prop->body_movement_animation.body_movement = ui->propBodyMovement->value();
        prop->spirit_power_index.spirit_power       = ui->propSpiritPower->value();
        prop->lucky_value_unknown.lucky_value       = ui->propLuckyValue->value();

    }
    else if(ui->propType->currentIndex() < 12) {

        prop->df_at_mp16.df_at.attack_power         = ui->propAttackPower->value();
        prop->df_at_mp16.df_at.defense_power        = ui->propDefensePower->value();

        prop->body_movement_animation.body_movement = ui->propBodyMovement->value();

        if(ui->groupEffectChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x10;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x10;
        }

        prop->effective_round = ui->effectiveRound->value();

    }

    QByteArray propDescription = gbkCodec->fromUnicode(ui->propDescription->toPlainText());
    memset(prop->description, 0xff, 102);
    memcpy_s(prop->description, 102, propDescription.data(), propDescription.size());
    prop->description[propDescription.size()] = '\0';
}

void MainWidget::on_propDescription_textChanged()
{
    int len8Bit = ui->propDescription->toPlainText().toLocal8Bit().length();
    if(len8Bit > 101) {
        ui->propDescription->textCursor().movePosition(QTextCursor::End);
        if(ui->propDescription->textCursor().hasSelection()) {
            ui->propDescription->textCursor().clearSelection();
        }
        do {
            ui->propDescription->textCursor().deletePreviousChar();
            len8Bit = ui->propDescription->toPlainText().toLocal8Bit().length();
        } while(len8Bit > 101);
    }
}
