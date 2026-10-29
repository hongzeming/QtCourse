# ============================================================
# 计算器自动化测试脚本（实验1：带键盘事件的计算器）
# 键盘输入：SendKeys 发送到前台窗口（模拟真实按键）
# 鼠标输入：UIA InvokePattern 触发按钮（模拟鼠标点击）
# 显示验证：UIA ValuePattern 读取显示区文本
# 运行：powershell -ExecutionPolicy Bypass -File test_calculator.ps1
# ============================================================

Add-Type -AssemblyName UIAutomationClient
Add-Type -AssemblyName UIAutomationTypes
Add-Type @'
using System;
using System.Runtime.InteropServices;
public class FGWin {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
}
'@

$ws = New-Object -ComObject WScript.Shell
$proc = Get-Process -Name "CalculatorKeyboard" | Where-Object { $_.MainWindowTitle -like "*计算器*" } | Select-Object -First 1
if (-not $proc) { Write-Output "FATAL: 找不到计算器进程"; exit 1 }

$results = [System.Collections.ArrayList]::new()
$pass = 0; $fail = 0

function Ensure-Foreground {
    [FGWin]::ShowWindow($proc.MainWindowHandle, 9) | Out-Null
    [FGWin]::SetForegroundWindow($proc.MainWindowHandle) | Out-Null
    Start-Sleep -Milliseconds 200
}

function Get-Display {
    $root = [System.Windows.Automation.AutomationElement]::FromHandle($proc.MainWindowHandle)
    $cond = New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::ControlTypeProperty, [System.Windows.Automation.ControlType]::Edit)
    $edit = $root.FindFirst([System.Windows.Automation.TreeScope]::Descendants, $cond)
    $vp = $edit.GetCurrentPattern([System.Windows.Automation.ValuePattern]::Pattern)
    return $vp.Current.Value
}

# 键盘输入测试用例
function Test-Keyboard([string]$name, [string]$keys, [string]$expected) {
    Ensure-Foreground
    $ws.SendKeys("{ESC}")           # 先清除，保证初始状态
    Start-Sleep -Milliseconds 300
    $ws.SendKeys($keys)
    Start-Sleep -Milliseconds 500
    $actual = Get-Display
    $ok = ($actual -eq $expected)
    if ($ok) { $script:pass++ } else { $script:fail++ }
    [void]$results.Add("$name|$keys|$expected|$actual|$(if($ok){'PASS'}else{'FAIL'})")
    Write-Output "$name : 期望=[$expected] 实际=[$actual] -> $(if($ok){'PASS'}else{'FAIL'})"
}

# 鼠标点击输入（UIA Invoke 模拟鼠标点击按钮）
function Click-Button([string]$btnName) {
    Ensure-Foreground
    $root = [System.Windows.Automation.AutomationElement]::FromHandle($proc.MainWindowHandle)
    $cond = New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, $btnName)
    $btn = $root.FindFirst([System.Windows.Automation.TreeScope]::Descendants, $cond)
    if (-not $btn) { Write-Output "  !! 找不到按钮 [$btnName]"; return $false }
    $invoke = $btn.GetCurrentPattern([System.Windows.Automation.InvokePattern]::Pattern)
    $invoke.Invoke()
    Start-Sleep -Milliseconds 250
    return $true
}

function Test-Mouse([string]$name, [string[]]$clicks, [string]$expected) {
    Ensure-Foreground
    Click-Button "C"                     # 清除
    Start-Sleep -Milliseconds 200
    foreach ($c in $clicks) { Click-Button $c }
    Start-Sleep -Milliseconds 400
    $actual = Get-Display
    $ok = ($actual -eq $expected)
    if ($ok) { $script:pass++ } else { $script:fail++ }
    [void]$results.Add("$name|(鼠标点击 $($clicks -join ','))|$expected|$actual|$(if($ok){'PASS'}else{'FAIL'})")
    Write-Output "$name : 期望=[$expected] 实际=[$actual] -> $(if($ok){'PASS'}else{'FAIL'})"
}

Write-Output "======== 键盘输入测试（实验步骤五：键盘事件处理）========"
Test-Keyboard "数字输入"          "12"          "12"
Test-Keyboard "加法运算"          "12{+}34="    "46"
Test-Keyboard "减法运算"          "50-25="      "25"
Test-Keyboard "乘法运算"          "6*7="        "42"
Test-Keyboard "除法运算"          "81/9="       "9"
Test-Keyboard "小数加法"          "1.5{+}2.25=" "3.75"
Test-Keyboard "连续小数点被忽略"  "3.14."       "3.14"
Test-Keyboard "退格键"            "123{BS}"     "12"
Test-Keyboard "Esc清除"           "12{ESC}"     "0"
Test-Keyboard "除零处理"          "5/0="        "错误：除数不能为0"
Test-Keyboard "错误后数字重置"    "7"           "7"
Test-Keyboard "连续运算中间结果"  "2{+}3{*}4="  "20"
Test-Keyboard "连续操作符替换"    "9{+}{+}3="   "12"
Test-Keyboard "结果后继续输入"    "2{+}3=5"     "5"
Test-Keyboard "结果后继续运算"    "2{+}3={*}4=" "20"
Test-Keyboard "小数末尾点"        "5."          "5."

Write-Output ""
Write-Output "======== 鼠标点击测试（与键盘共用同一套逻辑）========"
Test-Mouse "鼠标四则运算" @("1","+","2","=") "3"
Test-Mouse "鼠标与键盘混合" @("4","+") "4"    # 仅验证前半段（键盘补后半段见下）

# 混合输入：鼠标 4 + 键盘 + 键盘 5 + 鼠标 = （期望 9）
Ensure-Foreground
Click-Button "C"
Click-Button "4"
$ws.SendKeys("{+}5")
Start-Sleep -Milliseconds 300
Click-Button "="
$actual = Get-Display
$ok = ($actual -eq "9")
if ($ok) { $script:pass++ } else { $script:fail++ }
[void]$results.Add("鼠标+键盘混合输入|鼠标4+键盘+5+鼠标=|9|$actual|$(if($ok){'PASS'}else{'FAIL'})")
Write-Output "鼠标+键盘混合输入 : 期望=[9] 实际=[$actual] -> $(if($ok){'PASS'}else{'FAIL'})"

Write-Output ""
Write-Output "======== 汇总：共 $($pass + $fail) 项，PASS=$pass，FAIL=$fail ========"

# 保存测试记录
$outFile = "C:\Users\洪泽明\Doubao\chats\2026-10-29\new-chat\CalculatorKeyboard\test_results.txt"
$lines = [System.Collections.ArrayList]::new()
[void]$lines.Add("计算器自动化测试结果  时间: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')")
[void]$lines.Add("")
foreach ($r in $results) { [void]$lines.Add(($r -replace "\|", " | ")) }
[void]$lines.Add("")
[void]$lines.Add("汇总：共 $($pass + $fail) 项，PASS=$pass，FAIL=$fail")
[System.IO.File]::WriteAllLines($outFile, $lines, (New-Object System.Text.UTF8Encoding($false)))
Write-Output "测试记录已保存到: $outFile"
