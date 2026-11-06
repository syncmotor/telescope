LAUNCHXL-F2800157 LED blink + J4-40 PWM - one-click flash
Repo path: TIC2000/LaunchXL-f2800157/flash/
Device: TMS320F2800157. Probe: XDS110 (cJTAG 2-pin). SYSCLK 120 MHz (20 MHz XTAL x48 / 8).
Verified 2026-11-06 (Darshan AcerSync): DSLite from CCS 20.3.1 flashed+verified.
CCS 20.3.1 headless import+build (CGT 22.6.2.LTS): 0 errors, 0 warnings → CPU1_FLASH/blinky_flash.out.
LEDs verified by Darshan: LED4 (GPIO35 red) / LED5 (GPIO49 green) alternate 0.5 s (active-low).
  Note: SPRUJ54A §3.1.3 saying GPIO20/22 is wrong; schematic/SDK/E2E 1253487 → 35/49.
  GPIO35 defaults to JTAG TDI; main.c muxes it to GPIO (fine with cJTAG 2-pin).
Scope on J4-40: 20.00 kHz, period 50.00 us, +width 20.00 us, mean 1.35 V (docs/scope_gpio30_pwm.jpg).
1. Connect LaunchPad USB (onboard XDS110). Boot: S3 at its default (boot from flash).
2. Windows: double-click flash.bat (prefers newest numbered C:\ti\ccs*, else C:\ti\ccs).  Linux: ./flash.sh
   Scripts find UniFlash or CCS DSLite. Override: set DSLITE=<path>.
3. Press reset (or power-cycle). LED4/LED5 blink; J4-40 = GPIO30 / EPWM1A, 20 kHz, 40% duty, up-count
   (EPWMCLK=SYSCLK=120 MHz, no divider; TBPRD 5999, CMPA 2400; high at ZRO, low at CAU).
Manual UniFlash GUI: select TMS320F2800157 + XDS110, load blinky_flash.out, "Load Image".
