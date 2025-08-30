#include "mainwidget.h"

#include <QApplication>
#include <QTranslator>
#include <QScreen>

int main(int argc, char *argv[])
{
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    //QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);

    QApplication app(argc, argv);

    static QTranslator translator;
    translator.load(":/translations/qtbase_zh_CN.qm");
    QApplication::installTranslator(&translator);

    static QTranslator appTranslator;
    appTranslator.load(":/translations/zh_CN.qm");
    QApplication::installTranslator(&appTranslator);

    MainWidget mw;
    QRect geometry = mw.frameGeometry();
    geometry.moveCenter(QApplication::primaryScreen()->availableGeometry().center());
    mw.move(geometry.topLeft());
    mw.show();

    return app.exec();
}
