#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTimer>
#include <QSettings>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    QTimer stateTimer;
    void LoadCfg(void);
    void SaveCfg(void);

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void on_gamPath_fileNameChanged(QString fileName);
    void on_stateTimer_timeout();
    void on_separateBtn_clicked();
    void on_combineBtn_clicked();
};
#endif // MAINWIDGET_H
