#!/bin/sh
# Flash blinky_flash.out to LAUNCHXL-F28P65X (XDS110) via UniFlash or CCS DSLite.
cd "$(dirname "$0")"
DS="${DSLITE:-}"
if [ -z "$DS" ]; then
  for d in $HOME/ti/uniflash_* /opt/ti/uniflash_*; do [ -x "$d/dslite.sh" ] && DS="$d/dslite.sh"; done
fi
if [ -z "$DS" ]; then
  for d in $HOME/ti/ccs*/ccs /opt/ti/ccs*/ccs $HOME/ti/ccs* /opt/ti/ccs*; do
    f="$d/ccs_base/DebugServer/bin/DSLite"; [ -x "$f" ] && DS="$f"; done
fi
[ -z "$DS" ] && { echo "ERROR: No UniFlash (dslite.sh) or CCS DSLite found under ~/ti or /opt/ti. Install UniFlash or set DSLITE=/path/to/dslite.sh"; exit 1; }
echo "Using: $DS"
case "$DS" in *dslite.sh) "$DS" --mode flash -c TMS320F28P650DK9.ccxml -n 0 -e -f -v blinky_flash.out ;;
                *) "$DS" flash -c TMS320F28P650DK9.ccxml -n 0 -e -f -v blinky_flash.out ;; esac
rc=$?; [ $rc -eq 0 ] && echo "DONE. Press LaunchPad reset; LED4/LED5 (GPIO12/13) blink. GPIO8=EPWM5A 20 kHz 25%." || echo "ERROR: flashing failed (rc=$rc). Check USB/XDS110."
exit $rc
