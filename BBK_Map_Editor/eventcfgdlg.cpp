#include "eventcfgdlg.h"
#include "ui_eventcfgdlg.h"

EventCfgDlg::EventCfgDlg(QWidget *parent,  uint8_t event) : QDialog(parent), ui(new Ui::EventCfgDlg)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->event->setValue(event);
    ui->event->setFocus();
}

EventCfgDlg::~EventCfgDlg()
{
    delete ui;
}

void EventCfgDlg::on_btnBox_accepted()
{
    Event = ui->event->value();
}
