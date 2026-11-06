# F28P65X LED blink + GPIO8 PWM (LAUNCHXL-F28P65X, CPU1, runs from flash)

Repo path: `TIC2000/LaunchXL-f28P65x/`.

LED4 and LED5 blink alternately every 0.5 s, and GPIO8 runs a 20 kHz / 25% duty PWM. The program keeps running from flash after power-up. Device **TMS320F28P650DK9**, onboard **XDS110**, CPU1 FLASH build, C2000 CGT **22.6.1.LTS**, EABI, `fpu64`, `rts2800_fpu64_eabi.lib`. SYSCLK is **200 MHz** from the board's **25 MHz oscillator** (an oscillator, not a crystal).

> **Hardware status (2026-11-06):** flash + LEDs verified on Darshan's laptop. `flash.bat` auto-picked DSLite from CCS 20.3, flashed and verified over the onboard XDS110, and the LEDs blinked after reset. CCS import + build verified in CCS 12.7 (CGT 22.6.1): 0 errors, output `CPU1_FLASH/blinky_flash.out`; the one warning is harmless. Raph's static QE was green the same day. **Scope verified 2026-11-06 (Tek DSO on GPIO8): 20.00 kHz, period 50.00 us, high 12.51 us (25.0% duty), mean 835 mV**; see [docs/scope_gpio8_pwm.jpg](docs/scope_gpio8_pwm.jpg).

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB (onboard XDS110).
2. Set the boot switches for **flash boot** (see the board silkscreen / user guide).
3. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat`.
   - **Linux:** `cd flash && ./flash.sh`
4. When it prints `DONE`, press the LaunchPad reset button (or unplug/replug USB). LED4/LED5 should blink; GPIO8 should already be toggling PWM.

The script finds the flash tool by itself (UniFlash first, then CCS):
- Windows: `C:\ti\uniflash_*\dslite.bat`, then `C:\ti\ccs*\ccs\ccs_base\DebugServer\bin\DSLite.exe` (CCS 20.3 was picked automatically on the verified run)
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override** if the install is elsewhere or the wrong version is picked:
- Windows (Command Prompt, in `flash\`): `set DSLITE=D:\tools\uniflash_8.x\dslite.bat` then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases, programs, and verifies (`-e -f -v`) using `flash/TMS320F28P650DK9.ccxml` (XDS110).

**Manual UniFlash GUI:** pick TMS320F28P650DK9 + XDS110, load `flash/blinky_flash.out`, click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`TIC2000/LaunchXL-f28P65x`). It already has `.project`, `.cproject`, and `.ccsproject`.
2. Select the **CPU1_FLASH** build configuration, build, then Debug or Flash. Target config: `targetConfigs/TMS320F28P650DK9.ccxml`. Output: `CPU1_FLASH/blinky_flash.out`.

Verified with a headless import + build in **CCS 12.7** (CGT **22.6.1**): 0 errors, produced `CPU1_FLASH/blinky_flash.out`. The one warning is harmless (including a **"created with a newer version of CCS"** style warning).

**If that import fails or the project looks empty:** import `blinky_flash.projectspec` the same way. That copies sources into your workspace, so edits there won't change this folder.

Needs **C2000 compiler 22.6.1.LTS** with **fpu64** and `rts2800_fpu64_eabi.lib`. Install via **Help > Install Code Generation Compiler Tools** if CCS asks.

Command-line build: `./build.sh` (set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`). Writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see

**LEDs (verified)**
- **LED4** (GPIO12) and **LED5** (GPIO13) alternate: one on for 0.5 s, then the other (1 s full cycle). Active-low; the code writes `0` to turn one on.
- With boot set for flash, it starts on every power-up with no CCS required—just power.

**PWM on GPIO8 (scope verified)**
- **GPIO8** = **EPWM5A** (BoosterPack pin 40).
- **20 kHz**, **25%** duty, up-count.
- TBCLK = 200 MHz, **TBPRD = 9999**, **CMPA = 2500**.
- Action: high at zero, low at CMPA.

## 4. Troubleshooting

**Script can't find the probe / XDS110 / "cannot connect"**
- Use a data USB cable; avoid hubs.
- **Windows:** Device Manager should show the XDS110, not unknown devices. Repair UniFlash/CCS drivers if needed.
- **Linux:** install TI udev rules (`install_drivers.sh` under CCS/UniFlash), then replug.
- Close CCS before running the flash script (only one client can hold the probe).

**Flashed, but nothing blinks**
- Press reset or power-cycle; the chip may still be in a debug halt after flash.
- Confirm boot switches are set for flash boot (not SCI / wait-boot).
- Confirm power/isolation jumpers are in the factory USB-powered positions.
- Reflash with verify (scripts already use `-v`) and power-cycle if both LEDs stay solid on or off.

**LEDs blink, but GPIO8 looks wrong**
- Probe BoosterPack pin 40 (GPIO8 / EPWM5A). Expected: ~20 kHz, ~25% high.
- **Scope verified 2026-11-06 (Tek DSO on GPIO8): 20.00 kHz, period 50.00 us, high 12.51 us (25.0% duty), mean 835 mV.** Capture: `docs/scope_gpio8_pwm.jpg`.

**DSLite command-syntax error:** `DSLITE` points at the wrong binary. UniFlash wants `dslite.bat` / `dslite.sh`; CCS wants `DSLite.exe` / `DSLite`. The scripts use a different command form for each.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, XDS110 `.ccxml` |
| `main.c` | Blink loop + EPWM5A setup |
| `device/`, `lib/` | Device setup and prebuilt driverlib |
| `28p65x_generic_flash_lnk_cpu1.cmd` | Flash linker command file |
| `blinky_flash.projectspec` | Fallback CCS import |
| `build.sh`, `build.log` | Command-line build and last log |
| `CPU1_FLASH/blinky_flash.out` | CCS / `build.sh` output |
