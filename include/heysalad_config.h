/**
 * @file heysalad_config.h
 * @brief HeySalad T5 Terminal Configuration
 * @copyright Copyright (c) 2025 HeySalad OÜ. All Rights Reserved.
 */

#ifndef HEYSALAD_CONFIG_H
#define HEYSALAD_CONFIG_H

// ============================================
// WiFi Configuration
// ============================================
#ifndef WIFI_SSID
#define WIFI_SSID           "HeySalad_02"
#endif

#ifndef WIFI_PASS
#define WIFI_PASS           "GutenTag%800"
#endif

#define WIFI_CONNECT_TIMEOUT_MS  30000

// ============================================
// Tuya Device Credentials
// ============================================
#ifndef TUYA_DEVICE_ID
#define TUYA_DEVICE_ID      "uuid77f15ace31105442"
#endif

#ifndef TUYA_DEVICE_KEY
#define TUYA_DEVICE_KEY     "v7oqpoZ5nREXsudKP1s8hKIGkj6fxiOA"
#endif

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
#define PIN_USER_BUTTON     TUYA_GPIO_NUM_29  // P29 - Push-to-talk
#define PIN_USER_LED        TUYA_GPIO_NUM_9   // P9 - Status LED

// ============================================
// Debug Configuration
// ============================================
#define DEBUG_ENABLED       1

#endif // HEYSALAD_CONFIG_H
