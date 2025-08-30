#include "tilecfgdlg.h"
#include "ui_tilecfgdlg.h"

TileCfgDlg::TileCfgDlg(QWidget *parent,  uint8_t tileMax) : QDialog(parent), ui(new Ui::TileCfgDlg)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->tileMax->setValue(tileMax);
    ui->tileMax->setFocus();
}

TileCfgDlg::~TileCfgDlg()
{
    delete ui;
}

void TileCfgDlg::on_btnBox_accepted()
{
    TileMax = ui->tileMax->value();
}
