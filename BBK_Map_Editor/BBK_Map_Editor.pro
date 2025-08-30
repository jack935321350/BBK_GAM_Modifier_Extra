QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TRANSLATIONS += \
    translations/zh_CN.ts

DESTDIR = $$PWD/../bin/

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    eventcfgdlg.cpp \
    gam_type.cpp \
    mainwidget.cpp \
    main.cpp \
    mapcfgdlg.cpp \
    mapviewedit.cpp \
    tilecfgdlg.cpp \
    tileviewselect.cpp

HEADERS += \
    eventcfgdlg.h \
    gam_type.h \
    mainwidget.h \
    mapcfgdlg.h \
    mapviewedit.h \
    tilecfgdlg.h \
    tileviewselect.h

FORMS += \
    eventcfgdlg.ui \
    mainwidget.ui \
    mapcfgdlg.ui \
    tilecfgdlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
