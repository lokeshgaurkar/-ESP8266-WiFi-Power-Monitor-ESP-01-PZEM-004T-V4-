# -ESP8266-WiFi-Power-Monitor-ESP-01-PZEM-004T-V4-
This project provides a web-based dashboard that displays electrical parameters such as voltage, current, power, energy consumption, frequency, and power factor. It also includes a built-in electricity bill calculator with tariff configuration.
The system works completely offline using ESP8266 Access Point mode, making it suitable for home monitoring, IoT projects, and smart energy management.

Features
Real-Time Power Monitoring

Displays live electrical parameters:

Voltage (V)

Current (A)

Power (W)

Energy (kWh)

Frequency (Hz)

Power Factor (PF)

Energy Calculation

Energy is calculated using:

Energy (kWh) = Power × Time / 1000

Energy values are stored in EEPROM to prevent data loss after power failure.

Electricity Bill Calculator

Includes a built-in tariff calculator with configurable parameters:

Meter charge

Shipping charge per unit

Electricity tax percentage

Unit price slabs

Example tariff slabs:

0–100 units   → ₹4.28
101–300 units → ₹11.10
300+ units    → ₹15.38

Tax is applied only on energy charges, not the entire bill.

Bill Breakdown

The dashboard shows a detailed bill structure:

Energy Charges

Meter Charges

Shipping Charges

Electricity Tax

Total Bill

Two-Step Energy Reset Protection

Energy reset uses two-step verification with a countdown timer to prevent accidental reset.

Press Reset Energy

Press again within 5 seconds to confirm.

WiFi Access Point Mode

The system runs in AP-only mode for stability.

Connect to:

SSID: PowerMonitor
Password: 11223344

Open dashboard:

http://192.168.4.1

or

http://powermonitor.local
Web Dashboard

Responsive web interface with:

Real-time data updates

Mobile-friendly layout

Modern card-style UI

Configurable electricity tariff

Hardware Required

ESP8266 (ESP-01)

PZEM-004T V4

CT current transformer

3.3V voltage regulator (AMS1117 recommended)

AC load

Applications

Home energy monitoring

Smart electricity meter projects

Appliance power consumption analysis

IoT energy dashboards

Solar energy monitoring

Future Improvements

Possible upgrades:

Real-time power graphs

Daily / monthly energy history

Cloud data logging

Mobile notifications

Overload alerts

Made with ❤️ by Lokesh
