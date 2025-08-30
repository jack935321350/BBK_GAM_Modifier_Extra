#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QSpinBox>
#include <QSettings>
#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
#include <QDataStream>
#include <QTranslator>
#include <QDateTime>
#include <QLocale>
#include <QTimer>
#include <QFile>



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE



typedef struct
{
    QString     fileName;
    QByteArray  data;

} LibRes;

typedef struct
{
    uint8_t     blkIndex;
    uint16_t    blkOffset;

} BlkInfo;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;

    QTimer              stateTimer;

    QString             _buildDateTimeStr;

    QByteArray          _rcLib;
    uint16_t            _rcIndex;

    uint8_t             _currBlock;
    uint16_t            _currBlockOffset;

    bool                _errorTag;

    const uint16_t      _blockSize = 0x4000;
    const uint16_t      _rcBlockBase = 0x10;
    const uint16_t      _rcOffsetBase = 0x2000;

    QVector<LibRes> _libResVec;

    enum ResType
    {
        Res_LIB =  0,
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

    } _lastResType = Res_LIB;

    const QVector<QByteArray> ResTag {
        "LIB", "GUT", "MAP", "ARS", "MRS", "SRS", "GRS", "TIL", "ACP", "GDP", "GGJ", "PIC", "MLR"
    };

    void SetBlockSize(uint8_t block, uint16_t size);
    void SetBlockArgs(uint16_t rcIndex, ResType resType, uint8_t subType, uint8_t resIndex);
    void SetBlockOffset(uint16_t rcIndex, uint8_t block, uint16_t offset);

    void Append_Block(ResType resType);
    void Integrate_Block(ResType resType);

private slots:    
    void on_integrateResBtn_clicked();
    void on_libName_textChanged(const QString &str);
    void on_stateTimer_timeout();

};
#endif // MAINWIDGET_H
