#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "gam_type.h"

#include "iconvieweditdlg.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    roleBoxMenu = new QMenu(this);
    roleSaveData = new QAction(tr("Save Data"), this);
    roleBoxMenu->addAction(roleSaveData);
    connect(roleSaveData, &QAction::triggered, this, &MainWidget::on_roleSaveData);

    adjustSize();
    resize(0, 0);

    _buildDateTimeStr = QLocale(QLocale::English).toDateTime(QString(__DATE__ " " __TIME__).replace("  ", " 0"), "MMM dd yyyy hh:mm:ss").toString("yyyyMMdd_hhmm");

    setWindowTitle(tr("Npc_SceneObj_") + _buildDateTimeStr);

    connect(ui->loadGamBtn,         &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->saveGamBtn,         &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);
    connect(ui->editRoleIcon,       &QPushButton::clicked,          this, &MainWidget::on_pushButton_clicked);

    connect(ui->roleName,           &QLineEdit::textChanged,        this, &MainWidget::on_lineEdit_textChanged);

    connect(&roleAutoPlayTimer,     &QTimer::timeout,               this, &MainWidget::on_roleAutoPlayTimer_timeout);

    ui->roleFootstep->installEventFilter(this);
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
    if(obj == ui->roleFootstep) {
        QSpinBox *spinBox = static_cast<QSpinBox *>(obj);
        if(event->type() == QEvent::Wheel) {
            if(spinBox == ui->roleFootstep) {
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
    return QWidget::eventFilter(obj, event);
}

void MainWidget::on_pushButton_clicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());

    if(btn == ui->loadGamBtn) {

        QSettings settings("QtProject", "Npc_SceneObj_Modifier");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), settings.value("last_dir").toString(), "GAM(*.gam)");
        if(!fileName.isEmpty() && QFile(fileName).exists()) {
            settings.setValue("last_dir", QFileInfo(fileName).absoluteDir().path());
            settings.sync();
        }
        else {
            return;
        }

        setWindowTitle(tr("Npc_SceneObj_") + _buildDateTimeStr);

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

        QFile gamFile(fileName);
        if(!gamFile.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam File Open Failed!"));
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

            int block   = libraryBA.at(idx2++) & 0xff;
            int offsetL = libraryBA.at(idx2++) & 0xff;
            int offsetH = libraryBA.at(idx2++) & 0xff;

            offsetMap.insert(resType << 16 | subType << 8 | index, block * 0x4000 | offsetH << 8 | offsetL);
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
                }
            }
        }

        for(int subType = 1; subType <= 5; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                subTypeVec.append(getResOffset(Res_MRS, subType, resIndex));
            }
            while(subTypeVec.size() > 0 && subTypeVec.last() == 0) {
                subTypeVec.removeLast();
            }
            magicIndexVecVec.append(subTypeVec);
        }

        for(int subType = 1; subType <= 14; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                subTypeVec.append(getResOffset(Res_GRS, subType, resIndex));
            }
            while(subTypeVec.size() > 0 && subTypeVec.last() == 0) {
                subTypeVec.removeLast();
            }
            propIndexVecVec.append(subTypeVec);
        }

        for(int subType = 1; subType <= 4; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                subTypeVec.append(getResOffset(Res_ACP, subType, resIndex));
            }
            while(subTypeVec.size() > 0 && subTypeVec.last() == 0) {
                subTypeVec.removeLast();
            }
            roleIconIndexVecVec.append(subTypeVec);
        }

        for(int subType = 1; subType <= 14; subType++) {
            QVector<int> subTypeVec;
            for(int resIndex = 1; resIndex < 255; resIndex++) {
                subTypeVec.append(getResOffset(Res_GDP, subType, resIndex));
            }
            while(subTypeVec.size() > 0 && subTypeVec.last() == 0) {
                subTypeVec.removeLast();
            }
            propIconIndexVecVec.append(subTypeVec);
        }

        on_roleType_currentIndexChanged(ui->roleType->currentIndex());

        setWindowTitle(QFileInfo(fileName).fileName());

    }
    else if(btn == ui->saveGamBtn){

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam Data Not Loaded!"));
            return;
        }

        QSettings settings("QtProject", "BBK_GAM_Modifier");
        QFile gamFile(QFileDialog::getSaveFileName(this, tr("Save File"), settings.value("last_dir").toString(), "GAM(*.gam)"));
        if(!gamFile.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam File Save Failed!"));
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
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->clothes > 0 && propIndexVecVec[1].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[1][player->clothes - 1]);
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->cloak > 0 && propIndexVecVec[3].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[3][player->cloak - 1]);
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->wristlet > 0 && propIndexVecVec[4].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[4][player->wristlet - 1]);
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->weapon > 0 && propIndexVecVec[6].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[6][player->weapon - 1]);
                player->effective |= (prop->effective_multiplayer_magic.effective & 0x1f);
            }

            if(player->shoes > 0 && propIndexVecVec[2].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[2][player->shoes - 1]);
                player->immunity |= (prop->effective_multiplayer_magic.effective & 0x0f);
            }

            if(player->accessory1 > 0 && propIndexVecVec[5].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[5][player->accessory1 - 1]);
                player->hp_each_turn += prop->mp_hp_hp16.mp_hp.hpMax_hp.hp;
                player->mp_each_turn += prop->mp_hp_hp16.mp_hp.mpMax_mp.mp;
            }

            if(player->accessory2 > 0 && propIndexVecVec[5].size() > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[5][player->accessory2 - 1]);
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
    else if(btn == ui->editRoleIcon) {
        if(roleIconIndexVecVec.size() > 3) {
            int roleIndex = ui->roleType->currentIndex() == 0 ? 1 : 3;
            effects_icon_head_t *effects_icon_head = nullptr;
            for(int i = 0; i < roleIconIndexVecVec[roleIndex].size(); i++) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[roleIndex][i]);
                if(effects_icon_head->idx == ui->roleIcon->value()) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
            if(effects_icon_head != nullptr) {
                IconViewEditDlg iconViewEditDlg(this);
                iconViewEditDlg.UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                if(iconViewEditDlg.exec() == IconViewEditDlg::Accepted) {
                    iconViewEditDlg.WriteBack((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    on_roleIcon_valueChanged(ui->roleIcon->value());
                }
            }
        }
    }
}

void MainWidget::on_lineEdit_textChanged()
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(sender());
    int len8Bit = lineEdit->text().toLocal8Bit().length();
    if(len8Bit > 10) {
        lineEdit->setCursorPosition(lineEdit->text().size());
        do {
            lineEdit->backspace();
            len8Bit = lineEdit->text().toLocal8Bit().length();
        } while(len8Bit > 10);
    }
}



void MainWidget::on_roleType_currentIndexChanged(int index)
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    if(index < 1 && npcIndexVec.isEmpty()) {
        ui->roleIdx->setRange(0, 0);
        return;
    }
    else if(index < 2 && sceneObjIndexVec.isEmpty()) {
        ui->roleIdx->setRange(0, 0);
        return;
    }

    disconnect(ui->roleName,        &QLineEdit::editingFinished,                    this, &MainWidget::on_roleData_valueChanged);
    disconnect(ui->roleAction,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);
    disconnect(ui->rolePositionX,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);
    disconnect(ui->rolePositionY,   QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);
    disconnect(ui->roleDelay,       QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    if(index < 1) {
        ui->roleIdx->setRange(1, npcIndexVec.size());

        ui->roleToward->setEnabled(true);
        ui->rolePositionX->setEnabled(true);
        ui->rolePositionY->setEnabled(true);

        npc_t *npc = reinterpret_cast<npc_t *>(libraryBA.data() + npcIndexVec[ui->roleIdx->value() - 1]);

        ui->roleName->setText(gbkCodec->toUnicode((char *)npc->name));

        roleAutoPlayTimer.stop();

        if(roleIconIndexVecVec.size() > 3) {
            effects_icon_head_t *effects_icon_head = nullptr;
            for(int i = 0; i < roleIconIndexVecVec[1].size(); i++) {
                if(roleIconIndexVecVec[1][i] > 0) {
                    effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[1][i]);
                    if(effects_icon_head->idx == npc->icon) {
                        break;
                    } else {
                        effects_icon_head = nullptr;
                    }
                }
            }
            if(effects_icon_head != nullptr) {
                ui->roleIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                ui->roleIconView->SetIconIndex(ui->roleToward->currentIndex() * 3 + ui->roleFootstep->value());
                if(ui->roleAutoPlayChk->isChecked()) {
                    roleAutoPlayTimer.start(200);
                }
            } else {
                ui->roleIconView->ClearView();
            }
        }
        ui->roleIcon->setValue(npc->icon);

        ui->roleToward->setCurrentIndex(npc->toward - 1);
        ui->roleFootstep->setValue(npc->footstep);

        ui->roleAction->setValue(npc->action);
        ui->rolePositionX->setValue(npc->pos_x);
        ui->rolePositionY->setValue(npc->pos_y);
        ui->roleDelay->setValue(npc->delay);
    }
    else if(index < 2) {
        ui->roleIdx->setRange(1, sceneObjIndexVec.size());

        ui->roleToward->setEnabled(false);
        ui->rolePositionX->setEnabled(false);
        ui->rolePositionY->setEnabled(false);

        sceneobj_t *sceneObj = reinterpret_cast<sceneobj_t *>(libraryBA.data() + sceneObjIndexVec[ui->roleIdx->value() - 1]);

        ui->roleName->setText(gbkCodec->toUnicode((char *)sceneObj->name));

        roleAutoPlayTimer.stop();

        if(roleIconIndexVecVec.size() > 3) {
            effects_icon_head_t *effects_icon_head = nullptr;
            for(int i = 0; i < roleIconIndexVecVec[3].size(); i++) {
                if(roleIconIndexVecVec[3][i] > 0) {
                    effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[3][i]);
                    if(effects_icon_head->idx == sceneObj->icon) {
                        break;
                    } else {
                        effects_icon_head = nullptr;
                    }
                }
            }
            if(effects_icon_head != nullptr) {
                ui->roleIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                ui->roleIconView->SetIconIndex(ui->roleToward->currentIndex() * 3 + ui->roleFootstep->value());
                if(ui->roleAutoPlayChk->isChecked()) {
                    roleAutoPlayTimer.start(200);
                }
            }
        }
        ui->roleIcon->setValue(sceneObj->icon);

        ui->roleFootstep->setValue(sceneObj->footstep);

        ui->roleAction->setValue(sceneObj->action);
        ui->rolePositionX->setValue(0);
        ui->rolePositionY->setValue(0);
        ui->roleDelay->setValue(sceneObj->delay);
    }

    connect(ui->roleName,       &QLineEdit::editingFinished,                    this, &MainWidget::on_roleData_valueChanged);
    connect(ui->roleAction,     QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);
    connect(ui->rolePositionX,  QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);
    connect(ui->rolePositionY,  QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);
    connect(ui->roleDelay,      QOverload<int>::of(&QSpinBox::valueChanged),    this, &MainWidget::on_roleData_valueChanged);
}

void MainWidget::on_roleIdx_valueChanged(int index)
{
    Q_UNUSED(index)
    on_roleType_currentIndexChanged(ui->roleType->currentIndex());
}

void MainWidget::on_roleAutoPlayChk_clicked()
{
    on_roleType_currentIndexChanged(ui->roleType->currentIndex());
}

void MainWidget::on_roleAutoPlayTimer_timeout()
{
    if(ui->roleType->currentIndex() < 1) {
        ui->roleFootstep->setValue((ui->roleFootstep->value() + 1) % (ui->roleFootstep->maximum() + 1));
    }
    else if(ui->roleType->currentIndex() < 2) {
        static int currIndex = 0;
        int iconCount = ui->roleIconView->GetIconCount();
        // roleAutoPlayTimer.stop();
        if(iconCount > 1) {
            ui->roleIconView->SetIconIndex(currIndex);
            currIndex = (currIndex + 1) % iconCount;
            // if(ui->roleAutoPlayChk->isChecked()) {
            //     if(currIndex == 0) {
            //         roleAutoPlayTimer.start(1000);
            //     } else {
            //         roleAutoPlayTimer.start(200);
            //     }
            // }
        }
    }
}

void MainWidget::on_roleIcon_valueChanged(int index)
{
    if(roleIconIndexVecVec.size() > 3) {
        int roleIndex = 1;
        if(ui->roleType->currentIndex() < 1) {
            roleIndex = 1;
        }
        else if(ui->roleType->currentIndex() < 2) {
            roleIndex = 3;
        }
        effects_icon_head_t *effects_icon_head = nullptr;
        for(int i = 0; i < roleIconIndexVecVec[roleIndex].size(); i++) {
            if(roleIconIndexVecVec[roleIndex][i] > 0) {
                effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + roleIconIndexVecVec[roleIndex][i]);
                if(effects_icon_head->idx == index) {
                    break;
                } else {
                    effects_icon_head = nullptr;
                }
            }
        }
        if(effects_icon_head != nullptr) {
            ui->roleIconView->UpdateView((uint8_t *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
            if(ui->roleType->currentIndex() < 1) {
                ui->roleIconView->SetIconIndex(ui->roleToward->currentIndex() * 3 + ui->roleFootstep->value());
            }
            else if(ui->roleType->currentIndex() < 2) {
                ui->roleIconView->SetIconIndex(ui->roleFootstep->value());
            }
        } else {
            ui->roleIconView->ClearView();
        }
    }
}

void MainWidget::on_roleToward_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    on_roleIcon_valueChanged(ui->roleIcon->value());
}

void MainWidget::on_roleFootstep_valueChanged(int index)
{
    Q_UNUSED(index)
    on_roleIcon_valueChanged(ui->roleIcon->value());
}

void MainWidget::on_roleData_valueChanged()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    if(npcIndexVec[ui->roleIdx->value() - 1] == 0) {
        return;
    }

    if(sceneObjIndexVec[ui->roleIdx->value() - 1] == 0) {
        return;
    }

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    if(ui->roleType->currentIndex() < 1) {
        npc_t *npc = reinterpret_cast<npc_t *>(libraryBA.data() + npcIndexVec[ui->roleIdx->value() - 1]);

        QByteArray npcName = gbkCodec->fromUnicode(ui->roleName->text());
        memset(npc->name, 0xff, 12);
        memcpy_s(npc->name, 12, npcName.data(), npcName.size());
        npc->name[npcName.size()] = '\0';

        npc->action     = ui->roleAction->value();
        npc->pos_x      = ui->rolePositionX->value();
        npc->pos_y      = ui->rolePositionY->value();
        npc->delay      = ui->roleDelay->value();
    }
    else if(ui->roleType->currentIndex() < 2) {
        sceneobj_t *sceneObj = reinterpret_cast<sceneobj_t *>(libraryBA.data() + npcIndexVec[ui->roleIdx->value() - 1]);

        QByteArray sceneObjName = gbkCodec->fromUnicode(ui->roleName->text());
        memset(sceneObj->name, 0xff, 12);
        memcpy_s(sceneObj->name, 12, sceneObjName.data(), sceneObjName.size());
        sceneObj->name[sceneObjName.size()] = '\0';

        sceneObj->action    = ui->roleAction->value();
        sceneObj->delay     = ui->roleDelay->value();
    }
}

void MainWidget::on_roleSaveData()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    if(npcIndexVec[ui->roleIdx->value() - 1] == 0) {
        return;
    }

    if(sceneObjIndexVec[ui->roleIdx->value() - 1] == 0) {
        return;
    }

    QSettings settings("QtProject", "Npc_SceneObj_Modifier");
    QFile file(QFileDialog::getSaveFileName(this, tr("Save File"), settings.value("last_ars_dir").toString(), "ARS(*.ars)"));
    if(!file.open(QFile::WriteOnly)) {
        return;
    }

    settings.setValue("last_ars_dir", QFileInfo(file).absoluteDir().absolutePath());
    settings.sync();

    if(ui->roleType->currentIndex() < 1) {

        npc_t *npc = reinterpret_cast<npc_t *>(libraryBA.data() + npcIndexVec[ui->roleIdx->value() - 1]);
        QDataStream dataStream(&file);
        dataStream.writeRawData((const char *)npc, sizeof(npc_t));
    }
    else if(ui->roleType->currentIndex() < 2) {

        sceneobj_t *sceneobj = reinterpret_cast<sceneobj_t *>(libraryBA.data() + sceneObjIndexVec[ui->roleIdx->value() - 1]);
        QDataStream dataStream(&file);
        dataStream.writeRawData((const char *)sceneobj, sizeof(sceneobj_t));
    }

    file.close();
}

void MainWidget::on_roleBox_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    roleBoxMenu->exec(QCursor::pos());
}
