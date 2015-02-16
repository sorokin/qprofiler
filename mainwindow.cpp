#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include <cassert>

#include <QSettings>
#include <QFileDialog>

struct MyItem : QTreeWidgetItem
{
    MyItem(MyContext* ctx, QString const& function_name)
        : ctx(ctx)
        , hit_number(0)
    {
        this->setText(0, function_name);
    }

    void update_percentage()
    {
        setData(1, 0, QVariant((int)hit_number));
        setTextAlignment(1, Qt::AlignRight);
        setData(2, 0, QVariant(percentage()));
        setTextAlignment(2, Qt::AlignRight);
        if (percentage() < 0.5)
        {
            QBrush brush(QColor(Qt::gray));
            setForeground(0, brush);
            setForeground(1, brush);
            setForeground(2, brush);
        }
        for (std::map<QString, MyItem*>::const_iterator i = children.begin(); i != children.end(); ++i)
        {
            i->second->update_percentage();
        }

    }

    void expand_all_greater_than(QTreeWidget* widget, size_t limit)
    {
        if (hit_number <= limit)
            return;
        widget->expandItem(this);
        for (std::map<QString, MyItem*>::const_iterator i = children.begin(); i != children.end(); ++i)
        {
            i->second->expand_all_greater_than(widget, limit);
        }
    }

    void expand_all(QTreeWidget* widget)
    {
        widget->expandItem(this);
        for (std::map<QString, MyItem*>::const_iterator i = children.begin(); i != children.end(); ++i)
        {
            i->second->expand_all(widget);
        }
    }

    size_t hits()
    {
        return hit_number;
    }

    double percentage()
    {
        size_t total = ctx->total_hits();
        if (total == 0)
            return 100.;
        return 100. * hit_number / total;
    }

    void touch()
    {
        ++hit_number;
        //setText(1, QString::number(hit_number));
    }

    MyItem* push(QString const& function_name)
    {
        touch();

        MyItem* child = children[function_name];
        if (!child)
        {
            child = new MyItem(ctx, function_name);
            children[function_name] = child;
        }

        this->insertChild(childCount(), child);

        return child;
    }

private:
    MyContext* ctx;
    size_t hit_number;
    std::map<QString, MyItem*> children;
};

bool is_whitespace(char c)
{
    return c <= ' ';
}

bool is_not_whitespace(char c)
{
    return c > ' ';
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_file(QString const& file)
{
    ui->treeWidget->clear();
    ctx.set_root(0);

    MyItem* root = new MyItem(&ctx, "<root>");
    ctx.set_root(root);
    //for (size_t i = 0; i != 100; ++i)
        read_file(file);
    ctx.get_root()->update_percentage();
    ui->treeWidget->addTopLevelItem(root);
    ctx.get_root()->expand_all_greater_than(ui->treeWidget, ctx.total_hits() / 5);

    QSettings settings;
    settings.setValue("opened-file", QVariant(file));
}

bool starts_with(std::string const& a, std::string const& b)
{
    if (a.size() < b.size())
        return false;

    return std::string(a.begin(), a.begin() + b.size()) == b;
}

void MainWindow::read_file(const QString &file)
{
    std::ifstream f(file.toStdString().c_str());
    if (!f)
        return;

    for (;;)
    {
        std::string buf;
        std::getline(f, buf); // skip header
        if (!f)
            return;

        std::vector<std::string> funcs;

        for (;;)
        {
            std::getline(f, buf);
            if (!f)
                return;
            if (buf.empty())
                break;

            std::string::iterator i = std::find_if(buf.begin(), buf.end(), &is_not_whitespace);
            i = std::find_if(i, buf.end(), &is_whitespace);
            i = std::find_if(i, buf.end(), &is_not_whitespace);
            funcs.push_back(std::string(i, buf.end()));
        }

        MyItem* c = ctx.get_root();
        for (std::vector<std::string>::const_reverse_iterator i = funcs.rbegin(); i != funcs.rend(); ++i)
        {
            if (i == funcs.rbegin() && starts_with(*i, "[unknown]"))
                continue;

            c = c->push(QString::fromStdString(*i));
        }
        c->touch();
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings;
    settings.setValue("main-window-geometry", saveGeometry());
    settings.setValue("main-window-state", saveState());
}

void MainWindow::file_open_action()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Perf Output (*.txt)"));
    if (filename.size() != 0)
        open_file(filename);
}

void MainWindow::edit_expand_all_action()
{
    ctx.get_root()->expand_all(ui->treeWidget);
}


MyContext::MyContext()
{}

void MyContext::set_root(MyItem* item)
{
    root = item;
}

MyItem* MyContext::get_root()
{
    return root;
}

size_t MyContext::total_hits()
{
    assert(root);
    return root->hits();
}
