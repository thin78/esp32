#include "driver/i2c.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_wifi.h"
#include "esp_eth.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/timer.h"
#include "driver/sdmmc_types.h"
#include "driver/spi_common.h"
#include "esp_vfs_fat.h"
#include "esp_spi_flash.h"

//#include <bt/bluedroid/api/include/api/esp_gap_ble_api.h"
//#include <bt/bluedroid/api/include/api/esp_a2dp_api.h"


//https://docs.espressif.com/projects/esp-idf/zh_CN/stable/api-reference/index.html



//nvs_open(const char *name, nvs_open_modeopen_mode, nvs_handle *out_handle)
//nvs_get_str(nvs_handlehandle, const char *key, char *out_value, size_t *length)
//nvs_get_str(nvs_handlehandle, const char *key, char *out_value, size_t *length)
//nvs_set_str(nvs_handlehandle, const char *key, const char *value)
