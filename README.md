# Automatic Pill Dispenser

An ESP32-based automatic pill dispenser. A 28BYJ-48 stepper motor (driven
through a ULN2003 driver board) rotates a wheel with 8 compartments — one
for each day of the week, plus a refill slot — 45° at a time. The ESP32
syncs its clock over NTP and hosts a small web dashboard for manual
control, correcting the current day, and setting a daily auto-rotation
time.

## Hardware

- ESP32 dev board
- 28BYJ-48 stepper motor + ULN2003 driver
- Dispenser wheel with 8 compartments (45° apart)

### Wiring

| ULN2003 pin | ESP32 pin |
|-------------|-----------|
| IN1         | 19        |
| IN2         | 18        |
| IN3         | 5         |
| IN4         | 17        |

## Features

- Web dashboard (served by the ESP32) showing the current time and day
- Manual "one day forward / backward" rotation buttons
- Manual day correction if the wheel gets out of sync
- Configurable daily auto-rotation time
- Motor coils are de-energized between moves to avoid unnecessary heat/draw

## Setup

1. Install [PlatformIO](https://platformio.org/) (VS Code extension or CLI).
2. Copy `include/secrets.h.example` to `include/secrets.h` and fill in your
   Wi-Fi credentials:

   ```cpp
   const char* WIFI_SSID = "your-wifi-ssid";
   const char* WIFI_PASSWORD = "your-wifi-password";
   ```

   `secrets.h` is git-ignored and will not be committed.

3. Build and upload:

   ```bash
   pio run --target upload
   ```

4. Open the Serial Monitor (115200 baud) to see the ESP32's IP address
   once it connects to Wi-Fi, then visit that address in a browser.

## Project layout

```
src/main.cpp        Main firmware (Wi-Fi, web server, stepper control, scheduling)
include/secrets.h    Wi-Fi credentials (git-ignored, create from the .example file)
platformio.ini       Board/framework config and library dependencies
```

## Dependencies

- [AccelStepper](https://github.com/waspinator/AccelStepper) (`waspinator/AccelStepper@^1.64`)
