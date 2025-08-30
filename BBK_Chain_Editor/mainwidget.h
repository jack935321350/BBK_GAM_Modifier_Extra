#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QLocale>
#include <QDateTime>
#include <QSettings>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QLocalSocket>
#include <QDataStream>
#include <QAction>
#include <QMenu>



#define MLR_DATA_MAX_SIZE  1048576

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_loadMlr_clicked();
    void on_createMlr_clicked();
    void on_saveMlr_clicked();

    void on_mlrData_changed();
    void on_mlrUI_changed();

    void on_itemsList_itemSelectionChanged();

    void on_itemsList_customContextMenuRequested(const QPoint &pos);
    void on_addItem();
    void on_insItem();
    void on_delItem();

private:
    Ui::MainWidget *ui;

    QString         _buildDateTimeStr;

    QLocalSocket    mrsLocalSocket;

    uint8_t         *_MlrData = nullptr;
    int32_t         _MlrDataSize = 0;
    uint8_t         *_TmpData = nullptr;

    QMenu           *itemsListMenu;
    QAction         *addItem;
    QAction         *insItem;
    QAction         *delItem;
    QListWidgetItem *widgetItem = nullptr;

};
#endif // MAINWIDGET_H
