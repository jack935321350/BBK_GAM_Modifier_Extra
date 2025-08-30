#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QSpinBox>
#include <QSettings>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFileDialog>
#include <QDataStream>
#include <QTextCodec>
#include <QTranslator>
#include <QMouseEvent>
#include <QDateTime>
#include <QPainter>
#include <QLocale>
#include <QTimer>
#include <QFile>



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
    Ui::MainWidget *ui;

    QTimer stateTimer;

    QString _buildDateTimeStr;

    QByteArray engineBA;
    QByteArray libraryBA;

    QMap<int, int> offsetMap;

    enum ResType
    {
        Res_GUT =  1,
        Res_MAP =  2,
        Res_ARS =  3,
        Res_MRS =  4,
        Res_SRS =  5,
        Res_GRS =  6,
        Res_TIL =  7,
        Res_ACP =  8,
        Res_GDP =  9,
        Res_GGJ = 10,
        Res_PIC = 11,
        Res_MLR = 12,
    };

    int getResOffset(ResType resType, int subType, int resIndex) {
        if(offsetMap.isEmpty()) {
            return 0;
        }
        if(offsetMap.find(resType << 16 | subType << 8 | resIndex) == offsetMap.end()) {
            return 0;
        }
        return offsetMap.value(resType << 16 | subType << 8 | resIndex);
    }

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

    QString GetName(uint8_t *name) {
        QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
        return gbkCodec->toUnicode((char *)name);
    }

    void Save_GUT();
    void Save_MAP();
    void Save_ARS();
    void Save_MRS();
    void Save_SRS();
    void Save_GRS();
    void Save_TIL();
    void Save_ACP();
    void Save_GDP();
    void Save_GGJ();
    void Save_PIC();
    void Save_MLR();

private slots:    
    void on_loadGamBtn_clicked();
    void on_extractResBtn_clicked();
    void on_stateTimer_timeout();

};
#endif // MAINWIDGET_H
