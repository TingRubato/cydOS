/**
 * @file OTA_utils.h
 * @brief OTA update utilities header for cydOS.
 *
 * Declares functions for flashing firmware and handling OTA updates from SD card.
 */
#ifndef OTA_UTILS_H
#define OTA_UTILS_H

#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "SdFat.h"

/**
 * @enum PartitionType
 * @brief Enumerates the types of partitions for OTA updates.
 *
 * - BOOTLOADER: The bootloader partition.
 * - FIRMWARE: The main firmware partition.
 * - BOOT_APP0: The boot_app0 partition.
 */
enum PartitionType {
    BOOTLOADER, ///< The bootloader partition
    FIRMWARE,   ///< The main firmware partition
    BOOT_APP0   ///< The boot_app0 partition
};

/**
 * @brief OTA main task entry point.
 *
 * @param pvParameter Typically a folder name (char*), specifying the /apps/<dir_name>/ directory.
 */
void ota_task(void *pvParameter);

/**
 * @brief Flash a binary file to the specified partition.
 *
 * @param path Path to the firmware binary on the SD card.
 * @param partition Target partition to flash.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t flash_binary(const char *path, const esp_partition_t *partition);

/**
 * @brief If the file exists, flash it to the corresponding partition.
 *
 * @param full_path Path to the firmware file to check and flash.
 * @param partType Partition type (see PartitionType enum).
 * @param fileDesc Description of the file for logging purposes.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t flashIfExists(const char* full_path, PartitionType partType, const char* fileDesc);

#endif // OTA_UTILS_H