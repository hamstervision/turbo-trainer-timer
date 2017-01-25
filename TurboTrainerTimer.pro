#-------------------------------------------------
#
# Project created by QtCreator 2016-11-29T08:56:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(QtAwesome/QtAwesome.pri)

TARGET = TurboTrainerTimer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    stepwidget.cpp \
    stagingarea.cpp \
    turbosetmodel.cpp \
    showtimewidget.cpp \
    step.cpp \
    nowplayingwidget.cpp \
    stepresources.cpp \
    upnextwidget.cpp \
    showtimestepwidget.cpp

HEADERS  += mainwindow.h \
    stepwidget.h \
    stagingarea.h \
    turbosetmodel.h \
    types.h \
    showtimewidget.h \
    step.h \
    istepmanager.h \
    isetmanager.h \
    nowplayingwidget.h \
    stepresources.h \
    upnextwidget.h \
    showtimestepwidget.h \
    ifontawesome.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

RC_FILE += ttt.rc
