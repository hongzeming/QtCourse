#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentInput("0")
{
    ui->setupUi(this);

    // 按钮统一设置与连接：所有按键共用 onButtonClicked
    const auto buttons = centralWidget()->findChildren<QPushButton*>();
    for (QPushButton *btn : buttons) {
        btn->setFocusPolicy(Qt::NoFocus);   // 键盘事件统一由窗口处理
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(btn, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    }
    // 数字显示区不抢焦点
    ui->display->setFocusPolicy(Qt::NoFocus);

    // 让主窗口持有键盘焦点
    setFocusPolicy(Qt::StrongFocus);
    centralWidget()->setFocusPolicy(Qt::StrongFocus);
    centralWidget()->setFocus();

    updateDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 键盘事件将在后续迭代中实现
    QMainWindow::keyPressEvent(event);
}

void MainWindow::handleInput(const QString &key)
{
    Q_UNUSED(key);   // 计算逻辑将在后续迭代中实现
}

void MainWindow::onButtonClicked()
{
    // 按钮映射将在后续迭代中实现
}

void MainWindow::inputDigit(const QString &digit)
{
    Q_UNUSED(digit);
}

void MainWindow::inputDot()
{
}

void MainWindow::inputOperator(const QString &op)
{
    Q_UNUSED(op);
}

void MainWindow::calculate()
{
}

void MainWindow::backspace()
{
}

void MainWindow::clearAll()
{
    m_currentInput = "0";
    m_waitingForOperand = true;
    m_hasError = false;
    m_expression.clear();
    updateDisplay();
}

void MainWindow::updateDisplay()
{
    ui->display->setText(m_currentInput);
    ui->expressionLabel->setText(m_expression);
}

QString MainWindow::formatNumber(double value) const
{
    return QString::number(value, 'g', 12);
}
