LAUNCHXL-F28069M LED blink + J4-40 PWM - one-click flash
Repo path: TIC2000/LaunchXL-f28069m/flash/
Device: TMS320F28069M (ccxml/CCS id TMS320F28069). Probe: XDS100v2. SYSCLKOUT 90 MHz (INTOSC1 10 MHz x18 / 2; Q1 not fitted).
Build: COFF, bit-field headers (no driverlib), fpu32 + vcu0, rts2800_fpu32.lib.
Verified 2026-11-06 (Darshan AcerSync): DSLite from CCS 20.3.1 flashed+verified over XDS100v2.
CCS 20.3.1 headless import+build (CGT 22.6.2.LTS): 0 errors, 0 warnings → CPU1_FLASH/blinky_flash.out.
LEDs: D9 verified; D10 not blinking, open issue (suspected HW, to be checked).
  D9=GPIO34 (red) / D10=GPIO39 (blue), active-low, 0.5 s, driven with GPBSET/GPBCLEAR.
Scope on J4-40: 20.12 kHz, period 49.71 us, mean 1.31 V, 40% duty (+0.6% INTOSC; docs/scope_gpio0_pwm.jpg).
1. Connect LaunchPad USB (onboard XDS100v2). S1-3 must be UP for JTAG. Per SPRUI11B §5.3 Table 2 note,
   S1 UP-UP-UP also boots to flash when the XDS100v2 isn't connected.
2. Windows: double-click flash.bat (prefers newest numbered C:\ti\ccs*, else C:\ti\ccs).  Linux: ./flash.sh
   Scripts find UniFlash or CCS DSLite. Override: set DSLITE=<path>.
3. Press reset (or power-cycle). D9 should blink; J4-40 = GPIO0 / EPWM1A, 20 kHz, 40% duty, up-count
   (TBCLK=90 MHz, HSPCLKDIV=CLKDIV=1; TBPRD 4499, CMPA 1800; high at ZRO, low at CAU).
Manual UniFlash GUI: select TMS320F28069 + XDS100v2, load blinky_flash.out, "Load Image".
