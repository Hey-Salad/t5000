# HeySalad T5 Voice Terminal

Voice-enabled payment terminal firmware for the Tuya T5AI-Core development board, enabling merchants in emerging markets to accept mobile money payments through natural voice commands.

## Team

**HeySalad OÜ**
- Registration: 17327633
- Location: Pärnu mnt 139b, 11317 Tallinn, Estonia
- Contact: peter@heysalad.io

## Project Description

### Problem Statement

Small merchants in Zambia and other emerging markets face significant barriers to accepting digital payments:

1. **High Terminal Costs**: Traditional POS terminals cost $200-500, prohibitive for small vendors
2. **Complex Interfaces**: Existing solutions require literacy and technical knowledge
3. **Limited Connectivity**: Many areas have unreliable internet, making cloud-dependent solutions impractical
4. **Language Barriers**: Most payment systems only support English interfaces

### Solution

The HeySalad T5 Voice Terminal is a low-cost ($30), voice-first payment device that allows merchants to:

- Create payment requests using natural voice commands in local languages
- Display QR codes for mobile money payments (Airtel Money, MTN MoMo)
- Receive audio confirmation when payments complete
- Operate with minimal training required

### Target Users

- **Street Vendors**: Market sellers, food stalls, informal traders
- **Small Shops**: Corner stores, pharmacies, hardware shops
- **Service Providers**: Taxi drivers, hairdressers, repair technicians
- **Rural Merchants**: Areas with limited banking infrastructure

### Key Features

| Feature | Description |
|---------|-------------|
| Voice Commands | "Hey Salad, charge fifty kwacha" creates instant payment |
| QR Display | Shows scannable payment code on connected display |
| Audio Feedback | Text-to-speech confirms payment status |
| Push-to-Talk | Simple button press to activate voice input |
| WiFi Connected | Real-time payment verification via cloud |
| Low Power | Runs on USB power or battery pack |

## Design Principles

### System Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         HeySalad Payment Ecosystem                       │
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

### Data Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                              Payment Flow Sequence                               │
└─────────────────────────────────────────────────────────────────────────────────┘

  Merchant              T5 Terminal           Tuya Bridge          Payment Service
     │                      │                      │                      │
     │  Press Button        │                      │                      │
     │─────────────────────►│                      │                      │
     │                      │                      │                      │
     │  "Charge 50 kwacha"  │                      │                      │
     │─────────────────────►│                      │                      │
     │                      │                      │                      │
     │                      │  POST /payment       │                      │
     │                      │─────────────────────►│                      │
     │                      │                      │                      │
     │                      │                      │  Create Payment      │
     │                      │                      │─────────────────────►│
     │                      │                      │                      │
     │                      │                      │◄─────────────────────│
     │                      │                      │  QR Code URL         │
     │                      │◄─────────────────────│                      │
     │                      │  Payment URL         │                      │
     │                      │                      │                      │
     │  Display QR Code     │                      │                      │
     │◄─────────────────────│                      │                      │
     │                      │                      │                      │
     │  "Scan to pay 50 ZMW"│                      │                      │
     │◄─────────────────────│                      │                      │
     │                      │                      │                      │
```

### Hardware Connection Diagram

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

### Pin Configuration

| Pin | Function | Description |
|-----|----------|-------------|
| P29 | User Button | Push-to-talk activation |
| P9 | User LED | Status indicator (idle/listening/processing) |
| CH1 | Microphone | Onboard analog MEMS microphone |
| AMP | Speaker | 4Ω 1-3W speaker via JST connector |
| USB | Power/Debug | USB-C for power and serial debug |

## Hardware List

| Component | Model | Quantity | Purpose |
|-----------|-------|----------|---------|
| Development Board | Tuya T5AI-Core | 1 | Main MCU with WiFi/BLE |
| Speaker | 4Ω 1-3W | 1 | Audio feedback output |
| USB Cable | USB-C | 1 | Power and programming |
| Enclosure | 3D Printed | 1 | Protective housing (optional) |

### T5AI-Core Specifications

| Specification | Value |
|---------------|-------|
| MCU | T5-E1 (ARM Cortex-M33F) |
| Clock Speed | 480 MHz |
| Flash | 4 MB |
| RAM | 512 KB |
| WiFi | 802.11 b/g/n 2.4GHz |
| Bluetooth | BLE 5.4 |
| Audio | Onboard mic + 1W amp |
| Power | 5V via USB-C |
| Dimensions | 50mm x 30mm |

## Project Structure

```
heysalad-t5-terminal/
├── config/
│   └── heysalad_config.h      # Configuration (WiFi, API endpoints)
├── include/
│   └── heysalad_config.h      # Header includes
├── src/
│   ├── main.c                 # Application entry point
│   └── tuya_main.c            # Tuya SDK integration
├── CMakeLists.txt             # Build configuration
├── Kconfig                    # Menuconfig options
├── app_default.config         # Default build settings
└── README.md                  # This file
```

## Getting Started

### Prerequisites

1. **TuyaOpen SDK** - Tuya's IoT development framework
2. **CP2105 USB Driver** - For serial communication
3. **Python 3.8+** - For build tools

### Installation

```bash
# Clone TuyaOpen SDK
git clone https://github.com/tuya/TuyaOpen.git
cd TuyaOpen

# Setup Python environment
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# Copy this project to apps directory
cp -r /path/to/heysalad-t5-terminal apps/
```

### Configuration

Create `config/heysalad_config.h` with your settings:

```c
// WiFi credentials
#define WIFI_SSID           "your_wifi_ssid"
#define WIFI_PASS           "your_wifi_password"

// Device credentials (from Tuya IoT Platform)
#define TUYA_DEVICE_ID      "your_device_id"
#define TUYA_DEVICE_KEY     "your_device_key"
```

### Build & Flash

```bash
# Navigate to TuyaOpen directory
cd TuyaOpen

# Configure for T5AI-Core
tos.py config choice  # Select HEYSALAD_T5AI_CORE

# Build firmware
tos.py build

# Flash to device (find port with: ls /dev/tty.SLAB*)
tos.py flash --port /dev/tty.SLAB_USBtoUART
```

## Voice Commands

| Command | Action |
|---------|--------|
| "Hey Salad, charge [amount]" | Create payment QR for specified amount |
| "Hey Salad, check balance" | Query merchant account balance |
| "Hey Salad, last payment" | Show details of last transaction |
| "Hey Salad, help" | List available voice commands |

## LED Status Indicators

| Pattern | Meaning |
|---------|---------|
| Off | Idle, ready for input |
| Slow Blink | Listening for voice command |
| Fast Blink | Processing request |
| Solid On | Success / Connected |
| Triple Flash | Error occurred |

## API Integration

The terminal connects to HeySalad cloud services:

| Service | Purpose |
|---------|---------|
| Tuya Bridge | Device authentication and command relay |
| Voice Agent | Speech-to-text and text-to-speech |
| Payment Links | QR code generation and payment tracking |

## Security

- All API communications use HTTPS/TLS
- Device credentials stored in secure flash
- Payment tokens are single-use and time-limited
- No sensitive data stored on device

## License

MIT License - HeySalad OÜ 2025

## Contributing

1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Support

- Email: peter@heysalad.io
- GitHub Issues: [Hey-Salad/t5000](https://github.com/Hey-Salad/t5000/issues)
