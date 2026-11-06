LAUNCHXL-F280025C LED blink + J4-40 PWM - one-click flash
Repo path: TIC2000/LaunchXL-f280025c/flash/
Device: TMS320F280025C. Probe: XDS110 (cJTAG 2-pin). SYSCLK 100 MHz (20 MHz XTAL Y2 x30 / 6).
Verified 2026-11-06 (Darshan AcerSync): DSLite from CCS 20.3.1 flashed+verified.
CCS 20.3.1 headless import+build (CGT 22.6.2.LTS): 0 errors, 0 warnings → CPU1_FLASH/blinky_flash.out.
LEDs verified by Darshan: LED4 (GPIO31 red) / LED5 (GPIO34 green) alternate 0.5 s (active-low, GPIO_writePin).
Scope on J4-40: 20.00 kHz, +width 20.01 us, mean 1.34 V (docs/scope_gpio0_pwm.jpg).
1. Connect LaunchPad USB (onboard XDS110). Boot from flash: S3 at its factory default (GPIO24=1, GPIO32=1 → boot from flash) — SPRUIW8 §3.1.7 Table 3-1.
   S4 must not route GPIO32 to the CAN transceiver (SPRUIW8 §3.3.4); that can pull the boot pin.
2. Windows: double-click flash.bat (prefers newest numbered C:\ti\ccs*, else C:\ti\ccs).  Linux: ./flash.sh
   Scripts find UniFlash or CCS DSLite. Override: set DSLITE=<path>.
3. Press reset (or power-cycle). LED4/LED5 blink; J4-40 = GPIO0 / EPWM1A, 20 kHz, 40% duty, up-count
   (EPWMCLK=SYSCLK=100 MHz, no divider; TBPRD 4999, CMPA 2000; high at ZRO, low at CAU).
Manual UniFlash GUI: select TMS320F280025C + XDS110, load blinky_flash.out, "Load Image".
ccxml: TMS320F280025C.ccxml (XDS110 cJTAG).
