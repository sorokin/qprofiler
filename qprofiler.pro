#-------------------------------------------------
#
# Project created by QtCreator 2015-02-14T01:46:21
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -g -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qprofiler
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myitemdelegate.cpp \
    profile.cpp \
    my_item.cpp \
    my_context.cpp

HEADERS  += mainwindow.h \
    myitemdelegate.h \
    call_tree_column.h \
    benchmark_mode.h \
    profile.h \
    my_item.h \
    my_context.h

FORMS    += mainwindow.ui
