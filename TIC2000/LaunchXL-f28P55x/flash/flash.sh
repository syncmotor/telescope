#!/bin/sh
# Flash blinky_flash.out to LAUNCHXL-F28P55X (XDS110). Prefers newest CCS under ~/ti or /opt/ti.
cd "$(dirname "$0")"
DS="${DSLITE:-}"
if [ -z "$DS" ]; then
  for d in $HOME/ti/uniflash_* /opt/ti/uniflash_*; do [ -x "$d/dslite.sh" ] && DS="$d/dslite.sh"; done
fi
if [ -z "$DS" ]; then
  BESTVER=0; BESTDS=
  for d in $HOME/ti/ccs*/ccs /opt/ti/ccs*/ccs $HOME/ti/ccs* /opt/ti/ccs*; do
    f="$d/ccs_base/DebugServer/bin/DSLite"
    [ -x "$f" ] || continue
    base=$(basename "$(dirname "$d")" 2>/dev/null); base=$(basename "$d")
    case "$d" in
      */ccs) ver=$(basename "$(dirname "$d")" | tr -cd '0-9') ;;
      *)     ver=$(basename "$d" | tr -cd '0-9') ;;
    esac
    ver=${ver:-0}
    if [ "$ver" -gt "$BESTVER" ] 2>/dev/null; then BESTVER=$ver; BESTDS=$f; fi
  done
  DS=$BESTDS
fi
[ -z "$DS" ] && { echo "ERROR: No UniFlash/CCS DSLite found. Set DSLITE=/path/to/dslite.sh"; exit 1; }
echo "Using: $DS"
case "$DS" in *dslite.sh) "$DS" --mode flash -c TMS320F28P550SJ9.ccxml -n 0 -e -f -v blinky_flash.out ;;
                *) "$DS" flash -c TMS320F28P550SJ9.ccxml -n 0 -e -f -v blinky_flash.out ;; esac
rc=$?; [ $rc -eq 0 ] && echo "DONE. Press reset; LED4/LED5 (GPIO20/21) blink. GPIO0=EPWM1A 20 kHz 25%." || echo "ERROR: flashing failed (rc=$rc)."
exit $rc
