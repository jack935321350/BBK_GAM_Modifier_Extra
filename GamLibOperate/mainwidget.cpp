#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    ui->gamPath->setFilter("GAM(*.gam)");
    ui->headPath->setFilter("HEAD(*.head)");
    ui->libPath->setFilter("LIB(*.lib)");

    setMaximumHeight(height());

    connect(&stateTimer, SIGNAL(timeout()), this, SLOT(on_stateTimer_timeout()));

    LoadCfg();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e)
    SaveCfg();
}

void MainWidget::LoadCfg(void)
{
    QSettings settings("QtProject", "GamLibOperate");
}

void MainWidget::SaveCfg(void)
{
    QSettings settings("QtProject", "GamLibOperate");
}

void MainWidget::on_gamPath_fileNameChanged(QString fileName)
{
    fileName.remove(".gam");
    ui->headPath->setText(fileName + ".head");
    ui->libPath->setText(fileName + ".lib");
}

void MainWidget::on_stateTimer_timeout()
{
    stateTimer.stop();
    ui->stateLbl->clear();
}

void MainWidget::on_separateBtn_clicked()
{
    QFile gamFile(ui->gamPath->text());

    if(!gamFile.open(QIODevice::ReadOnly))
    {
        ui->stateLbl->setText(u8"<font color=red>GAM File Open Error...</font>");
        stateTimer.start(3000);
        return;
    }

    QByteArray gamBA = gamFile.readAll();

    gamFile.close();



    int libIndex = gamBA.indexOf(QByteArray("LIB", 3));

    if(libIndex < 0 || libIndex > (gamFile.size() - 5))
    {
        ui->stateLbl->setText(u8"<font color=red>Can't Find LIB Tag...</font>");
        stateTimer.start(3000);
        return;
    }



    QFile headFile(ui->headPath->text());

    if(!headFile.open(QIODevice::WriteOnly))
    {
        ui->stateLbl->setText(u8"<font color=red>HEAD File Save Error...</font>");
        stateTimer.start(3000);
        return;
    }

    QDataStream headStream(&headFile);
    QByteArray headBA = gamBA.left(libIndex);
    headStream.writeRawData(headBA.data(), headBA.size());

    headFile.close();



    QFile libFile(ui->libPath->text());

    if(!libFile.open(QIODevice::WriteOnly))
    {
        ui->stateLbl->setText(u8"<font color=red>LIB File Save Error...</font>");
        stateTimer.start(3000);
        return;
    }

    QDataStream libStream(&libFile);
    QByteArray libBA = gamBA.mid(libIndex);
    libStream.writeRawData(libBA.data(), libBA.size());

    libFile.close();

    ui->stateLbl->setText(u8"<font color=green>Separate OK...</font>");
    stateTimer.start(3000);
}

void MainWidget::on_combineBtn_clicked()
{
    QFile headFile(ui->headPath->text());

    if(!headFile.open(QIODevice::ReadOnly))
    {
        ui->stateLbl->setText(u8"<font color=red>HEAD File Open Error...</font>");
        stateTimer.start(3000);
        return;
    }
    QByteArray headBA = headFile.readAll();

    headFile.close();



    QFile libFile(ui->libPath->text());

    if(!libFile.open(QIODevice::ReadOnly))
    {
        ui->stateLbl->setText(u8"<font color=red>LIB File Open Error...</font>");
        stateTimer.start(3000);
        return;
    }
    QByteArray libBA = libFile.readAll();

    libFile.close();



    QFile gamFile(ui->gamPath->text());

    if(!gamFile.open(QIODevice::WriteOnly))
    {
        ui->stateLbl->setText(u8"<font color=red>GAM File Save Error...</font>");
        stateTimer.start(3000);
        return;
    }

    QDataStream gamStream(&gamFile);
    gamStream.writeRawData(headBA.data(), headBA.size());
    gamStream.writeRawData(libBA.data(), libBA.size());

    gamFile.close();

    ui->stateLbl->setText(u8"<font color=green>Combine OK...</font>");
    stateTimer.start(3000);
}

