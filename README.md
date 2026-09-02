# Smart Alcohol & Accident Detection System

A compact ESP32-based system to detect alcohol levels and vehicle accidents (impact), show status on a 16x2 LCD, send telemetry to Blynk, and report GPS location and speed.

<!-- Replace VIDEO_DRIVE_LINK with your Google Drive share link (must be set to "Anyone with the link can view"). -->

[![Project Screenshot 1](https://raw.githubusercontent.com/mirzamamun547/Iot-Project-/main/pictures/iot1.jpeg)](https://raw.githubusercontent.com/mirzamamun547/Iot-Project-/main/pictures/iot1.jpeg)
[![Project Screenshot 2](https://raw.githubusercontent.com/mirzamamun547/Iot-Project-/main/pictures/iot2.jpeg)](https://raw.githubusercontent.com/mirzamamun547/Iot-Project-/main/pictures/iot2.jpeg)
[![Project Screenshot 3](https://raw.githubusercontent.com/mirzamamun547/Iot-Project-/main/pictures/iot4.jpeg)](https://raw.githubusercontent.com/mirzamamun547/Iot-Project-/main/pictures/iot4.jpeg)

> Demo video: **Replace the link below with your Google Drive share link**

[Watch the project demo (Google Drive)](VIDEO_DRIVE_LINK)

---

Table of contents
- [Features](#features)
- [Hardware](#hardware)
- [Wiring / Pinout](#wiring--pinout)
- [Software Requirements](#software-requirements)
- [Configuration](#configuration)
- [Usage](#usage)
- [Calibration & Tuning](#calibration--tuning)
- [Blynk Virtual Pins](#blynk-virtual-pins)
- [Serial Output / Debugging](#serial-output--debugging)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Features
- Alcohol detection using MQ-3 sensor (analog read).
- Crash/impact detection using MPU6050 accelerometer.
- GPS location and speed using a GPS module.
- Local status display on a 16x2 LCD.
- Telemetry to Blynk dashboard (alcohol, crash flag, GPS coordinates, speed, impact magnitude, impact angle).
- Alert LED for accident or alcohol detection.

## Hardware
- ESP32 development board
- MQ-3 alcohol sensor (analog)
- MPU6050 (I2C accelerometer + gyro)
- GPS module (e.g., NEO-6M)
- 16x2 LCD (parallel interface)
- Alert LED
- Wires, breadboard, power supply

## Wiring / Pinout
Pins are taken from the IOT.ino sketch (main branch). Adjust if you rewire.

- MQ-3 (analog) -> ESP32 analog pin 36
- Alert LED -> GPIO 13
- GPS RX -> GPIO 16 (GPS TX -> ESP32 RX)
- GPS TX -> GPIO 17 (GPS RX -> ESP32 TX)
- LCD (parallel):
  - RS -> GPIO 12
  - EN -> GPIO 14
  - D4 -> GPIO 27
  - D5 -> GPIO 26
  - D6 -> GPIO 25
  - D7 -> GPIO 33
- I2C (MPU6050):
  - SDA -> GPIO 21
  - SCL -> GPIO 22

Note: GPS is initialized on HardwareSerial(2) at 9600 bps.

## Software Requirements
- Arduino IDE or PlatformIO
- ESP32 board support installed
- Libraries used in the code (install via Library Manager or PlatformIO):
  - BlynkSimpleEsp32
  - LiquidCrystal
  - Adafruit_MPU6050
  - Adafruit_Sensor
  - TinyGPSPlus
  - Wire

## Configuration
- Blynk template/token and WiFi credentials are present in IOT.ino. **Do not commit production tokens or passwords.**
- Replace the placeholder VIDEO_DRIVE_LINK above with a Google Drive share link (set sharing to "Anyone with the link can view").

## Key parameters and defaults (from code)
- MQ-3 pin: 36
- alcoholThreshold: 1700 (raw ADC average)
- MPU sensitivity threshold (impact): 15.0
- devibrate (vibration debounce): 5 cycles
- GPS baud: 9600
- Timer sampling interval: 500 ms

## Blynk Virtual Pins
- V0: alcoholValue
- V1: crashDetected
- V2: gpsLatitude
- V3: gpsLongitude
- V4: gpsSpeed
- V5: magnitude
- V6: impactAngle

## Usage
1. Connect the hardware following the wiring section.
2. Open IOT.ino in Arduino IDE (or import into PlatformIO).
3. Install required libraries.
4. Replace the Blynk token and WiFi credentials with your own.
5. Select the correct ESP32 board and flash the firmware.
6. Open Serial Monitor at 115200 baud to view debug output.

## Calibration & Tuning
- MQ-3: Warm-up and observe baseline; adjust alcoholThreshold accordingly.
- MPU6050 sensitivity: change `sensitivity` in code (lower = more sensitive).
- devibrate: increase to lengthen suppression time after a trigger.

## Serial Output / Debugging
The firmware prints periodic telemetry useful for calibration and debugging:
- Alcohol, Alcohol Detected, Crash, Magnitude, Impact Angle, GPS Fix, Latitude, Longitude, Speed

## Troubleshooting
- MPU6050 not detected: check I2C wiring and power, run an I2C scanner.
- GPS no fix: test outdoors, confirm RX/TX wiring, confirm baud rate.
- MQ-3 noisy: warm-up sensor, add smoothing or filter.
- Blynk not connecting: confirm WiFi credentials and token/template are correct.

## Contributing
Thank you for your interest! To contribute, follow these steps (commands work on macOS/Linux/Windows with Git installed):

1) Fork the repository on GitHub (click "Fork" on the project page).

2) Clone your fork locally:

```bash
# Replace <your-username> with your GitHub username
git clone https://github.com/<your-username>/Iot-Project-.git
cd Iot-Project-
```

3) Add the upstream remote (original repo) and fetch:

```bash
git remote add upstream https://github.com/mirzamamun547/Iot-Project-.git
git fetch upstream
```

4) Create a new branch for your work (use a descriptive name):

```bash
git checkout -b feature/your-feature-name
```

5) Make your changes (edit code, add docs, add images). When ready, stage and commit:

```bash
git add .
git commit -m "Add short descriptive message about your change"
```

6) Push your branch to your fork:

```bash
git push origin feature/your-feature-name
```

7) Open a Pull Request on GitHub from your branch to the original repository's `main` branch. In the PR description, explain what you changed and why.

Keeping your fork up to date (optional handy steps):

```bash
# on your fork's local main branch
git checkout main
git fetch upstream
git merge upstream/main
# or rebase if you prefer
# git rebase upstream/main
git push origin main
```

Local build/upload (examples)

- Using PlatformIO (recommended if you use VSCode + PlatformIO extension):

```bash
# build
platformio run
# upload (modify environment name as defined in platformio.ini)
platformio run -t upload
```

- Using Arduino CLI (example; set correct FQBN for your ESP32 board):

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 IOT.ino
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 IOT.ino
```

Pull request guidelines
- Keep changes focused and well-described.
- Provide screenshots or logs for UI/behavior changes.
- Ensure sensitive data (tokens/passwords) are not included.

## License
Add a LICENSE file if you want to open-source this project. A common choice is MIT.

## Acknowledgements
- Blynk
- Adafruit libraries
- TinyGPSPlus

---

If you'd like, I can commit this README.md to the repository now. I can also replace the placeholder VIDEO_DRIVE_LINK with a real Google Drive share link if you provide it, or remove the embedded Blynk token and WiFi credentials in IOT.ino and replace them with placeholders. What would you like me to do next?