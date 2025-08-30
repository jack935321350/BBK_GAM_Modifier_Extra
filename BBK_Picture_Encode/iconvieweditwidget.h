#ifndef ICONVIEWEDITWIDGET_H
#define ICONVIEWEDITWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QTranslator>
#include <QLocale>
#include <QImage>
#include <QDate>

#include "iconviewedit.h"

namespace Ui {
class IconViewEditWidget;
}

class IconViewEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IconViewEditWidget(QWidget *parent = nullptr);
    ~IconViewEditWidget();

private:
    Ui::IconViewEditWidget *ui;

    QString buildDateStr;
    QString buildTimeStr;

    QImage srcImage;
    QString currSuffix = "TIL(*.til)";

private slots:
    void on_pushButton_clicked();
    void on_brushTypeChanged(IconViewEdit::BrushType brushType);
    void on_iconType_currentIndexChanged(int index);
    void on_iconViewIdx_valueChanged(int index);
    void on_gaussianBlur_valueChanged(int value);
    void on_pictureEncode_configChanged(void);

};

#endif // ICONVIEWEDITWIDGET_H
