# Wiring Diagram

| Raspberry Pi 5 Pin | Function | Connects To |
|---|---|---|
| Pin 11 (GPIO17) | Output | 330Ω resistor → LED anode (+) |
| Pin 9 (GND) | Ground | LED cathode (-) |

GPIO17 is configured as OUTPUT by the driver and toggled HIGH/LOW
to turn the LED ON/OFF.
