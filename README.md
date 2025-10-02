
# `Shadow-Login-ESP`

**A educational ESP8266-based captive portal platform designed to demonstrate and analyze common public WiFi authentication techniques for cybersecurity training and authorized penetration testing.**

---

## ⚠️ **Critical Disclaimer: For Authorized Use Only**

`Shadow-Login-ESP` is a tool developed **strictly for educational purposes and authorized security assessments**. It is intended to be used by:

*   **Security Researchers** and **Educators** to demonstrate network vulnerabilities.
*   **Students** learning about ethical hacking and network defense.
*   **Penetration Testers** conducting tests on systems they have explicit, written permission to assess.

**Misuse of this software to gain unauthorized access to any network or data is illegal and strictly prohibited. The developers and contributors assume no liability for any misuse or damage caused by this software. You are solely responsible for ensuring your actions are legal and ethical.**

---

## 📖 Project Overview

`Shadow-Login-ESP` turns an inexpensive ESP8266 module into a controlled environment for studying captive portal security. It mimics the behavior of public WiFi login pages to provide hands-on experience in both offensive security techniques and defensive recognition.

## ✨ Key Features

*   **Captive Portal Activation:** Automatically intercepts web requests, redirecting users to a custom login portal, just like public hotspots.
*   **Credential Harvesting Simulation:** Demonstrates how credentials can be captured in an isolated, offline environment (no data is transmitted externally).
*   **Local Data Storage:** All captured data is stored locally on the device's filesystem for analysis and is wiped upon reset.
*   **Admin Management Interface:** A simple, password-protected web interface to view and manage demonstration data.
*   **OTP/2FA Simulation:** Includes a multi-step login process to teach about complex authentication flows.
*   **Self-Contained & Offline:** The entire system operates independently, with no internet connection or data exfiltration capabilities, ensuring it is safe for lab use.

## 🎯 Educational Value

This project is an invaluable resource for understanding:
*   The mechanics of **captive portals** and **DNS redirection**.
*   The risks associated with connecting to **untrusted public WiFi**.
*   **Social engineering** tactics used in phishing attacks.
*   **Defensive strategies** to identify and avoid malicious networks.

## 🛠️ Hardware Requirements

*   ESP8266 Module (NodeMCU, Wemos D1 Mini, etc.)
*   Micro-USB Cable
*   A Computer (for programming and access to the admin panel)

## 🔧 Getting Started

1.  **Clone the repository.**
2.  **Open the source code** in Arduino IDE or PlatformIO.
3.  **Install the required libraries** (ESP8266WiFi, DNSServer, ESP8266WebServer, LittleFS).
4.  **Upload the code** to your ESP8266 board.
5.  **Power the board** and connect to its WiFi network to begin the demonstration.

## 📜 License

This project is licensed under the **MIT License with a added liability disclaimer**.

**MIT License**

Copyright (c) 2024 [Your Name]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

**ADDITIONAL DISCLAIMER:**

> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.
>
> **This software is intended for EDUCATIONAL AND AUTHORIZED TESTING PURPOSES ONLY. The author is not responsible for any misuse or damage caused by this program. It is the end user's responsibility to obey all applicable local, state, and federal laws.**

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. All contributors must agree that this tool is for educational purposes only.

## 📝 Code of Conduct

Please note that this project is released with a Contributor Code of Conduct. By participating in this project you agree to abide by its terms, which emphasize ethical and legal use of the knowledge and code provided.
