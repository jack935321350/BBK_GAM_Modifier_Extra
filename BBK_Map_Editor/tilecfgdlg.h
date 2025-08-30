#ifndef TILECFGDLG_H
#define TILECFGDLG_H

#include <QDialog>

namespace Ui {
class TileCfgDlg;
}

class TileCfgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TileCfgDlg(QWidget *parent = nullptr, uint8_t tileMax = 1);
    ~TileCfgDlg();
    uint8_t TileMax;

private slots:
    void on_btnBox_accepted();

private:
    Ui::TileCfgDlg *ui;

};

#endif // TILECFGDLG_H
