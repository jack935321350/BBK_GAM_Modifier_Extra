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



#define GRS_DATA_MAX_SIZE  1048576

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

private:
    Ui::MainWidget      *ui;

    QString             buildDateTimeStr;

    uint8_t             *_GrsData = nullptr;
    int32_t             _GrsDataSize = 0;

    QSet<QString>       getDirFiles(const QString &path);

    int8_t fromNumber(uint8_t num) {
        if(num & 0x80) {
            return -(num & 0x7f);
        }
        return num;
    }

    uint8_t toNumber(int8_t num) {
        if(num < 0) {
            return (-num | 0x80);
        }
        return num;
    }

private slots:
    void on_loadGrs_clicked();
    void on_createGrs_clicked();
    void on_saveGrs_clicked();

    void on_propType_currentIndexChanged(int index);
    void on_propIdx_valueChanged(int index);
    void on_propData_valueChanged();
    void on_propDescription_textChanged();

};
#endif // MAINWIDGET_H
