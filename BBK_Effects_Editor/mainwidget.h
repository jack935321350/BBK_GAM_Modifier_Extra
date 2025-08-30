#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QTranslator>
#include <QLocale>
#include <QAction>
#include <QImage>
#include <QMenu>
#include <QDate>



#define EFFECTS_DATA_MAX_SIZE  1048576

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;

    QString         _buildDateTimeStr;

    uint8_t         *_EffectsData = nullptr;
    int32_t         _EffectsDataSize = 0;
    uint8_t         *_EffectsSrcData = nullptr;
    int32_t         _EffectsSrcDataSize = 0;
    uint8_t         *_PicData = nullptr;

    QMenu           *framesListMenu;
    QAction         *addFrame;
    QAction         *insFrame;
    QAction         *delFrame;
    QListWidgetItem *frameWidgetItem = nullptr;

    QMenu           *iconsListMenu;
    QAction         *edtIcon;
    QAction         *addIcon;
    QAction         *insIcon;
    QAction         *delIcon;
    QListWidgetItem *iconWidgetItem = nullptr;

private slots:
    void on_srsData_changed();
    void on_srsUI_changed();

    void on_createSrs_clicked();
    void on_loadSrs_clicked();
    void on_resetSrs_clicked();
    void on_saveSrs_clicked();

    void on_effectsView_playStopChanged(bool play);
    void on_effectsView_currFrameChanged(uint8_t currFrame);
    void on_effectsView_currTickChanged(uint32_t currTick);

    void on_tickTimer_valueChanged(int ms);

    void on_prevTick_clicked();
    void on_playTick_clicked();
    void on_nextTick_clicked();

    void on_framesList_itemSelectionChanged();
    void on_frameArgsUI_changed();

    void on_framesList_customContextMenuRequested(const QPoint &pos);
    void on_addFrame();
    void on_insFrame();
    void on_delFrame();

    void on_iconsList_customContextMenuRequested(const QPoint &pos);
    void on_edtIcon();
    void on_addIcon();
    void on_insIcon();
    void on_delIcon();

};

#endif // MAINWIDGET_H
