# LED Device Driver for Raspberry Pi 5 — Embedded Linux

A Linux kernel character device driver that controls an LED via GPIO 17 on Raspberry Pi 5 running kernel 6.12.

## What This Project Demonstrates

- Linux kernel module development (LKM)
- Character device driver with `file_operations`
- GPIO control from kernelspace
- Userspace to kernel communication via `/dev/myled`

## Hardware

- Raspberry Pi 5
- LED + 330Ω resistor
- GPIO 17 (Pin 11)

## Wiring

```
Pi Pin 11 (GPIO17) → 330Ω resistor → LED(+)
Pi Pin 9  (GND)    → LED(-)
```

See [docs/wiring.md](docs/wiring.md) for details.

## Project Structure

```
led-kernel-driver-rpi5/
├── driver/
│   ├── led_driver.c    # Main kernel module
│   └── Makefile         # Kernel build file
├── test/
│   └── test_led.c       # Userspace test program
├── docs/
│   └── wiring.md         # GPIO wiring diagram
└── images/               # Demo screenshots
```

## How to Build & Run

```bash
# Install headers
sudo apt install linux-headers-$(uname -r) build-essential

# Compile
cd driver
make

# Load driver
sudo insmod led_driver.ko

# Test
echo "1" | sudo tee /dev/myled   # LED ON
echo "0" | sudo tee /dev/myled   # LED OFF

# Unload
sudo rmmod led_driver
```

## Kernel Log Output

```
[ 1641.376582] myled: Initializing driver
[ 1641.376593] myled: Registered with major number 509
[ 1995.869333] myled: LED ON
[ 1995.886686] myled: LED OFF
[ 2239.096074] myled: Driver unloaded
```

## Environment

- **Board:** Raspberry Pi 5
- **OS:** Raspberry Pi OS (Debian Trixie)
- **Kernel:** 6.12.62+rpt-rpi-2712
- **Architecture:** aarch64
