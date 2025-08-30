#include "mylineeditdir.h"

MyLineEditDir::MyLineEditDir(QWidget *parent) : QLineEdit(parent)
{

}

MyLineEditDir::~MyLineEditDir()
{

}

void MyLineEditDir::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    QSettings settings("QtProject", "BBK_Resource_Extract");
    QString dirName = QFileDialog::getExistingDirectory(this, "Select a Dir.", settings.value("last_res_dir").toString());
    if(!dirName.isEmpty() && QDir(dirName).exists())
    {
        settings.setValue("last_res_dir", QDir(dirName).absolutePath());
        settings.sync();
        setText(dirName);
    }
}
