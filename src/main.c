/**
 * HeySalad T5 Voice Terminal
 * 
 * Voice-enabled payment terminal using Tuya T5AI-Core
 * 
 * Features:
 * - Push-to-talk voice commands
 * - HTTP client for HeySalad cloud APIs
 * - Audio feedback via TTS
 * - WiFi connectivity
 */

#include "tuya_cloud_types.h"
#include "tal_api.h"
#include "tal_wifi.h"
#include "tal_network.h"
#include "tkl_output.h"

// Include configuration
#include "heysalad_config.h"

// Timeout configuration
#define WIFI_TIMEOUT_MS     WIFI_CONNECT_TIMEOUT_MS

// Pin definitions for T5AI-Core
#define PIN_USER_BUTTON     29
#define PIN_USER_LED        9

// State
static volatile BOOL_T g_wifi_connected = FALSE;
static volatile BOOL_T g_button_pressed = FALSE;

// Forward declarations
static void wifi_event_cb(WF_EVENT_E event, void *arg);
static void button_irq_cb(void *arg);
static OPERATE_RET http_post_json(const char *url, const char *json, char *response, size_t resp_len);

/**
 * @brief Main application entry
 */
void tuya_app_main(void)
{
    OPERATE_RET rt = OPRT_OK;
    
    tkl_log_output("\n========================================\n");
    tkl_log_output("HeySalad T5 Voice Terminal v1.0\n");
    tkl_log_output("========================================\n");
    
    // Initialize LED
    TUYA_GPIO_BASE_CFG_T led_cfg = {
        .mode = TUYA_GPIO_PUSH_PULL,
        .direct = TUYA_GPIO_OUTPUT,
        .level = TUYA_GPIO_LEVEL_LOW,
    };
    tal_gpio_init(PIN_USER_LED, &led_cfg);
    
    // Initialize button with interrupt
    TUYA_GPIO_IRQ_T btn_irq = {
        .mode = TUYA_GPIO_IRQ_FALL,
        .cb = button_irq_cb,
        .arg = NULL,
    };
    tal_gpio_irq_init(PIN_USER_BUTTON, &btn_irq);
    tal_gpio_irq_enable(PIN_USER_BUTTON);
    
    // Initialize WiFi
    tkl_log_output("[WiFi] Connecting to %s...\n", WIFI_SSID);
    tal_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_HIGH);  // LED on during connect
    
    rt = tal_wifi_init(wifi_event_cb);
    if (rt != OPRT_OK) {
        tkl_log_output("[WiFi] Init failed: %d\n", rt);
        return;
    }
    
    WF_STATION_CFG_T wifi_cfg = {0};
    strncpy((char *)wifi_cfg.ssid, WIFI_SSID, sizeof(wifi_cfg.ssid) - 1);
    strncpy((char *)wifi_cfg.passwd, WIFI_PASS, sizeof(wifi_cfg.passwd) - 1);
    
    rt = tal_wifi_station_connect(&wifi_cfg);
    if (rt != OPRT_OK) {
        tkl_log_output("[WiFi] Connect failed: %d\n", rt);
        return;
    }
    
    // Wait for WiFi connection
    int timeout = WIFI_TIMEOUT_MS / 100;
    while (!g_wifi_connected && timeout > 0) {
        tal_system_sleep(100);
        timeout--;
    }
    
    if (!g_wifi_connected) {
        tkl_log_output("[WiFi] Connection timeout!\n");
        tal_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_LOW);
        return;
    }
    
    tkl_log_output("[WiFi] Connected!\n");
    tal_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_LOW);
    
    // Test the bridge connection
    tkl_log_output("[HeySalad] Testing bridge connection...\n");
    char response[512] = {0};
    char test_payload[256] = {0};
    snprintf(test_payload, sizeof(test_payload), 
        "{\"amount\":10,\"currency\":\"%s\",\"device_id\":\"%s\",\"description\":\"Test\"}",
        DEFAULT_CURRENCY, TUYA_DEVICE_ID);
    rt = http_post_json(HEYSALAD_TUYA_BRIDGE "/api/payment/create",
        test_payload, response, sizeof(response));
    
    if (rt == OPRT_OK) {
        tkl_log_output("[HeySalad] Bridge OK: %s\n", response);
    } else {
        tkl_log_output("[HeySalad] Bridge error: %d\n", rt);
    }
    
    tkl_log_output("[Main] Ready! Press button to create payment.\n");
    
    // Main loop
    while (1) {
        if (g_button_pressed) {
            g_button_pressed = FALSE;
            
            tkl_log_output("[Button] Creating payment...\n");
            tal_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_HIGH);
            
            // Create payment
            char resp[512] = {0};
            char payment_payload[256] = {0};
            snprintf(payment_payload, sizeof(payment_payload),
                "{\"amount\":50,\"currency\":\"%s\",\"device_id\":\"%s\"}",
                DEFAULT_CURRENCY, TUYA_DEVICE_ID);
            rt = http_post_json(HEYSALAD_TUYA_BRIDGE "/api/payment/create",
                payment_payload, resp, sizeof(resp));
            
            if (rt == OPRT_OK) {
                tkl_log_output("[Payment] Created: %s\n", resp);
                // TODO: Parse QR URL and display/speak it
            } else {
                tkl_log_output("[Payment] Failed: %d\n", rt);
            }
            
            tal_gpio_write(PIN_USER_LED, TUYA_GPIO_LEVEL_LOW);
        }
        
        tal_system_sleep(50);
    }
}

/**
 * @brief WiFi event callback
 */
static void wifi_event_cb(WF_EVENT_E event, void *arg)
{
    switch (event) {
        case WF_EVENT_CONNECT:
            tkl_log_output("[WiFi] Event: Connected\n");
            g_wifi_connected = TRUE;
            break;
        case WF_EVENT_DISCONNECT:
            tkl_log_output("[WiFi] Event: Disconnected\n");
            g_wifi_connected = FALSE;
            break;
        default:
            break;
    }
}

/**
 * @brief Button interrupt callback
 */
static void button_irq_cb(void *arg)
{
    g_button_pressed = TRUE;
}

/**
 * @brief Simple HTTP POST with JSON body
 */
static OPERATE_RET http_post_json(const char *url, const char *json, char *response, size_t resp_len)
{
    OPERATE_RET rt = OPRT_OK;
    
    // Use Tuya HTTP client
    HTTP_CLIENT_T *http = NULL;
    rt = tal_http_client_create(&http);
    if (rt != OPRT_OK) {
        return rt;
    }
    
    // Set headers
    tal_http_client_set_header(http, "Content-Type", "application/json");
    tal_http_client_set_header(http, "X-Device-ID", TUYA_DEVICE_ID);
    
    // POST request
    rt = tal_http_client_post(http, url, json, strlen(json));
    if (rt != OPRT_OK) {
        tal_http_client_destroy(http);
        return rt;
    }
    
    // Get response
    int status = tal_http_client_get_status(http);
    if (status == 200) {
        size_t len = tal_http_client_get_body_len(http);
        if (len > 0 && len < resp_len) {
            memcpy(response, tal_http_client_get_body(http), len);
            response[len] = '\0';
        }
    } else {
        rt = OPRT_COM_ERROR;
    }
    
    tal_http_client_destroy(http);
    return rt;
}
