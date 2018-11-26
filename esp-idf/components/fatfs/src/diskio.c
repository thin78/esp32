/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/* ESP-IDF port Copyright 2016 Espressif Systems (Shanghai) PTE LTD      */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "diskio.h"		/* FatFs lower layer API */
#include "ffconf.h"
#include "ff.h"
#include "esp_spi_flash.h"


//#define USE_SELF

static ff_diskio_impl_t * s_impls[FF_VOLUMES] = { NULL };

#if FF_MULTI_PARTITION		/* Multiple partition configuration */
PARTITION VolToPart[] = {
    {0, 0},    /* Logical drive 0 ==> Physical drive 0, auto detection */
    {1, 0}     /* Logical drive 1 ==> Physical drive 1, auto detection */
};
#endif

esp_err_t ff_diskio_get_drive(BYTE* out_pdrv)
{
    BYTE i;
    for(i=0; i<FF_VOLUMES; i++) {
        if (!s_impls[i]) {
            *out_pdrv = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

void ff_diskio_register(BYTE pdrv, const ff_diskio_impl_t* discio_impl)
{
    assert(pdrv < FF_VOLUMES);

    if (s_impls[pdrv]) {
        ff_diskio_impl_t* im = s_impls[pdrv];
        s_impls[pdrv] = NULL;
        free(im);
    }

    if (!discio_impl) {
        return;
    }

    ff_diskio_impl_t * impl = (ff_diskio_impl_t *)malloc(sizeof(ff_diskio_impl_t));
    assert(impl != NULL);
    memcpy(impl, discio_impl, sizeof(ff_diskio_impl_t));
    s_impls[pdrv] = impl;
}

DSTATUS ff_disk_initialize (BYTE pdrv)
{
    #ifdef USE_SELF
    return RES_OK;
    #else
    return s_impls[pdrv]->init(pdrv);
    #endif
}
DSTATUS ff_disk_status (BYTE pdrv)
{
    #ifdef USE_SELF
    return RES_OK;
    #else
    return s_impls[pdrv]->status(pdrv);
    #endif
}
DRESULT ff_disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    esp_err_t r;
    
    #ifdef USE_SELF
    r = spi_flash_read(OFFSET+sector*SSIZE,buff,count*SSIZE);
    return (r==ESP_OK)?RES_OK:RES_ERROR;
    #else
    return s_impls[pdrv]->read(pdrv, buff, sector, count);
    #endif
}
DRESULT ff_disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
    esp_err_t r;
    
    #ifdef USE_SELF
    r = spi_flash_write(OFFSET+sector*SSIZE,buff,count*SSIZE);
    return (r==ESP_OK)?RES_OK:RES_ERROR;
    #else
    return s_impls[pdrv]->write(pdrv, buff, sector, count);
    #endif
}
DRESULT ff_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
    #ifdef USE_SELF
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD *) buff) = SCOUNT;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *) buff) = SSIZE;
            return RES_OK;
        case GET_BLOCK_SIZE:
            return RES_ERROR;
    }
    return RES_OK;
    #else
    return s_impls[pdrv]->ioctl(pdrv, cmd, buff);
    #endif
}

DWORD get_fattime(void)
{
    time_t t = time(NULL);
    struct tm tmr;
    localtime_r(&t, &tmr);
    int year = tmr.tm_year < 80 ? 0 : tmr.tm_year - 80;
    return    ((DWORD)(year) << 25)
            | ((DWORD)(tmr.tm_mon + 1) << 21)
            | ((DWORD)tmr.tm_mday << 16)
            | (WORD)(tmr.tm_hour << 11)
            | (WORD)(tmr.tm_min << 5)
            | (WORD)(tmr.tm_sec >> 1);
}
