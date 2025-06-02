/**
 * @file SD_utils.h 
 * @brief SD Card Filesystem Utilities for Embedded Systems
 * 
 * Provides robust filesystem operations with:
 * - Error handling for embedded environments
 * - Minimal memory footprint
 * - Blocking/non-blocking control
 * 
 * Key Features:
 * - FAT32 filesystem support
 * - Directory operations
 * - File listing with metadata
 *
 * Design Principles:
 * - No dynamic memory allocation in critical paths
 * - Reentrant and thread-safe implementations  
 * - Bounded execution time
 * - Hardware fault tolerance
 *
 * @version 2.1
 * @date 2025-06-01
 * @copyright Copyright (c) 2025 BHS Inc. All Rights Reserved.
 */

#ifndef SD_UTILS_H
#define SD_UTILS_H

#include <SdFat.h>
#include <vector>
#include <string>

// Forward declarations to minimize dependencies
class SdFile;

// Global SD card instance (extern to avoid multiple definitions)
extern SdFat sd;

/**
 * @brief File metadata structure
 * @note Fixed-size buffers for embedded safety
 */
struct FileInfo {
    char name[64];      ///< Null-terminated filename (8.3 format compatible)
    uint32_t size;      ///< File size in bytes (0 for directories)
};

/**
 * @brief Initialize SD card interface
 * @return true if successful, false on error
 * @note Implements hardware fault recovery
 * @warning Blocking operation (max 2s timeout)
 */
bool init_sd_card();

/**
 * @brief Verify/Create directory path
 * @param path Directory path (absolute)
 * @return true if path exists/was created
 * @note Atomic operation (power-safe)
 */
bool check_and_create_dir(const char *path);

/**
 * @brief Open directory handle
 * @param[out] dir Reference to SdFile object
 * @param path Directory path to open  
 * @return true if opened successfully
 * @warning Handle must be closed by caller
 */
bool open_dir(SdFile &dir, const char *path);

/**
 * @brief List directory contents
 * @param dir Valid directory handle
 * @return Vector of FileInfo structures
 * @note Memory-efficient implementation
 */
std::vector<FileInfo> list_files_in_dir(SdFile &dir);

/**
 * @brief Create new directory
 * @param path Parent directory path
 * @param dirName New directory name
 * @return true if created successfully
 * @note Implements full path creation if needed
 */
bool create_directory(const char *path, const char *dirName);

#endif // SD_UTILS_H
