#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include <cassert>

#include <QSettings>
#include <QFileDialog>

#include "myitemdelegate.h"
#include "call_tree_column.h"
#include "benchmark_mode.h"
#include "my_item.h"
#include "profile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treeWidget->setItemDelegate(new MyItemDelegate);
    ui->treeWidget->header()->resizeSection(0, 900);
    ui->treeWidget->header()->setStretchLastSection(false);
    ui->treeWidget->sortByColumn(1, Qt::DescendingOrder);

    {
        QSettings settings;
        restoreGeometry(settings.value("main-window-geometry").toByteArray());
        restoreState(settings.value("main-window-state").toByteArray());

        QString filename = settings.value("opened-file").toString();
        if (filename.size() != 0)
            open_file(filename);
    }

    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(file_open_action()));
    connect(ui->actionExpand_All, SIGNAL(triggered()), SLOT(edit_expand_all_action()));
    connect(ui->actionCall_Tree, SIGNAL(triggered()), SLOT(view_call_tree()));
    connect(ui->actionReverse_Call_Tree, SIGNAL(triggered()), SLOT(view_reverse_call_tree()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_file(QString const& file)
{
    clear_tree();
    p.open_perf_data(file.toStdString());

    QSettings settings;
    settings.setValue("opened-file", QVariant(file));
    this->setWindowTitle(QString("%1 - QProfiler").arg(file));

    p.build_tree(ctx.get_root());
    show_tree();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings;
    settings.setValue("main-window-geometry", saveGeometry());
    settings.setValue("main-window-state", saveState());
}

void MainWindow::file_open_action()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Perf Data (*.data)"));
    if (filename.size() != 0)
        open_file(filename);
}

void MainWindow::edit_expand_all_action()
{
    ctx.get_root()->expand_all(ui->treeWidget);
}

void MainWindow::view_call_tree()
{
    clear_tree();
    p.build_tree(ctx.get_root());
    show_tree();
}

void MainWindow::view_reverse_call_tree()
{
    clear_tree();
    p.build_reverse_tree(ctx.get_root());
    show_tree();
}

void MainWindow::clear_tree()
{
    ui->treeWidget->clear();
    ctx.set_root(0);
    MyItem* root = new MyItem(&ctx, "<root>");
    ctx.set_root(root);
}

void MainWindow::show_tree()
{
    ctx.get_root()->update_percentage();
    ui->treeWidget->addTopLevelItem(ctx.get_root());
    ctx.get_root()->expand_all_greater_than(ui->treeWidget, ctx.total_hits() / 5);
}
