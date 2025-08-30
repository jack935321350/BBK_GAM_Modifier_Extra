#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>
#include <QColorDialog>

class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget *parent);
    ~ColorButton();

    void SetColor(QColor color) {
        _color = color;
    }

    QColor GetColor(void) {
        return _color;
    }

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QColor _color = QColor(255, 0, 0);

signals:
    void colorChanged();

};

#endif // MYPUSHBUTTON_H
