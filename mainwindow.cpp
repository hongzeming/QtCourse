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
    } else if (key == "=") {
        calculate();
    } else if (key == "+" || key == "-" || key == "*" || key == "/") {
        inputOperator(key);
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
    } else if (name == QLatin1String("btnPlus")) {
        handleInput("+");
    } else if (name == QLatin1String("btnMinus")) {
        handleInput("-");
    } else if (name == QLatin1String("btnMultiply")) {
        handleInput("*");
    } else if (name == QLatin1String("btnDivide")) {
        handleInput("/");
    } else if (name == QLatin1String("btnEquals")) {
        handleInput("=");
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
    if (m_hasError) {              // 出错后按数字：整体重置后从新数字开始
        clearAll();
    }
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
    if (m_hasError) {
        clearAll();
    }
    if (m_waitingForOperand) {
        m_currentInput = "0.";
        m_waitingForOperand = false;
    } else if (!m_currentInput.contains('.')) {
        m_currentInput += '.';
    }
    // 若当前操作数已含小数点，直接忽略本次输入
    updateDisplay();
}

// ---------------------------------------------------------------------------
// 输入运算符
// ---------------------------------------------------------------------------
void MainWindow::inputOperator(const QString &op)
{
    if (m_hasError)               // 错误状态下忽略操作符，须先按 C 或数字
        return;

    if (m_pendingOperator.isEmpty()) {
        // 情况1：还没有操作符 → 当前值作为第一操作数
        m_firstOperand = m_currentInput.toDouble();
        m_pendingOperator = op;
        m_waitingForOperand = true;                 // 等待输入第二操作数
        m_expression = formatNumber(m_firstOperand) + " " + op + " ";
    } else if (m_waitingForOperand) {
        // 情况2：连续按操作符（如 1 + +）→ 替换上一个操作符
        m_pendingOperator = op;
        m_expression = formatNumber(m_firstOperand) + " " + op + " ";
    } else {
        // 情况3：已有两个操作数 → 先算出中间结果，再继续运算（连续运算）
        calculate();
        m_firstOperand = m_currentInput.toDouble();
        m_pendingOperator = op;
        m_waitingForOperand = true;
        m_expression = formatNumber(m_firstOperand) + " " + op + " ";
    }
    updateDisplay();
}

// ---------------------------------------------------------------------------
// 计算并显示结果
// ---------------------------------------------------------------------------
void MainWindow::calculate()
{
    if (m_hasError)
        return;
    if (m_pendingOperator.isEmpty())   // 没有操作符，无需计算
        return;

    // 第二操作数：若用户还没输入（如 5 + =），则以第一操作数为第二操作数
    const double second = m_waitingForOperand ? m_firstOperand : m_currentInput.toDouble();
    const double first = m_firstOperand;

    double result = 0.0;
    bool ok = true;
    if (m_pendingOperator == "+") {
        result = first + second;
    } else if (m_pendingOperator == "-") {
        result = first - second;
    } else if (m_pendingOperator == "*") {
        result = first * second;
    } else if (m_pendingOperator == "/") {
        if (second == 0.0) {         // 除数为 0 → 错误状态
            ok = false;
        } else {
            result = first / second;
        }
    }

    if (!ok) {
        m_hasError = true;
        m_waitingForOperand = true;
        m_pendingOperator.clear();
        m_expression.clear();
        m_currentInput = "错误：除数不能为0";
        updateDisplay();
        return;
    }

    // 记录完整表达式便于用户查看
    m_expression = formatNumber(first) + " " + m_pendingOperator + " "
                 + formatNumber(second) + " =";
    m_currentInput = formatNumber(result);
    m_firstOperand = result;           // 结果可作为下一次运算的第一操作数（结果后继续运算）
    m_pendingOperator.clear();
    m_waitingForOperand = true;        // 结果已显示，等待新的输入
    updateDisplay();
}

// ---------------------------------------------------------------------------
// 退格
// ---------------------------------------------------------------------------
void MainWindow::backspace()
{
    if (m_hasError)
        return;
    if (m_waitingForOperand) {
        // 等待第二操作数时退格：撤销刚输入的操作符（回到第一操作数状态）
        if (!m_pendingOperator.isEmpty()) {
            m_pendingOperator.clear();
            m_expression.clear();
        }
        return;                        // 结果状态下退格无操作
    }
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

// ---------------------------------------------------------------------------
// 数字格式化：避免浮点精度造成多余尾数（如 0.1+0.2 -> 0.3）
// ---------------------------------------------------------------------------
QString MainWindow::formatNumber(double value) const
{
    QString s = QString::number(value, 'g', 12);
    if (s == "-0")                 // 修正负零显示
        s = "0";
    return s;
}
