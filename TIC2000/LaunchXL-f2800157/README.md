# F2800157 LED blink + J4-40 PWM (LAUNCHXL-F2800157, CPU1, runs from flash)

Repo path: `TIC2000/LaunchXL-f2800157/`.

LED4 and LED5 blink alternately every 0.5 s, and **J4-40** runs a 20 kHz / 40% duty PWM. The program keeps running from flash after power-up. Device **TMS320F2800157**, onboard **XDS110** (cJTAG 2-pin), CPU1 FLASH build, EABI, `fpu32` + `tmu0`, `rts2800_fpu32_eabi.lib`. SYSCLK is **120 MHz** (20 MHz XTAL ×48 / 8).

**J4-40** (BoosterPack site 1) = **GPIO30** = **EPWM1A** (`GPIO_30_EPWM1_A`). Cited: C2000Ware `LAUNCHXL_F2800157.syscfg.json` BoosterPack1 pin 40 → GPIO30; SPRR476 schematic (`J4_40` / GPIO30); pinmux `GPIO_30_EPWM1_A` in `pin_map.h`. On F280015x there is **no PERCLKDIVSEL / EPWMCLKDIV** — **EPWMCLK = SYSCLK = 120 MHz**.

> **Hardware status (2026-11-06, Darshan's AcerSync):** DSLite from CCS **20.3.1** flashed and verified over XDS110. CCS **20.3.1** headless import + full build (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, output `CPU1_FLASH/blinky_flash.out`. **LEDs verified** by Darshan (LED4/LED5 blink after reset). **Scope verified on J4-40:** 20.00 kHz, period 50.00 µs, positive width 20.00 µs, mean 1.35 V — see [docs/scope_gpio30_pwm.jpg](docs/scope_gpio30_pwm.jpg).

## Software versions

| Component | Version |
|-----------|---------|
| CCS (HW flash/import, Darshan PC) | **20.3.1** (`C:\ti\ccs2031`, preferred) |
| CGT (CCS 20.3.1 on Darshan PC) | **22.6.2.LTS** |
| DSLite (flash verified) | from **CCS 20.3.1** (`C:\ti\ccs2031\...`) |
| CGT (box / `build.sh`) | 22.6.1.LTS |
| C2000Ware | v26.02.00.00.LTS (`REL_C2000Ware_v26.02.00.LTS`) |
| Mirror | TexasInstruments/c2000ware-core-sdk @ `699b8c6aa190cf7430a36bdcf7f5b1ea2e928063` |
| driverlib_eabi.lib producer | TI C2000 CGT **v25.11.1.LTS** (links OK with 22.6.1; from SDK `driverlib/ccs/Release`) |
| Device | TMS320F2800157 |
| Probe | XDS110 (LaunchPad onboard, cJTAG 2-pin) |

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB (onboard XDS110, cJTAG 2-pin).
2. Boot from flash: **S3 at its default (boot from flash)** — see silkscreen / SPRUJ54A.
3. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat` (prefers newest numbered `C:\ti\ccs*`, e.g. `ccs2031`; falls back to `C:\ti\ccs`).
   - **Linux:** `cd flash && ./flash.sh`
4. When it prints `DONE`, press the LaunchPad reset button (or unplug/replug USB). LED4/LED5 should blink; J4-40 should already be toggling PWM.

The script finds the flash tool by itself (UniFlash first, then newest CCS DSLite):
- Windows: `C:\ti\uniflash_*\dslite.bat`, then newest numbered `C:\ti\ccs*\...\DSLite.exe` (CCS **20.3.1** used on the verified flash)
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override** if the install is elsewhere or the wrong version is picked:
- Windows (Command Prompt, in `flash\`): `set DSLITE=C:\ti\ccs2031\ccs\ccs_base\DebugServer\bin\DSLite.exe` then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases, programs, and verifies (`-e -f -v`) using `flash/TMS320F2800157.ccxml` (XDS110 cJTAG).

**Manual UniFlash GUI:** pick TMS320F2800157 + XDS110, load `flash/blinky_flash.out`, click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`TIC2000/LaunchXL-f2800157`). It already has `.project`, `.cproject`, and `.ccsproject`.
2. Select the **CPU1_FLASH** build configuration, build, then Debug or Flash. Target config: `targetConfigs/TMS320F2800157.ccxml`. Output: `CPU1_FLASH/blinky_flash.out`.

Verified with a headless import + full build in **CCS 20.3.1** (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, produced `CPU1_FLASH/blinky_flash.out`. Prefer CCS 20.3.1 on Darshan's system.

**If that import fails or the project looks empty:** import `blinky_flash.projectspec` the same way. That copies sources into your workspace, so edits there won't change this folder.

Needs a **C2000 22.6.x.LTS** compiler with **fpu32** / **tmu0** and `rts2800_fpu32_eabi.lib` (projectspec pins 22.6.1.LTS; CCS 20.3.1 used 22.6.2.LTS successfully). Install via **Help > Install Code Generation Compiler Tools** if CCS asks.

Command-line build: `./build.sh` (set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`). Writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see

**LEDs (verified)**
- **LED4** (GPIO35, red) and **LED5** (GPIO49, green) alternate: one on for 0.5 s, then the other (1 s full cycle). Active-low; the code writes `0` to turn one on.
- Evidence for GPIO35/49: SPRR476 schematic, the SDK/`device.h` under `_LAUNCHXL_F2800157`, and [TI E2E 1253487](https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/1253487). **SPRUJ54A §3.1.3 text saying GPIO20/22 is wrong** — use 35/49.
- GPIO35 defaults to JTAG TDI; `main.c` muxes it to GPIO. That is fine with **cJTAG 2-pin** (the shipped ccxml).
- With S3 at its default (boot from flash), it starts on every power-up with no CCS required—just power.

**PWM on J4-40 (scope verified)**
- **J4-40** = **GPIO30** = **EPWM1A** (BoosterPack site 1).
- **20 kHz**, **40%** duty, up-count.
- EPWMCLK = SYSCLK = **120 MHz** (no divider on F280015x).
- **TBPRD = 5999**, **CMPA = 2400**.
- Action: high at ZRO, low at CAU.
- **Scope (2026-11-06):** 20.00 kHz, period 50.00 µs, positive width 20.00 µs, mean 1.35 V. Capture: [docs/scope_gpio30_pwm.jpg](docs/scope_gpio30_pwm.jpg).

## 4. Troubleshooting

**Script can't find the probe / XDS110 / "cannot connect"**
- Use a data USB cable; avoid hubs.
- This board uses **XDS110 cJTAG (2-pin)** — the shipped `TMS320F2800157.ccxml` is already set for that.
- **Windows:** Device Manager should show the XDS110, not unknown devices. Repair UniFlash/CCS drivers if needed.
- **Linux:** install TI udev rules (`install_drivers.sh` under CCS/UniFlash), then replug.
- Close CCS before running the flash script (only one client can hold the probe).
- Prefer CCS **20.3.1** / set `DSLITE` explicitly if an older CCS is picked.

**Flashed, but nothing blinks**
- Press reset or power-cycle; the chip may still be in a debug halt after flash.
- **Boot:** S3 at its default (boot from flash). See SPRUJ54A.
- Confirm power/isolation jumpers are in the factory USB-powered positions.
- Reflash with verify (scripts already use `-v`) and power-cycle if both LEDs stay solid on or off.
- If only LED5 blinks and LED4 stays dark: GPIO35 may still be in JTAG TDI mode — confirm you're on the cJTAG 2-pin ccxml and that `main.c` still calls `GPIO_setPinConfig(DEVICE_GPIO_CFG_LED1)`.

**PWM looks wrong**
- Probe **J4-40** (GPIO30 / EPWM1A). Expected: ~20 kHz, ~40% high (~20 µs high of 50 µs period).
- Compare against [docs/scope_gpio30_pwm.jpg](docs/scope_gpio30_pwm.jpg).

**DSLite command-syntax error:** `DSLITE` points at the wrong binary. UniFlash wants `dslite.bat` / `dslite.sh`; CCS wants `DSLite.exe` / `DSLite`. The scripts use a different command form for each.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, XDS110 cJTAG `.ccxml` |
| `main.c` | Blink loop + EPWM1A on GPIO30 (GPIO35 muxed off TDI) |
| `device/`, `lib/` | Device setup and prebuilt driverlib (C2000Ware v26.02.00.00) |
| `280015x_generic_flash_lnk.cmd` | Flash linker command file |
| `blinky_flash.projectspec` | Fallback CCS import |
| `build.sh`, `build.log` | Command-line build and last log |
| `CPU1_FLASH/blinky_flash.out` | CCS / `build.sh` output |
| `docs/scope_gpio30_pwm.jpg` | Scope capture of J4-40 PWM |
