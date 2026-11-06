# F28027 LED blink + J6-1 PWM (LAUNCHXL-F28027, runs from flash)

Repo path: `TIC2000/LaunchXL-f28027/`.

User LEDs **D4/D5** (GPIO2/GPIO3) are driven alternately every 0.5 s, and **J6-1** runs a 20 kHz / 40% duty PWM. The program keeps running from flash after power-up. Device **TMS320F28027**, onboard **XDS100v2**, FLASH build, **COFF** ABI with bit-field headers (**no driverlib**), fixed-point `rts2800_ml.lib` (no FPU). SYSCLKOUT is **60 MHz** (INTOSC1 10 MHz × 12 / 2). Crystal footprint is optional (SPRUHH2C §4.4).

**J6-1** = **GPIO0** = **EPWM1A**. Cited: SPRUHH2C Table 1 (J6 pin 1 → GPIO0 / EPWM1A); C2000Ware `f2802x_epwm.c` `InitEPwm1Gpio` (`GPAMUX1.GPIO0 = 1`). TBCLK = SYSCLKOUT / (HSPCLKDIV×CLKDIV) with both = 1 → **60 MHz**.

> **Hardware status (2026-11-06, Darshan's AcerSync):** DSLite from CCS **20.3.1** flashed and verified over XDS100v2 with **S1-3 UP** (first try failed with error **-1015** until S1-3 was UP and the USB was replugged). CCS **20.3.1** headless import + full build (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, output `CPU1_FLASH/blinky_flash.out`. **LEDs verified** by Darshan (D4/D5 alternating after reset). **Scope verified on J6-1:** ~20.08 kHz, positive width ~19.92 µs (40%), mean 1.33 V — the 0.4% offset is INTOSC1 tolerance; see [docs/scope_gpio0_pwm.jpg](docs/scope_gpio0_pwm.jpg).

## Software versions

| Component | Version |
|-----------|---------|
| CCS (HW flash/import, Darshan PC) | **20.3.1** (`C:\ti\ccs2031`, preferred) |
| CGT (CCS 20.3.1 on Darshan PC) | **22.6.2.LTS** |
| DSLite (flash verified) | from **CCS 20.3.1** (`C:\ti\ccs2031\...`) |
| CGT (box / `build.sh`) | 22.6.1.LTS |
| C2000Ware | v26.02.00.00.LTS (`REL_C2000Ware_v26.02.00.00.LTS`) |
| Mirror | TexasInstruments/c2000ware-core-sdk @ `699b8c6aa190cf7430a36bdcf7f5b1ea2e928063` |
| Device support | `device_support/f2802x` (bit-field headers; **no driverlib**) |
| Device | TMS320F28027 |
| Probe | XDS100v2 (LaunchPad onboard) |
| ABI / RTS | COFF / `rts2800_ml.lib` |

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB (onboard XDS100v2).
2. **S1 switch 3 (TRSTn) must be UP** for JTAG/DSLite (SPRUHH2C §4.3 and FAQ §7.3). For standalone flash boot after programming, use **S1 UP-UP-DOWN** (GPIO34=1, GPIO37=1, TRSTn=0) — the demo-boot setting in SPRUHH2C §3.1, which selects GetMode → flash when OTP is default (see also SPRUFN6).
3. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat` (prefers newest numbered `C:\ti\ccs*`, e.g. `ccs2031`; falls back to `C:\ti\ccs`).
   - **Linux:** `cd flash && ./flash.sh`
4. When it prints `DONE`, press the LaunchPad reset button (or unplug/replug USB). D4/D5 should alternate; J6-1 should already be toggling PWM.

The script finds the flash tool by itself (UniFlash first, then newest CCS DSLite):
- Windows: `C:\ti\uniflash_*\dslite.bat`, then newest numbered `C:\ti\ccs*\...\DSLite.exe` (CCS **20.3.1** used on the verified flash)
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override** if the install is elsewhere or the wrong version is picked:
- Windows (Command Prompt, in `flash\`): `set DSLITE=C:\ti\ccs2031\ccs\ccs_base\DebugServer\bin\DSLite.exe` then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases, programs, and verifies (`-e -f -v`) using `flash/TMS320F28027.ccxml` (XDS100v2).

**Manual UniFlash GUI:** pick TMS320F28027 + XDS100v2, load `flash/blinky_flash.out`, click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`TIC2000/LaunchXL-f28027`). It already has `.project`, `.cproject`, and `.ccsproject`.
2. Select the **CPU1_FLASH** build configuration, build, then Debug or Flash. Target config: `targetConfigs/TMS320F28027.ccxml`. Output: `CPU1_FLASH/blinky_flash.out`.

Verified with a headless import + full build in **CCS 20.3.1** (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, produced `CPU1_FLASH/blinky_flash.out`. Prefer CCS 20.3.1 on Darshan's system.

**If that import fails or the project looks empty:** import `blinky_flash.projectspec` the same way. That copies sources into your workspace, so edits there won't change this folder.

Needs a **C2000 22.6.x.LTS** compiler and `rts2800_ml.lib` (**COFF**, no FPU). Install via **Help > Install Code Generation Compiler Tools** if CCS asks.

**Linker note:** the CCS `.cproject` does not pass `F2802x_Headers_nonBIOS.cmd` explicitly. CCS auto-links every `.cmd` in the project root, and listing that file a second time caused memory-range errors on sibling boards. Keep `F28027.cmd` as the only `LINKER_COMMAND_FILE`; leave `F2802x_Headers_nonBIOS.cmd` in the project root for auto-link.

Command-line build: `./build.sh` (set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`). Writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see

**LEDs (verified)**
- **D4** (GPIO2) and **D5** (GPIO3) are driven alternately: one on for 0.5 s, then the other (1 s full cycle). Active-low through SN74LVC2G07 (U2/U3); the code writes `0` (via `GPACLEAR`) to turn one on and `1` (via `GPASET`) to turn it off.
- Evidence for GPIO0..3 → D2..D5: SPRUHH2C Figure 3 schematic; C2000Ware `Example_F2802xLaunchPadDemo.c`.
- **Pin conflict:** J6-1 / EPWM1A is GPIO0, which is also LED **D2**. PWM owns GPIO0, so D2 is not blinked — **D2 is lit about 60% of the time (active-low; on while EPWM1A is low), so it looks nearly steadily on**. **D3 (GPIO1) held off by its pull-up**. Only D4/D5 alternate.

**PWM on J6-1 (scope verified)**
- **J6-1** = **GPIO0** = **EPWM1A**.
- **20 kHz**, **40%** duty, up-count.
- TBCLK = **60 MHz** (HSPCLKDIV = CLKDIV = 1).
- **TBPRD = 2999**, **CMPA = 1200**.
- Action: high at ZRO, low at CAU.
- **Scope (2026-11-06):** ~20.08 kHz, positive width ~19.92 µs (40%), mean 1.33 V. The 0.4% offset from nominal 20 kHz is INTOSC1 tolerance. Capture: [docs/scope_gpio0_pwm.jpg](docs/scope_gpio0_pwm.jpg).

## 4. Troubleshooting

**Script can't find the probe / XDS100v2 / "cannot connect" / error -1015**
- Use a data USB cable; avoid hubs.
- **S1-3 must be UP** so TRSTn reaches the XDS100v2 (SPRUHH2C §4.3 / FAQ §7.3). On 2026-11-06 the first DSLite attempt failed with error **-1015** until S1-3 was UP and the USB was replugged.
- JP1/JP2/JP3 populated for USB power (both power LEDs lit) — SPRUHH2C §4.1.
- **Windows:** Device Manager should show the XDS100v2, not unknown devices.
- **Linux:** install TI udev rules, then replug.
- Close CCS before running the flash script.
- Prefer CCS **20.3.1** / set `DSLITE` explicitly if an older CCS is picked.

**Flashed, but nothing blinks**
- Press reset or power-cycle; the chip may still be in a debug halt after flash.
- **Boot:** S1-3 UP for JTAG. For standalone flash boot: S1 **UP-UP-DOWN** (SPRUHH2C §3.1).
- Confirm power/isolation jumpers are in the factory USB-powered positions.
- Reflash with verify (scripts already use `-v`) and power-cycle if LEDs stay solid on or off.
- Expect D4/D5 only — D2 follows PWM (lit ~60% of the time, looks nearly steadily on); D3 (GPIO1) held off by its pull-up.

**PWM looks wrong**
- Probe **J6-1** (GPIO0 / EPWM1A). Expected: ~20 kHz (±INTOSC), ~40% high (~20 µs high of ~50 µs period). Measured ~20.08 kHz / ~19.92 µs / mean 1.33 V (INTOSC1 tolerance).
- Compare against [docs/scope_gpio0_pwm.jpg](docs/scope_gpio0_pwm.jpg).

**CCS link errors about overlapping memory ranges**
- Do not add `F2802x_Headers_nonBIOS.cmd` as a second `LINKER_COMMAND_FILE`. CCS already auto-links every `.cmd` in the project root.

**DSLite command-syntax error:** `DSLITE` points at the wrong binary. UniFlash wants `dslite.bat` / `dslite.sh`; CCS wants `DSLite.exe` / `DSLite`.

## 5. Known issues

- **GPIO0 shared:** J6-1 PWM and LED D2 share GPIO0. Firmware keeps PWM on J6-1 and blinks D4/D5 only. D2 is lit about 60% of the time (active-low; on while EPWM1A is low), so it looks nearly steadily on. D3 (GPIO1) held off by its pull-up.
- **INTOSC1:** SYSCLKOUT is from INTOSC1 (no crystal required). Scope frequency can sit ~0.4% off nominal 20 kHz; that is expected oscillator tolerance, not a firmware bug.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, XDS100v2 `.ccxml` |
| `main.c` | Blink loop (GPASET/GPACLEAR on GPIO2/3) + EPWM1A on GPIO0 |
| `device/` | F2802x bit-field sources + includes (C2000Ware `device_support/f2802x`; no driverlib) |
| `F28027.cmd` | Flash linker command file (`LINKER_COMMAND_FILE`) |
| `F2802x_Headers_nonBIOS.cmd` | Peripheral register placement (auto-linked; do not also pass explicitly) |
| `blinky_flash.projectspec` | Fallback CCS import |
| `build.sh`, `build.log` | Command-line build and last log |
| `CPU1_FLASH/blinky_flash.out` | CCS / `build.sh` output |
| `docs/scope_gpio0_pwm.jpg` | Scope capture of J6-1 PWM |
| `targetConfigs/TMS320F28027.ccxml` | TI stock XDS100v2 target config |
