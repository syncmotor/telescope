@echo off
REM Flash blinky_flash.out to LAUNCHXL-F28027 (XDS100v2) via UniFlash or CCS DSLite.
REM Prefers newest numbered CCS (e.g. C:\ti\ccs2031); falls back to C:\ti\ccs (no digits).
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
set "FALLBACK="
for /d %%D in (C:\ti\ccs*) do (
  set "CAND="
  if exist "%%D\ccs\ccs_base\DebugServer\bin\DSLite.exe" set "CAND=%%D\ccs\ccs_base\DebugServer\bin\DSLite.exe"
  if exist "%%D\ccs_base\DebugServer\bin\DSLite.exe" set "CAND=%%D\ccs_base\DebugServer\bin\DSLite.exe"
  if not "!CAND!"=="" (
    set "RAW=%%~nxD"
    set "VER=!RAW:ccs=!"
    set "VER=!VER:CCS=!"
    set "NUM=0"
    set /a "NUM=!VER!+0" 2>nul
    if !NUM! leq 0 (
      rem Folder has no digits (e.g. C:\ti\ccs) — keep as fallback only
      if "!FALLBACK!"=="" set "FALLBACK=!CAND!"
    ) else (
      if !NUM! gtr !BESTVER! (
        set "BESTVER=!NUM!"
        set "BESTDS=!CAND!"
      )
    )
  )
)
if not "!BESTDS!"=="" set "DS=!BESTDS!"
if "!DS!"=="" if not "!FALLBACK!"=="" set "DS=!FALLBACK!"
if not "!DS!"=="" goto found
echo ERROR: No UniFlash or CCS DSLite found under C:\ti\uniflash_* / C:\ti\ccs* / C:\ti\ccs.
echo Install UniFlash, or run: set DSLITE=full\path\to\dslite.bat
pause & exit /b 1
:found
echo Using: !DS!
call "!DS!" !MODE! flash -c TMS320F28027.ccxml -n 0 -e -f -v blinky_flash.out
if errorlevel 1 (echo ERROR: flashing failed. Check USB / XDS100v2 ^(S1 switch 3 UP for JTAG^).& pause & exit /b 1)
echo DONE. Press reset; LED D4/D5 ^(GPIO2/3^) blink. J6-1=GPIO0/EPWM1A 20 kHz 40%%.
pause
