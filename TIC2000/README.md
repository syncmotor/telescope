# F28379D LED blinky (LAUNCHXL-F28379D, CPU1, runs from flash)

Two LEDs blink alternately every 0.5 s, and the program keeps running from flash after power-up. It's based on TI C2000Ware 6.00.01.00 `led_ex1_blinky` (driverlib), with the entry point `code_start` at 0x080000.

> **Hardware status: untested.** The firmware builds cleanly on our build machine (`build.log` ends with `OK`), but nothing here has run on a real LaunchPad yet. That includes both flash scripts, the CCS import, and the LED behavior. Steps marked ⚠️ come from TI's tooling conventions, not from a tested run.

## 1. One-click flash (prebuilt, no build needed)

1. Plug the LaunchPad into USB. The onboard debug probe is an XDS100v2.
2. Run the script in `flash/`:
   - **Windows:** double-click `flash\flash.bat`.
   - **Linux:** `cd flash && ./flash.sh`
3. When it prints `DONE`, press the LaunchPad's reset button or unplug and replug USB.

The script finds the flash tool by itself, checking UniFlash first and then CCS:
- Windows: `C:\ti\uniflash_*\dslite.bat`, then `C:\ti\ccs*\ccs\ccs_base\DebugServer\bin\DSLite.exe`
- Linux: `~/ti/uniflash_*/dslite.sh` or `/opt/ti/uniflash_*/dslite.sh`, then CCS `.../ccs_base/DebugServer/bin/DSLite`

**DSLITE override.** If you installed UniFlash or CCS somewhere else, or the script picks the wrong version, point it at the tool yourself:
- Windows (Command Prompt, in `flash\`): `set DSLITE=D:\tools\uniflash_8.x\dslite.bat` and then `flash.bat`
- Linux: `DSLITE=/path/to/uniflash/dslite.sh ./flash.sh`

The script erases flash, programs it, and verifies (`-e -f -v`) using `flash/TMS320F28379D.ccxml`, which is set up for the XDS100v2.

**Manual fallback in the UniFlash GUI:** pick the TMS320F28379D device and the XDS100v2 connection, load `flash/blinky_flash.out`, then click **Load Image**.

## 2. Open it in CCS

1. **File > Import > Code Composer Studio > CCS Projects**, then choose this folder (`f28379d-blinky`). It already contains the `.project`, `.cproject` and `.ccsproject` files.
2. Pick the **CPU1_FLASH** build configuration, build it, and then click Debug or Flash. The target config is `targetConfigs/TMS320F28379D.ccxml`.

**If that import fails or the project shows up empty:** go to **File > Import > CCS Projects** again, but select `blinky_flash.projectspec`. ⚠️ This copies the sources into your workspace, so any edits you make there won't change this folder.

The project needs the **C2000 compiler 22.6.1.LTS**. If CCS asks for it, install it with **Help > Install Code Generation Compiler Tools**, or pick a newer 22.x in Project Properties. ⚠️ We haven't tested a newer compiler.

To build from the command line instead, run `./build.sh`. It needs the same compiler; set `CG_TOOL_ROOT` if the compiler isn't in `~/ti/ti-cgt-c2000_22.6.1.LTS`. The build writes `CPU1_FLASH/blinky_flash.out` and copies it into `flash/`.

## 3. What you should see ⚠️

- **LD10** (GPIO31) and **LD9** (GPIO34) take turns: one is on for 0.5 s, then the other, so a full cycle is 1 s. The LEDs are active-low, so the code writes `0` to turn one on.
- With the boot switches set to boot from flash, it starts by itself every time the board powers up. You don't need CCS or a USB host for that, just power.

## 4. Troubleshooting ⚠️

**The script can't find the probe, or reports an XDS100 or "cannot connect" error**
- Use a USB cable that carries data, and plug straight into the PC instead of through a hub.
- **Windows:** Device Manager should list the XDS100v2 as two serial converter interfaces, not as unknown devices. If they're unknown, reinstall or repair UniFlash or CCS so their emulation drivers get installed.
- **Linux:** the probe needs TI's udev rules. Run the driver install script that ships with CCS or UniFlash, usually `install_scripts/install_drivers.sh` under `ccs_base/common/uscif/...` or under UniFlash's `deskdb`, with sudo. Then replug the board.
- Close CCS before running the script, because only one program can use the probe at a time.

**Flashing worked, but nothing blinks**
- Press reset, or unplug and replug USB. Right after flashing, the chip may still be sitting in its debug state.
- **Boot-mode switches:** on the LaunchPad's boot switch block (labeled S1 on the board), both switches should be in the "boot from flash" position, which is up/ON by default. If someone moved them for SCI or wait-boot, the chip won't run from flash. Check the silkscreen or TI's LAUNCHXL-F28379D user guide (SPRUI77) for the exact positions on your board revision.
- Make sure the board's power and isolation jumpers are in their factory positions so it runs from USB power.
- If LD10 and LD9 just stay solidly on or off, reflash with verify (both scripts already use `-v`) and then power-cycle.

**The DSLite script ran but complained about the command syntax:** you probably pointed `DSLITE` at the wrong file. UniFlash needs `dslite.bat` or `dslite.sh`; CCS needs `DSLite.exe` or `DSLite`. The scripts use a different command format for each one.

## Files

| Path | What it is |
|---|---|
| `flash/` | Prebuilt `blinky_flash.out`, `flash.bat`, `flash.sh`, and the XDS100v2 `.ccxml` |
| `main.c` | The blink loop |
| `device/`, `lib/` | Device setup and the prebuilt driverlib library from C2000Ware |
| `2837xD_FLASH_lnk_cpu1.cmd` | Flash linker command file |
| `blinky_flash.projectspec` | Fallback way to import into CCS |
| `build.sh`, `build.log` | Command-line build and its last log |
