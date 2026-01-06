QT += core gui sql widgets

CONFIG += c++17

TARGET = Rent
TEMPLATE = app
CONFIG += qt c++17 release  # 这里添加 release

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    bookmodel.cpp \
    readermodel.cpp \
    borrowmodel.cpp \
    bookwidget.cpp \
    readerwidget.cpp \
    borrowwidget.cpp \
    statisticswidget.cpp \
    logindialog.cpp

HEADERS += \
    mainwindow.h \
    database.h \
    bookmodel.h \
    readermodel.h \
    borrowmodel.h \
    bookwidget.h \
    readerwidget.h \
    borrowwidget.h \
    statisticswidget.h \
    logindialog.h

FORMS += \
    mainwindow.ui \
    bookwidget.ui \
    readerwidget.ui \
    borrowwidget.ui \
    statisticswidget.ui \
    logindialog.ui

RESOURCES += \
    resources.qrc
