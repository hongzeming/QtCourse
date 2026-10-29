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

// ---------------------------------------------------------------------------
// 统一处理入口：鼠标点击与键盘输入共用（键盘映射在后续迭代加入）
// ---------------------------------------------------------------------------
void MainWindow::handleInput(const QString &key)
{
    if (key == "clear") {
        clearAll();
    } else if (key == "backspace") {
        backspace();
    } else if (key == ".") {
        inputDot();
    } else if (key.length() == 1 && key.at(0).isDigit()) {
        inputDigit(key);
    }
}

// ---------------------------------------------------------------------------
// 按钮点击统一槽：根据控件 objectName 转换为统一指令
// ---------------------------------------------------------------------------
void MainWindow::onButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;

    const QString name = btn->objectName();
    if (name.startsWith(QLatin1String("btnDigit"))) {
        handleInput(name.right(1));        // btnDigit7 -> "7"
    } else if (name == QLatin1String("btnDot")) {
        handleInput(".");
    } else if (name == QLatin1String("btnBackspace")) {
        handleInput("backspace");
    } else if (name == QLatin1String("btnClear")) {
        handleInput("clear");
    }
}

// ---------------------------------------------------------------------------
// 输入数字
// ---------------------------------------------------------------------------
void MainWindow::inputDigit(const QString &digit)
{
    if (m_waitingForOperand) {     // 新操作数从第一个数字开始（覆盖旧显示）
        m_currentInput = digit;
        m_waitingForOperand = false;
    } else if (m_currentInput == QLatin1String("0")) {
        m_currentInput = digit;    // 避免出现 "0123"
    } else if (m_currentInput.length() < MaxInputLength) {
        m_currentInput += digit;   // 长度限制，防止显示溢出
    }
    updateDisplay();
}

// ---------------------------------------------------------------------------
// 输入小数点：同一操作数内只允许一个小数点（重复输入被忽略）
// ---------------------------------------------------------------------------
void MainWindow::inputDot()
{
    if (m_waitingForOperand) {
        m_currentInput = "0.";
        m_waitingForOperand = false;
    } else if (!m_currentInput.contains('.')) {
        m_currentInput += '.';
    }
    // 若当前操作数已含小数点，直接忽略本次输入
    updateDisplay();
}

void MainWindow::inputOperator(const QString &op)
{
    Q_UNUSED(op);   // 运算逻辑将在后续迭代中实现
}

void MainWindow::calculate()
{
}

// ---------------------------------------------------------------------------
// 退格
// ---------------------------------------------------------------------------
void MainWindow::backspace()
{
    if (m_waitingForOperand)
        return;                    // 等待新操作数时退格无操作
    if (m_currentInput.length() > 1) {
        m_currentInput.chop(1);
    } else {
        m_currentInput = "0";
    }
    updateDisplay();
}

// ---------------------------------------------------------------------------
// 全部清除（C）
// ---------------------------------------------------------------------------
void MainWindow::clearAll()
{
    m_currentInput = "0";
    m_firstOperand = 0.0;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasError = false;
    m_expression.clear();
    updateDisplay();
}

// ---------------------------------------------------------------------------
// 刷新显示区
// ---------------------------------------------------------------------------
void MainWindow::updateDisplay()
{
    ui->display->setText(m_currentInput);
    ui->expressionLabel->setText(m_expression);
}

QString MainWindow::formatNumber(double value) const
{
    return QString::number(value, 'g', 12);
}
