@echo off
REM Flash blinky_flash.out to LAUNCHXL-F28P65X (XDS110) via UniFlash or CCS DSLite.
setlocal enabledelayedexpansion
cd /d "%~dp0"
set "DS=%DSLITE%"
set "MODE="
if not "%DS%"=="" (
  echo "%DS%" | findstr /i "dslite.bat" >nul && set "MODE=--mode"
  goto found
)
for /d %%D in (C:\ti\uniflash_*) do if exist "%%D\dslite.bat" set "DS=%%D\dslite.bat"& set "MODE=--mode"
if not "!DS!"=="" goto found
for /d %%D in (C:\ti\ccs*) do (
  if exist "%%D\ccs\ccs_base\DebugServer\bin\DSLite.exe" set "DS=%%D\ccs\ccs_base\DebugServer\bin\DSLite.exe"
  if exist "%%D\ccs_base\DebugServer\bin\DSLite.exe" set "DS=%%D\ccs_base\DebugServer\bin\DSLite.exe"
)
if not "!DS!"=="" goto found
echo ERROR: No UniFlash (C:\ti\uniflash_*\dslite.bat) or CCS (C:\ti\ccs*\ccs\ccs_base\DebugServer\bin\DSLite.exe) found.
echo Install UniFlash, or run: set DSLITE=full\path\to\dslite.bat
pause & exit /b 1
:found
echo Using: !DS!
call "!DS!" !MODE! flash -c TMS320F28P650DK9.ccxml -n 0 -e -f -v blinky_flash.out
if errorlevel 1 (echo ERROR: flashing failed. Check USB cable / XDS110 driver.& pause & exit /b 1)
echo DONE. Press the LaunchPad reset button; LED4/LED5 (GPIO12/13) should blink. GPIO8 = EPWM5A 20 kHz 25%% duty.
pause
