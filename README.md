# HeySalad® T5 Voice Terminal

![HeySalad T5 Terminal](Image%20Mock%20Ups/Gemini_Generated_Image_8vikbm8vikbm8vik.png)

> **Voice-enabled payment terminal for emerging markets — accept mobile money with natural speech commands**

A low-cost ($30) voice-first payment device built on the Tuya T5AI-Core, enabling merchants in Zambia and other emerging markets to accept Airtel Money and MTN MoMo payments through simple voice commands.

[![Tuya SDK](https://img.shields.io/badge/Tuya-TuyaOpen%20SDK-orange.svg)](https://github.com/tuya/TuyaOpen)
[![ARM Cortex](https://img.shields.io/badge/MCU-ARM%20Cortex--M33F-blue.svg)](https://developer.arm.com/Processors/Cortex-M33)
[![WiFi](https://img.shields.io/badge/WiFi-802.11%20b%2Fg%2Fn-green.svg)](#hardware-specifications)
[![BLE](https://img.shields.io/badge/BLE-5.4-blue.svg)](#hardware-specifications)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](#license)

---

## 🏆 **Hackathon Submission**

**Tuya IoT Hackathon 2025** — Voice-controlled payment terminal for financial inclusion

---

## 👥 **Team**

**HeySalad OÜ**
| | |
|---|---|
| **Registration** | 17327633 |
| **Location** | Pärnu mnt 139b, 11317 Tallinn, Estonia |
| **Contact** | peter@heysalad.io |

---

## 📋 **Table of Contents**

- [Problem Statement](#-problem-statement)
- [Solution](#-solution)
- [Key Features](#-key-features)
- [Design Mockups](#-design-mockups)
- [System Architecture](#-system-architecture)
- [Hardware Specifications](#-hardware-specifications)
- [3D Printable Enclosure](#-3d-printable-enclosure)
- [Getting Started](#-getting-started)
- [Voice Commands](#-voice-commands)
- [API Integration](#-api-integration)
- [Project Structure](#-project-structure)
- [License](#-license)

---

## 🎯 **Problem Statement**

Small merchants in Zambia and other emerging markets face significant barriers to accepting digital payments:

| Challenge | Impact |
|-----------|--------|
| **💰 High Terminal Costs** | Traditional POS terminals cost $200-500, prohibitive for small vendors |
| **📚 Complex Interfaces** | Existing solutions require literacy and technical knowledge |
| **📶 Limited Connectivity** | Many areas have unreliable internet |
| **🌍 Language Barriers** | Most payment systems only support English interfaces |

---

## 💡 **Solution**

The HeySalad T5 Voice Terminal is a **$30 voice-first payment device** that allows merchants to:

- ✅ Create payment requests using **natural voice commands** in local languages
- ✅ Display **QR codes** for mobile money payments (Airtel Money, MTN MoMo)
- ✅ Receive **audio confirmation** when payments complete
- ✅ Operate with **minimal training** required

### **Target Users**

| User Type | Examples |
|-----------|----------|
| 🛒 **Street Vendors** | Market sellers, food stalls, informal traders |
| 🏪 **Small Shops** | Corner stores, pharmacies, hardware shops |
| 🚕 **Service Providers** | Taxi drivers, hairdressers, repair technicians |
| 🌾 **Rural Merchants** | Areas with limited banking infrastructure |

---

## ✨ **Key Features**

| Feature | Description |
|---------|-------------|
| 🎙️ **Voice Commands** | "Hey Salad, charge fifty kwacha" creates instant payment |
| 📱 **QR Display** | Shows scannable payment code on connected display |
| 🔊 **Audio Feedback** | Text-to-speech confirms payment status |
| 👆 **Push-to-Talk** | Simple button press to activate voice input |
| 📶 **WiFi Connected** | Real-time payment verification via cloud |
| 🔋 **Low Power** | Runs on USB power or battery pack |

---

## 🎨 **Design Mockups**

| Front View | Perspective View |
|:----------:|:----------------:|
| ![Mockup 1](Image%20Mock%20Ups/Gemini_Generated_Image_8vikbm8vikbm8vik.png) | ![Mockup 2](Image%20Mock%20Ups/Gemini_Generated_Image_l5bdfxl5bdfxl5bd.png) |

---

## 🏗️ **System Architecture**

### **Payment Ecosystem Overview**

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      HeySalad Payment Ecosystem                          │
└─────────────────────────────────────────────────────────────────────────┘

┌──────────────┐     WiFi      ┌──────────────────┐     HTTPS     ┌─────────────────┐
│              │◄─────────────►│                  │◄─────────────►│                 │
│  T5 Terminal │               │   Tuya Bridge    │               │  HeySalad Cloud │
│              │               │   (Cloudflare)   │               │                 │
└──────────────┘               └──────────────────┘               └─────────────────┘
       │                              │                                   │
       │ Audio                        │ WebSocket                         │
       ▼                              ▼                                   ▼
┌──────────────┐               ┌──────────────────┐               ┌─────────────────┐
│   Speaker    │               │   Voice Agent    │               │  Payment Links  │
│   + Mic      │               │   (ElevenLabs)   │               │  (QR Codes)     │
└──────────────┘               └──────────────────┘               └─────────────────┘
                                                                          │
                                                                          ▼
                                                                  ┌─────────────────┐
                                                                  │  Mobile Money   │
                                                                  │  (Airtel/MTN)   │
                                                                  └─────────────────┘
```

### **Payment Flow Sequence**

```
  Merchant              T5 Terminal           Tuya Bridge          Payment Service
     │                      │                      │                      │
     │  1. Press Button     │                      │                      │
     │─────────────────────►│                      │                      │
     │                      │                      │                      │
     │  2. "Charge 50 ZMW"  │                      │                      │
     │─────────────────────►│                      │                      │
     │                      │                      │                      │
     │                      │  3. POST /payment    │                      │
     │                      │─────────────────────►│                      │
     │                      │                      │                      │
     │                      │                      │  4. Create Payment   │
     │                      │                      │─────────────────────►│
     │                      │                      │                      │
     │                      │                      │◄─────────────────────│
     │                      │                      │  5. QR Code URL      │
     │                      │◄─────────────────────│                      │
     │                      │  6. Payment URL      │                      │
     │                      │                      │                      │
     │  7. Display QR Code  │                      │                      │
     │◄─────────────────────│                      │                      │
     │                      │                      │                      │
     │  8. "Scan to pay"    │                      │                      │
     │◄─────────────────────│                      │                      │
     │                      │                      │                      │
```

### **Hardware Connection Diagram**

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                           T5AI-Core Development Board                            │
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────┐    │
│  │                              T5-E1 MCU                                   │    │
│  │                        ARM Cortex-M33F @ 480MHz                          │    │
│  │                                                                          │    │
│  │   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐               │    │
│  │   │  WiFi    │  │   BLE    │  │  Audio   │  │  GPIO    │               │    │
│  │   │ 2.4GHz   │  │   5.4    │  │  Codec   │  │  Pins    │               │    │
│  │   └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘               │    │
│  └────────┼─────────────┼─────────────┼─────────────┼───────────────────────┘    │
│           │             │             │             │                            │
└───────────┼─────────────┼─────────────┼─────────────┼────────────────────────────┘
            │             │             │             │
            │             │             │             │
    ┌───────▼───────┐     │     ┌───────▼───────┐     │
    │   Antenna     │     │     │   Speaker     │     │
    │   (Onboard)   │     │     │   (JST Conn)  │     │
    └───────────────┘     │     │   4Ω 1-3W     │     │
                          │     └───────────────┘     │
                          │                           │
                    ┌─────▼─────┐              ┌──────▼──────┐
                    │  Future   │              │             │
                    │  BLE      │              │  P29: Button│──► Push-to-Talk
                    │  Beacon   │              │  P9:  LED   │──► Status
                    └───────────┘              │             │
                                               └─────────────┘
```

---

## 🔧 **Hardware Specifications**

### **Bill of Materials**

| Component | Model | Qty | Purpose | Est. Cost |
|-----------|-------|:---:|---------|----------:|
| Development Board | Tuya T5AI-Core | 1 | Main MCU with WiFi/BLE | $25 |
| Speaker | 4Ω 1-3W | 1 | Audio feedback output | $2 |
| USB Cable | USB-C | 1 | Power and programming | $2 |
| Enclosure | 3D Printed | 1 | Protective housing | $1 |
| **Total** | | | | **~$30** |

### **T5AI-Core Specifications**

| Specification | Value |
|---------------|-------|
| **MCU** | T5-E1 (ARM Cortex-M33F) |
| **Clock Speed** | 480 MHz |
| **Flash** | 4 MB |
| **RAM** | 512 KB |
| **WiFi** | 802.11 b/g/n 2.4GHz |
| **Bluetooth** | BLE 5.4 |
| **Audio** | Onboard MEMS mic + 1W amp |
| **Power** | 5V via USB-C |
| **Dimensions** | 50mm x 30mm |

### **Pin Configuration**

| Pin | Function | Description |
|:---:|----------|-------------|
| P29 | User Button | Push-to-talk activation |
| P9 | User LED | Status indicator |
| CH1 | Microphone | Onboard analog MEMS microphone |
| AMP | Speaker | 4Ω 1-3W speaker via JST connector |
| USB | Power/Debug | USB-C for power and serial debug |

### **LED Status Indicators**

| Pattern | Meaning |
|---------|---------|
| ⚫ Off | Idle, ready for input |
| 🟢 Slow Blink | Listening for voice command |
| 🟡 Fast Blink | Processing request |
| 🟢 Solid On | Success / Connected |
| 🔴 Triple Flash | Error occurred |

---

## 🖨️ **3D Printable Enclosure**

### **STL Files (Ready to Print)**

| File | Description | Download |
|------|-------------|:--------:|
| `heysalad_terminal_top.stl` | Top cover with speaker grille and LED window | [📥](STL%20Files/heysalad_terminal_top.stl) |
| `heysalad_terminal_bottom.stl` | Base with T5AI-Core mounting points | [📥](STL%20Files/heysalad_terminal_bottom.stl) |
| `heysalad_buttons.stl` | Push-to-talk button cap | [📥](STL%20Files/heysalad_buttons.stl) |

### **OpenSCAD Source Files (Customizable)**

| File | Description | Download |
|------|-------------|:--------:|
| `heysalad_terminal_top.scad` | Parametric top cover design | [📥](OpenSCAD/heysalad_terminal_top.scad) |
| `heysalad_terminal_bottom.scad` | Parametric base design | [📥](OpenSCAD/heysalad_terminal_bottom.scad) |
| `heysalad_buttons.scad` | Parametric button design | [📥](OpenSCAD/heysalad_buttons.scad) |

### **Recommended Print Settings**

| Parameter | Value |
|-----------|-------|
| **Material** | PLA or PETG |
| **Layer Height** | 0.2mm |
| **Infill** | 20% |
| **Supports** | Yes (for top cover) |
| **Print Time** | ~3 hours total |

---

## 🚀 **Getting Started**

### **Prerequisites**

- 🐍 Python 3.8+
- 📦 TuyaOpen SDK
- 🔌 CP2105 USB Driver ([Download](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers))

### **1. Install TuyaOpen SDK**

```bash
# Clone TuyaOpen
git clone https://github.com/tuya/TuyaOpen.git
cd TuyaOpen

# Setup Python environment
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

### **2. Clone This Project**

```bash
# Copy to apps directory
git clone https://github.com/Hey-Salad/t5000.git apps/heysalad-t5-terminal
```

### **3. Configure Credentials**

Edit `config/heysalad_config.h`:

```c
// WiFi credentials
#define WIFI_SSID           "your_wifi_ssid"
#define WIFI_PASS           "your_wifi_password"

// Device credentials (from Tuya IoT Platform)
#define TUYA_DEVICE_ID      "your_device_id"
#define TUYA_DEVICE_KEY     "your_device_key"
```

> ⚠️ **Security:** Never commit credentials to version control!

### **4. Build & Flash**

```bash
# Configure for T5AI-Core
tos.py config choice  # Select HEYSALAD_T5AI_CORE

# Build firmware
tos.py build

# Flash to device
tos.py flash --port /dev/tty.SLAB_USBtoUART
```

---

## 🎙️ **Voice Commands**

| Command | Action |
|---------|--------|
| 🗣️ "Hey Salad, charge [amount]" | Create payment QR for specified amount |
| 🗣️ "Hey Salad, check balance" | Query merchant account balance |
| 🗣️ "Hey Salad, last payment" | Show details of last transaction |
| 🗣️ "Hey Salad, help" | List available voice commands |

### **Example Usage**

```
Merchant: *presses button*
Merchant: "Hey Salad, charge fifty kwacha"

Terminal: "Creating payment for 50 Zambian Kwacha..."
Terminal: *displays QR code*
Terminal: "Scan to pay 50 ZMW. Payment expires in 2 minutes."

Customer: *scans QR with Airtel Money app*

Terminal: "Payment received! Thank you."
```

---

## 🔌 **API Integration**

The terminal connects to HeySalad cloud services:

| Service | Purpose | Endpoint |
|---------|---------|----------|
| **Tuya Bridge** | Device authentication and command relay | `tuya-bridge.heysalad-o.workers.dev` |
| **Voice Agent** | Speech-to-text and text-to-speech | `voice-agent.heysalad-o.workers.dev` |
| **Payment Links** | QR code generation and payment tracking | `pay.heysalad.app` |

### **Security**

- 🔒 All API communications use **HTTPS/TLS**
- 🔐 Device credentials stored in **secure flash**
- ⏱️ Payment tokens are **single-use** and **time-limited**
- 🚫 No sensitive data stored on device

---

## 📁 **Project Structure**

```
heysalad-t5-terminal/
├── 📁 config/
│   └── heysalad_config.h          # Configuration (WiFi, API endpoints)
├── 📁 include/
│   └── heysalad_config.h          # Header includes
├── 📁 src/
│   ├── main.c                     # Application entry point
│   └── tuya_main.c                # Tuya SDK integration
├── 📁 Image Mock Ups/             # Concept renders
│   ├── Gemini_Generated_Image_8vikbm8vikbm8vik.png
│   └── Gemini_Generated_Image_l5bdfxl5bdfxl5bd.png
├── 📁 OpenSCAD/                   # Parametric CAD source files
│   ├── heysalad_terminal_top.scad
│   ├── heysalad_terminal_bottom.scad
│   └── heysalad_buttons.scad
├── 📁 STL Files/                  # Ready-to-print 3D models
│   ├── heysalad_terminal_top.stl
│   ├── heysalad_terminal_bottom.stl
│   └── heysalad_buttons.stl
├── CMakeLists.txt                 # Build configuration
├── Kconfig                        # Menuconfig options
├── app_default.config             # Default build settings
└── README.md                      # This file
```

---

## ⚖️ **License**

MIT License — HeySalad Inc. 2025

See [LICENSE](LICENSE) for details.

---

## 🤝 **Contributing**

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

---

## 📞 **Support**

| Channel | Link |
|---------|------|
| 📧 Email | [peter@heysalad.io](mailto:peter@heysalad.io) |
| 🐛 Issues | [GitHub Issues](https://github.com/Hey-Salad/t5000/issues) |
| 💬 Discussions | [GitHub Discussions](https://github.com/Hey-Salad/t5000/discussions) |

---

<div align="center">

**Built with ❤️ for financial inclusion in emerging markets**

*Voice-Controlled • Low-Cost • Open Source*

[⭐ Star this repo](https://github.com/Hey-Salad/t5000) • [🐛 Report Issues](https://github.com/Hey-Salad/t5000/issues) • [💬 Discussions](https://github.com/Hey-Salad/t5000/discussions)

**Powered by:**

[Tuya IoT](https://www.tuya.com/) • [ElevenLabs](https://elevenlabs.io/) • [Cloudflare Workers](https://workers.cloudflare.com/)

</div>

## License

Copyright (C) 2025-2026 HeySalad Inc.

Licensed under the GNU Affero General Public License v3.0 only (AGPL-3.0-only). See [LICENSE](LICENSE) and [NOTICE](NOTICE). If you run a modified version of this software to provide a network service, the AGPL requires you to offer its users the corresponding source code. Versions published before this change remain under their previous licence.
