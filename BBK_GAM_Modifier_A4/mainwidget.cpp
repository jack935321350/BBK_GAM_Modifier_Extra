#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "gam_type.h"

#include "iconvieweditdlg.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    static QTranslator translator;
    translator.load(":/translations/zh_CN.qm");
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);

    adjustSize();
    resize(0, 0);

    buildDateStr = QLocale(QLocale::English).toDate(QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy").toString("yyyyMMdd");
    buildTimeStr = QLocale(QLocale::English).toTime(QString(__TIME__), "hh:mm:ss").toString("hhmm");

    setWindowTitle(tr("BBK_GAM_Modifier_A4_") + buildDateStr + '_' + buildTimeStr);

    playerPropSpinBoxVec.append(ui->playerHat);
    playerPropSpinBoxVec.append(ui->playerClothes);
    playerPropSpinBoxVec.append(ui->playerCloak);
    playerPropSpinBoxVec.append(ui->playerWristlet);
    playerPropSpinBoxVec.append(ui->playerWeapon);
    playerPropSpinBoxVec.append(ui->playerShoes);
    playerPropSpinBoxVec.append(ui->playerAccessory1);
    playerPropSpinBoxVec.append(ui->playerAccessory2);

    enemyPropSpinBoxVec.append(ui->enemyStealType);
    enemyPropSpinBoxVec.append(ui->enemyStealIndex);
    enemyPropSpinBoxVec.append(ui->enemyStealCount);
    enemyPropSpinBoxVec.append(ui->enemyDropType);
    enemyPropSpinBoxVec.append(ui->enemyDropIndex);

    connect(ui->loadBdaBtn,             &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->allPlayer999Btn,        &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->copyPlayer1Wear,        &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->randomWear,             &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->saveBdaBtn,             &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);

    connect(ui->editPlayerIcon,         &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->editEnemyIcon,          &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->editPropIcon,           &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);

    connect(ui->playerName,             &QLineEdit::textChanged,        this, &MainWidget::on_lineEdit_textChanged);
    connect(ui->enemyName,              &QLineEdit::textChanged,        this, &MainWidget::on_lineEdit_textChanged);
    connect(ui->propName,               &QLineEdit::textChanged,        this, &MainWidget::on_lineEdit_textChanged);
    connect(ui->magicName,              &QLineEdit::textChanged,        this, &MainWidget::on_lineEdit_textChanged);

    connect(&playerAutoPlayTimer,       &QTimer::timeout,               this, &MainWidget::on_playerAutoPlayTimer_timeout);
    connect(&enemyAutoPlayTimer,        &QTimer::timeout,               this, &MainWidget::on_enemyAutoPlayTimer_timeout);

    ui->playerFootstep->installEventFilter(this);
    ui->playerHat->installEventFilter(this);
    ui->playerClothes->installEventFilter(this);
    ui->playerCloak->installEventFilter(this);
    ui->playerWristlet->installEventFilter(this);
    ui->playerWeapon->installEventFilter(this);
    ui->playerShoes->installEventFilter(this);
    ui->playerAccessory1->installEventFilter(this);
    ui->playerAccessory2->installEventFilter(this);

    ui->enemyStealType->installEventFilter(this);
    ui->enemyStealIndex->installEventFilter(this);
    ui->enemyStealCount->installEventFilter(this);
    ui->enemyDropType->installEventFilter(this);
    ui->enemyDropIndex->installEventFilter(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // qDebug() << size();
}

bool MainWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->playerFootstep) {
        QSpinBox *spinBox = static_cast<QSpinBox *>(obj);
        if(event->type() == QEvent::Wheel) {
            if(spinBox == ui->playerFootstep) {
                QWheelEvent *ev = reinterpret_cast<QWheelEvent *>(event);
                if(ev->angleDelta().y() > 0 && spinBox->value() >= spinBox->maximum()) {
                    spinBox->setValue(spinBox->minimum());
                    return true;
                }
                else if(ev->angleDelta().y() < 0 && spinBox->value() <= spinBox->minimum()) {
                    spinBox->setValue(spinBox->maximum());
                    return true;
                }
            }
        }
        else if(event->type() == QEvent::KeyPress) {
            QKeyEvent *ev = reinterpret_cast<QKeyEvent *>(event);
            if(ev->key() == Qt::Key_Up && spinBox->value() >= spinBox->maximum()) {
                spinBox->setValue(spinBox->minimum());
                return true;
            }
            else if(ev->key() == Qt::Key_Down && spinBox->value() <= spinBox->minimum()) {
                spinBox->setValue(spinBox->maximum());
                return true;
            }
        }

    }
    else if(event->type() == QEvent::FocusIn) {
        QSpinBox *spinBox = static_cast<QSpinBox *>(obj);
        if(spinBox == ui->playerHat) {
            ui->propType->setCurrentIndex(0);
            ui->propIdx->setValue(spinBox->value());
        }
        else if(spinBox == ui->playerClothes) {
            ui->propType->setCurrentIndex(1);
            ui->propIdx->setValue(spinBox->value());
        }
        else if(spinBox == ui->playerCloak) {
            ui->propType->setCurrentIndex(3);
            ui->propIdx->setValue(spinBox->value());
        }
        else if(spinBox == ui->playerWristlet) {
            ui->propType->setCurrentIndex(4);
            ui->propIdx->setValue(spinBox->value());
        }
        else if(spinBox == ui->playerWeapon) {
            ui->propType->setCurrentIndex(6);
            ui->propIdx->setValue(spinBox->value());
        }
        else if(spinBox == ui->playerShoes) {
            ui->propType->setCurrentIndex(2);
            ui->propIdx->setValue(spinBox->value());
        }
        else if(spinBox == ui->playerAccessory1 || spinBox == ui->playerAccessory2) {
            ui->propType->setCurrentIndex(5);
            ui->propIdx->setValue(spinBox->value());
        }
        else if(spinBox == ui->enemyStealType || spinBox == ui->enemyStealIndex || spinBox == ui->enemyStealCount) {
            if(ui->enemyStealType->value() > 0) {
                ui->propType->setCurrentIndex(ui->enemyStealType->value() - 1);
                ui->enemyStealIndex->setRange(0, propIndexVecVec[ui->enemyStealType->value() - 1].size());
                ui->propIdx->setValue(ui->enemyStealIndex->value());
            }
        }
        else if(spinBox == ui->enemyDropType || spinBox == ui->enemyDropIndex) {
            if(ui->enemyDropType->value() > 0) {
                ui->propType->setCurrentIndex(ui->enemyDropType->value() - 1);
                ui->enemyDropIndex->setRange(0, propIndexVecVec[ui->enemyDropType->value() - 1].size());
                ui->propIdx->setValue(ui->enemyDropIndex->value());
            }
        }

    }
    return QWidget::eventFilter(obj, event);
}

void MainWidget::on_pushButton_clicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());

    if(btn == ui->loadBdaBtn) {

        QSettings settings("QtProject", "BBK_GAM_Modifier_A4");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), settings.value("last_dir").toString(), "BDA(*.bda)");
        if(!fileName.isEmpty() && QFile(fileName).exists()) {
            settings.setValue("last_dir", QFileInfo(fileName).absoluteDir().path());
            settings.setValue("last_file", QFileInfo(fileName).fileName());
            settings.sync();
        }
        else {
            return;
        }

        setWindowTitle(tr("BBK_GAM_Modifier_A4 ") + buildDateStr + ' ' + buildTimeStr);

        engineBA.clear();
        libraryBA.clear();

        offsetMap.clear();

        playerIndexVec.clear();
        npcIndexVec.clear();
        enemyIndexVec.clear();
        sceneObjIndexVec.clear();

        propIndexVecVec.clear();
        magicIndexVecVec.clear();

        roleIconIndexVecVec.clear();
        propIconIndexVecVec.clear();

        ui->playerIdx->setRange(0, 0);
        ui->enemyIdx->setRange(0, 0);

        QFile gamFile(fileName);
        if(!gamFile.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Warning"), tr("Bda File Open Failed!"));
            return;
        }
        QByteArray gamBA = gamFile.readAll();
        gamFile.close();

        int libIndex = gamBA.indexOf(QByteArray("LIB", 3));
        if(libIndex < 0 || libIndex > (gamFile.size() - 0x4000)) {
            QMessageBox::warning(this, tr("Warning"), tr("Tag LIB Seek Failed!"));
            return;
        }

        engineBA = gamBA.left(libIndex);
        libraryBA = gamBA.mid(libIndex);

        int idx1 = 0x10, idx2 = 0x2000;
        while(libraryBA.at(idx1) != -1) {
            int resType = libraryBA.at(idx1++) & 0xff;
            int subType = libraryBA.at(idx1++) & 0xff;
            int index   = libraryBA.at(idx1++) & 0xff;

            int offsetL = libraryBA.at(idx2++) & 0xff;
            int offsetM = libraryBA.at(idx2++) & 0xff;
            int offsetH = libraryBA.at(idx2++) & 0xff;

            offsetMap.insert(resType << 16 | subType << 8 | index, offsetH << 16 | offsetM << 8 | offsetL + 2);
        }

        for(int subType = 1; subType <= 4; subType++) {
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                int offset = getResOffset(Res_ARS, subType, resIndex);
                if(offset > 0) {
                    if(subType == 1) {
                        playerIndexVec.append(offset);
                    }
                    else if(subType == 2) {
                        npcIndexVec.append(offset);
                    }
                    else if(subType == 3) {
                        enemyIndexVec.append(offset);
                    }
                    else if(subType == 4) {
                        sceneObjIndexVec.append(offset);
                    }
                } else {
                    break;
                }
            }
        }

        for(int subType = 1; subType <= 5; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                int offset = getResOffset(Res_MRS, subType, resIndex);
                if(offset > 0) {
                    subTypeVec.append(offset);
                } else {
                    break;
                }
            }
            magicIndexVecVec.append(subTypeVec);
        }

        for(int subType = 1; subType <= 14; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                int offset = getResOffset(Res_GRS, subType, resIndex);
                if(offset > 0) {
                    subTypeVec.append(offset);
                } else {
                    break;
                }
            }
            propIndexVecVec.append(subTypeVec);
        }

        for(int subType = 1; subType <= 4; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                subTypeVec.append(getResOffset(Res_ACP, subType, resIndex));
            }
            roleIconIndexVecVec.append(subTypeVec);
        }

        for(int subType = 1; subType <= 14; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                int offset = getResOffset(Res_GDP, subType, resIndex);
                if(offset > 0) {
                    subTypeVec.append(offset);
                } else {
                    break;
                }
            }
            propIconIndexVecVec.append(subTypeVec);
        }



        ui->playerIdx->setRange(1, playerIndexVec.size());

        ui->playerHat->setRange(0, propIndexVecVec[0].size());
        ui->playerClothes->setRange(0, propIndexVecVec[1].size());
        ui->playerCloak->setRange(0, propIndexVecVec[3].size());
        ui->playerWristlet->setRange(0, propIndexVecVec[4].size());
        ui->playerWeapon->setRange(0, propIndexVecVec[6].size());
        ui->playerShoes->setRange(0, propIndexVecVec[2].size());
        ui->playerAccessory1->setRange(0, propIndexVecVec[5].size());
        ui->playerAccessory2->setRange(0, propIndexVecVec[5].size());

        ui->enemyIdx->setRange(1, enemyIndexVec.size());

        ui->enemyStealType->setRange(0, propIndexVecVec.size());
        ui->enemyDropType->setRange(0, propIndexVecVec.size());

        on_playerIdx_valueChanged(ui->playerIdx->value());
        on_enemyIdx_valueChanged(ui->enemyIdx->value());
        on_propType_currentIndexChanged(ui->propType->currentIndex());
        on_magicType_currentIndexChanged(ui->magicType->currentIndex());

        setWindowTitle(QFileInfo(fileName).fileName());

    }
    else if(btn == ui->allPlayer999Btn) {

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Bda Data Not Loaded!"));
            return;
        }

        for(int i = 0; i < playerIndexVec.size(); i++) {

            player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[i]);

            player->attack_power = 999;
            player->defense_power = 999;
            player->body_movement = 99;
            player->spirit_power = 55;
            player->lucky_value = 99;
            player->hp_max = 999;
            player->hp = 999;
            player->mp_max = 999;
            player->mp = 999;

        }

        QMessageBox::information(this, tr("Information"), tr("All Player 999 OK!"));

        on_playerIdx_valueChanged(ui->playerIdx->value());

    }
    else if(btn == ui->copyPlayer1Wear) {

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Bda Data Not Loaded!"));
            return;
        }

        if(playerIndexVec.isEmpty()) {
            return;
        }

        player_t *player1 = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[0]);

        for(int i = 1; i < playerIndexVec.size(); i++) {

            player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[i]);

            player->hat         = player1->hat;
            player->clothes     = player1->clothes;
            player->cloak       = player1->cloak;
            player->wristlet    = player1->wristlet;
            player->weapon      = player1->weapon;
            player->shoes       = player1->shoes;
            player->accessory1  = player1->accessory1;
            player->accessory2  = player1->accessory2;

        }

        QMessageBox::information(this, tr("Information"), tr("Copy Player1 Wear OK!"));

        on_playerIdx_valueChanged(ui->playerIdx->value());

    }
    else if(btn == ui->randomWear) {

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Bda Data Not Loaded!"));
            return;
        }

        for(int i = 0; i < playerIndexVec.size(); i++) {

            player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[i]);

            player->hat         = QRandomGenerator::global()->bounded(ui->playerHat->minimum() + 1,         ui->playerHat->maximum());
            player->clothes     = QRandomGenerator::global()->bounded(ui->playerClothes->minimum() + 1,     ui->playerClothes->maximum());
            player->cloak       = QRandomGenerator::global()->bounded(ui->playerCloak->minimum() + 1,       ui->playerCloak->maximum());
            player->wristlet    = QRandomGenerator::global()->bounded(ui->playerWristlet->minimum() + 1,    ui->playerWristlet->maximum());
            player->weapon      = QRandomGenerator::global()->bounded(ui->playerWeapon->minimum() + 1,      ui->playerWeapon->maximum());
            player->shoes       = QRandomGenerator::global()->bounded(ui->playerShoes->minimum() + 1,       ui->playerShoes->maximum());
            player->accessory1  = QRandomGenerator::global()->bounded(ui->playerAccessory1->minimum() + 1,  ui->playerAccessory1->maximum());
            player->accessory2  = QRandomGenerator::global()->bounded(ui->playerAccessory2->minimum() + 1,  ui->playerAccessory2->maximum());

        }

        on_playerIdx_valueChanged(ui->playerIdx->value());

    }
    else if(btn == ui->saveBdaBtn) {

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Bda Data Not Loaded!"));
            return;
        }

        QSettings settings("QtProject", "BBK_GAM_Modifier_A4");
        QFile gamFile(QFileDialog::getSaveFileName(this, tr("Save File"), settings.value("last_dir").toString() + '/' + settings.value("last_file").toString(), "BDA(*.bda)"));
        if(!gamFile.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, tr("Warning"), tr("Bda File Save Failed!"));
            return;
        }

        for(int i = 0; i < playerIndexVec.size(); i++) {

            player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[i]);

            player->immunity = 0;
            player->effective = 0;
            player->hp_each_turn = 0;
            player->mp_each_turn = 0;

            if(player->hat > 0 && propIndexVecVec[0].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[0][player->hat - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->clothes > 0 && propIndexVecVec[1].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[1][player->clothes - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->cloak > 0 && propIndexVecVec[3].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[3][player->cloak - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->wristlet > 0 && propIndexVecVec[4].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[4][player->wristlet - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->weapon > 0 && propIndexVecVec[6].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[6][player->weapon - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->effective |= (prop->effective_multiplayer_magic.effective & 0x1f);
            }

            if(player->shoes > 0 && propIndexVecVec[2].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[2][player->shoes - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->accessory1 > 0 && propIndexVecVec[5].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[5][player->accessory1 - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->hp_each_turn += prop->mp_hp_hp16.mp_hp.hpMax_hp.hp;
                player->mp_each_turn += prop->mp_hp_hp16.mp_hp.mpMax_mp.mp;
            }

            if(player->accessory2 > 0 && propIndexVecVec[5].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[5][player->accessory2 - 1]);
                prop->who_can_use |= (1 << (player->idx - 1));
                player->hp_each_turn += prop->mp_hp_hp16.mp_hp.hpMax_hp.hp;
                player->mp_each_turn += prop->mp_hp_hp16.mp_hp.mpMax_mp.mp;
            }

            player->abnormal_round = 0x5f;

        }

        QDataStream gamStream(&gamFile);
        gamStream.writeRawData(engineBA.data(), engineBA.size());
        gamStream.writeRawData(libraryBA.data(), libraryBA.size());

        gamFile.close();

    }
    else if(btn == ui->editPlayerIcon) {

        if(roleIconIndexVecVec.size() > 0) {
            effects_icon_head_t *effects_icon_head = nullptr;
            for(int i = 0; i < roleIconIndexVecVec[0].size(); i++) {
                if(roleIconIndexVecVec[0][i] > 0) {
                    effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[0][i]);
                    if(effects_icon_head->idx == ui->playerIcon->value()) {
                        break;
                    } else {
                        effects_icon_head = nullptr;
                    }
                }
            }
            if(effects_icon_head != nullptr) {
                IconViewEditDlg iconViewEditDlg(this);
                iconViewEditDlg.UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                if(iconViewEditDlg.exec() == IconViewEditDlg::Accepted) {
                    iconViewEditDlg.WriteBack((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    on_playerIcon_valueChanged(ui->playerIcon->value());
                }
            }
        }
    }
    else if(btn == ui->editEnemyIcon) {

        if(roleIconIndexVecVec.size() > 2) {
            effects_icon_head_t *effects_icon_head = nullptr;
            for(int i = 0; i < roleIconIndexVecVec[2].size(); i++) {
                if(roleIconIndexVecVec[2][i] > 0) {
                    effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[2][i]);
                    if(effects_icon_head->idx == ui->enemyIcon->value()) {
                        break;
                    } else {
                        effects_icon_head = nullptr;
                    }
                }
            }
            if(effects_icon_head != nullptr) {
                IconViewEditDlg iconViewEditDlg(this);
                iconViewEditDlg.UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                if(iconViewEditDlg.exec() == IconViewEditDlg::Accepted) {
                    iconViewEditDlg.WriteBack((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    on_enemyIcon_valueChanged(ui->enemyIcon->value());
                }

            }
        }
    }
    else if(btn == ui->editPropIcon) {

        if(propIconIndexVecVec.size() > 13) {
            effects_icon_head_t *effects_icon_head = nullptr;
            for(int i = 0; i < propIconIndexVecVec[ui->propType->currentIndex()].size(); i++) {
                if(propIconIndexVecVec[ui->propType->currentIndex()][i] > 0) {
                    effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + propIconIndexVecVec[ui->propType->currentIndex()][i]);
                    if(effects_icon_head->idx == ui->propIcon->value()) {
                        break;
                    } else {
                        effects_icon_head = nullptr;
                    }
                }
            }
            if(effects_icon_head != nullptr) {
                IconViewEditDlg iconViewEditDlg(this);
                iconViewEditDlg.UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                if(iconViewEditDlg.exec() == IconViewEditDlg::Accepted) {
                    iconViewEditDlg.WriteBack((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    on_propIcon_valueChanged(ui->propIcon->value());
                }
            }
        }
    }
}

void MainWidget::on_lineEdit_textChanged()
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(sender());
    int len8Bit = lineEdit->text().toLocal8Bit().length();
    if(len8Bit > 11) {
        lineEdit->setCursorPosition(lineEdit->text().size());
        do {
            lineEdit->backspace();
            len8Bit = lineEdit->text().toLocal8Bit().length();
        } while(len8Bit > 11);
    }
}



void MainWidget::on_playerIdx_valueChanged(int index)
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    disconnect(ui->playerName,          &QLineEdit::editingFinished,                    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerMagicList,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerLearned,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerIcon,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerHat,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerClothes,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerCloak,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerWristlet,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerWeapon,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerShoes,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerAccessory1,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerAccessory2,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerAttackPower,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerDefensePower,  QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerHpMax,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerHp,            QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerMpMax,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerMp,            QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerBodyMovement,  QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerSpiritPower,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerLuckyValue,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);

    player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[index - 1]);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    ui->playerName->setText(gbkCodec->toUnicode((char *)player->name));

    playerAutoPlayTimer.stop();

    if(roleIconIndexVecVec.size() > 0) {
        effects_icon_head_t *effects_icon_head = nullptr;
        for(int i = 0; i < roleIconIndexVecVec[0].size(); i++) {
            if(roleIconIndexVecVec[0][i] > 0) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[0][i]);
                if(effects_icon_head->idx == player->icon) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
        }
        if(effects_icon_head != nullptr) {
            ui->playerIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
            ui->playerIconView->SetIconIndex(ui->playerToward->currentIndex() * 3 + ui->playerFootstep->value());
            if(ui->playerAutoPlayChk->isChecked()) {
                playerAutoPlayTimer.start(200);
            }
        } else {
            ui->playerIconView->ClearView();
        }
    }
    ui->playerIcon->setValue(player->icon);

    ui->playerMagicList->setValue(player->magic_list);
    ui->playerLearned->setValue(player->learned);

    ui->playerHat->setValue(player->hat);
    ui->playerClothes->setValue(player->clothes);
    ui->playerCloak->setValue(player->cloak);
    ui->playerWristlet->setValue(player->wristlet);
    ui->playerWeapon->setValue(player->weapon);
    ui->playerShoes->setValue(player->shoes);
    ui->playerAccessory1->setValue(player->accessory1);
    ui->playerAccessory2->setValue(player->accessory2);

    ui->playerAttackPower->setValue(player->attack_power);
    ui->playerDefensePower->setValue(player->defense_power);

    ui->playerHpMax->setValue(player->hp_max);
    ui->playerHp->setValue(player->hp);
    ui->playerMpMax->setValue(player->mp_max);
    ui->playerMp->setValue(player->mp);

    ui->playerBodyMovement->setValue(player->body_movement);
    ui->playerSpiritPower->setValue(player->spirit_power);
    ui->playerLuckyValue->setValue(player->lucky_value);

    connect(ui->playerName,         &QLineEdit::editingFinished,                    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerMagicList,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerLearned,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerIcon,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerHat,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerClothes,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerCloak,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerWristlet,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerWeapon,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerShoes,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerAccessory1,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerAccessory2,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerAttackPower,  QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerDefensePower, QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerHpMax,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerHp,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerMpMax,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerMp,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerBodyMovement, QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerSpiritPower,  QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerLuckyValue,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_playerData_valueChanged);
}

void MainWidget::on_playerAutoPlayChk_clicked()
{
    on_playerIdx_valueChanged(ui->playerIdx->value());
}

void MainWidget::on_playerAutoPlayTimer_timeout()
{
    ui->playerFootstep->setValue((ui->playerFootstep->value() + 1) % (ui->playerFootstep->maximum() + 1));
}

void MainWidget::on_playerIcon_valueChanged(int index)
{
    if(roleIconIndexVecVec.size() > 0) {
        effects_icon_head_t *effects_icon_head = nullptr;
        for(int i = 0; i < roleIconIndexVecVec[0].size(); i++) {
            if(roleIconIndexVecVec[0][i] > 0) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[0][i]);
                if(effects_icon_head->idx == index) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
        }
        if(effects_icon_head != nullptr) {
            ui->playerIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
            ui->playerIconView->SetIconIndex(ui->playerToward->currentIndex() * 3 + ui->playerFootstep->value());
        } else {
            ui->playerIconView->ClearView();
        }
    }
}

void MainWidget::on_playerToward_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    on_playerIcon_valueChanged(ui->playerIcon->value());
}

void MainWidget::on_playerFootstep_valueChanged(int index)
{
    Q_UNUSED(index)
    on_playerIcon_valueChanged(ui->playerIcon->value());
}

void MainWidget::on_playerData_valueChanged()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    for(int i = 0; i < playerPropSpinBoxVec.size(); i++) {
        if(sender() == playerPropSpinBoxVec[i]) {
            QSpinBox *spinBox = static_cast<QSpinBox *>(sender());
            ui->propIdx->setValue(spinBox->value());
            break;
        }
    }

    player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[ui->playerIdx->value() - 1]);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    QByteArray playerName = gbkCodec->fromUnicode(ui->playerName->text());
    memset(player->name, 0xff, 12);
    memcpy_s(player->name, 12, playerName.data(), playerName.size());
    player->name[playerName.size()] = '\0';

    player->magic_list      = ui->playerMagicList->value();
    player->learned         = ui->playerLearned->value();

    player->icon            = ui->playerIcon->value();

    player->hat             = ui->playerHat->value();
    player->clothes         = ui->playerClothes->value();
    player->cloak           = ui->playerCloak->value();
    player->wristlet        = ui->playerWristlet->value();
    player->weapon          = ui->playerWeapon->value();
    player->shoes           = ui->playerShoes->value();
    player->accessory1      = ui->playerAccessory1->value();
    player->accessory2      = ui->playerAccessory2->value();

    player->attack_power    = ui->playerAttackPower->value();
    player->defense_power   = ui->playerDefensePower->value();

    player->hp_max          = ui->playerHpMax->value();
    player->hp              = ui->playerHp->value();
    player->mp_max          = ui->playerMpMax->value();
    player->mp              = ui->playerMp->value();

    player->body_movement   = ui->playerBodyMovement->value();
    player->spirit_power    = ui->playerSpiritPower->value();
    player->lucky_value     = ui->playerLuckyValue->value();

}



void MainWidget::on_enemyIdx_valueChanged(int index)
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    disconnect(ui->enemyName,           &QLineEdit::editingFinished,                    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyMagicList,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyLearned,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyIcon,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyAttackPower,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyDefensePower,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyHpMax,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyHp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyMpMax,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyMp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyOwnMoney,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyExpReward,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyBodyMovement,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemySpiritPower,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyLuckyValue,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyIQValue,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyStealType,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyStealIndex,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyStealCount,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyDropType,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->enemyDropIndex,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    disconnect(ui->eImmunityPoisonChk,  &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eImmunityMessChk,    &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eImmunitySealChk,    &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eImmunitySleepChk,   &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eEffectivePoisonChk, &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eEffectiveMessChk,   &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eEffectiveSealChk,   &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eEffectiveSleepChk,  &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->eEffectiveRound,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);

    enemy_t *enemy = reinterpret_cast<enemy_t *>(libraryBA.data() + enemyIndexVec[index - 1]);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    ui->enemyName->setText(gbkCodec->toUnicode((char *)enemy->name));

    ui->enemyMagicList->setValue(enemy->magic_list);
    ui->enemyLearned->setValue(enemy->learned);

    enemyAutoPlayTimer.stop();

    if(roleIconIndexVecVec.size() > 2) {
        effects_icon_head_t *effects_icon_head = nullptr;
        for(int i = 0; i < roleIconIndexVecVec[2].size(); i++) {
            if(roleIconIndexVecVec[2][i] > 0) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[2][i]);
                if(effects_icon_head->idx == enemy->icon) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
        }
        if(effects_icon_head != nullptr) {
            ui->enemyIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
            ui->enemyIconView->SetIconIndex(0);
            if(ui->enemyAutoPlayChk->isChecked()) {
                enemyAutoPlayTimer.start(200);
            }
        } else {
            ui->enemyIconView->ClearView();
        }
    }
    ui->enemyIcon->setValue(enemy->icon);

    ui->enemyAttackPower->setValue(enemy->attack_power);
    ui->enemyDefensePower->setValue(enemy->defense_power);

    ui->enemyHpMax->setValue(enemy->hp_max);
    ui->enemyHp->setValue(enemy->hp);
    ui->enemyMpMax->setValue(enemy->mp_max);
    ui->enemyMp->setValue(enemy->mp);

    ui->enemyOwnMoney->setValue(enemy->own_money);
    ui->enemyExpReward->setValue(enemy->exp_reward);

    ui->enemyBodyMovement->setValue(enemy->body_movement);
    ui->enemySpiritPower->setValue(enemy->spirit_power);
    ui->enemyLuckyValue->setValue(enemy->lucky_value);
    ui->enemyIQValue->setValue(enemy->iq_value);

    ui->enemyStealType->setValue(enemy->steal_type);
    ui->enemyStealIndex->setValue(enemy->steal_index);
    ui->enemyStealCount->setValue(enemy->steal_count);
    ui->enemyDropType->setValue(enemy->drop_type);
    ui->enemyDropIndex->setValue(enemy->drop_index);

    ui->eImmunityPoisonChk->setChecked(enemy->immunity & 0x08);
    ui->eImmunityMessChk->setChecked(enemy->immunity & 0x04);
    ui->eImmunitySealChk->setChecked(enemy->immunity & 0x02);
    ui->eImmunitySleepChk->setChecked(enemy->immunity & 0x01);

    ui->eEffectivePoisonChk->setChecked(enemy->effective & 0x08);
    ui->eEffectiveMessChk->setChecked(enemy->effective & 0x04);
    ui->eEffectiveSealChk->setChecked(enemy->effective & 0x02);
    ui->eEffectiveSleepChk->setChecked(enemy->effective & 0x01);

    ui->eEffectiveRound->setValue(enemy->effective_round);

    connect(ui->enemyName,              &QLineEdit::editingFinished,                    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyMagicList,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyLearned,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyIcon,              QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyAttackPower,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyDefensePower,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyHpMax,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyHp,                QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyMpMax,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyMp,                QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyOwnMoney,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyExpReward,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyBodyMovement,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemySpiritPower,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyLuckyValue,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyIQValue,           QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyStealType,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyStealIndex,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyStealCount,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyDropType,          QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->enemyDropIndex,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
    connect(ui->eImmunityPoisonChk,     &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eImmunityMessChk,       &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eImmunitySealChk,       &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eImmunitySleepChk,      &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eEffectivePoisonChk,    &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eEffectiveMessChk,      &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eEffectiveSealChk,      &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eEffectiveSleepChk,     &QCheckBox::clicked,                            this, &MainWidget::on_propData_valueChanged);
    connect(ui->eEffectiveRound,        QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_enemyData_valueChanged);
}

void MainWidget::on_enemyAutoPlayChk_clicked()
{
    on_enemyIdx_valueChanged(ui->enemyIdx->value());
}

void MainWidget::on_enemyAutoPlayTimer_timeout()
{
    static int currIndex = 0;
    int iconCount = ui->enemyIconView->GetIconCount();
    enemyAutoPlayTimer.stop();
    if(iconCount > 1) {
        ui->enemyIconView->SetIconIndex(currIndex);
        currIndex = (currIndex + 1) % iconCount;
        if(ui->enemyAutoPlayChk->isChecked()) {
            if(currIndex == 0) {
                enemyAutoPlayTimer.start(1000);
            } else {
                enemyAutoPlayTimer.start(200);
            }
        }
    }
}

void MainWidget::on_enemyIcon_valueChanged(int index)
{
    if(roleIconIndexVecVec.size() > 2) {
        effects_icon_head_t *effects_icon_head = nullptr;
        for(int i = 0; i < roleIconIndexVecVec[2].size(); i++) {
            if(roleIconIndexVecVec[2][i] > 0) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[2][i]);
                if(effects_icon_head->idx == index) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
        }
        if(effects_icon_head != nullptr) {
            ui->enemyIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
            ui->enemyIconView->SetIconIndex(0);
        } else {
            ui->enemyIconView->ClearView();
        }
    }
}

void MainWidget::on_enemyData_valueChanged()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    for(int i = 0; i < enemyPropSpinBoxVec.size(); i++) {
        QSpinBox *spinBox = static_cast<QSpinBox *>(sender());
        if(spinBox == ui->enemyStealType || spinBox == ui->enemyStealIndex || spinBox == ui->enemyStealCount) {
            if(ui->enemyStealType->value() > 0) {
                ui->propType->setCurrentIndex(ui->enemyStealType->value() - 1);
                ui->enemyStealIndex->setRange(0, propIndexVecVec[ui->enemyStealType->value() - 1].size());
                ui->propIdx->setValue(ui->enemyStealIndex->value());
            }
            break;
        }
        else if(spinBox == ui->enemyDropType || spinBox == ui->enemyDropIndex) {
            if(ui->enemyDropType->value() > 0) {
                ui->propType->setCurrentIndex(ui->enemyDropType->value() - 1);
                ui->enemyDropIndex->setRange(0, propIndexVecVec[ui->enemyDropType->value() - 1].size());
                ui->propIdx->setValue(ui->enemyDropIndex->value());
            }
            break;
        }
    }

    enemy_t *enemy = reinterpret_cast<enemy_t *>(libraryBA.data() + enemyIndexVec[ui->enemyIdx->value() - 1]);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    QByteArray enemyName = gbkCodec->fromUnicode(ui->enemyName->text());
    memset(enemy->name, 0xff, 12);
    memcpy_s(enemy->name, 12, enemyName.data(), enemyName.size());
    enemy->name[enemyName.size()] = '\0';

    enemy->magic_list       = ui->enemyMagicList->value();
    enemy->learned          = ui->enemyLearned->value();

    enemy->icon             = ui->enemyIcon->value();

    enemy->attack_power     = ui->enemyDefensePower->value();
    enemy->defense_power    = ui->enemyDefensePower->value();

    enemy->hp_max           = ui->enemyHpMax->value();
    enemy->hp               = ui->enemyHp->value();
    enemy->mp_max           = ui->enemyMpMax->value();
    enemy->mp               = ui->enemyMp->value();

    enemy->own_money        = ui->enemyOwnMoney->value();
    enemy->exp_reward       = ui->enemyExpReward->value();

    enemy->body_movement    = ui->enemyBodyMovement->value();
    enemy->spirit_power     = ui->enemySpiritPower->value();
    enemy->lucky_value      = ui->enemyLuckyValue->value();
    enemy->iq_value         = ui->enemyIQValue->value();

    enemy->steal_type       = ui->enemyStealType->value();
    enemy->steal_index      = ui->enemyStealIndex->value();
    enemy->steal_count      = ui->enemyStealCount->value();
    enemy->drop_type        = ui->enemyDropType->value();
    enemy->drop_index       = ui->enemyDropIndex->value();

    if(ui->eImmunityPoisonChk->isChecked()) {
        enemy->immunity |= 0x08;
    } else {
        enemy->immunity &= ~0x08;
    }
    if(ui->eImmunityMessChk->isChecked()) {
        enemy->immunity |= 0x04;
    } else {
        enemy->immunity &= ~0x04;
    }
    if(ui->eImmunitySealChk->isChecked()) {
        enemy->immunity |= 0x02;
    } else {
        enemy->immunity &= ~0x02;
    }
    if(ui->eImmunitySleepChk->isChecked()) {
        enemy->immunity |= 0x01;
    } else {
        enemy->immunity &= ~0x01;
    }

    if(ui->eEffectivePoisonChk->isChecked()) {
        enemy->effective |= 0x08;
    } else {
        enemy->effective &= ~0x08;
    }
    if(ui->eEffectiveMessChk->isChecked()) {
        enemy->effective |= 0x04;
    } else {
        enemy->effective &= ~0x04;
    }
    if(ui->eEffectiveSealChk->isChecked()) {
        enemy->effective |= 0x02;
    } else {
        enemy->effective &= ~0x02;
    }
    if(ui->eEffectiveSleepChk->isChecked()) {
        enemy->effective |= 0x01;
    } else {
        enemy->effective &= ~0x01;
    }

    enemy->effective_round = ui->eEffectiveRound->value();
}



void MainWidget::on_propType_currentIndexChanged(int index)
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
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

    if(index > (propIconIndexVecVec.size() - 1) || propIndexVecVec[index].isEmpty()) {
        ui->propIdx->setRange(0, 0);
        ui->propName->clear();
        ui->propDescription->clear();
        return;
    }

    ui->propIdx->setRange(1, propIndexVecVec[index].size());

    prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[index][ui->propIdx->value() - 1]);

    // qDebug() << getResOffset(Res_GRS, index + 1, ui->propIdx->value()) << propIndexVecVec[index][ui->propIdx->value() - 1];

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    ui->propName->setText(gbkCodec->toUnicode((char *)prop->name));

    if(propIconIndexVecVec.size() > 13) {
        effects_icon_head_t *effects_icon_head = nullptr;
        for(int i = 0; i < propIconIndexVecVec[index].size(); i++) {
            if(propIconIndexVecVec[index][i] > 0) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + propIconIndexVecVec[index][i]);
                if(effects_icon_head->idx == prop->icon) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
        }
        if(effects_icon_head != nullptr) {
            ui->propIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
        } else {
            ui->propIconView->ClearView();
        }
    }
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

void MainWidget::on_propIcon_valueChanged(int index)
{
    if(propIconIndexVecVec.size() > 13) {
        effects_icon_head_t *effects_icon_head = nullptr;
        for(int i = 0; i < propIconIndexVecVec[ui->propType->currentIndex()].size(); i++) {
            if(propIconIndexVecVec[ui->propType->currentIndex()][i] > 0) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + propIconIndexVecVec[ui->propType->currentIndex()][i]);
                if(effects_icon_head->idx == index) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
        }
        if(effects_icon_head != nullptr) {
            ui->propIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
        } else {
            ui->propIconView->ClearView();
        }
    }
}

void MainWidget::on_propData_valueChanged()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    if(propIndexVecVec[ui->propType->currentIndex()].isEmpty()) {
        return;
    }

    prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[ui->propType->currentIndex()][ui->propIdx->value() - 1]);

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

void MainWidget::on_magicType_currentIndexChanged(int index)
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    disconnect(ui->magicName,           &QLineEdit::editingFinished,                    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicHp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicMp,             QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicAttackPower,    QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicDefensePower,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicBodyMovement,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicMpCost,         QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicPoisonChk,      &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicMessChk,        &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicSealChk,        &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicSleepChk,       &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicGroupEffectChk, &QCheckBox::clicked,                            this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicEffectiveRound, QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_magicData_valueChanged);
    disconnect(ui->magicDescription,    &QTextEdit::textChanged,                        this, &MainWidget::on_magicData_valueChanged);

    if(magicIndexVecVec[index].isEmpty()) {
        ui->magicIdx->setRange(0, 0);
        ui->magicName->clear();
        ui->magicDescription->clear();
        return;
    }

    ui->magicIdx->setRange(1, magicIndexVecVec[index].size());

    magic_t *magic = reinterpret_cast<magic_t *>(libraryBA.data() + magicIndexVecVec[index][ui->magicIdx->value() - 1]);

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
    ui->magicDescription->setText(gbkCodec->toUnicode((char *)magic->description));

    connect(ui->magicName,           &QLineEdit::editingFinished,                   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicHp,             QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicMp,             QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicAttackPower,    QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicDefensePower,   QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicBodyMovement,   QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
    connect(ui->magicMpCost,         QOverload<int>::of(&QSpinBox::valueChanged),   this, &MainWidget::on_magicData_valueChanged);
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
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    if(magicIndexVecVec[ui->magicType->currentIndex()].isEmpty()) {
        return;
    }

    magic_t *magic = reinterpret_cast<magic_t *>(libraryBA.data() + magicIndexVecVec[ui->magicType->currentIndex()][ui->magicIdx->value() - 1]);

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

        ui->effectiveRound->setValue(ui->magicEffectiveRound->value() & 0x7f);

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

        ui->effectiveRound->setValue(ui->magicEffectiveRound->value() & 0x7f);

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
