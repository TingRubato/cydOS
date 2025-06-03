/**
 * @file file_utils.h
 * @brief File utility class for reading and writing files on SD card.
 *
 * Provides static methods for reading and writing files using Arduino SD library.
 */
#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>

class FileUtils {
public:
    /**
     * @brief Reads the contents of a file from the SD card.
     * @param path Path to the file to read.
     * @return The file contents as a String, or an empty String if the file cannot be opened.
     */
    static String readFile(const char* path) {
        File file = SD.open(path, FILE_READ);
        if (!file) return "";
        String content = file.readString();
        file.close();
        return content;
    }
    /**
     * @brief Writes data to a file on the SD card.
     * @param path Path to the file to write.
     * @param data Data to write to the file.
     * @param overwrite If true, overwrites the file; if false, appends to the file.
     * @return True if the write was successful, false otherwise.
     */
    static bool writeFile(const char* path, const char* data, bool overwrite = true) {
        File file = SD.open(path, overwrite ? FILE_WRITE : FILE_APPEND);
        if (!file) return false;
        file.print(data);
        file.close();
        return true;
    }
};

#endif