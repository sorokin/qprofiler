#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include <cassert>

#include <QSettings>
#include <QFileDialog>

#include "my_item_delegate.h"
#include "call_tree_column.h"
#include "benchmark_mode.h"
#include "my_item.h"
#include "profile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treeWidget->setItemDelegate(new my_item_delegate);
    ui->treeWidget->header()->resizeSection(0, 900);
    ui->treeWidget->header()->setStretchLastSection(false);
    ui->treeWidget->sortByColumn(1, Qt::DescendingOrder);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(show_context_menu(QPoint)));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    {
        QSettings settings;
        restoreGeometry(settings.value("main-window-geometry").toByteArray());
        restoreState(settings.value("main-window-state").toByteArray());

        QString filename = settings.value("opened-file").toString();
        if (filename.size() != 0)
            open_file(filename);
    }

    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(file_open_action()));
    connect(ui->actionUndo, SIGNAL(triggered()), SLOT(undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), SLOT(redo()));
    connect(ui->actionExpand_All, SIGNAL(triggered()), SLOT(edit_expand_all_action()));
    connect(ui->actionCall_Tree, SIGNAL(triggered()), SLOT(view_call_tree()));
    connect(ui->actionReverse_Call_Tree, SIGNAL(triggered()), SLOT(view_reverse_call_tree()));
    connect(ui->actionAll_Instances, SIGNAL(triggered()), SLOT(view_all_instances()));

    selection_changed();
    update_undo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_file(QString const& file)
{
    clear_tree();
    p.open(file.toStdString());

    QSettings settings;
    settings.setValue("opened-file", QVariant(file));
    this->setWindowTitle(QString("%1 - QProfiler").arg(file));

    refresh_tree();
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
    transformation trs;
    trs.direction = transformation::direction_type::forward;
    trs.roots.clear();
    undo_stack.reset(std::move(trs));
    update_undo();

    refresh_tree();
}

void MainWindow::view_reverse_call_tree()
{
    transformation trs;
    trs.direction = transformation::direction_type::backward;
    trs.roots.clear();
    undo_stack.reset(trs);
    update_undo();

    refresh_tree();
}

void MainWindow::view_all_instances()
{
    auto selected_frames = get_selected_frames();

    if (selected_frames.empty())
    {
        assert(false);
        return;
    }

    transformation trs = undo_stack.current();
    trs.roots.push_back(transformation::frames_set(selected_frames.begin(), selected_frames.end()));
    undo_stack.push(std::move(trs));
    update_undo();

    refresh_tree();
}

void MainWindow::undo()
{
    undo_redo(::undo_stack<transformation>::action_type::undo);
}

void MainWindow::redo()
{
    undo_redo(::undo_stack<transformation>::action_type::redo);
}

void MainWindow::show_context_menu(QPoint const& point)
{
    QMenu menu;
    menu.addAction(ui->actionAll_Instances);
    menu.exec(ui->treeWidget->viewport()->mapToGlobal(point));
}

void MainWindow::selection_changed()
{
    ui->actionAll_Instances->setEnabled(!get_selected_frames().empty());
}

void MainWindow::undo_redo(::undo_stack<transformation>::action_type action)
{
    if (!undo_stack.can_undo(action))
    {
        assert(false);
        return;
    }

    transformation trs = undo_stack.peek_undo_item(action);
    undo_stack.undo(action);
    update_undo();
    refresh_tree();
}

std::vector<profile::frame_index_type> MainWindow::get_selected_frames() const
{
    std::vector<profile::frame_index_type> result;

    QList<QTreeWidgetItem*> selected_items = ui->treeWidget->selectedItems();
    for (auto i = selected_items.begin(); i != selected_items.end(); ++i)
    {
        my_item* citem = static_cast<my_item*>(*i);
        profile::frame_index_type findex = citem->frame_index();
        if (findex == profile::invalid_frame_index)
            continue;
        result.push_back(citem->frame_index());
    }

    return result;
}

void MainWindow::update_undo()
{
    ui->actionUndo->setEnabled(undo_stack.can_undo(::undo_stack<transformation>::action_type::undo));
    ui->actionRedo->setEnabled(undo_stack.can_undo(::undo_stack<transformation>::action_type::redo));
}

void MainWindow::refresh_tree()
{
    clear_tree();
    p.build_tree(ctx.get_root(), undo_stack.current());
    show_tree();
}

void MainWindow::clear_tree()
{
    ui->treeWidget->clear();
    ctx.set_root(0);
    my_item* root = new my_item(&ctx, profile::invalid_frame_index, "<root>", "<no module>");
    ctx.set_root(root);
}

void MainWindow::show_tree()
{
    ctx.get_root()->update_percentage();
    ui->treeWidget->addTopLevelItem(ctx.get_root());
    ctx.get_root()->expand_all_greater_than(ui->treeWidget, ctx.total_hits() / 5);
}
