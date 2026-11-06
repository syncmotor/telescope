@echo off
REM Flash blinky_flash.out to LAUNCHXL-F28P55X (XDS110) via UniFlash or CCS DSLite.
REM Prefers newest CCS install (e.g. C:\ti\ccs2031) when multiple are present.
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
set "BESTVER=0"
set "BESTDS="
for /d %%D in (C:\ti\ccs*) do (
  set "CAND="
  if exist "%%D\ccs\ccs_base\DebugServer\bin\DSLite.exe" set "CAND=%%D\ccs\ccs_base\DebugServer\bin\DSLite.exe"
  if exist "%%D\ccs_base\DebugServer\bin\DSLite.exe" set "CAND=%%D\ccs_base\DebugServer\bin\DSLite.exe"
  if not "!CAND!"=="" (
    set "VER=0"
    for /f "tokens=1 delims=" %%V in ('echo %%~nxD^| findstr /r "[0-9]"') do (
      set "RAW=%%~nxD"
      set "RAW=!RAW:ccs=!"
      set "RAW=!RAW:CCS=!"
      set "VER=!RAW!"
    )
    rem numeric compare: pad short names; ccs2031 -> 2031, ccs1270 -> 1270
    if !VER! gtr !BESTVER! (
      set "BESTVER=!VER!"
      set "BESTDS=!CAND!"
    )
  )
)
if not "!BESTDS!"=="" set "DS=!BESTDS!"
if not "!DS!"=="" goto found
echo ERROR: No UniFlash (C:\ti\uniflash_*\dslite.bat) or CCS (C:\ti\ccs*\...\DSLite.exe) found.
echo Install UniFlash, or run: set DSLITE=full\path\to\dslite.bat
pause & exit /b 1
:found
echo Using: !DS!
call "!DS!" !MODE! flash -c TMS320F28P550SJ9.ccxml -n 0 -e -f -v blinky_flash.out
if errorlevel 1 (echo ERROR: flashing failed. Check USB cable / XDS110 driver.& pause & exit /b 1)
echo DONE. Press LaunchPad reset; LED4/LED5 (GPIO20/21) blink. GPIO0=EPWM1A 20 kHz 25%%.
pause
