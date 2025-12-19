/**
 * @file tuya_main.c
 * @brief HeySalad T5 Voice Terminal - Main Entry Point
 *
 * Voice-enabled payment terminal for HeySalad
 * - Push-to-talk voice commands
 * - QR code payment generation
 * - Audio feedback via TTS
 * - WiFi connectivity to HeySalad cloud
 *
 * @copyright Copyright (c) 2025 HeySalad OÜ. All Rights Reserved.
 */

#include "tuya_cloud_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tal_api.h"
#include "tuya_config.h"
#include "tuya_iot.h"
#include "netmgr.h"
#include "tkl_output.h"

#if defined(ENABLE_WIFI) && (ENABLE_WIFI == 1)
#include "netconn_wifi.h"
#endif

#include "heysalad_config.h"

// Tuya device handle
static tuya_iot_client_t heysalad_client;

// State
static volatile int g_wifi_connected = 0;
static volatile int g_button_pressed = 0;
static volatile int g_recording = 0;

// Audio buffer
#define AUDIO_BUFFER_MAX (16000 * 5)  // 5 seconds at 16kHz
static uint8_t g_audio_buffer[AUDIO_BUFFER_MAX];
static size_t g_audio_len = 0;

// LED status
typedef enum {
    LED_STATUS_IDLE = 0,
    LED_STATUS_LISTENING,
    LED_STATUS_PROCESSING,
    LED_STATUS_SUCCESS,
    LED_STATUS_ERROR
} led_status_t;

static THREAD_HANDLE g_led_thread = NULL;
static led_status_t g_led_status = LED_STATUS_IDLE;

/**
 * @brief Log output callback
 */
void user_log_output_cb(const char *str)
{
    tal_uart_write(TUYA_UART_NUM_0, (const uint8_t *)str, strlen(str));
}

/**
 * @brief LED control thread
 */
static void led_thread_func(void *arg)
{
    int led_state = 0;
    int blink_delay = 500;
    
    while (1) {
        switch (g_led_status) {
            case LED_STATUS_IDLE:
                tkl_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_LOW);
                tal_system_sleep(100);
                break;
                
            case LED_STATUS_LISTENING:
                blink_delay = 300;
                led_state = !led_state;
                tkl_gpio_write(PIN_USER_LED, led_state ? TUYA_GPIO_LEVEL_HIGH : TUYA_GPIO_LEVEL_LOW);
                tal_system_sleep(blink_delay);
                break;
                
            case LED_STATUS_PROCESSING:
                blink_delay = 100;
                led_state = !led_state;
                tkl_gpio_write(PIN_USER_LED, led_state ? TUYA_GPIO_LEVEL_HIGH : TUYA_GPIO_LEVEL_LOW);
                tal_system_sleep(blink_delay);
                break;
                
            case LED_STATUS_SUCCESS:
                tkl_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_HIGH);
                tal_system_sleep(100);
                break;
                
            case LED_STATUS_ERROR:
                // Triple flash
                for (int i = 0; i < 3; i++) {
                    tkl_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_HIGH);
                    tal_system_sleep(100);
                    tkl_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_LOW);
                    tal_system_sleep(100);
                }
                g_led_status = LED_STATUS_IDLE;
                break;
        }
    }
}

/**
 * @brief Set LED status
 */
static void set_led_status(led_status_t status)
{
    g_led_status = status;
}

/**
 * @brief Button interrupt callback
 */
static void button_irq_cb(void *arg)
{
    TUYA_GPIO_LEVEL_E level;
    tkl_gpio_read(PIN_USER_BUTTON, &level);
    g_button_pressed = (level == TUYA_GPIO_LEVEL_LOW);  // Active low
}

/**
 * @brief Initialize GPIO
 */
static void gpio_init(void)
{
    // LED output
    TUYA_GPIO_BASE_CFG_T led_cfg = {
        .mode = TUYA_GPIO_PUSH_PULL,
        .direct = TUYA_GPIO_OUTPUT,
        .level = TUYA_GPIO_LEVEL_LOW,
    };
    tkl_gpio_init(PIN_USER_LED, &led_cfg);
    
    // Button input with interrupt
    TUYA_GPIO_BASE_CFG_T btn_cfg = {
        .mode = TUYA_GPIO_PULLUP,
        .direct = TUYA_GPIO_INPUT,
        .level = TUYA_GPIO_LEVEL_HIGH,
    };
    tkl_gpio_init(PIN_USER_BUTTON, &btn_cfg);
    
    TUYA_GPIO_IRQ_T irq_cfg = {
        .mode = TUYA_GPIO_IRQ_BOTH,
        .cb = button_irq_cb,
        .arg = NULL,
    };
    tkl_gpio_irq_init(PIN_USER_BUTTON, &irq_cfg);
    tkl_gpio_irq_enable(PIN_USER_BUTTON);
}

/**
 * @brief HTTP POST request helper
 */
static int http_post(const char *url, const char *content_type, 
                     const uint8_t *body, size_t body_len,
                     char *response, size_t response_max)
{
    // Use Tuya HTTP client
    HTTP_HANDLE_T http = NULL;
    int ret = -1;
    
    http = http_client_create();
    if (!http) {
        PR_ERR("Failed to create HTTP client");
        return -1;
    }
    
    http_client_set_url(http, url);
    http_client_set_method(http, HTTP_POST);
    http_client_set_header(http, "Content-Type", content_type);
    http_client_set_body(http, (char *)body, body_len);
    
    ret = http_client_execute(http);
    if (ret == 0) {
        char *resp_body = NULL;
        size_t resp_len = 0;
        http_client_get_response_body(http, &resp_body, &resp_len);
        if (resp_body && resp_len > 0 && response) {
            size_t copy_len = resp_len < response_max - 1 ? resp_len : response_max - 1;
            memcpy(response, resp_body, copy_len);
            response[copy_len] = '\0';
        }
    }
    
    http_client_destroy(http);
    return ret;
}

/**
 * @brief Send voice to HeySalad cloud
 */
static int send_voice_to_cloud(const uint8_t *audio, size_t len, char *response, size_t response_len)
{
    char url[256];
    snprintf(url, sizeof(url), "%s/api/voice/chat", HEYSALAD_TUYA_BRIDGE);
    
    return http_post(url, "audio/wav", audio, len, response, response_len);
}

/**
 * @brief Create payment via HeySalad API
 */
static int create_payment(float amount, const char *currency, char *qr_url, size_t url_len)
{
    char url[256];
    snprintf(url, sizeof(url), "%s/api/payment/create", HEYSALAD_TUYA_BRIDGE);
    
    char body[256];
    snprintf(body, sizeof(body), 
        "{\"amount\":%.2f,\"currency\":\"%s\",\"device_id\":\"%s\"}",
        amount, currency, TUYA_DEVICE_ID);
    
    char response[1024] = {0};
    int ret = http_post(url, "application/json", (uint8_t *)body, strlen(body), response, sizeof(response));
    
    if (ret == 0 && strlen(response) > 0) {
        // Parse QR URL from response
        char *qr_start = strstr(response, "\"qr_url\":\"");
        if (qr_start) {
            qr_start += 10;
            char *qr_end = strchr(qr_start, '"');
            if (qr_end) {
                size_t len = qr_end - qr_start;
                if (len < url_len) {
                    strncpy(qr_url, qr_start, len);
                    qr_url[len] = '\0';
                    return 0;
                }
            }
        }
    }
    
    return -1;
}

/**
 * @brief Play TTS audio
 */
static void play_tts(const char *text)
{
    PR_INFO("TTS: %s", text);
    
    char url[256];
    snprintf(url, sizeof(url), "%s/api/voice/speak", HEYSALAD_VOICE_AGENT);
    
    char body[512];
    snprintf(body, sizeof(body), "{\"text\":\"%s\"}", text);
    
    // TODO: Implement audio playback
    // For now, just log
}

/**
 * @brief Process voice response
 */
static void process_voice_response(const char *response)
{
    PR_INFO("Processing response: %s", response);
    
    // Check for payment action
    if (strstr(response, "\"action\":\"payment\"") != NULL) {
        char *amount_str = strstr(response, "\"amount\":");
        if (amount_str) {
            float amount = atof(amount_str + 9);
            
            char qr_url[256] = {0};
            if (create_payment(amount, DEFAULT_CURRENCY, qr_url, sizeof(qr_url)) == 0) {
                PR_INFO("Payment QR: %s", qr_url);
                set_led_status(LED_STATUS_SUCCESS);
                play_tts("Payment created. Customer can scan the QR code.");
            } else {
                set_led_status(LED_STATUS_ERROR);
                play_tts("Failed to create payment");
            }
        }
    }
    // Check for text response
    else if (strstr(response, "\"text\":") != NULL) {
        char *text_start = strstr(response, "\"text\":\"");
        if (text_start) {
            text_start += 8;
            char *text_end = strchr(text_start, '"');
            if (text_end) {
                size_t len = text_end - text_start;
                char text[512] = {0};
                if (len < sizeof(text) - 1) {
                    strncpy(text, text_start, len);
                    play_tts(text);
                }
            }
        }
    }
}

/**
 * @brief WiFi event callback
 */
static void wifi_event_cb(NETMGR_STATUS_E status)
{
    switch (status) {
        case NETMGR_LINK_UP:
            PR_INFO("WiFi connected!");
            g_wifi_connected = 1;
            set_led_status(LED_STATUS_SUCCESS);
            break;
            
        case NETMGR_LINK_DOWN:
            PR_INFO("WiFi disconnected");
            g_wifi_connected = 0;
            set_led_status(LED_STATUS_ERROR);
            break;
            
        default:
            break;
    }
}

/**
 * @brief Main entry point
 */
void tuya_app_main(void)
{
    PR_INFO("========================================");
    PR_INFO("HeySalad T5 Voice Terminal v1.0");
    PR_INFO("Device ID: %s", TUYA_DEVICE_ID);
    PR_INFO("========================================");
    
    // Initialize GPIO
    gpio_init();
    
    // Start LED thread
    THREAD_CFG_T led_thread_cfg = {
        .priority = THREAD_PRIO_2,
        .stackDepth = 1024,
        .thrdname = "led_ctrl",
    };
    tal_thread_create_and_start(&g_led_thread, NULL, NULL, led_thread_func, NULL, &led_thread_cfg);
    
    set_led_status(LED_STATUS_PROCESSING);
    
    // Connect to WiFi
    PR_INFO("Connecting to WiFi: %s", WIFI_SSID);
    
#if defined(ENABLE_WIFI) && (ENABLE_WIFI == 1)
    netmgr_conn_set(NETCONN_WIFI, wifi_event_cb);
    
    NETCONN_WIFI_CFG_T wifi_cfg = {
        .ssid = WIFI_SSID,
        .pswd = WIFI_PASS,
    };
    netmgr_conn_config(NETCONN_WIFI, &wifi_cfg);
    netmgr_conn_start(NETCONN_WIFI);
#endif
    
    // Wait for WiFi
    int timeout = WIFI_CONNECT_TIMEOUT_MS / 100;
    while (!g_wifi_connected && timeout > 0) {
        tal_system_sleep(100);
        timeout--;
    }
    
    if (!g_wifi_connected) {
        PR_ERR("WiFi connection failed!");
        set_led_status(LED_STATUS_ERROR);
        return;
    }
    
    set_led_status(LED_STATUS_IDLE);
    play_tts("HeySalad terminal ready");
    
    // Main loop
    PR_INFO("Entering main loop - press button to speak");
    
    while (1) {
        if (g_button_pressed && !g_recording) {
            // Start recording
            g_recording = 1;
            g_audio_len = 0;
            set_led_status(LED_STATUS_LISTENING);
            PR_INFO("Recording started...");
            // TODO: Start audio capture
        }
        else if (!g_button_pressed && g_recording) {
            // Stop recording and process
            g_recording = 0;
            set_led_status(LED_STATUS_PROCESSING);
            PR_INFO("Recording stopped, processing %zu bytes", g_audio_len);
            
            if (g_audio_len > 0) {
                char response[1024] = {0};
                int ret = send_voice_to_cloud(g_audio_buffer, g_audio_len, response, sizeof(response));
                
                if (ret == 0 && strlen(response) > 0) {
                    set_led_status(LED_STATUS_SUCCESS);
                    process_voice_response(response);
                } else {
                    set_led_status(LED_STATUS_ERROR);
                    play_tts("Sorry, I didn't understand that");
                }
            }
            
            tal_system_sleep(500);
            set_led_status(LED_STATUS_IDLE);
        }
        
        tal_system_sleep(10);
    }
}
