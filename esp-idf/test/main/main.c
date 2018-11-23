#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "mdns.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_event_legacy.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "esp_system.h"
//#include <sys/socket.h>
#include "inc.h"


#define TAG "test"
#define AP_SSID "esp32"
#define AP_PASS "esp123"

#define TO_SSID "CdKobble"
#define TO_PASS "Kobble2014"

#define LABEL  "web"
#define WPATH  "/"//"/web"

static wl_handle_t wh;
static EventGroupHandle_t egrp;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{

#if 1
    switch(event->event_id) {
        
        case SYSTEM_EVENT_STA_START:
        //esp_wifi_connect();
        break;
        
        case SYSTEM_EVENT_SCAN_DONE:
        //xEventGroupSetBits(wifi_event_group, SCAN_DONE_BIT);        //设置事件位
        break;
        
        case SYSTEM_EVENT_STA_GOT_IP:
        break;
        
        case SYSTEM_EVENT_AP_STA_GOT_IP6:
        break;
        
        case SYSTEM_EVENT_STA_DISCONNECTED:
        break;
        
        case SYSTEM_EVENT_ETH_CONNECTED:
        break;
        
        case SYSTEM_EVENT_ETH_DISCONNECTED:
        break;
        
        case SYSTEM_EVENT_ETH_GOT_IP:
        break;
        
        default:
        break;
    }
#endif
    
    return ESP_OK;
}


static void wifi_init()
{
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    /* Initializing WiFi */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); 
    
#if 1    
    wifi_config_t sta_config = {
        .sta = {
            .ssid = TO_PASS,
            .password = TO_PASS,
            .bssid_set = false,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
#endif

    wifi_config_t ap_config = {
        .ap = {
            .authmode = WIFI_AUTH_OPEN,
            .password = "",//AP_PASS;
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .max_connection = 5,//CONFIG_MAX_STA_CONN,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

static void eth_init()
{
    
}

static void dns_init()
{
    //mdns_init();
    //mdns_hostname_set();
    //mdns_query_a();
    
    
}

static void net_init()
{
    tcpip_adapter_init();
    wifi_init();
    eth_init();
    dns_init();
}

static void nvs_init()
{
    nvs_flash_init();
}

#define MAXL   512
#define DOTDIR(a) ((a[0]=='.' && a[1]==0) || (a[0]=='.' && a[1]=='.' && a[2]==0))
static int f_scan(char *path)
{
    int l;
    char *fn;
    FF_DIR dir;
    FILINFO finfo;
    char npath[MAXL];
    
    fn = finfo.fname;
    if (f_opendir(&dir,(const TCHAR*)path)==FR_OK) {
        while(f_readdir(&dir, &finfo)==FR_OK) {
            if (!fn[0]) break;  
            if (DOTDIR(fn)) continue;
            
            printf("___f: %s\n", fn);
            if (finfo.fattrib & AM_DIR) {
                l = strlen(path);
                if(path[l-1]=='/') {
                    sprintf(npath, "%s%s", path, fn);
                }
                else {
                    sprintf(npath, "%s/%s", path, fn);
                }
                f_scan(npath);
            }
        } 
        f_closedir(&dir);
    }
    else {
        printf("opendir %s failed\n", path);
    }    
    
    return 0;
}

static void fs_test()
{
    f_scan(WPATH); 
}

static void spiffs_init(const char *path, const char *label)
{
    const esp_vfs_spiffs_conf_t cfg = {
        .base_path = path,
        .partition_label = label,
        .max_files = 100,
        .format_if_mount_failed = false
    };
    
    esp_err_t err = esp_vfs_spiffs_register(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to mount spiffs (%s)", esp_err_to_name(err));
        return;
    }
    
    //for test
    const char *fpath="/web/index.html";
    FILE* fd = fopen(fpath, "rb");
    if (fd == NULL) {
        ESP_LOGE(TAG, "Failed to open file!");
        return;
    }

    char buf[200];
    size_t rl=0;
    rl = fread(buf, 100, 1, fd);
    buf[100]=0;
    printf("___rl: %d\n", rl);
    for(int i=0; i<20; i++) {
        printf("___buf[%d]: 0x%02x\n", i, buf[i]);
    }
    
    fclose(fd);
}



#if 0
static void gpio_init2()
{
    gpio_config_t gc;
    
    gc.intr_type = GPIO_PIN_INTR_DISABLE;
    gc.pin_bit_mask = 1;
    gc.mode = GPIO_MODE_INPUT;
    gc.pull_up_en = 1;
    gc.pull_down_en = 0;
    gpio_config(&gc);
    
    //bool new_level = gpio_get_level(GPIO_NUM_0);
}


static void get_info()
{
    tcpip_adapter_ip_info_t ip_info;
    
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info);
    char *ip_addr = ip4addr_ntoa(&ip_info.ip);
    //strcpy(resp_data->conn_info.ip_addr, ip_addr);

    /* AP information to which STA is connected */
    wifi_ap_record_t ap_info;
    esp_wifi_sta_get_ap_info(&ap_info);
}
#endif

static void fs_init()
{
    const esp_vfs_fat_mount_config_t cfg = {
        .max_files = 100,
        .format_if_mount_failed = false,
        //.allocation_unit_size = 512
    };
    
    esp_err_t err = esp_vfs_fat_rawflash_mount(WPATH, LABEL, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to mount fatfs (%s)", esp_err_to_name(err));
        return;
    }
}

static void fs_free()
{
    esp_vfs_fat_rawflash_unmount(WPATH, LABEL);
}



void app_main(void)
{
    uint32_t bs,bits = BIT0 | BIT1;
    
    nvs_init();
    net_init();
    fs_init();
    
    fs_test();
    
    xTaskCreate(http_task, "http_task", 9000, NULL, uxTaskPriorityGet(NULL), NULL);
    xTaskCreate(ws_task,   "ws_task",   9000, NULL, uxTaskPriorityGet(NULL), NULL);
    
    egrp = xEventGroupCreate();
    while(1) {
        
        #if 0
        bs = xEventGroupWaitBits(egrp, bits, false, true, 0);
        if(bs & BIT0) {
            //do something...
        }
        #else
        vTaskDelay(2000/portTICK_PERIOD_MS);
        #endif
    }
}





