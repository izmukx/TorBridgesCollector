QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TorBridgesCollector
TEMPLATE = app

RC_ICONS = images/icon.ico

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        torbridgescollector.cpp \
    settingsdialog.cpp \
    getbridgeswindow.cpp \
    entercaptcha.cpp \
    aboutpage.cpp \
    showqrcode.cpp

HEADERS += \
        torbridgescollector.h \
    settingsdialog.h \
    getbridgeswindow.h \
    entercaptcha.h \
    aboutpage.h \
    showqrcode.h

FORMS += \
        torbridgescollector.ui \
    settingsdialog.ui \
    getbridgeswindow.ui \
    entercaptcha.ui \
    aboutpage.ui \
    showqrcode.ui

RESOURCES += \
    torbridgescollector.qrc
