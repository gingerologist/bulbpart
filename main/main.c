#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_system.h"

/**
 * The following options must be enabled in menuconfig
 *
 * - CONFIG_SPI_FLASH_USE_LEGACY_IMPL
 * - SPI_FLASH_DANGEROUS_WRITE_ALLOWED
 *
 * Using legacy (spi flash) api
 *
 * ref: git@github.com:igrr/esp32-spi-flash-example.git
 */

const char *TAG = "bulbpart";

extern const uint8_t start[] asm("_binary_partition_table_bin_start");
extern const uint8_t end[] asm("_binary_partition_table_bin_end");

uint8_t buf[4096];

void app_main(void) {
    esp_err_t err;
    uint32_t base_addr = 0x8000;
    uint32_t size = (uint32_t)end - (uint32_t)start;
    ESP_LOGI(TAG, "base address: 0x%08x", base_addr);
    ESP_LOGI(TAG, "SPI_FLASH_SEC_SIZE: %d", SPI_FLASH_SEC_SIZE);
    ESP_LOGI(TAG, "new partition data start: %u, end: %u, size: %u",
             (uint32_t)start, (uint32_t)end, size);

    err = spi_flash_read(base_addr, buf, size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_flash_read failed, %s", esp_err_to_name(err));
    }

    bool equal = true;
    for (int i = 0; i < size; i++) {
        if (start[i] != buf[i]) {
            equal = false;
            break;
        }
    }

    if (equal) {
        ESP_LOGI(TAG, "the device has required partition table.");
        vTaskDelay(portMAX_DELAY);
    } else {
        err = spi_flash_erase_sector(base_addr / SPI_FLASH_SEC_SIZE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "spi_flash_erase_sector error: %s",
                     esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "spi_flash_erase_sector succeeded.");
        }

        err = spi_flash_write(base_addr, start, (size_t)end - (size_t)start);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "spi_flash_write failed.");
        } else {
            ESP_LOGI(TAG, "spi_flash_write succeeded.");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
    }
}

