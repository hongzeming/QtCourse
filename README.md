# 带键盘事件的计算器（实验一）

东莞理工学院《Qt应用程序开发》课程实验一：设计一个带键盘事件的计算器，
支持加减乘除四则运算、小数输入、退格、清除，并可用键盘完成全部操作。

## 功能特性

- 四则运算：+、−、×、÷
- 小数输入：同一操作数防重复小数点（`3.14.` 会被忽略第二个点）
- 退格（⌫）、清除（C）
- 除数为 0 时显示"错误：除数不能为0"，按数字或 C 恢复
- 连续运算：`2+3×4=` → 20（先算中间结果）
- 连续操作符：`9++3=` → 12（替换而非重复计算）
- 结果后继续输入 / 继续运算：`2+3=` 后按 `5` 或 `×4=`
- **键盘与鼠标共用同一套处理逻辑**（handleInput 统一入口）
- 样式表美化：手机计算器风格（深色主题、橙色运算符、圆角按钮）

## 开发环境

- Windows 64 位 + Qt 6.12.0（MinGW 13.1.0 64-bit）
- CMake 3.30.5 + Ninja 构建

## 构建与运行

### 方式一：Qt Creator

1. 打开 Qt Creator → `文件` → `打开文件或项目`
2. 选择本目录下的 `CMakeLists.txt`
3. 选择 Kit：`Qt 6.12.0 (MinGW 13.1.0 64-bit)`，配置后点击运行

### 方式二：命令行

```powershell
$env:Path = "E:\Qt\6.12.0\mingw_64\bin;E:\Qt\Tools\mingw1310_64\bin;E:\Qt\Tools\CMake_64\bin;E:\Qt\Tools\Ninja;" + $env:Path
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=E:/Qt/6.12.0/mingw_64
cmake --build build
build\CalculatorKeyboard.exe
```

发布部署（将 Qt 运行库复制到 exe 目录，双击即可运行）：

```powershell
E:\Qt\6.12.0\mingw_64\bin\windeployqt.exe build\CalculatorKeyboard.exe --release
```

## 键盘操作说明

| 键盘按键 | 功能 |
|---|---|
| 0-9（含小键盘） | 数字输入 |
| . 或 , | 小数点 |
| + - * / | 四则运算 |
| = 或 回车 | 计算结果 |
| Backspace | 退格 |
| Esc 或 Delete | 清除（C） |

## 测试

自动化测试脚本 `test_calculator.ps1`（键盘输入用 SendKeys，鼠标输入用 UIA，
显示验证用 UIA ValuePattern）：

```powershell
# 先运行程序，然后执行：
powershell -ExecutionPolicy Bypass -File test_calculator.ps1
```

测试结果见 `test_results.txt`：共 19 项用例全部通过（PASS=19, FAIL=0），
覆盖四则运算、小数、退格、清除、除零、连续运算、连续操作符、
结果后继续输入、鼠标与键盘混合输入等场景。

运行状态截图见 `screenshots/` 目录。

## Git 提交历史（7 次，体现代码迭代与问题修复）

1. 工程初始化：创建 Qt Widgets 计算器框架与 UI 设计
2. 实现数字/小数点/退格/清除输入与显示更新
3. 实现四则运算与计算状态管理
4. 修复异常输入与边界处理问题（测试发现 9++3 计算错误等 4 个问题并修复）
5. 实现键盘事件，与鼠标输入复用同一套处理逻辑
6. 使用样式表改善界面（手机计算器风格）
7. 添加自动化测试脚本、运行截图与文档
