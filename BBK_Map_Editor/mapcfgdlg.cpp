#include "mapcfgdlg.h"
#include "ui_mapcfgdlg.h"

MapCfgDlg::MapCfgDlg(QWidget *parent, map_head_t map_head, bool whEnable) : QDialog(parent), ui(new Ui::MapCfgDlg)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->width->setEnabled(whEnable);
    ui->height->setEnabled(whEnable);
    ui->type->setValue(map_head.type);
    ui->index->setValue(map_head.idx);
    ui->tile->setValue(map_head.tile_idx);
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    ui->name->setText(gbkCodec->toUnicode((char *)map_head.name));
    ui->width->setValue(map_head.width);
    ui->height->setValue(map_head.height);
}

MapCfgDlg::~MapCfgDlg()
{
    delete ui;
}

void MapCfgDlg::on_btnBox_accepted()
{
    Map_Head.type = ui->type->value();
    Map_Head.idx = ui->index->value();
    Map_Head.tile_idx = ui->tile->value();
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    QByteArray enemyName = gbkCodec->fromUnicode(ui->name->text());
    memset(Map_Head.name, 0, 11);
    memcpy_s(Map_Head.name, 11, enemyName.data(), enemyName.size());
    Map_Head.reserved1 = 'M';
    Map_Head.reserved2 = 'P';
    Map_Head.width = ui->width->value();
    Map_Head.height = ui->height->value();
}

void MapCfgDlg::on_name_textChanged(const QString &str)
{
    Q_UNUSED(str);

    int len8Bit = ui->name->text().toLocal8Bit().length();
    if(len8Bit > 10) {
        ui->name->setCursorPosition(ui->name->text().size());
        do {
            ui->name->backspace();
            len8Bit = ui->name->text().toLocal8Bit().length();
        } while(len8Bit > 10);
    }
}

