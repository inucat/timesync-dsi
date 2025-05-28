# timesync-dsi

NTP Time synchronization tool for Nintendo DSi

## NOTICE

As of 2024-11-22, this program is not compatible with libnds v2.0.0 and later.
Migration is in progress.

## Before build, specify your timezone

**Please change the offset for your timezone BEFORE BUILDING**.
Open `./arm9/source/offset.h` and change the value.

```c
#define TIMEZONE_OFFSET (9 * 3600) // seconds
```

## How to use

You may need to setup Docker and Docker Compose before building.

1. Clone the repo
2. In the repo, run `docker compose run make`
3. Copy `work.nds` to the SD card
4. Run it on the console
