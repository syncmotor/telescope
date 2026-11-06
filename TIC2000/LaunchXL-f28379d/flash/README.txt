LAUNCHXL-F28379D LED blinky - one-click flash
Repo path: TIC2000/LaunchXL-f28379d/flash/
Verified 2026-11-06: flash.bat auto-picked DSLite from CCS 20.3, flashed+verified over XDS100v2; LD9/LD10 blink after reset.
1. Connect LaunchPad by USB (onboard XDS100v2). Boot switch S1: GPIO72 and GPIO84 both up (flash boot).
2. Windows: double-click flash.bat.  Linux: ./flash.sh
   Scripts find UniFlash (C:\ti\uniflash_* / ~/ti/uniflash_*) or CCS DSLite (C:\ti\ccs*).
   Override: set DSLITE=<path to dslite.bat / dslite.sh or DSLite.exe / DSLite>.
3. Press reset (or power-cycle). LD10 (GPIO31) and LD9 (GPIO34) blink alternately, 0.5 s each.
Manual UniFlash GUI: select TMS320F28379D + XDS100v2, load blinky_flash.out, "Load Image".
