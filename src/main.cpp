// Ceiling Speaker — AirPlay 2 receiver firmware
// Hardware: Seeed Studio XIAO ESP32C5
//           Adafruit PCM5100 I2S DAC (#6251)
//           HiLetgo TPA3116D2 100W Mono Class D amp (ASIN B082F7P184)
//           OSD Audio ICE800TTWRS passive ceiling speaker

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "ceiling-speaker";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Ceiling Speaker starting");
    // TODO: initialize WiFi (with periodic watchdog in main loop)
    // TODO: initialize I2S → PCM5100 DAC
    // TODO: start AirPlay 2 receiver (rbouteiller/airplay-esp32)
}
