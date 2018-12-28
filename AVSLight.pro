#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T03:24:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = AVSLight
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    serialthread.cpp

HEADERS  += mainwindow.h \
    serialthread.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:/Qt/library/libvlc_220rc2/include
LIBS     += -LC:/Qt/library/libvlc_220rc2/lib -llibvlc -llibvlccore
