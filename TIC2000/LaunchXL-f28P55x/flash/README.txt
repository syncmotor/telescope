LAUNCHXL-F28P55X LED blink + GPIO0 PWM - one-click flash
Repo path: TIC2000/LaunchXL-f28P55x/flash/
Device: TMS320F28P550SJ9. Probe: XDS110. SYSCLK 150 MHz (20 MHz crystal).
Verified 2026-11-06: flash.bat flashed+verified over XDS110 (DSLite from CCS 20.3.1).
CCS 20.3.1 headless import+build: 0 errors, 0 warnings → CPU1_FLASH/blinky_flash.out (CGT 22.6.2.LTS).
LEDs verified 2026-11-06 (LED4/LED5 blink after reset, confirmed by Darshan). Scope verified 2026-11-06 (Tek DSO on GPIO0, J4.40): 20.00 kHz, high 12.50 us (25.0% duty), mean 847 mV.
1. Connect LaunchPad USB (onboard XDS110). Boot switches set for flash boot.
2. Windows: double-click flash.bat (prefers newest CCS, e.g. ccs2031).  Linux: ./flash.sh
   Scripts find UniFlash or CCS DSLite. Override: set DSLITE=<path>.
3. Press reset (or power-cycle). LED4 (GPIO20) / LED5 (GPIO21) should blink alternately, 0.5 s each.
   GPIO0 = EPWM1A on J4 pin 40 (BoosterPack site 1), 20 kHz, 25% duty, up-count
   (TBCLK=SYSCLK=150 MHz, no EPWMCLK divider; TBPRD 7499, CMPA 1875). Not GPIO8 (SPIA_PICO).
Manual UniFlash GUI: select TMS320F28P550SJ9 + XDS110, load blinky_flash.out, "Load Image".
