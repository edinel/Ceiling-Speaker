# Hardware Notes — Bedside Speaker Project

## Overview
A handmade hexagonal Baltic birch speaker driven by an ESP32 Feather V2 with
AirPlay 2. Primary use case: myNoise.net ambient soundscapes at bedside listening
levels. A gift — finish aesthetic was chosen with the recipient in mind.

## Enclosure
- **Type:** Regular hexagonal prism
- **Material:** 1/8" (3.175mm) Baltic birch plywood, raw finish
- **Generated with:** [boxes.py RegularBox](https://boxes.hackerspace-bamberg.de/RegularBox)
- **Hex side length:** 2 7/8" (73.0mm) external
- **Panel height:** 4.25" (107.9mm)
- **Flat-to-flat diameter:** ~4.98" (126.5mm) external
- **Usable baffle width (inside finger joints):** 64.63mm
- **Internal volume:** ~1.29 liters
- **Joinery:** Finger joints — seal all interior joints with wood glue or silicone
- **Damping:** Add polyfill stuffing to raise effective Qtc toward 0.5–0.6

## Driver
- **Model:** Tang Band W2-2136S 2" Aluminum Full-Range Neodymium
- **Source:** Parts Express
- **Impedance:** 4Ω
- **Power handling:** 10W RMS / 20W max
- **Frequency response:** 120–15,000Hz
- **Sensitivity:** 83dB @ 2.83V/1m
- **Fs:** 120Hz, **Qts:** 0.31, **Vas:** 0.258L
- **Frame:** 57mm square (proud mount — sits on top of baffle face)
- **Baffle cutout diameter:** 56mm (clears the irregular ~55mm diagonal of driver basket)
- **Cutout method:** SendCutSend laser cut into hex face panel

## Mounting Hole Positions
The driver has a square mounting pattern, 46mm center-to-center in all 4 directions.
Measured from inside edge of finger joints, along panel centerline:

- **Panel width (inside joints):** 64.63mm
- **Panel center:** 32.315mm from either edge
- **Hole 1:** 9.165mm from inside edge
- **Hole 2:** 55.465mm from inside edge
- **Hole spacing check:** 55.465 - 9.165 = 46.3mm ✓
- **Pattern:** Square — same spacing applies on both axes

## Grille & Escutcheon
- **Grille material:** Natural mulberry silk (3 colors ordered, recipient chooses)
- **Method:** Fabric stretched over driver cutout, clamped under escutcheon
- **Escutcheon:** Square, slightly larger than 57mm driver frame, material TBD
  (options: darker wood veneer, brass sheet, black acrylic — laser cut)
- **Aesthetic:** Raw birch + silk + escutcheon — Arts & Crafts / craft-forward

## DAC + Amplifier
- **Board:** Adafruit TLV320DAC3100 I2S DAC with built-in Class D amp (Product #6309)
- **No external amplifier needed** — TLV320DAC3100 drives the 4Ω speaker directly
- **Output power:** 2.5W into 4Ω (sufficient for bedside myNoise at close range)
- **Configuration:** Requires I2C initialization before audio flows — not plug-and-play
  like PCM5102A. Must call Adafruit_TLV320_I2S library (Arduino) or
  adafruit_tlv320 (CircuitPython) at startup. Integrate into AirPlay firmware
  startup sequence.


### TLV320DAC3100 Wiring
| TLV320DAC3100 Pin | Feather V2 silkscreen | GPIO |
|---|---|---|
| VIN | USB | 5V (must be 5V for speaker output) |
| GND | GND | — |
| SCL | SCL | GPIO20 |
| SDA | SDA | GPIO22 |
| BCK | 27 | GPIO27 |
| WSEL | A0 | GPIO26 |
| DIN | A1 | GPIO25 |
| RST | 33 | GPIO33 |

### Touch Pad Wiring
| Component | Feather V2 silkscreen | GPIO |
|---|---|---|
| Copper hex wire | A5 | GPIO4 |
| 1MΩ resistor → GND | GND | — |

### NeoPixel Chain Wiring
| NeoPixel | Feather V2 silkscreen | Notes |
|---|---|---|
| 5V | USB | 5V rail |
| GND | GND | — |
| DIN | 14 | GPIO14 |

- I2S pins are software-assigned on ESP32 — any available GPIO works
- Avoid GPIO 34–39 (input-only), GPIO 0/2/5/12/15 (strapping), GPIO 6–11 (flash)

## Power
- **Simple 5V USB-C wall wart** — no Power Delivery negotiation needed
- TLV320DAC3100 VIN → ESP32 Feather V2 USB/5V pin
- No PD trigger board, no buck converter, no barrel jack required
- ESP32 USB-C → wall wart directly

## Microcontroller
- **Board:** Adafruit ESP32 Feather V2 (Product #5400)
- **Chip:** ESP32 dual-core 240MHz, 8MB Flash, 2MB PSRAM
- **Power input:** USB-C only (no direct 5V pin bypass)
- **Built-in:** NeoPixel RGB LED (pin 0), red LED (pin 13), user button (GPIO38)

## Touch Interface
- **Method:** ESP32 built-in capacitive touch sensing — no external IC needed
- **Touch pin:** GPIO4 / A5 (Touch0 / T0) — confirmed touch-capable on ESP32
- **Touch pad:** 3/4" copper hexagon stamping blank, 16 gauge (Etsy)
  - Mounted on one hex side panel — hex-on-hex geometry, deliberate design element
  - Large enough to locate and touch confidently in the dark
  - Raw copper patinas over time — complements raw Baltic birch beautifully
- **Wiring:** Solder wire to back of copper hex, run to GPIO4
  - Copper is a significant heat sink — use flux liberally, crank iron to 380–400°C,
    pre-tin both wire and pad separately, heat the copper not the solder
- **Required:** 1MΩ pulldown resistor between GPIO4 and GND for reliable readings
- **Code:** `touchRead(4)` returns lower values when touched; calibrate threshold
  by printing raw values to serial — untouched ~1200, touched ~200 (newer Arduino core)
- **Note:** GPIO37 (also on Feather V2) is NOT touch-capable — input-only pin

## LEDs
- External LEDs planned (count/type TBD)
- Power budget: 5V rail has ~500mA from regulator; ESP32 draws ~240mA peak,
  TLV320 ~30mA, leaving ~230mA for LEDs (~11 standard 20mA LEDs)
- Drive via GPIO with current-limiting resistors, or use NeoPixel chain for
  simpler wiring with more flexibility

## Audio Chain
```
ESP32 (I2S + I2C) → TLV320DAC3100 (built-in Class D) → W2-2136S speaker
```

## Software
- **Framework:** Arduino (ESP32 Arduino core)
- **Bluetooth audio:** [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) by Phil Schatzmann
  - Implements Bluetooth A2DP sink — phone pairs once and connects automatically
  - Outputs audio via I2S directly to TLV320DAC3100
- **DAC init:** Adafruit_TLV320_I2S Arduino library — handles I2C configuration at startup
- **Primary use:** [myNoise.net](https://mynoise.net) ambient soundscapes via Bluetooth
- **Expected firmware size:** ~50–100 lines of straightforward Arduino code
- **Note:** One device pairs at a time — fine for a personal bedside speaker

## Acoustic Notes
- Box volume (~1.29L) is ~5× larger than driver Vas (0.258L)
- Sealed alignment gives Qtc ≈ 0.34 (overdamped) — gentle rolloff from ~130Hz
- Polyfill stuffing recommended to raise effective Qtc
- No port planned — myNoise content needs no deep bass
- At 2.5W max output and 83dB sensitivity, SPL is more than adequate for bedside

---

# Hardware Notes — Bathroom Ceiling Speaker Project

## Overview
An ESP32-S3-based AirPlay 2 receiver driving a passive ceiling speaker, powered
entirely from an existing attic light bulb socket. Primary use case: podcast
listening in the bathroom. All electronics live in the attic above the bathroom.

## Speaker
- **Model:** OSD Audio ICE800TTWRS 8" dual voice coil ceiling speaker
- **Note:** Dual voice coil = two pairs of terminals (L+R stereo). Using one pair only for mono.
- **Location:** Installed in bathroom ceiling, wired up through ceiling to attic

## Amplifier
- **Model:** HiLetgo TPA3116D2 Mono 100W
- **ASIN:** B082F7P184
- **Chip:** TPA3116D2 (Class D, BTL output)
- **Supply voltage:** DC 12–26V
- **Impedance:** 4–8Ω compatible
- **BTL warning:** Never ground either OUT+ or OUT− — floating BTL output

## DAC
- **Board:** Adafruit PCM5122 I2S DAC (Product #6421)
- **No I2C configuration needed** — plug-and-play
- **Line-level output only** — minimum 1kΩ load; cannot drive headphones directly

### PCM5122 DAC Wiring (I2S)
| PCM5122 Pin | XIAO ESP32-S3 silkscreen | GPIO | Notes |
|---|---|---|---|
| VIN | 5V (from USB-C breakout) | — | Accepts 3.3–5V |
| GND | GND | — | — |
| BCK | D3 | GPIO4 | Bit clock |
| DIN | D4 | GPIO5 | Data |
| WSEL | D5 | GPIO6 | Word select |
| MUTE (XSMT) | 3V3 | — | MUST tie high — failsafe-low pin, floats to muted |
| FMT | leave floating | — | Defaults to I2S format |
| MCK | leave unconnected | — | DAC auto-generates from BCK |
| SCL, SDA | leave unconnected | — | Hardware I2S mode, no I2C needed |

### PCM5122 → TPA3116D2 Analog Wiring

Stereo DAC output summed to mono via two equal resistors before amp input.

| PCM5122 | Via | TPA3116D2 |
|---|---|---|
| LOUT | 2.2kΩ to summing node | IN+ |
| ROUT | 2.2kΩ to summing node | IN+ |
| AGND | direct | IN− (signal ground) |

- Use LOUT/ROUT pads, not the 3.5mm headphone jack (line-level only anyway)
- OUT+/OUT− connect directly to one speaker terminal pair (BTL — do not ground either)

## Microcontroller
- **Board:** Seeed Studio XIAO ESP32-S3 (standard, not Sense)
- **Chip:** ESP32-S3R8 — Xtensa dual-core 240MHz, 8MB QIO flash, 8MB OPI PSRAM
- **Antenna:** External 2.4GHz antenna via U.FL connector — required for reliable WiFi
  (PCB trace antenna gives ~-84 dBm at 3 feet; external gives ~-25 dBm)
- **Bluetooth:** BLE 5.0 only — no Classic Bluetooth, A2DP is impossible on this chip
- **Power:** USB-C

### XIAO ESP32-S3 Pin Usage
| Silkscreen | GPIO | Use |
|---|---|---|
| D3 | GPIO4 | I2S BCK |
| D4 | GPIO5 | I2S DIN |
| D5 | GPIO6 | I2S WSEL |
| D6 | GPIO43 | USB CDC TX (console) |
| D7 | GPIO44 | USB CDC RX (console) |
| GPIO21 | GPIO21 | Onboard amber LED (status) |
| D0, D1, D2 | GPIO1, 2, 3 | Free |
| D8, D9, D10 | GPIO7, 8, 9 | Free |

## Power Chain
```
E26 attic light socket
  → Light socket to AC outlet adapter
  → USB-C PD wall wart (65W recommended)
  → USB-C PD trigger board (set to 20V)
  → TPA3116D2 amp (via barrel jack)
  → MPM3610 buck converter (20V → 5V; EN pin MUST tie to VIN)
  → USB-C breakout (VBUS=5V)
  → XIAO ESP32-S3 (USB-C)
```

- MPM3610 EN pin: tie to VIN — if left floating, converter passes 20V unregulated and kills the MCU
- Attic light bulb remains functional — outlet adapter passes through to E26 socket

## LED Status (GPIO21 — onboard amber)
| State | Pattern |
|---|---|
| Standby (no client) | Slow blink |
| Connected / paused | Medium blink |
| Playing | Solid on |

## Audio Chain
```
XIAO ESP32-S3 (I2S) → PCM5122 DAC → 2×2.2kΩ stereo-to-mono → TPA3116D2 amp → ceiling speaker
```

## Software / Firmware
- **Framework:** ESP-IDF (NOT Arduino)
- **Repo:** Ceiling-Speaker (airplay-esp32 merged in)
- **PlatformIO env:** `xiao-esp32s3` (default_envs set to this)
- **WiFi credentials:** stored in gitignored `sdkconfig.defaults.secrets` — copy from `sdkconfig.defaults.secrets.example`
- **Device name / config:** via web UI at device IP address (flash SPIFFS first: `pio run -e xiao-esp32s3 -t uploadfs`)
- **Home Assistant:** auto-discovers as AirPlay media player under Settings → Devices & Services

## Attic Environment Notes
- Electronics are dry (attic, not bathroom) — no moisture concern
- Attic can get hot in summer — avoid placing enclosure in direct sun path
- Use twisted wire pairs for signal cables (Class D amp EMI)
- Keep input signal cables routed away from amp output/switching components

## Context Notes for Code Generation
- **Bedside:** Arduino framework — ESP32-A2DP + Adafruit_TLV320_I2S
  — TLV320DAC3100 (#6309) requires I2C init — drives speaker directly
  — Adafruit ESP32 Feather V2 (#5400) — powered by plain 5V USB-C
  — hexagonal birch enclosure — W2-2136S driver
  — capacitive touch on GPIO4 with 1MΩ pulldown to GND
- **Ceiling:** ESP-IDF framework — AirPlay 2 only (no BT on S3)
  — XIAO ESP32-S3 — PCM5122 (#6421) plug-and-play I2S — feeds TPA3116D2 amp
  — powered from attic light socket via 20V PD chain + MPM3610 buck
  — passive ceiling speaker (OSD ICE800TTWRS, one voice coil pair used)
- Neither project requires battery power — both permanently mains-powered
