#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    gbkCodec = QTextCodec::codecForName("GBK");
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e)
}

void MainWidget::on_gbkEdit_textChanged(const QString &text)
{
    QByteArray hexBA = gbkCodec->fromUnicode(text).toHex().toUpper();

    QByteArray disBA;
    for(int i = 0; i < hexBA.count(); i++) {
        disBA.append(hexBA.at(i));
        if(i % 2 > 0)
            disBA.append(' ');
    }

    ui->hexEdit->setText(disBA);
}
