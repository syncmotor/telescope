# F28069M LED blink + J4-40 PWM (LAUNCHXL-F28069M, runs from flash)

Repo path: `TIC2000/LaunchXL-f28069m/`.

User LEDs D9/D10 are driven alternately every 0.5 s, and **J4-40** runs a 20 kHz / 40% duty PWM. The program keeps running from flash after power-up. Device **TMS320F28069M** (CCS device id **TMS320F28069**), onboard **XDS100v2**, FLASH build, **COFF** ABI with bit-field headers (**no driverlib**), `fpu32` + `vcu0`, `rts2800_fpu32.lib`. SYSCLKOUT is **90 MHz** (INTOSC1 10 MHz × 18 / 2). There is **no crystal** (Q1 not fitted).

**J4-40** (BoosterPack site 1) = **GPIO0** = **EPWM1A**. Cited: SPRUI11B Table 4 (J4 pin 40 → GPIO0 / EPWM1A); C2000Ware `F2806x_EPwm.c` `InitEPwm1Gpio` (`GPAMUX1.GPIO0 = 1`). TBCLK = SYSCLKOUT / (HSPCLKDIV×CLKDIV) with both = 1 → **90 MHz**.

> **Hardware status (2026-11-06, Darshan's AcerSync):** DSLite from CCS **20.3.1** flashed and verified over the XDS100v2. CCS **20.3.1** headless import + full build (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, output `CPU1_FLASH/blinky_flash.out`. **LEDs: D9 verified; D10 not blinking, open issue (suspected HW, to be checked).** **Scope verified on J4-40:** 20.12 kHz, period 49.71 µs, mean 1.31 V, 40% duty (+0.6% from INTOSC tolerance) — see [docs/scope_gpio0_pwm.jpg](docs/scope_gpio0_pwm.jpg).

## Software versions

| Component | Version |
|-----------|---------|
| CCS (HW flash/import, Darshan PC) | **20.3.1** (`C:\ti\ccs2031`, preferred) |
| CGT (CCS 20.3.1 on Darshan PC) | **22.6.2.LTS** |
| DSLite (flash verified) | from **CCS 20.3.1** (`C:\ti\ccs2031\...`) |
| CGT (box / `build.sh`) | 22.6.1.LTS |
| C2000Ware | v26.02.00.00.LTS (`REL_C2000Ware_v26.02.00.00.LTS`) |
| Mirror | TexasInstruments/c2000ware-core-sdk @ `699b8c6aa190cf7430a36bdcf7f5b1ea2e928063` |
| Device support | `device_support/f2806x` (bit-field headers; **no driverlib**) |
| Device | TMS320F28069M (ccxml / CCS id: TMS320F28069) |
| Probe | XDS100v2 (LaunchPad onboard) |
| ABI / RTS | COFF / `rts2800_fpu32.lib` |

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB (onboard XDS100v2).
2. **S1-3 must be UP** for JTAG/DSLite. Per **SPRUI11B §5.3 Table 2 note**, **S1 UP-UP-UP** also boots to flash when the XDS100v2 isn't connected.
3. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat` (prefers newest numbered `C:\ti\ccs*`, e.g. `ccs2031`; falls back to `C:\ti\ccs`).
   - **Linux:** `cd flash && ./flash.sh`
4. When it prints `DONE`, press the LaunchPad reset button (or unplug/replug USB). D9 should blink; J4-40 should already be toggling PWM.

The script finds the flash tool by itself (UniFlash first, then newest CCS DSLite):
- Windows: `C:\ti\uniflash_*\dslite.bat`, then newest numbered `C:\ti\ccs*\...\DSLite.exe` (CCS **20.3.1** used on the verified flash)
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override** if the install is elsewhere or the wrong version is picked:
- Windows (Command Prompt, in `flash\`): `set DSLITE=C:\ti\ccs2031\ccs\ccs_base\DebugServer\bin\DSLite.exe` then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases, programs, and verifies (`-e -f -v`) using `flash/TMS320F28069.ccxml` (XDS100v2).

**Manual UniFlash GUI:** pick TMS320F28069 + XDS100v2, load `flash/blinky_flash.out`, click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`TIC2000/LaunchXL-f28069m`). It already has `.project`, `.cproject`, and `.ccsproject`.
2. Select the **CPU1_FLASH** build configuration, build, then Debug or Flash. Target config: `targetConfigs/TMS320F28069.ccxml`. Output: `CPU1_FLASH/blinky_flash.out`.

Verified with a headless import + full build in **CCS 20.3.1** (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, produced `CPU1_FLASH/blinky_flash.out`. Prefer CCS 20.3.1 on Darshan's system.

**If that import fails or the project looks empty:** import `blinky_flash.projectspec` the same way. That copies sources into your workspace, so edits there won't change this folder.

Needs a **C2000 22.6.x.LTS** compiler with **fpu32** / **vcu0** and `rts2800_fpu32.lib` (**COFF**; projectspec pins 22.6.1.LTS; CCS 20.3.1 used 22.6.2.LTS successfully). Install via **Help > Install Code Generation Compiler Tools** if CCS asks.

**Linker note:** the CCS `.cproject` no longer passes `F2806x_Headers_nonBIOS.cmd` explicitly. CCS auto-links every `.cmd` in the project root, and listing that file a second time caused memory-range errors. Keep `F28069M.cmd` as the only `LINKER_COMMAND_FILE`; leave `F2806x_Headers_nonBIOS.cmd` in the project root for auto-link.

Command-line build: `./build.sh` (set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`). Writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see

**LEDs (D9 verified; D10 open issue)**
- **D9** (GPIO34, red) and **D10** (GPIO39, blue) are driven alternately: one on for 0.5 s, then the other (1 s full cycle). Active-low through SN74LVC2G07 (U9); the code writes `0` (via `GPBCLEAR`) to turn one on and `1` (via `GPBSET`) to turn it off.
- Evidence for GPIO34/39: SPRUI11B F28069_B schematic (GPIO34/39 → U9 → LEDs); C2000Ware `Example_F2806xLaunchPadDemo.c`; MathWorks C2000 F28069M LED pins 34/39.
- Note: GPIO34 is also an S1 boot-mode pin; after boot the app owns it as an LED.
- With **S1 UP-UP-UP**, it boots from flash when the XDS100v2 isn't connected (SPRUI11B §5.3 Table 2 note) — power alone is enough after that.
- **Status:** D9 verified; D10 not blinking, open issue (suspected HW, to be checked).

**PWM on J4-40 (scope verified)**
- **J4-40** = **GPIO0** = **EPWM1A** (BoosterPack site 1).
- **20 kHz**, **40%** duty, up-count.
- TBCLK = **90 MHz** (HSPCLKDIV = CLKDIV = 1).
- **TBPRD = 4499**, **CMPA = 1800**.
- Action: high at ZRO, low at CAU.
- **Scope (2026-11-06):** 20.12 kHz, period 49.71 µs, mean 1.31 V, 40% duty. The +0.6% vs nominal 20.00 kHz is from INTOSC tolerance (no crystal / Q1 not fitted). Capture: [docs/scope_gpio0_pwm.jpg](docs/scope_gpio0_pwm.jpg).

## 4. Troubleshooting

**Script can't find the probe / XDS100v2 / "cannot connect"**
- Use a data USB cable; avoid hubs.
- **S1-3 must be UP** so TRSTn reaches the XDS100v2 (SPRUI11B §5.3 Table 2 note).
- JP1/JP2 populated for USB power (both power LEDs lit).
- **Windows:** Device Manager should show the XDS100v2, not unknown devices. Repair UniFlash/CCS drivers if needed.
- **Linux:** install TI udev rules (`install_drivers.sh` under CCS/UniFlash), then replug.
- Close CCS before running the flash script (only one client can hold the probe).
- Prefer CCS **20.3.1** / set `DSLITE` explicitly if an older CCS is picked.

**Flashed, but nothing blinks**
- Press reset or power-cycle; the chip may still be in a debug halt after flash.
- **Boot:** S1-3 UP for JTAG. Per SPRUI11B §5.3 Table 2 note, S1 UP-UP-UP also boots to flash when the XDS100v2 isn't connected.
- Confirm power/isolation jumpers are in the factory USB-powered positions.
- Reflash with verify (scripts already use `-v`) and power-cycle if LEDs stay solid on or off.
- If D9 blinks but D10 stays dark: see Known issues below (open HW check).

**PWM looks wrong**
- Probe **J4-40** (GPIO0 / EPWM1A). Expected: ~20 kHz (±INTOSC), ~40% high (~20 µs high of ~50 µs period).
- Compare against [docs/scope_gpio0_pwm.jpg](docs/scope_gpio0_pwm.jpg). A ~0.6% high reading (~20.12 kHz) is normal on INTOSC with Q1 not fitted.

**CCS link errors about overlapping memory ranges**
- Do not add `F2806x_Headers_nonBIOS.cmd` as a second `LINKER_COMMAND_FILE`. CCS already auto-links every `.cmd` in the project root; a duplicate caused the memory-range errors. The shipped `.cproject` lists only `F28069M.cmd`.

**DSLite command-syntax error:** `DSLITE` points at the wrong binary. UniFlash wants `dslite.bat` / `dslite.sh`; CCS wants `DSLite.exe` / `DSLite`. The scripts use a different command form for each.

## 5. Known issues

- **D9 verified; D10 not blinking, open issue (suspected HW, to be checked).** Firmware drives GPIO39 with `GPBSET`/`GPBCLEAR` the same way as GPIO34; Darshan will re-check the board hardware later.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, XDS100v2 `.ccxml` |
| `main.c` | Blink loop (GPBSET/GPBCLEAR on GPIO34/39) + EPWM1A on GPIO0 |
| `device/` | F2806x bit-field sources + includes (C2000Ware `device_support/f2806x`; no driverlib) |
| `F28069M.cmd` | Flash linker command file (`LINKER_COMMAND_FILE`) |
| `F2806x_Headers_nonBIOS.cmd` | Peripheral register placement (auto-linked; do not also pass explicitly) |
| `blinky_flash.projectspec` | Fallback CCS import |
| `build.sh`, `build.log` | Command-line build and last log |
| `CPU1_FLASH/blinky_flash.out` | CCS / `build.sh` output |
| `docs/scope_gpio0_pwm.jpg` | Scope capture of J4-40 PWM |
| `targetConfigs/TMS320F28069.ccxml` | TI stock XDS100v2 target config |
