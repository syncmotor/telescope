LAUNCHXL-F28027 LED blink + J6-1 PWM - one-click flash
Repo path: TIC2000/LaunchXL-f28027/flash/
Device: TMS320F28027. Probe: XDS100v2. SYSCLKOUT 60 MHz (INTOSC1 10 MHz x12 / 2).
Build: COFF, bit-field headers (no driverlib), no FPU, rts2800_ml.lib.
Verified 2026-11-06 (Darshan AcerSync): DSLite from CCS 20.3.1 flashed+verified over XDS100v2 with S1-3 UP
  (first try failed with error -1015 until S1-3 was UP and USB was replugged).
CCS 20.3.1 headless import+build (CGT 22.6.2.LTS): 0 errors, 0 warnings → CPU1_FLASH/blinky_flash.out.
LEDs verified by Darshan: D4 (GPIO2) / D5 (GPIO3) alternate 0.5 s (active-low, GPASET/GPACLEAR).
  D2 (GPIO0) shares J6-1/EPWM1A — D2 is lit about 60% of the time (active-low; on while EPWM1A is low),
  so it looks nearly steadily on. D3 (GPIO1) held off by its pull-up.
Scope on J6-1: ~20.08 kHz, +width ~19.92 us (40%), mean 1.33 V (0.4% offset = INTOSC1 tolerance;
  docs/scope_gpio0_pwm.jpg).
1. Connect LaunchPad USB (onboard XDS100v2). S1-3 must be UP for JTAG (SPRUHH2C §4.3).
   For standalone flash boot after programming: S1 UP-UP-DOWN (SPRUHH2C §3.1).
2. Windows: double-click flash.bat (prefers newest numbered C:\ti\ccs*, else C:\ti\ccs).  Linux: ./flash.sh
   Scripts find UniFlash or CCS DSLite. Override: set DSLITE=<path>.
3. Press reset (or power-cycle). D4/D5 should blink; J6-1 = GPIO0 / EPWM1A, 20 kHz, 40% duty, up-count
   (TBCLK=60 MHz, HSPCLKDIV=CLKDIV=1; TBPRD 2999, CMPA 1200; high at ZRO, low at CAU).
Manual UniFlash GUI: select TMS320F28027 + XDS100v2, load blinky_flash.out, "Load Image".
ccxml: TMS320F28027.ccxml (XDS100v2).
