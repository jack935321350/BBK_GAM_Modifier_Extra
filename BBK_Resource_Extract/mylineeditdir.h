#ifndef MYLINEEDITDIR_H
#define MYLINEEDITDIR_H

#include <QLineEdit>
#include <QFileDialog>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QSettings>

class MyLineEditDir : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEditDir(QWidget *parent);
    ~MyLineEditDir();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

};

#endif // MYLINEEDITDIR_H
