#!/usr/bin/env bash
#
# Install and launch the iOS build of Mergin Maps on a connected device.
#
# Usage:
#   scripts/ios_run_on_device.bash [DEVICE_ID] [APP_PATH]
#
#   DEVICE_ID  devicectl identifier of the device (also via $DEVICE_ID).
#              If omitted, the only connected iOS device is used.
#   APP_PATH   path to the .app bundle (also via $APP_PATH).
#              Default: ./build-ios/app/RelWithDebInfo-iphoneos/MerginMaps.app
#
# Requires Xcode command line tools (xcrun devicectl) and jq.

set -euo pipefail

DEVICE_ID="${1:-${DEVICE_ID:-}}"
APP_PATH="${2:-${APP_PATH:-./build-ios/app/RelWithDebInfo-iphoneos/MerginMaps.app}}"

if [[ ! -d "$APP_PATH" ]]; then
  echo "Error: app bundle not found at '$APP_PATH'" >&2
  exit 1
fi

command -v xcrun >/dev/null || { echo "Error: xcrun is required (install Xcode)" >&2; exit 1; }
xcrun --find devicectl >/dev/null 2>&1 || { echo "Error: devicectl not found (requires Xcode 15+, check xcode-select -p)" >&2; exit 1; }
command -v jq >/dev/null || { echo "Error: jq is required (brew install jq)" >&2; exit 1; }

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

# 1. List devices
echo "==> Listing devices"
# --quiet suppresses devicectl's own table (which shows UDIDs, not the identifier we use)
xcrun devicectl list devices --quiet --omit-deprecated-fields-in-json --json-output "$TMP_DIR/devices.json"

# Normalise each device to "identifier<TAB>name (model)<TAB>platform<TAB>reality<TAB>pairing<TAB>state"
# using the `properties` dictionary.
DEVICES_TSV="$(jq -r '
  .result.devices[]
  | (.properties // {}) as $p
  | [
      .identifier,
      (($p.state.name // "?") + " (" + ($p.hardware.marketingName // "?") + ")"),
      ($p.hardware.platform       // ""),
      ($p.hardware.reality        // ""),
      ($p.connection.pairingState // ""),
      ($p.connection.state        // "")
    ]
  | @tsv' "$TMP_DIR/devices.json")"

{
  printf '%s\t%s\t%s\t%s\t%s\n' "IDENTIFIER" "NAME (MODEL)" "REALITY" "PAIRING" "STATE"
  awk -F'\t' '{ printf "%s\t%s\t%s\t%s\t%s\n", $1, $2, $4, $5, $6 }' <<< "$DEVICES_TSV"
} | column -t -s $'\t'

if [[ -z "$DEVICE_ID" ]]; then
  # Pick paired physical iOS devices that are reachable
  # (state "unavailable" = not plugged in / not on the network)
  CONNECTED=()
  CONNECTED_DESC=()
  while IFS=$'\t' read -r id desc platform reality pairing state; do
    [[ -z "$id" ]] && continue
    [[ "$platform" == "iOS" && "$reality" == "physical" && "$pairing" == "paired" && "$state" != "unavailable" ]] || continue
    CONNECTED+=("$id")
    CONNECTED_DESC+=("$id  $desc  [$state]")
  done <<< "$DEVICES_TSV"

  if [[ ${#CONNECTED[@]} -eq 0 ]]; then
    echo "Error: no connected, paired physical iOS device found (see list above)" >&2
    exit 1
  elif [[ ${#CONNECTED[@]} -gt 1 ]]; then
    echo "Error: multiple devices found, pass one as the first argument:" >&2
    printf '  %s\n' "${CONNECTED_DESC[@]}" >&2
    exit 1
  fi
  DEVICE_ID="${CONNECTED[0]}"
fi
echo "==> Using device $DEVICE_ID"

# 2. Install the app
echo "==> Installing $APP_PATH"
xcrun devicectl device install app \
  --device "$DEVICE_ID" \
  --json-output "$TMP_DIR/install.json" \
  --quiet \
  "$APP_PATH"

BUNDLE_ID="$(jq -r '.result.installedApplications[0].bundleID // empty' "$TMP_DIR/install.json")"
if [[ -z "$BUNDLE_ID" ]]; then
  echo "Error: could not read bundle id from install output" >&2
  exit 1
fi

# 3. Launch with console attached
echo "==> Launching Mergin Maps"
exec xcrun devicectl device process launch --console --device "$DEVICE_ID" "$BUNDLE_ID"
