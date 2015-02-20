#include <QApplication>

#include "mainwindow.h"
#include "benchmark_mode.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

#ifndef BENCHMARK_MODE
    return a.exec();
#endif
}
