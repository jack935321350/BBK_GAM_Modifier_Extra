#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent) : QLineEdit(parent)
{

}

MyLineEdit::~MyLineEdit()
{

}

void MyLineEdit::setFilter(QString filter)
{
    _filter = filter;
}

void MyLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    QSettings settings("QtProject", "GamLibOperate");
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File.",  settings.value("last_dir").toString(), _filter);
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        settings.setValue("last_dir", QFileInfo(fileName).absoluteDir().path());
        settings.sync();
        setText(fileName);
        emit fileNameChanged(fileName);
    }
}

#if 0
void MyLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MyLineEdit::dropEvent(QDropEvent *event)
{
    QString fileName = event->mimeData()->urls().front().toLocalFile();
    if(!fileName.isEmpty() && QFile(fileName).exists())
    {
        QSettings settings("QtProject", "GamLibOperate");
        settings.setValue("last_dir", QFileInfo(fileName).absoluteDir().path());
        setText(fileName);
    }
}
#endif
