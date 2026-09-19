#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QAction *actNew;
    QAction *actOpen;
    QAction *actSave;
    QAction *actExit;

    QAction *actCut;
    QAction *actCopy;
    QAction *actPaste;

    QAction *actAbout;

    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;

private slots:
    void onActNew();
    void onActOpen();
    void onActSave();
    void onActExit();

    void onActCut();
    void onActCopy();
    void onActPaste();

    void onActAbout();
};
#endif // MAINWINDOW_H
