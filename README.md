# Automatic Pill & Supplement Dispenser

A cheap, open-source, WiFi-controlled pill dispenser — built to compete with $50-100+ commercial devices using easily sourced electronics and a 3D printer.

## Why

I kept forgetting to take my daily pills and supplements, and every "smart" dispenser on the market was either overpriced, locked behind a subscription app, or both. This project is my answer: a rotating carousel dispenser you control from your phone's browser, no app or account required, built entirely from cheap parts and 3D printed components.

## Features

- **8-slot rotating carousel** — one slot per day (Mon–Sun) plus a dedicated refill slot
- **WiFi controlled** — connect from your phone's browser, no app or account needed
- **ESP32 brains** — cheap, capable, and easy to flash
- **Stepper motor driven** — precise, repeatable carousel rotation
- **Micro USB powered** — plug into any standard USB power source
- **3D printed** — assembles with threaded inserts, no glue required
- **Fully open source** — hardware files, wiring, and firmware all included

## Status

This repo covers the V1 design (USB powered). A V2 battery-powered version is currently in progress — follow this repo for updates.

## Hardware

| Component | Notes |
|---|---|
| ESP32 dev board | Any standard variant should work |
| Stepper motor | *(add model/specs)* |
| Stepper driver | *(add model, e.g. A4988/DRV8825)* |
| 3D printed parts | STL files in `/stl` |
| Threaded inserts | *(add size, e.g. M3)* |
| Misc hardware | Screws, wiring — *(add full BOM)* |

Full bill of materials with links to where I sourced each part is loacted in the BOM V1 file

## 3D Printing

STL files are in the `/stl` folder. *(Add recommended print settings: material, layer height, infill, supports needed, print time/cost.)*

## Assembly

*(Add step-by-step assembly instructions or link to a build guide/photos.)*

## Firmware / Setup

1. Flash `firmware/` to your ESP32 using *(Arduino IDE / PlatformIO — specify)*
2. *(Add WiFi setup steps — e.g. connects as access point on first boot)*
3. Open the dispenser's IP address in any browser to control it — no app, no account
4. *(Add any configuration steps, e.g. setting time/day)*

## How It Works

The carousel holds 8 compartments arranged around a central hub. A stepper motor rotates the carousel to align the correct day's compartment with the dispensing opening on a schedule you set through the web interface. The 8th slot is left open for easy refilling without disturbing the weekly rotation.

## Credits

Shoutout to Konrad's original design — the daily-pill-organizer concept was the spark for this project. I wanted to take the idea further with automation, so I designed this from scratch as a motorized, WiFi-controlled version rather than remixing their files. You can check out his project here: 

https://makerworld.com/en/models/1323881-supplement-dispenser-pill-dispenser#profileId-1360644

## Contributing

Issues and pull requests welcome — this is an early, actively developed project and feedback is appreciated.
