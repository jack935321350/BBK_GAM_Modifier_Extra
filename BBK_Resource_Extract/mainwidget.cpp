#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "gam_type.h"
#include "gut_engine.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    _buildDateTimeStr = QLocale(QLocale::English).toDateTime(QString(__DATE__ " " __TIME__).replace("  ", " 0"), "MMM dd yyyy hh:mm:ss").toString("yyyyMMdd_hhmm");

    setWindowTitle(tr("BBK_Resource_Extract_") + _buildDateTimeStr);

    connect(&stateTimer, SIGNAL(timeout()), this, SLOT(on_stateTimer_timeout()));

    ui->rcType->setItemData(Res_GUT - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_MAP - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_ARS - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_MRS - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_SRS - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_GRS - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_TIL - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_ACP - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_GDP - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_GGJ - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_PIC - 1, QVariant(-1), Qt::UserRole - 1);
    ui->rcType->setItemData(Res_MLR - 1, QVariant(-1), Qt::UserRole - 1);

    QSettings settings("QtProject", "BBK_Resource_Extract");
    ui->saveDir->setText(settings.value("last_res_dir").toString());
}

MainWidget::~MainWidget()
{    
    delete ui;
}

void MainWidget::Save_GUT()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression gutRegExp("/GUT$");
    if(!gutRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/GUT";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("GUT")) {
        if(!saveDir.mkdir("GUT")) {
            return;
        }
    }

    for(int subType = 0; subType < 256; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_GUT, subType, resIndex);
            if(offset > 0) {
                drama_head_t *drama_head = reinterpret_cast<drama_head_t *>(libraryBA.data() + offset);
                QFile file(saveDirPath + '/' + QString("%1-%2").arg(drama_head->chapter, 3, 10, QLatin1Char('0')).arg(drama_head->section, 3, 10, QLatin1Char('0')) + ".gut");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    if(calc_drama_size(drama_head) > 0) {
                        stream.writeRawData((const char *)drama_head, calc_drama_size(drama_head));
                    }
                    file.close();
                }
                if(calc_drama_size(drama_head) > 0) {
                    GutEngine gutEngine(this, (uint8_t *)drama_head, calc_drama_size(drama_head));
                    if(gutEngine.Process()) {
                        QFile file(saveDirPath + '/' + QString("%1-%2").arg(drama_head->chapter, 3, 10, QLatin1Char('0')).arg(drama_head->section, 3, 10, QLatin1Char('0')) + ".txt");
                        if(file.open(QFile::WriteOnly)) {
                            QByteArray ba = gutEngine.GetOutBA();
                            QDataStream stream(&file);
                            stream.writeRawData((const char *)ba.data(), ba.size());
                            file.close();
                        }
                    }
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_MAP()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression mapRegExp("/MAP$");
    if(!mapRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/MAP";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("MAP")) {
        if(!saveDir.mkdir("MAP")) {
            return;
        }
    }

    for(int subType = 0; subType < 256; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_MAP, subType, resIndex);
            if(offset > 0) {
                map_head_t *map_head = reinterpret_cast<map_head_t *>(libraryBA.data() + offset);
                QFile file(saveDirPath + '/' + QString("%1-%2").arg(map_head->type, 3, 10, QLatin1Char('0')).arg(map_head->idx, 3, 10, QLatin1Char('0')) + '_' + GetName(map_head->name) + ".map");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)map_head, calc_map_size(map_head));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_ARS()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression arsRegExp("/ARS$");
    if(!arsRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/ARS";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("ARS")) {
        if(!saveDir.mkdir("ARS")) {
            return;
        }
    }

    for(int subType = 1; subType <= 4; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_ARS, subType, resIndex);
            if(offset > 0) {
                if(subType == 1) {
                    player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + offset);
                    QFile file(saveDirPath + '/' + QString("%1-%2").arg(player->type).arg(player->idx, 3, 10, QLatin1Char('0')) + '_' + GetName(player->name) + ".ars");
                    if(file.open(QFile::WriteOnly)) {
                        QDataStream stream(&file);
                        stream.writeRawData((const char *)player, sizeof(player_t));
                        file.close();
                    }
                }
                else if(subType == 2) {
                    npc_t *npc = reinterpret_cast<npc_t *>(libraryBA.data() + offset);
                    QFile file(saveDirPath + '/' + QString("%1-%2").arg(npc->type).arg(npc->idx, 3, 10, QLatin1Char('0')) + '_' + GetName(npc->name) + ".ars");
                    if(file.open(QFile::WriteOnly)) {
                        QDataStream stream(&file);
                        stream.writeRawData((const char *)npc, sizeof(npc_t));
                        file.close();
                    }
                }
                else if(subType == 3) {
                    enemy_t *enemy = reinterpret_cast<enemy_t *>(libraryBA.data() + offset);
                    QFile file(saveDirPath + '/' + QString("%1-%2").arg(enemy->type).arg(enemy->idx, 3, 10, QLatin1Char('0')) + '_' + GetName(enemy->name) + ".ars");
                    if(file.open(QFile::WriteOnly)) {
                        QDataStream stream(&file);
                        stream.writeRawData((const char *)enemy, sizeof(enemy_t));
                        file.close();
                    }
                }
                else if(subType == 4) {
                    sceneobj_t *sceneobj = reinterpret_cast<sceneobj_t *>(libraryBA.data() + offset);
                    QFile file(saveDirPath + '/' + QString("%1-%2").arg(sceneobj->type).arg(sceneobj->idx, 3, 10, QLatin1Char('0')) + '_' + GetName(sceneobj->name) + ".ars");
                    if(file.open(QFile::WriteOnly)) {
                        QDataStream stream(&file);
                        stream.writeRawData((const char *)sceneobj, sizeof(sceneobj_t));
                        file.close();
                    }
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_MRS()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression mrsRegExp("/MRS$");
    if(!mrsRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/MRS";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("MRS")) {
        if(!saveDir.mkdir("MRS")) {
            return;
        }
    }

    for(int subType = 1; subType <= 5; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_MRS, subType, resIndex);
            if(offset > 0) {
                magic_t *magic = reinterpret_cast<magic_t *>(libraryBA.data() + offset);
                QFile file(saveDirPath + '/' + QString("%1-%2").arg(magic->type).arg(magic->idx, 3, 10, QLatin1Char('0')) + '_' + GetName(magic->name) + ".mrs");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)magic, sizeof(magic_t));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_SRS()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression srsRegExp("/SRS$");
    if(!srsRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/SRS";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("SRS")) {
        if(!saveDir.mkdir("SRS")) {
            return;
        }
    }

    for(int subType = 1; subType <= 2; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_SRS, subType, resIndex);
            if(offset > 0) {
                effects_head_t *effects_head = reinterpret_cast<effects_head_t *>(libraryBA.data() + offset);
                QFile file(saveDirPath + '/' + QString("%1-%2").arg(effects_head->type).arg(effects_head->idx, 3, 10, QLatin1Char('0')) + ".srs");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)effects_head, calc_effects_size(effects_head));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_GRS()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression grsRegExp("/GRS$");
    if(!grsRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/GRS";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("GRS")) {
        if(!saveDir.mkdir("GRS")) {
            return;
        }
    }

    for(int subType = 1; subType <= 14; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_GRS, subType, resIndex);
            if(offset > 0) {
                prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + offset);
                QFile file(saveDirPath + '/' + QString("%1-%2").arg(prop->type, 2, 10, QLatin1Char('0')).arg(prop->idx, 3, 10, QLatin1Char('0')) + '_' + GetName(prop->name) + ".grs");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)prop, sizeof(prop_t));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_TIL()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression tilRegExp("/TIL$");
    if(!tilRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/TIL";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("TIL")) {
        if(!saveDir.mkdir("TIL")) {
            return;
        }
    }

    for(int resIndex = 0; resIndex < 256; resIndex++) {
        int offset = getResOffset(Res_TIL, 1, resIndex);
        if(offset > 0) {
            effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + offset);

            QImage image(20 * effects_icon_head->width, (effects_icon_head->count / 20 + 1) * effects_icon_head->height, QImage::Format_ARGB32);

            QPainter painter(&image);

            painter.setPen(Qt::NoPen);

            if(effects_icon_head->transparency < 2) {
                painter.setBrush(Qt::white);
            }
            else if(effects_icon_head->transparency < 3) {
                painter.setBrush(Qt::red);
            }

            painter.drawRect(image.rect());

            for(uint8_t idx = 0; idx < effects_icon_head->count; idx++) {
                uint8_t *dataPtr = reinterpret_cast<uint8_t *>(effects_icon_head) + sizeof(effects_icon_head_t) +
                    effects_icon_head->width * effects_icon_head->height * effects_icon_head->transparency / 8 * idx;
                int sx = (idx % 20) * effects_icon_head->width;
                int sy = (idx / 20) * effects_icon_head->height;
                if(effects_icon_head->transparency < 2) {
                    for(int y = 0; y < effects_icon_head->height; y++) {
                        for(int i = 0; i < (effects_icon_head->width / 8); i++) {
                            uint8_t data = dataPtr[y * effects_icon_head->width / 8 + i];
                            for(int j = 0; j < 8; j++) {
                                if(data & (1 << (7 - j))) {
                                    painter.setPen(Qt::black);
                                } else {
                                    painter.setPen(Qt::white);
                                }
                                painter.drawPoint(sx + i * 8 + j, sy + y);
                            }
                        }
                    }
                }
                else if(effects_icon_head->transparency < 3) {
                    for(int y = 0; y < effects_icon_head->height; y++) {
                        for(int i = 0; i < (effects_icon_head->width / 4); i++) {
                            uint8_t data = dataPtr[y * effects_icon_head->width / 4 + i];
                            for(int j = 0; j < 4; j++) {
                                if((data & (1 << ((7 - j * 2)))) == 0) {
                                    if(data & (1 << ((6 - j * 2)))) {
                                        painter.setPen(Qt::black);
                                    } else {
                                        painter.setPen(Qt::white);
                                    }
                                    painter.drawPoint(sx + i * 4 + j, sy + y);
                                }
                            }
                        }
                    }
                }
            }

            painter.end();
            image.save(saveDirPath + '/' + QString("%1").arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".bmp");

            QFile file(saveDirPath + '/' + QString("%1").arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".til");
            if(file.open(QFile::WriteOnly)) {
                QDataStream stream(&file);
                stream.writeRawData((const char *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                file.close();
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_ACP()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression acpRegExp("/ACP$");
    if(!acpRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/ACP";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("ACP")) {
        if(!saveDir.mkdir("ACP")) {
            return;
        }
    }

    for(int subType = 1; subType <= 4; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_ACP, subType, resIndex);
            if(offset > 0) {
                effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + offset);

                int width = effects_icon_head->width;
                if(width % 8 > 0) {
                    width += (8 - width % 8);
                }

                QImage image(width * effects_icon_head->count, effects_icon_head->height, QImage::Format_ARGB32);

                QPainter painter(&image);

                painter.setPen(Qt::NoPen);

                if(effects_icon_head->transparency < 2) {
                    painter.setBrush(Qt::white);
                }
                else if(effects_icon_head->transparency < 3) {
                    painter.setBrush(Qt::red);
                }

                painter.drawRect(image.rect());

                for(uint8_t idx = 0; idx < effects_icon_head->count; idx++) {
                    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(effects_icon_head) + sizeof(effects_icon_head_t) +
                        width * effects_icon_head->height * effects_icon_head->transparency / 8 * idx;
                    if(effects_icon_head->transparency < 2) {
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int i = 0; i < (width / 8); i++) {
                                uint8_t data = dataPtr[y * width / 8 + i];
                                for(int j = 0; j < 8; j++) {
                                    if(data & (1 << (7 - j))) {
                                        painter.setPen(Qt::black);
                                    } else {
                                        painter.setPen(Qt::white);
                                    }
                                    painter.drawPoint(width * idx + i * 8 + j, y);
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
                                            painter.setPen(Qt::black);
                                        } else {
                                            painter.setPen(Qt::white);
                                        }
                                        painter.drawPoint(width * idx + i * 4 + j, y);
                                    }
                                }
                            }
                        }
                    }
                    if(effects_icon_head->width < width) {
                        if(effects_icon_head->transparency < 2) {
                            painter.setPen(Qt::white);
                        }
                        else if(effects_icon_head->transparency < 3) {
                            painter.setPen(Qt::red);
                        }
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int x = effects_icon_head->width; x < width; x++)
                            {
                                painter.drawPoint(x, y);
                            }
                        }
                    }
                }

                painter.end();
                image.save(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".bmp");

                QFile file(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".acp");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_GDP()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression gdpRegExp("/GDP$");
    if(!gdpRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/GDP";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("GDP")) {
        if(!saveDir.mkdir("GDP")) {
            return;
        }
    }

    for(int subType = 1; subType <= 14; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_GDP, subType, resIndex);
            if(offset > 0) {
                effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + offset);

                int width = effects_icon_head->width;
                if(width % 8 > 0) {
                    width += (8 - width % 8);
                }

                QImage image(width * effects_icon_head->count, effects_icon_head->height, QImage::Format_ARGB32);

                QPainter painter(&image);

                painter.setPen(Qt::NoPen);

                if(effects_icon_head->transparency < 2) {
                    painter.setBrush(Qt::white);
                }
                else if(effects_icon_head->transparency < 3) {
                    painter.setBrush(Qt::red);
                }

                painter.drawRect(image.rect());

                for(uint8_t idx = 0; idx < effects_icon_head->count; idx++) {
                    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(effects_icon_head) + sizeof(effects_icon_head_t) +
                        width * effects_icon_head->height * effects_icon_head->transparency / 8 * idx;
                    if(effects_icon_head->transparency < 2) {
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int i = 0; i < (width / 8); i++) {
                                uint8_t data = dataPtr[y * width / 8 + i];
                                for(int j = 0; j < 8; j++) {
                                    if(data & (1 << (7 - j))) {
                                        painter.setPen(Qt::black);
                                    } else {
                                        painter.setPen(Qt::white);
                                    }
                                    painter.drawPoint(width * idx + i * 8 + j, y);
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
                                            painter.setPen(Qt::black);
                                        } else {
                                            painter.setPen(Qt::white);
                                        }
                                        painter.drawPoint(width * idx + i * 4 + j, y);
                                    }
                                }
                            }
                        }
                    }
                    if(effects_icon_head->width < width) {
                        if(effects_icon_head->transparency < 2) {
                            painter.setPen(Qt::white);
                        }
                        else if(effects_icon_head->transparency < 3) {
                            painter.setPen(Qt::red);
                        }
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int x = effects_icon_head->width; x < width; x++)
                            {
                                painter.drawPoint(x, y);
                            }
                        }
                    }
                }

                painter.end();
                image.save(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type, 2, 10, QLatin1Char('0')).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".bmp");

                QFile file(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type, 2, 10, QLatin1Char('0')).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".gdp");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_GGJ()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression ggjRegExp("/GGJ$");
    if(!ggjRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/GGJ";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("GGJ")) {
        if(!saveDir.mkdir("GGJ")) {
            return;
        }
    }

    for(int subType = 1; subType <= 2; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_GGJ, subType, resIndex);
            if(offset > 0) {
                effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + offset);

                int width = effects_icon_head->width;
                if(width % 8 > 0) {
                    width += (8 - width % 8);
                }

                QImage image(width * effects_icon_head->count, effects_icon_head->height, QImage::Format_ARGB32);

                QPainter painter(&image);

                painter.setPen(Qt::NoPen);

                if(effects_icon_head->transparency < 2) {
                    painter.setBrush(Qt::white);
                }
                else if(effects_icon_head->transparency < 3) {
                    painter.setBrush(Qt::red);
                }

                painter.drawRect(image.rect());

                for(uint8_t idx = 0; idx < effects_icon_head->count; idx++) {
                    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(effects_icon_head) + sizeof(effects_icon_head_t) +
                        width * effects_icon_head->height * effects_icon_head->transparency / 8 * idx;
                    if(effects_icon_head->transparency < 2) {
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int i = 0; i < (width / 8); i++) {
                                uint8_t data = dataPtr[y * width / 8 + i];
                                for(int j = 0; j < 8; j++) {
                                    if(data & (1 << (7 - j))) {
                                        painter.setPen(Qt::black);
                                    } else {
                                        painter.setPen(Qt::white);
                                    }
                                    painter.drawPoint(width * idx + i * 8 + j, y);
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
                                            painter.setPen(Qt::black);
                                        } else {
                                            painter.setPen(Qt::white);
                                        }
                                        painter.drawPoint(width * idx + i * 4 + j, y);
                                    }
                                }
                            }
                        }
                    }
                    if(effects_icon_head->width < width) {
                        if(effects_icon_head->transparency < 2) {
                            painter.setPen(Qt::white);
                        }
                        else if(effects_icon_head->transparency < 3) {
                            painter.setPen(Qt::red);
                        }
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int x = effects_icon_head->width; x < width; x++)
                            {
                                painter.drawPoint(x, y);
                            }
                        }
                    }
                }

                painter.end();
                image.save(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".bmp");

                QFile file(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".ggj");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_PIC()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression picRegExp("/PIC$");
    if(!picRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/PIC";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("PIC")) {
        if(!saveDir.mkdir("PIC")) {
            return;
        }
    }

    for(int subType = 1; subType <= 5; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_PIC, subType, resIndex);
            if(offset > 0) {
                effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(libraryBA.data() + offset);

                int width = effects_icon_head->width;
                if(width % 8 > 0) {
                    width += (8 - width % 8);
                }

                QImage image(width * effects_icon_head->count, effects_icon_head->height, QImage::Format_ARGB32);

                QPainter painter(&image);

                painter.setPen(Qt::NoPen);

                if(effects_icon_head->transparency < 2) {
                    painter.setBrush(Qt::white);
                }
                else if(effects_icon_head->transparency < 3) {
                    painter.setBrush(Qt::red);
                }

                painter.drawRect(image.rect());

                for(uint8_t idx = 0; idx < effects_icon_head->count; idx++) {
                    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(effects_icon_head) + sizeof(effects_icon_head_t) +
                        width * effects_icon_head->height * effects_icon_head->transparency / 8 * idx;
                    if(effects_icon_head->transparency < 2) {
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int i = 0; i < (width / 8); i++) {
                                uint8_t data = dataPtr[y * width / 8 + i];
                                for(int j = 0; j < 8; j++) {
                                    if(data & (1 << (7 - j))) {
                                        painter.setPen(Qt::black);
                                    } else {
                                        painter.setPen(Qt::white);
                                    }
                                    painter.drawPoint(width * idx + i * 8 + j, y);
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
                                            painter.setPen(Qt::black);
                                        } else {
                                            painter.setPen(Qt::white);
                                        }
                                        painter.drawPoint(width * idx + i * 4 + j, y);
                                    }
                                }
                            }
                        }
                    }
                    if(effects_icon_head->width < width) {
                        if(effects_icon_head->transparency < 2) {
                            painter.setPen(Qt::white);
                        }
                        else if(effects_icon_head->transparency < 3) {
                            painter.setPen(Qt::red);
                        }
                        for(int y = 0; y < effects_icon_head->height; y++) {
                            for(int x = effects_icon_head->width; x < width; x++)
                            {
                                painter.drawPoint(x, y);
                            }
                        }
                    }
                }

                painter.end();
                image.save(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".bmp");

                QFile file(saveDirPath + '/' + QString("%1-%2").arg(effects_icon_head->type).arg(effects_icon_head->idx, 3, 10, QLatin1Char('0')) + ".pic");
                if(file.open(QFile::WriteOnly)) {
                    QDataStream stream(&file);
                    stream.writeRawData((const char *)effects_icon_head, calc_effects_icon_size(effects_icon_head));
                    file.close();
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::Save_MLR()
{
    QString saveDirPath = ui->saveDir->text();
    static QRegularExpression mlrRegExp("/MLR$");
    if(!mlrRegExp.match(saveDirPath).hasMatch()) {
        saveDirPath += "/MLR";
    }

    QDir saveDir(ui->saveDir->text());
    if(!saveDir.exists("MLR")) {
        if(!saveDir.mkdir("MLR")) {
            return;
        }
    }

    for(int subType = 1; subType <= 2; subType++) {
        for(int resIndex = 0; resIndex < 256; resIndex++) {
            int offset = getResOffset(Res_MLR, subType, resIndex);
            if(offset > 0) {
                if(subType == 1) {
                    magic_list_head_t *magic_list_head = reinterpret_cast<magic_list_head_t *>(libraryBA.data() + offset);
                    QFile file(saveDirPath + '/' + QString("%1-%2").arg(magic_list_head->type).arg(magic_list_head->idx, 3, 10, QLatin1Char('0')) + ".mlr");
                    if(file.open(QFile::WriteOnly)) {
                        QDataStream stream(&file);
                        stream.writeRawData((const char *)magic_list_head, calc_magic_list_size(magic_list_head));
                        file.close();
                    }
                }
                else if(subType == 2) {
                    level_list_head_t *level_list_head = reinterpret_cast<level_list_head_t *>(libraryBA.data() + offset);
                    QFile file(saveDirPath + '/' + QString("%1-%2").arg(level_list_head->type).arg(level_list_head->idx, 3, 10, QLatin1Char('0')) + ".mlr");
                    if(file.open(QFile::WriteOnly)) {
                        QDataStream stream(&file);
                        stream.writeRawData((const char *)level_list_head, calc_level_list_size(level_list_head));
                        file.close();
                    }
                }
            }
        }
    }

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::on_loadGamBtn_clicked()
{
    QSettings settings("QtProject", "BBK_Resource_Extract");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), settings.value("last_gam_dir").toString(), "GAM(*.gam)");
    if(!fileName.isEmpty() && QFile(fileName).exists()) {
        settings.setValue("last_gam_dir", QFileInfo(fileName).absoluteDir().path());
        settings.sync();
    } else {
        return;
    }

    setWindowTitle(tr("BBK_Resource_Extract_") + _buildDateTimeStr);

    engineBA.clear();
    libraryBA.clear();

    offsetMap.clear();

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

    setWindowTitle(QFileInfo(fileName).fileName());
}

void MainWidget::on_extractResBtn_clicked()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Gam Data Not Loaded!"));
        return;
    }

    switch(ResType(ui->rcType->currentIndex() + 1))
    {
    case Res_GUT: Save_GUT(); break;
    case Res_MAP: Save_MAP(); break;
    case Res_ARS: Save_ARS(); break;
    case Res_MRS: Save_MRS(); break;
    case Res_SRS: Save_SRS(); break;
    case Res_GRS: Save_GRS(); break;
    case Res_TIL: Save_TIL(); break;
    case Res_ACP: Save_ACP(); break;
    case Res_GDP: Save_GDP(); break;
    case Res_GGJ: Save_GGJ(); break;
    case Res_PIC: Save_PIC(); break;
    case Res_MLR: Save_MLR(); break;
    default: break;
    }
}

void MainWidget::on_stateTimer_timeout()
{
    stateTimer.stop();
    ui->stateLbl->clear();
}
