LAUNCHXL-F28P65X LED blink + GPIO8 PWM - one-click flash
Repo path: TIC2000/LaunchXL-f28P65x/flash/
Device: TMS320F28P650DK9. Probe: XDS110.
Verified 2026-11-06: flash.bat auto-picked DSLite from CCS 20.3, flashed+verified over XDS110; LED4/LED5 blink after reset.
CCS import+build verified in CCS 12.7 (CGT 22.6.1): 0 errors → CPU1_FLASH/blinky_flash.out; one warning harmless. Scope verified 2026-11-06 (Tek DSO on GPIO8): 20.00 kHz, period 50.00 us, high 12.51 us (25.0% duty), mean 835 mV.
1. Connect LaunchPad USB (onboard XDS110). Boot switches set for flash boot.
2. Windows: double-click flash.bat.  Linux: ./flash.sh
   Scripts find UniFlash (C:\ti\uniflash_* / ~/ti/uniflash_*) or CCS DSLite (C:\ti\ccs*).
   Override: set DSLITE=<path to dslite.bat / dslite.sh or DSLite.exe / DSLite>.
3. Press reset (or power-cycle). LED4 (GPIO12) / LED5 (GPIO13) blink alternately, 0.5 s each.
   GPIO8 = EPWM5A (BoosterPack pin 40), 20 kHz, 25% duty, up-count (TBPRD 9999, CMPA 2500).
Manual UniFlash GUI: select TMS320F28P650DK9 + XDS110, load blinky_flash.out, "Load Image".
