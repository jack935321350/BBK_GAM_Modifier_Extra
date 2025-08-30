#ifndef MAPCFGDLG_H
#define MAPCFGDLG_H

#include <QDialog>
#include <QTextCodec>

#include "gam_type.h"

namespace Ui {
class MapCfgDlg;
}

class MapCfgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MapCfgDlg(QWidget *parent = nullptr, map_head_t map_head = { 0 }, bool whEnable = true);
    ~MapCfgDlg();
    map_head_t Map_Head;

private slots:
    void on_btnBox_accepted();
    void on_name_textChanged(const QString &str);

private:
    Ui::MapCfgDlg *ui;

};

#endif // MAPCFGDLG_H
