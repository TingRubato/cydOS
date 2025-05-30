#ifndef OTA_UTILS_H
#define OTA_UTILS_H

#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "SdFat.h"

// 分区类型枚举，用于明确目标分区
enum PartitionType {
    BOOTLOADER,
    FIRMWARE,
    BOOT_APP0
};

/**
 * @brief OTA 主任务入口。
 * 
 * @param pvParameter 通常是文件夹名（char*），指定 /apps/<dir_name>/ 目录
 */
void ota_task(void *pvParameter);

/**
 * @brief 执行固件烧写。
 * 
 * @param path SD 卡上的固件文件路径
 * @param partition 目标分区
 * @return esp_err_t ESP_OK 表示成功，否则返回错误码
 */
esp_err_t flash_binary(const char *path, const esp_partition_t *partition);

/**
 * @brief 通用函数：如果文件存在，则烧写到对应分区。
 * 
 * @param full_path 要检查的固件文件路径
 * @param partType 分区类型（使用 PartitionType 枚举）
 * @param fileDesc 日志中用来说明当前操作对象
 * @return esp_err_t ESP_OK 表示成功，否则返回错误码
 */
esp_err_t flashIfExists(const char* full_path, PartitionType partType, const char* fileDesc);

#endif // OTA_UTILS_H