# ============================================================
# 抓取计算器各运行状态截图（实验报告素材）
# 状态：初始 / 输入表达式 / 运算结果 / 除零错误 / 清除后
# ============================================================

Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName UIAutomationClient
Add-Type -AssemblyName UIAutomationTypes
Add-Type @'
using System;
using System.Runtime.InteropServices;
public class CapWin {
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr hwnd, IntPtr hdcBlt, uint nFlags);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hwnd, out RECT rect);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
}
'@

$ws = New-Object -ComObject WScript.Shell
$proc = Get-Process -Name "CalculatorKeyboard" | Where-Object { $_.MainWindowTitle -like "*计算器*" } | Select-Object -First 1
if (-not $proc) { Write-Output "FATAL: 找不到计算器进程"; exit 1 }
$outDir = "C:\Users\洪泽明\Doubao\chats\2026-10-29\new-chat\CalculatorKeyboard\screenshots"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

function Capture-Window([string]$outFile) {
    $h = $proc.MainWindowHandle
    $cr = New-Object CapWin+RECT
    [CapWin]::GetClientRect($h, [ref]$cr) | Out-Null
    $w = $cr.Right - $cr.Left; $ht = $cr.Bottom - $cr.Top
    $bmp = New-Object System.Drawing.Bitmap($w, $ht)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $g.GetHdc()
    [CapWin]::PrintWindow($h, $hdc, 2) | Out-Null
    $g.ReleaseHdc($hdc)
    $bmp.Save($outFile, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
}

function Foreground {
    [CapWin]::ShowWindow($proc.MainWindowHandle, 9) | Out-Null
    [CapWin]::SetForegroundWindow($proc.MainWindowHandle) | Out-Null
    Start-Sleep -Milliseconds 200
}

# 状态1：初始界面
Foreground
$ws.SendKeys("{ESC}")
Start-Sleep -Milliseconds 400
Capture-Window "$outDir\01_initial.png"
Write-Output "01_initial.png captured"

# 状态2：输入表达式 12+34
$ws.SendKeys("12{+}34")
Start-Sleep -Milliseconds 500
Capture-Window "$outDir\02_input_expr.png"
Write-Output "02_input_expr.png captured"

# 状态3：运算结果 46
$ws.SendKeys("=")
Start-Sleep -Milliseconds 500
Capture-Window "$outDir\03_result.png"
Write-Output "03_result.png captured"

# 状态4：除零错误
$ws.SendKeys("{ESC}5/0=")
Start-Sleep -Milliseconds 500
Capture-Window "$outDir\04_divide_by_zero.png"
Write-Output "04_divide_by_zero.png captured"

# 状态5：清除后
$ws.SendKeys("{ESC}")
Start-Sleep -Milliseconds 400
Capture-Window "$outDir\05_cleared.png"
Write-Output "05_cleared.png captured"
