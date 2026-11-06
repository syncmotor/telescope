#!/bin/sh
# Flash blinky_flash.out to LAUNCHXL-F280025C (XDS110). Prefers newest numbered CCS; falls back to plain 'ccs'.
cd "$(dirname "$0")"
DS="${DSLITE:-}"
if [ -z "$DS" ]; then
  for d in $HOME/ti/uniflash_* /opt/ti/uniflash_*; do [ -x "$d/dslite.sh" ] && DS="$d/dslite.sh"; done
fi
if [ -z "$DS" ]; then
  BESTVER=0; BESTDS=; FALLBACK=
  for d in $HOME/ti/ccs*/ccs /opt/ti/ccs*/ccs $HOME/ti/ccs* /opt/ti/ccs*; do
    f="$d/ccs_base/DebugServer/bin/DSLite"
    [ -x "$f" ] || continue
    case "$d" in
      */ccs) name=$(basename "$(dirname "$d")") ;;
      *)     name=$(basename "$d") ;;
    esac
    ver=$(echo "$name" | tr -cd '0-9')
    if [ -z "$ver" ]; then
      [ -z "$FALLBACK" ] && FALLBACK=$f
    else
      if [ "$ver" -gt "$BESTVER" ] 2>/dev/null; then BESTVER=$ver; BESTDS=$f; fi
    fi
  done
  DS=${BESTDS:-$FALLBACK}
fi
[ -z "$DS" ] && { echo "ERROR: No UniFlash/CCS DSLite found. Set DSLITE=/path/to/dslite.sh"; exit 1; }
echo "Using: $DS"
case "$DS" in *dslite.sh) "$DS" --mode flash -c TMS320F280025C.ccxml -n 0 -e -f -v blinky_flash.out ;;
                *) "$DS" flash -c TMS320F280025C.ccxml -n 0 -e -f -v blinky_flash.out ;; esac
rc=$?; [ $rc -eq 0 ] && echo "DONE. Press reset; LED4/LED5 blink. J4-40=GPIO0/EPWM1A 20 kHz 40%." || echo "ERROR: flashing failed (rc=$rc)."
exit $rc
