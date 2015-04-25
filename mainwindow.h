#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "my_context.h"
#include "profile.h"
#include "transformation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void open_file(QString const& file);
    void closeEvent(QCloseEvent*);

private slots:
    void file_open_action();
    void edit_expand_all_action();
    void view_call_tree();
    void view_reverse_call_tree();
    void view_all_instances();

    void show_context_menu(QPoint const&);
    void selection_changed();

private:
    void clear_tree();
    void show_tree();

private:
    Ui::MainWindow *ui;
    MyContext ctx;
    profile p;
    transformation trs;
};

#endif // MAINWINDOW_H
