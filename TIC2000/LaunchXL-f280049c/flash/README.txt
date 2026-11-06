LAUNCHXL-F280049C LED blink + J4-40 PWM - one-click flash
Repo path: TIC2000/LaunchXL-f280049c/flash/
Device: TMS320F280049C. Probe: XDS110 (cJTAG 2-pin). SYSCLK 100 MHz (20 MHz XTAL x10 /2).
Verified 2026-11-06 (Darshan AcerSync): DSLite from CCS 20.3.1 flashed+verified.
CCS 20.3.1 headless import+build (CGT 22.6.2.LTS): 0 errors, 0 warnings → CPU1_FLASH/blinky_flash.out.
Scope on J4-40: 20.00 kHz, period 50.00 us, +width 20.01 us, mean 1.33 V (docs/scope_gpio10_pwm.jpg).
LEDs: verified by Darshan 2026-11-06 (GPIO23 red / GPIO34 green alternate 0.5 s).
1. Connect LaunchPad USB (onboard XDS110). Boot from Flash: S2 at its factory default (boot from flash).
2. Windows: double-click flash.bat (prefers newest numbered C:\ti\ccs*, else C:\ti\ccs).  Linux: ./flash.sh
   Scripts find UniFlash or CCS DSLite. Override: set DSLITE=<path>.
3. Press reset (or power-cycle). LED4 (GPIO23 red) / LED5 (GPIO34 green) should blink alternately, 0.5 s each (active-low).
   J4-40 (BoosterPack site 1) = GPIO10 / EPWM6A, 20 kHz, 40% duty, up-count
   (EPWMCLK=SYSCLK=100 MHz, no divider; TBPRD 4999, CMPA 2000; high at ZRO, low at CAU).
Manual UniFlash GUI: select TMS320F280049C + XDS110, load blinky_flash.out, "Load Image".
