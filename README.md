# ESP8266 Power Monitor ⚡

A **Wi-Fi enabled real-time power monitoring system** built using an ESP8266 and PZEM-004T energy meter module.
The device hosts a **local web dashboard** where users can monitor electrical parameters, track energy consumption, estimate electricity bills, and update firmware over-the-air.

---

## ✨ Features

* ⚡ **Real-time power monitoring**
* 📊 **Voltage, Current, Power, Energy, Frequency, Power Factor display**
* 💡 **Electricity bill calculator**
* 💾 **EEPROM storage for energy and tariff settings**
* 🔁 **Two-step energy reset protection**
* 📡 **Wi-Fi Access Point (no router required)**
* 🌐 **Local web dashboard**
* 🔔 **Browser notifications for power status**
* 📳 **Mobile vibration feedback on button press**
* 🔄 **OTA firmware update**
* 🧠 **Energy calculation independent of web refresh**
* 📱 **Responsive mobile-friendly UI**

---

## 🧰 Hardware Required

| Component           | Description                      |
| ------------------- | -------------------------------- |
| ESP8266             | NodeMCU / ESP-01 / Wemos D1 Mini |
| PZEM-004T v3        | AC energy monitoring module      |
| Current Transformer | Comes with PZEM module           |
| AC Load             | Device being monitored           |
| Power Supply        | 5V supply for ESP8266            |

---

## 🔌 Hardware Connections

Typical UART connection between ESP8266 and PZEM-004T:

| PZEM Pin | ESP8266 Pin |
| -------- | ----------- |
| TX       | RX          |
| RX       | TX          |
| VCC      | 5V          |
| GND      | GND         |

> ⚠️ Always follow proper isolation and safety practices when working with mains voltage.

---

## 📶 Wi-Fi Connection

After powering the device, it creates a Wi-Fi network:

```
SSID: PowerMonitor
Password: 11223344
```

Connect to the network and open:

```
http://192.168.4.1
```

Or if mDNS works:

```
http://powermonitor.local
```

---

## 📊 Dashboard Parameters

The web dashboard displays:

* Voltage (V)
* Current (A)
* Power (W)
* Energy (kWh)
* Frequency (Hz)
* Power Factor (PF)

Data updates every **1.5 seconds**.

---

## 💰 Electricity Bill Calculator

The dashboard includes a built-in electricity bill estimator.

Inputs:

* Meter charge
* Shipping / unit charge
* Tax percentage
* Unit slab rates

Tariff model:

```
0 – 100 units
101 – 300 units
300+ units
```

Tax is applied on the **total charges**:

```
Total Bill = (Energy + Meter + Shipping) + Tax
```

---

## 🔁 Energy Reset

To prevent accidental reset, a **two-step confirmation** is implemented.

1. Press **Reset Energy**
2. Confirm within **5 seconds**

---

## 🔔 Browser Notifications

When power is detected, the browser can display notifications showing:

```
Voltage
Power
Energy
```

Notifications are limited to once per minute to avoid spam.

---

## 📳 Mobile Haptic Feedback

When buttons are pressed on supported devices, the browser triggers a short vibration using the **Vibration API**.

Supported mainly on **Android browsers**.

---

## 🔄 OTA Firmware Update

Firmware can be updated directly from the browser.

Open:

```
http://192.168.4.1/update
```

Upload the compiled `.bin` firmware file.

The device will:

1. Upload firmware
2. Flash it
3. Reboot automatically

---

## ⚙️ Software Architecture

The ESP8266 runs a lightweight web server using:

* ESP8266WebServer
* ESP8266HTTPUpdateServer
* PZEM004Tv30 library
* EEPROM storage

System workflow:

```
PZEM Sensor
      ↓
ESP8266 reads electrical data
      ↓
Energy calculation
      ↓
Web API provides J
```
📂 API Endpoints
Endpoint	Function
/	Dashboard UI
/data	Returns sensor data (JSON)
/reset	Reset energy counter
/tariff	Save billing settings
/update	OTA firmware update
📦 Libraries Used

ESP8266WiFi

ESP8266WebServer

ESP8266HTTPUpdateServer

ESP8266mDNS

PZEM004Tv30

EEPROM

⚡ Energy Calculation

Energy is calculated using:

Energy (kWh) = Power (W) × Time (hours) / 1000

The system integrates power over time to estimate energy consumption.

📱 Supported Browsers

Fully supported:

Chrome

Edge

Android Chrome

Samsung Internet

Limited support:

iOS Safari (no vibration API)

🚀 Possible Future Improvements

📈 Live power consumption graphs

📅 Daily / monthly energy history

☁️ Cloud data logging

📲 Progressive Web App (PWA)

🔌 Smart relay control

🏠 Home automation integration

📜 License

MIT License

👨‍💻 Author

Lokesh Gaurkar

Made with ❤️ using ESP8266 and PZEM energy monitoring.
