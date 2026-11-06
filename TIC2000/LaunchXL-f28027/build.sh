#!/bin/sh
# Command-line FLASH build with TI cl2000 (COFF / no FPU). Override CG_TOOL_ROOT if needed.
set -e
cd "$(dirname "$0")"
CG=${CG_TOOL_ROOT:-$HOME/ti/ti-cgt-c2000_22.6.1.LTS}
CL=$CG/bin/cl2000
OUT=CPU1_FLASH; mkdir -p $OUT
# F2802x has no FPU — COFF + rts2800_ml.lib (fixed-point).
OPTS="--abi=coffabi -v28 -ml -mt -O2 --define=_FLASH --define=LARGE_MODEL --diag_warning=225 --diag_suppress=10063 --display_error_number -I$CG/include -Idevice/include"
SRCS="main.c \
  device/f2802x_sysctrl.c \
  device/f2802x_piectrl.c \
  device/f2802x_pievect.c \
  device/f2802x_defaultisr.c \
  device/F2802x_GlobalVariableDefs.c \
  device/f2802x_codestartbranch.asm \
  device/f2802x_usdelay.asm \
  device/f2802x_csmpasswords.asm"
for f in $SRCS; do
  $CL $OPTS --obj_directory=$OUT "$f"
done
$CL $OPTS -z -m$OUT/blinky_flash.map --stack_size=0x200 --entry_point=code_start --warn_sections \
  -i$CG/lib -i$CG/include --reread_libs --rom_model -o $OUT/blinky_flash.out \
  $OUT/main.obj \
  $OUT/f2802x_sysctrl.obj \
  $OUT/f2802x_piectrl.obj \
  $OUT/f2802x_pievect.obj \
  $OUT/f2802x_defaultisr.obj \
  $OUT/F2802x_GlobalVariableDefs.obj \
  $OUT/f2802x_codestartbranch.obj \
  $OUT/f2802x_usdelay.obj \
  $OUT/f2802x_csmpasswords.obj \
  F28027.cmd F2802x_Headers_nonBIOS.cmd \
  -lrts2800_ml.lib
$CG/bin/hex2000 -i $OUT/blinky_flash.out -o $OUT/blinky_flash.hex -romwidth 16 -memwidth 16 >/dev/null
cp $OUT/blinky_flash.out flash/
echo "OK: $OUT/blinky_flash.out"
