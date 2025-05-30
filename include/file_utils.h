#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>

class FileUtils {
public:
    static String readFile(const char* path) {
        File file = SD.open(path, FILE_READ);
        if (!file) return "";
        String content = file.readString();
        file.close();
        return content;
    }
    static bool writeFile(const char* path, const char* data, bool overwrite = true) {
        File file = SD.open(path, overwrite ? FILE_WRITE : FILE_APPEND);
        if (!file) return false;
        file.print(data);
        file.close();
        return true;
    }
};

#endif