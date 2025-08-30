#ifndef EVENTCFGDLG_H
#define EVENTCFGDLG_H

#include <QDialog>

namespace Ui {
class EventCfgDlg;
}

class EventCfgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit EventCfgDlg(QWidget *parent = nullptr, uint8_t event = 0);
    ~EventCfgDlg();
    uint8_t Event;

private slots:
    void on_btnBox_accepted();

private:
    Ui::EventCfgDlg *ui;
};

#endif // EVENTDLG_H
