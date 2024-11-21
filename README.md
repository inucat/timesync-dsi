# timesync-dsi

NTP Time synchronization tool for Nintendo DSi

## NOTICE

As of 2024-11-22, this program is not compatible with libnds v2.0.0 and later.
Migration is in progress.

## Before build, specify your timezone

The clock offset to UTC MUST be specified before building it.
Open `./arm9/source/offset.h` and change the value shown there.

## Build

Once you set up devkitPro toolchain, run `make` on this directory.
