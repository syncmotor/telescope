# F280049C LED blink + J4-40 PWM (LAUNCHXL-F280049C, CPU1, runs from flash)

Repo path: `TIC2000/LaunchXL-f280049c/`.

LED4 and LED5 blink alternately every 0.5 s, and **J4-40** runs a 20 kHz / 40% duty PWM. The program keeps running from flash after power-up. Device **TMS320F280049C**, onboard **XDS110** (cJTAG 2-pin), CPU1 FLASH build, EABI, `fpu32`, `rts2800_fpu32_eabi.lib`. SYSCLK is **100 MHz** (20 MHz XTAL ×10 /2).

**J4-40** (BoosterPack site 1) = **GPIO10** = **EPWM6A** (`GPIO_10_EPWM6_A`). Cited: SPRUII7B (LAUNCHXL-F280049C UG) BoosterPack pin map; TIDU986A Table 2 (`J4[40] GPIO10 (EPWM6_A)`). On F28004x, EPWMCLKDIV is hardwired /1 — **EPWMCLK = SYSCLK = 100 MHz** (no divider API).

> **Hardware status (2026-11-06, Darshan's AcerSync):** DSLite from CCS **20.3.1** flashed and verified over XDS110. CCS **20.3.1** headless import + full build (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, output `CPU1_FLASH/blinky_flash.out`. **Scope verified on J4-40:** 20.00 kHz, period 50.00 µs, positive width 20.01 µs, mean 1.33 V — see [docs/scope_gpio10_pwm.jpg](docs/scope_gpio10_pwm.jpg). **LEDs verified:** LED4 (GPIO23, red) and LED5 (GPIO34, green) alternate every 0.5 s after reset.

## Software versions

| Component | Version |
|-----------|---------|
| CCS (HW flash/import, Darshan PC) | **20.3.1** (`C:\ti\ccs2031`, preferred) |
| CGT (CCS 20.3.1 on Darshan PC) | **22.6.2.LTS** |
| DSLite (flash verified) | from **CCS 20.3.1** (`C:\ti\ccs2031\...`) |
| CGT (box / `build.sh`) | 22.6.1.LTS |
| C2000Ware | v26.02.00.00.LTS (`REL_C2000Ware_v26.02.00.LTS`) |
| Mirror | TexasInstruments/c2000ware-core-sdk @ `699b8c6aa190cf7430a36bdcf7f5b1ea2e928063` |
| driverlib_eabi.lib producer | TI C2000 CGT **v25.11.1.LTS** (links OK with 22.6.1) |
| Device | TMS320F280049C |
| Probe | XDS110 (LaunchPad onboard, cJTAG 2-pin) |

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB (onboard XDS110, cJTAG 2-pin).
2. Boot from flash: **S2 at its factory default (boot from flash)** (see silkscreen / SPRUII7B).
3. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat` (prefers newest numbered `C:\ti\ccs*`, e.g. `ccs2031`; falls back to `C:\ti\ccs`).
   - **Linux:** `cd flash && ./flash.sh`
4. When it prints `DONE`, press the LaunchPad reset button (or unplug/replug USB).

The script finds the flash tool by itself (UniFlash first, then newest CCS DSLite):
- Windows: `C:\ti\uniflash_*\dslite.bat`, then newest numbered `C:\ti\ccs*\...\DSLite.exe` (CCS **20.3.1** used on the verified flash)
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override** if the install is elsewhere or the wrong version is picked:
- Windows (Command Prompt, in `flash\`): `set DSLITE=C:\ti\ccs2031\ccs\ccs_base\DebugServer\bin\DSLite.exe` then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases, programs, and verifies (`-e -f -v`) using `flash/TMS320F280049C.ccxml` (XDS110 cJTAG).

**Manual UniFlash GUI:** pick TMS320F280049C + XDS110, load `flash/blinky_flash.out`, click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`TIC2000/LaunchXL-f280049c`). It already has `.project`, `.cproject`, and `.ccsproject`.
2. Select the **CPU1_FLASH** build configuration, build, then Debug or Flash. Target config: `targetConfigs/TMS320F280049C.ccxml`. Output: `CPU1_FLASH/blinky_flash.out`.

Verified with a headless import + full build in **CCS 20.3.1** (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, produced `CPU1_FLASH/blinky_flash.out`. Prefer CCS 20.3.1 on Darshan's system.

**If that import fails or the project looks empty:** import `blinky_flash.projectspec` the same way. That copies sources into your workspace, so edits there won't change this folder.

Needs a **C2000 22.6.x.LTS** compiler with **fpu32** and `rts2800_fpu32_eabi.lib` (projectspec pins 22.6.1.LTS; CCS 20.3.1 used 22.6.2.LTS successfully). Install via **Help > Install Code Generation Compiler Tools** if CCS asks.

Command-line build: `./build.sh` (set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`). Writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see

**LEDs — verified by Darshan on 2026-11-06**
- **LED4** (GPIO23, red) and **LED5** (GPIO34, green) should alternate: one on for 0.5 s, then the other (1 s full cycle). Active-low; the code writes `0` to turn one on.
- With S2 set for flash boot, it should start on every power-up with no CCS required—just power.

**PWM on J4-40 (scope verified)**
- **J4-40** = **GPIO10** = **EPWM6A** (BoosterPack site 1).
- **20 kHz**, **40%** duty, up-count.
- EPWMCLK = SYSCLK = **100 MHz** (no divider on F28004x).
- **TBPRD = 4999**, **CMPA = 2000**.
- Action: high at ZRO, low at CAU.
- **Scope (2026-11-06):** 20.00 kHz, period 50.00 µs, positive width 20.01 µs, mean 1.33 V. Capture: [docs/scope_gpio10_pwm.jpg](docs/scope_gpio10_pwm.jpg).

## 4. Troubleshooting

**Script can't find the probe / XDS110 / "cannot connect"**
- Use a data USB cable; avoid hubs.
- This board uses **XDS110 cJTAG (2-pin)** — make sure the ccxml / UniFlash connection matches (the shipped `TMS320F280049C.ccxml` is already set for that).
- **Windows:** Device Manager should show the XDS110, not unknown devices. Repair UniFlash/CCS drivers if needed.
- **Linux:** install TI udev rules (`install_drivers.sh` under CCS/UniFlash), then replug.
- Close CCS before running the flash script (only one client can hold the probe).
- Prefer CCS **20.3.1** / set `DSLITE` explicitly if an older CCS is picked.

**Flashed, but nothing blinks**
- Press reset or power-cycle; the chip may still be in a debug halt after flash.
- **Boot:** S2 at its factory default (boot from flash) for flash boot (not SCI / wait-boot). See SPRUII7B.
- Confirm power/isolation jumpers are in the factory USB-powered positions.
- Reflash with verify (scripts already use `-v`) and power-cycle if both LEDs stay solid on or off.
- **LEDs:** visually confirmed by Darshan (2026-11-06).

**PWM looks wrong**
- Probe **J4-40** (GPIO10 / EPWM6A). Expected: ~20 kHz, ~40% high (~20 µs high of 50 µs period).
- Compare against [docs/scope_gpio10_pwm.jpg](docs/scope_gpio10_pwm.jpg).

**DSLite command-syntax error:** `DSLITE` points at the wrong binary. UniFlash wants `dslite.bat` / `dslite.sh`; CCS wants `DSLite.exe` / `DSLite`. The scripts use a different command form for each.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, XDS110 cJTAG `.ccxml` |
| `main.c` | Blink loop + EPWM6A on GPIO10 |
| `device/`, `lib/` | Device setup and prebuilt driverlib (C2000Ware v26.02.00.00) |
| `28004x_generic_flash_lnk.cmd` | Flash linker command file |
| `blinky_flash.projectspec` | Fallback CCS import |
| `build.sh`, `build.log` | Command-line build and last log |
| `CPU1_FLASH/blinky_flash.out` | CCS / `build.sh` output |
| `docs/scope_gpio10_pwm.jpg` | Scope capture of J4-40 PWM |
