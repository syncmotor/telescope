# F28379D LED blinky (LAUNCHXL-F28379D, CPU1, runs from flash)

Repo path: `TIC2000/LaunchXL-f28379d/`.

Two LEDs blink alternately every 0.5 s, and the program keeps running from flash after power-up. Based on TI C2000Ware 6.00.01.00 `led_ex1_blinky` (driverlib), entry `code_start` at 0x080000.

> **Hardware status: verified 2026-11-06** on Darshan's LaunchPad. `flash.bat` auto-picked DSLite from CCS 20.3, flashed and verified over the onboard XDS100v2, and LD9/LD10 blinked after reset. CCS import + build verified in CCS 12.7 with compiler 22.6.1.LTS.

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB (onboard XDS100v2).
2. Set boot switch **S1** so **GPIO72** and **GPIO84** are both **up** (flash boot).
3. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat`.
   - **Linux:** `cd flash && ./flash.sh`
4. When it prints `DONE`, press the LaunchPad reset button (or unplug/replug USB). LD9/LD10 should blink.

The script finds the flash tool by itself (UniFlash first, then CCS):
- Windows: `C:\ti\uniflash_*\dslite.bat`, then `C:\ti\ccs*\ccs\ccs_base\DebugServer\bin\DSLite.exe` (CCS 20.3 was picked automatically on the verified run)
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override** if the install is elsewhere or the wrong version is picked:
- Windows (Command Prompt, in `flash\`): `set DSLITE=D:\tools\uniflash_8.x\dslite.bat` then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases, programs, and verifies (`-e -f -v`) using `flash/TMS320F28379D.ccxml` (XDS100v2).

**Manual UniFlash GUI:** pick TMS320F28379D + XDS100v2, load `flash/blinky_flash.out`, click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`TIC2000/LaunchXL-f28379d`). It already has `.project`, `.cproject`, and `.ccsproject`.
2. Select the **CPU1_FLASH** build configuration, build, then Debug or Flash. Target config: `targetConfigs/TMS320F28379D.ccxml`. Output: `CPU1_FLASH/blinky_flash.out`.

A **"created with a newer version of CCS"** warning on import is harmless (verified with CCS 12.7).

**If that import fails or the project looks empty:** import `blinky_flash.projectspec` the same way. That copies sources into your workspace, so edits there won't change this folder.

Needs **C2000 compiler 22.6.1.LTS** (verified). Install via **Help > Install Code Generation Compiler Tools** if CCS asks, or pick a newer 22.x in Project Properties (newer compilers not verified here).

Commit `05a1cb8` fixed the CCS build: compiler include path (`${CG_TOOL_ROOT}/include`) and explicit `rts2800_fpu32_eabi.lib`.

Command-line build: `./build.sh` (set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`). Writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see

- **LD10** (GPIO31) and **LD9** (GPIO34) alternate: one on for 0.5 s, then the other (1 s full cycle). Active-low LEDs; the code writes `0` to turn one on.
- With S1 set for flash boot, it starts on every power-up with no CCS or USB host required—just power.

## 4. Troubleshooting

**Script can't find the probe / XDS100 / "cannot connect"**
- Use a data USB cable; avoid hubs.
- **Windows:** Device Manager should show the XDS100v2 as two serial converters, not unknown devices. Repair UniFlash/CCS drivers if needed.
- **Linux:** install TI udev rules (`install_drivers.sh` under CCS/UniFlash), then replug.
- Close CCS before running the flash script (only one client can hold the probe).

**Flashed, but nothing blinks**
- Press reset or power-cycle; the chip may still be in a debug halt after flash.
- **Boot switch S1:** GPIO72 and GPIO84 both **up** for flash boot. Other positions (SCI / wait-boot) won't run from flash. See silkscreen or SPRUI77 for your board revision.
- Confirm power/isolation jumpers are in the factory USB-powered positions.
- Reflash with verify (scripts already use `-v`) and power-cycle if both LEDs stay solid on or off.

**DSLite command-syntax error:** `DSLITE` points at the wrong binary. UniFlash wants `dslite.bat` / `dslite.sh`; CCS wants `DSLite.exe` / `DSLite`. The scripts use a different command form for each.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, XDS100v2 `.ccxml` |
| `main.c` | Blink loop |
| `device/`, `lib/` | Device setup and prebuilt driverlib from C2000Ware |
| `2837xD_FLASH_lnk_cpu1.cmd` | Flash linker command file |
| `blinky_flash.projectspec` | Fallback CCS import |
| `build.sh`, `build.log` | Command-line build and last log |
| `CPU1_FLASH/blinky_flash.out` | CCS / `build.sh` output |
