/**
 * HeySalad T5 Terminal Configuration
 * 
 * Copy this file and fill in your credentials.
 * DO NOT commit credentials to version control.
 */

#ifndef HEYSALAD_CONFIG_H
#define HEYSALAD_CONFIG_H

// ============================================
// WiFi Configuration
// ============================================
#define WIFI_SSID           "your_wifi_ssid"
#define WIFI_PASS           "your_wifi_password"
#define WIFI_CONNECT_TIMEOUT_MS  30000

// ============================================
// Tuya Device Credentials
// Get these from Tuya IoT Platform: https://iot.tuya.com
// ============================================
#define TUYA_DEVICE_ID      "your_device_id"
#define TUYA_DEVICE_KEY     "your_device_key"

// ============================================
// HeySalad API Endpoints
// ============================================
#define HEYSALAD_TUYA_BRIDGE    "https://tuya-bridge.heysalad-o.workers.dev"
#define HEYSALAD_VOICE_AGENT    "https://voice-agent.heysalad-o.workers.dev"
#define HEYSALAD_PAYMENT_LINKS  "https://pay.heysalad.app"

// ============================================
// Audio Configuration
// ============================================
#define AUDIO_SAMPLE_RATE   16000
#define AUDIO_BIT_DEPTH     16
#define AUDIO_CHANNELS      1
#define AUDIO_BUFFER_SIZE   1024

// ============================================
// Voice Recognition
// ============================================
#define WAKE_WORD_ENABLED   0  // 0 = push-to-talk, 1 = wake word
#define WAKE_WORD           "Hey Salad"
#define VOICE_TIMEOUT_MS    5000

// ============================================
// Payment Configuration
// ============================================
#define DEFAULT_CURRENCY    "ZMW"
#define PAYMENT_TIMEOUT_SEC 120  // 2 minutes

// ============================================
// Hardware Pins (T5AI-Core)
// ============================================
#define PIN_USER_BUTTON     29  // P29 - Push-to-talk
#define PIN_USER_LED        9   // P9 - Status LED
#define PIN_DISPLAY_CS      15  // Optional SPI display
#define PIN_DISPLAY_DC      16
#define PIN_DISPLAY_RST     18

// ============================================
// LED Status Patterns
// ============================================
#define LED_IDLE            0   // Off
#define LED_LISTENING       1   // Slow blink
#define LED_PROCESSING      2   // Fast blink
#define LED_SUCCESS         3   // Solid on
#define LED_ERROR           4   // Triple flash

// ============================================
// Debug Configuration
// ============================================
#define DEBUG_ENABLED       1
#define DEBUG_UART_BAUD     115200

#if DEBUG_ENABLED
#define DEBUG_LOG(fmt, ...) printf("[HeySalad] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_LOG(fmt, ...)
#endif

#endif // HEYSALAD_CONFIG_H
