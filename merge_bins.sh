#!/bin/bash

# This script merges ESP32 binaries for web flashing
# To use: ./merge_bins.sh

# Get the directory of the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$DIR/.pio/build/airq"

# Output file name
OUTPUT="$BUILD_DIR/AirQMonitor-merged.bin"

# Run esptool.py to merge binaries
esptool.py --chip esp32s3 merge_bin \
  -o "$OUTPUT" \
  --flash_mode dio \
  --flash_freq 80m \
  --flash_size 16MB \
  0x0 "$BUILD_DIR/bootloader.bin" \
  0x8000 "$BUILD_DIR/partitions.bin" \
  0x10000 "$BUILD_DIR/firmware.bin"

# Check if successful
if [ $? -eq 0 ]; then
  echo "Merged binary created at: $OUTPUT"
else
  echo "Error: Failed to merge binaries"
  exit 1
fi
