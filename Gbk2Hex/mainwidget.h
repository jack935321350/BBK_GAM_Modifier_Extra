#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QTextCodec>

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
    QTextCodec *gbkCodec;

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void on_gbkEdit_textChanged(const QString &text);
};
#endif // MAINWIDGET_H
