#include "mainwidget.h"

#include <QApplication>
#include <QTranslator>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    static QTranslator translator;
    translator.load(":/translations/qtbase_zh_CN.qm");
    QApplication::installTranslator(&translator);

    static QTranslator appTranslator;
    appTranslator.load(":/translations/zh_CN.qm");
    QApplication::installTranslator(&appTranslator);

    MainWidget w;
    QRect geometry = w.frameGeometry();
    geometry.moveCenter(QGuiApplication::primaryScreen()->availableGeometry().center());
    w.move(geometry.topLeft());
    w.show();

    return a.exec();
}
