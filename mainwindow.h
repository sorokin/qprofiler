#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "my_context.h"
#include "profile.h"
#include "transformation.h"
#include "undo_stack.h"

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
    void undo();

    void show_context_menu(QPoint const&);
    void selection_changed();

private:
    std::vector<profile::frame_index_type> get_selected_frames() const;
    void update_undo();
    void refresh_tree();
    void clear_tree();
    void show_tree();

private:
    Ui::MainWindow *ui;
    MyContext ctx;
    profile p;
    ::undo_stack<transformation> undo_stack;
};

#endif // MAINWINDOW_H
