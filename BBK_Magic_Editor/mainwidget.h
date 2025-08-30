#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QRegularExpression>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTextCodec>
#include <QSettings>
#include <QDateTime>
#include <QLocale>
#include <QSet>



#define MRS_DATA_MAX_SIZE  1048576

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

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::MainWidget  *ui;

    QString         buildDateTimeStr;

    QLocalServer    magicLocalServer;
    QLocalSocket    *magicLocalSocket = nullptr;

    QString             mrsPath;
    QVector<QByteArray> mrsDataVec;

    uint8_t             *_MrsData = nullptr;
    int32_t             _MrsDataSize = 0;

    QSet<QString>       getDirFiles(const QString &path);

private slots:
    void on_magicLocalServer_newConnection();
    void on_magicLocalSocket_readyRead();
    void on_magicLocalSocket_disconnected();

    void on_loadMrs_clicked();
    void on_createMrs_clicked();
    void on_monitorDir_clicked();
    void on_saveMrs_clicked();

    void on_magicType_currentIndexChanged(int index);
    void on_magicIdx_valueChanged(int index);
    void on_magicData_valueChanged();
    void on_magicDescription_textChanged();

};
#endif // MAINWIDGET_H
