LAUNCHXL-F28379D LED blinky - one-click flash
1. Connect the LaunchPad by USB (onboard XDS100v2). Boot switches S1: default (all up / boot-from-flash).
2. Windows: double-click flash.bat.  Linux: ./flash.sh
   Scripts find UniFlash (C:\ti\uniflash_* / ~/ti/uniflash_*) or CCS DSLite (C:\ti\ccs*).
   Override: set DSLITE=<path to dslite.bat or DSLite.exe>.
3. Press reset (or power-cycle). LD10 (GPIO31) and LD9 (GPIO34) blink alternately, 0.5 s each.
Manual UniFlash GUI: select TMS320F28379D + XDS100v2, load blinky_flash.out, "Load Image".
