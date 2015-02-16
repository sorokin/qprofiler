#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

struct MyItem;

struct MyContext
{
    MyContext();
    void set_root(MyItem* item);
    MyItem* get_root();
    size_t total_hits();

private:
    MyItem* root;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void open_file(QString const& file);
    void read_file(QString const& file);
    void closeEvent(QCloseEvent*);

private slots:
    void file_open_action();

private:
    Ui::MainWindow *ui;
    MyContext ctx;
};

#endif // MAINWINDOW_H
