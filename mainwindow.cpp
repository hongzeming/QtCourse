#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    actNew = new QAction(tr("新建(&N)"), this);
    actNew->setShortcut(QKeySequence::New);
    actNew->setStatusTip(tr("新建文件"));

    actOpen = new QAction(tr("打开(&O)"), this);
    actOpen->setShortcut(QKeySequence::Open);
    actOpen->setStatusTip(tr("打开文件"));

    actSave = new QAction(tr("保存(&S)"), this);
    actSave->setShortcut(QKeySequence::Save);
    actSave->setStatusTip(tr("保存文件"));

    actExit = new QAction(tr("退出(&X)"), this);
    actExit->setStatusTip(tr("退出程序"));

    actCut = new QAction(tr("剪切(&T)"), this);
    actCut->setShortcut(QKeySequence::Cut);
    actCut->setStatusTip(tr("剪切选中内容"));

    actCopy = new QAction(tr("复制(&C)"), this);
    actCopy->setShortcut(QKeySequence::Copy);
    actCopy->setStatusTip(tr("复制选中内容"));

    actPaste = new QAction(tr("粘贴(&P)"), this);
    actPaste->setShortcut(QKeySequence::Paste);
    actPaste->setStatusTip(tr("粘贴内容"));

    actAbout = new QAction(tr("关于(&A)"), this);
    actAbout->setStatusTip(tr("查看开发信息"));

    //菜单栏
    menuFile = menuBar()->addMenu(tr("文件(&F)"));
    menuFile->addAction(actNew);
    menuFile->addAction(actOpen);
    menuFile->addAction(actSave);
    menuFile->addSeparator();
    menuFile->addAction(actExit);

    menuEdit = menuBar()->addMenu(tr("编辑(&E)"));
    menuEdit->addAction(actCut);
    menuEdit->addAction(actCopy);
    menuEdit->addAction(actPaste);

    menuHelp = menuBar()->addMenu(tr("帮助(&H)"));
    menuHelp->addAction(actAbout);

    // ==========这里是修改后的工具栏代码==========
    QToolBar *toolBar = addToolBar("mainToolBar");
    toolBar->addAction(actNew);
    toolBar->addAction(actOpen);
    toolBar->addAction(actSave);
    toolBar->addSeparator();
    toolBar->addAction(actCut);
    toolBar->addAction(actCopy);
    toolBar->addAction(actPaste);
    toolBar->addSeparator();
    toolBar->addAction(actAbout);

    //绑定信号槽
    connect(actNew, &QAction::triggered, this, &MainWindow::onActNew);
    connect(actOpen, &QAction::triggered, this, &MainWindow::onActOpen);
    connect(actSave, &QAction::triggered, this, &MainWindow::onActSave);
    connect(actExit, &QAction::triggered, this, &MainWindow::onActExit);

    connect(actCut, &QAction::triggered, this, &MainWindow::onActCut);
    connect(actCopy, &QAction::triggered, this, &MainWindow::onActCopy);
    connect(actPaste, &QAction::triggered, this, &MainWindow::onActPaste);

    connect(actAbout, &QAction::triggered, this, &MainWindow::onActAbout);

    statusBar()->showMessage(tr("就绪"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onActNew()
{
    statusBar()->showMessage(tr("新建文件"));
}
void MainWindow::onActOpen()
{
    statusBar()->showMessage(tr("打开文件"));
}
void MainWindow::onActSave()
{
    statusBar()->showMessage(tr("保存文件"));
}
void MainWindow::onActExit()
{
    close();
}

void MainWindow::onActCut()
{
    statusBar()->showMessage(tr("剪切"));
}
void MainWindow::onActCopy()
{
    statusBar()->showMessage(tr("复制"));
}
void MainWindow::onActPaste()
{
    statusBar()->showMessage(tr("粘贴"));
}

void MainWindow::onActAbout()
{
    QMessageBox::about(this, tr("samp2_4"),
                       tr("开发人员信息：\n姓名：洪泽明\n学号：xxxxxx"));
}
