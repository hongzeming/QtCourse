#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 键盘事件：把键盘输入转换为与鼠标相同的指令
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // 所有按钮点击的统一入口（通过 objectName 区分功能）
    void onButtonClicked();

private:
    // 鼠标与键盘共用的核心处理逻辑，key 为统一指令：
    // "0"~"9"、"."、"+"/"-"/"*"/"/"、"="/"backspace"/"clear"
    void handleInput(const QString &key);

    void inputDigit(const QString &digit);
    void inputDot();
    void inputOperator(const QString &op);
    void calculate();
    void backspace();
    void clearAll();
    void updateDisplay();
    QString formatNumber(double value) const;

    Ui::MainWindow *ui;

    // ------- 计算器状态 -------
    QString m_currentInput;       // 当前正在输入的操作数（字符串保存，避免浮点显示误差）
    double  m_firstOperand = 0.0; // 第一个操作数
    QString m_pendingOperator;    // 待执行的运算符（+ - * /）
    bool    m_waitingForOperand = true; // true 表示等待输入下一个操作数
    bool    m_hasError = false;   // 是否处于错误状态（如除数为 0）
    QString m_expression;         // 顶部表达式提示文本

    static constexpr int MaxInputLength = 15; // 操作数最大长度，防止显示溢出
};

#endif // MAINWINDOW_H
