# F28P55X LED blink + GPIO0 PWM (LAUNCHXL-F28P55X, CPU1, runs from flash)

Repo path: `TIC2000/LaunchXL-f28P55x/`.

LED4 and LED5 blink alternately every 0.5 s, and GPIO0 runs a 20 kHz / 25% duty PWM. The program keeps running from flash after power-up. Device **TMS320F28P550SJ9**, onboard **XDS110**, CPU1 FLASH build, EABI, `fpu32`, `rts2800_fpu32_eabi.lib`. SYSCLK is **150 MHz** from the board's **20 MHz crystal**.

GPIO0 (EPWM1A on **J4 pin 40**, BoosterPack site 1) was chosen instead of GPIO8 because GPIO8 is **SPIA_PICO** on this board. On F28P55x there is **no EPWMCLK divider** — EPWMCLK = SYSCLK = 150 MHz (Raph's QE fix).

> **Hardware status (2026-11-06):** `flash.bat` flashed and verified over the onboard XDS110. CCS **20.3.1** headless import + build passed (**0 errors, 0 warnings**), output `CPU1_FLASH/blinky_flash.out`. Raph's QE is green after his fixes. **LEDs verified 2026-11-06 (LED4/LED5 blink after reset, confirmed by Darshan). Scope verified 2026-11-06 (Tek DSO on GPIO0, J4.40): 20.00 kHz, high 12.50 us (25.0% duty), mean 847 mV**; see [docs/scope_gpio0_pwm.jpg](docs/scope_gpio0_pwm.jpg).

## Software versions

| Component | Version |
|-----------|---------|
| CCS (HW flash/import, Darshan PC) | **20.3.1** (`C:\ti\ccs2031`, preferred) |
| CGT (CCS 20.3.1 on Darshan PC) | **22.6.2.LTS** |
| DSLite (flash verified) | from **CCS 20.3.1** (`C:\ti\ccs2031\...`) |
| CGT (box / `build.sh`) | 22.6.1.LTS |
| C2000Ware | v26.02.00.00.LTS (`REL_C2000Ware_v26.02.00.LTS`) |
| Mirror | TexasInstruments/c2000ware-core-sdk @ `699b8c6aa190cf7430a36bdcf7f5b1ea2e928063` |
| driverlib.lib built with | TI C2000 CGT **v25.11.1.LTS** (EABI ELF; links OK with 22.6.1) |
| Device | TMS320F28P550SJ9 |
| Probe | XDS110 (LaunchPad onboard) |

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB (onboard XDS110).
2. Set the boot switches for **flash boot** (see the board silkscreen / user guide).
3. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat` (prefers the newest CCS install, e.g. `ccs2031`).
   - **Linux:** `cd flash && ./flash.sh`
4. When it prints `DONE`, press the LaunchPad reset button (or unplug/replug USB).

The script finds the flash tool by itself (UniFlash first, then newest CCS DSLite):
- Windows: `C:\ti\uniflash_*\dslite.bat`, then newest `C:\ti\ccs*\...\DSLite.exe` (CCS **20.3.1** used on the verified flash)
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override** if the install is elsewhere or the wrong version is picked:
- Windows (Command Prompt, in `flash\`): `set DSLITE=C:\ti\ccs2031\ccs\ccs_base\DebugServer\bin\DSLite.exe` then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases, programs, and verifies (`-e -f -v`) using `flash/TMS320F28P550SJ9.ccxml` (XDS110).

**Manual UniFlash GUI:** pick TMS320F28P550SJ9 + XDS110, load `flash/blinky_flash.out`, click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`TIC2000/LaunchXL-f28P55x`). It already has `.project`, `.cproject`, and `.ccsproject`.
2. Select the **CPU1_FLASH** build configuration, build, then Debug or Flash. Target config: `targetConfigs/TMS320F28P550SJ9.ccxml`. Output: `CPU1_FLASH/blinky_flash.out`.

Verified with a headless import + build in **CCS 20.3.1** (CGT **22.6.2.LTS**): **0 errors, 0 warnings**, produced `CPU1_FLASH/blinky_flash.out`. Prefer CCS 20.3.1 on Darshan's system.

**If that import fails or the project looks empty:** import `blinky_flash.projectspec` the same way. That copies sources into your workspace, so edits there won't change this folder.

Needs a **C2000 22.6.x.LTS** compiler with **fpu32** and `rts2800_fpu32_eabi.lib` (projectspec pins 22.6.1.LTS; CCS 20.3.1 used 22.6.2.LTS successfully). Install via **Help > Install Code Generation Compiler Tools** if CCS asks.

Command-line build: `./build.sh` (set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`). Writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see

**LEDs — verified on hardware**
- **LED4** (GPIO20) and **LED5** (GPIO21) should alternate: one on for 0.5 s, then the other (1 s full cycle). Active-low; the code writes `0` to turn one on.
- With boot set for flash, it should start on every power-up with no CCS required—just power.

**PWM on GPIO0 — scope verified**
- **GPIO0** = **EPWM1A** on **J4 pin 40** (BoosterPack site 1). Not GPIO8 (that pin is SPIA_PICO here).
- **20 kHz**, **25%** duty, up-count.
- TBCLK = SYSCLK = **150 MHz** (no EPWMCLK divider on F28P55x).
- **TBPRD = 7499**, **CMPA = 1875**.
- Action: high at zero, low at CMPA.

## 4. Troubleshooting

**Script can't find the probe / XDS110 / "cannot connect"**
- Use a data USB cable; avoid hubs.
- **Windows:** Device Manager should show the XDS110, not unknown devices. Repair UniFlash/CCS drivers if needed.
- **Linux:** install TI udev rules (`install_drivers.sh` under CCS/UniFlash), then replug.
- Close CCS before running the flash script (only one client can hold the probe).
- Prefer CCS **20.3.1** / set `DSLITE` explicitly if an older CCS is picked.

**Flashed, but nothing blinks**
- Press reset or power-cycle; the chip may still be in a debug halt after flash.
- Confirm boot switches are set for flash boot (not SCI / wait-boot).
- Confirm power/isolation jumpers are in the factory USB-powered positions.
- Reflash with verify (scripts already use `-v`) and power-cycle if both LEDs stay solid on or off.
- **LEDs verified 2026-11-06 (LED4/LED5 blink after reset, confirmed by Darshan).**

**Looking for PWM on the wrong pin**
- Probe **J4 pin 40** (GPIO0 / EPWM1A), not GPIO8. Expected: ~20 kHz, ~25% high.
- **Scope verified 2026-11-06 (Tek DSO on GPIO0, J4.40): 20.00 kHz, high 12.50 us (25.0% duty), mean 847 mV.** Capture: `docs/scope_gpio0_pwm.jpg`.

**DSLite command-syntax error:** `DSLITE` points at the wrong binary. UniFlash wants `dslite.bat` / `dslite.sh`; CCS wants `DSLite.exe` / `DSLite`. The scripts use a different command form for each.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, XDS110 `.ccxml` |
| `main.c` | Blink loop + EPWM1A setup |
| `device/`, `lib/` | Device setup and prebuilt driverlib (C2000Ware v26.02.00.00) |
| `28p55x_generic_flash_lnk.cmd` | Flash linker command file |
| `blinky_flash.projectspec` | Fallback CCS import |
| `build.sh`, `build.log` | Command-line build and last log |
| `CPU1_FLASH/blinky_flash.out` | CCS / `build.sh` output |
