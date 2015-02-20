#-------------------------------------------------
#
# Project created by QtCreator 2015-02-14T01:46:21
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -g

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qprofiler
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myitemdelegate.cpp

HEADERS  += mainwindow.h \
    myitemdelegate.h \
    call_tree_column.h

FORMS    += mainwindow.ui
