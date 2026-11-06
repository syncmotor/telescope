#!/bin/sh
# Command-line FLASH build with TI cl2000. Override CG_TOOL_ROOT if needed.
set -e
cd "$(dirname "$0")"
CG=${CG_TOOL_ROOT:-$HOME/ti/ti-cgt-c2000_22.6.1.LTS}
CL=$CG/bin/cl2000
OUT=CPU1_FLASH; mkdir -p $OUT
OPTS="--abi=eabi -v28 -ml -mt --float_support=fpu32 --tmu_support=tmu0 -O2 --define=CPU1 --define=_FLASH --define=_LAUNCHXL_F2800157 --diag_warning=225 --diag_suppress=10063 --display_error_number -I$CG/include -Idevice -Idevice/driverlib"
for f in main.c device/device.c device/f280015x_codestartbranch.asm; do
  $CL $OPTS --obj_directory=$OUT "$f"
done
$CL $OPTS -z -m$OUT/blinky_flash.map --stack_size=0x200 --entry_point=code_start --warn_sections \
  -i$CG/lib -i$CG/include --reread_libs --rom_model -o $OUT/blinky_flash.out \
  $OUT/main.obj $OUT/device.obj $OUT/f280015x_codestartbranch.obj \
  280015x_generic_flash_lnk.cmd lib/driverlib_eabi.lib -lrts2800_fpu32_eabi.lib
$CG/bin/hex2000 -i $OUT/blinky_flash.out -o $OUT/blinky_flash.hex -romwidth 16 -memwidth 16 >/dev/null
cp $OUT/blinky_flash.out flash/
echo "OK: $OUT/blinky_flash.out"
