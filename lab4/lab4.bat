@echo off
setlocal

set OUT_DIR=C:\SetiTelekomunikacii\lab4
set OUT_OEM=%OUT_DIR%\lab4_results_oem.txt
set OUT_UTF=%OUT_DIR%\lab4_results.txt

chcp 866 >nul

echo === Лабораторная работа №4. Сбор результатов утилит === > "%OUT_OEM%"
echo Дата: %DATE% %TIME% >> "%OUT_OEM%"
echo. >> "%OUT_OEM%"

call :run ipconfig
call :run "ipconfig /all"
call :run "arp -a"
call :run "ipconfig /displaydns"
call :run "ipconfig /allcompartments"
call :run getmac
call :run "nbtstat -n"
call :run "netstat -an"
call :run "net share"
call :run "net view"
call :run "net accounts"
call :run "net localgroup administrators"
call :run "net user"
call :run "route print"
call :run "nslookup google.com"
call :run "nslookup yandex.ru 8.8.8.8"
call :run "ping -a -n 1 -r 9 -w 1000 yandex.ru"
call :run "ping -a -n 1 -r 9 -w 1000 google.com"
call :run "tracert google.com"

echo. >> "%OUT_OEM%"
echo === Сбор завершён === >> "%OUT_OEM%"

REM --- Конвертация CP866 -> UTF-8 ---
powershell -NoProfile -Command "Get-Content -Path '%OUT_OEM%' -Encoding OEM | Set-Content -Path '%OUT_UTF%' -Encoding UTF8"

echo.
echo Готово. Результаты:
echo   OEM 866 (исходный): %OUT_OEM%
echo   UTF-8 (читаемый):   %OUT_UTF%
pause
exit /b

:run
echo. >> "%OUT_OEM%"
echo ============================================================ >> "%OUT_OEM%"
echo  ^>^> %~1 >> "%OUT_OEM%"
echo ============================================================ >> "%OUT_OEM%"
%~1 >> "%OUT_OEM%" 2>&1
goto :eof