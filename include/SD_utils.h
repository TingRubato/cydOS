#ifndef SD_UTILS_H
#define SD_UTILS_H

#include <SdFat.h>
#include <vector>
#include <string>

// Forward declarations
class SdFile;

// External declaration of sd object
extern SdFat sd;

struct FileInfo {
    char name[64];
    uint32_t size;
};

bool init_sd_card(); // The magic trick to make storage appear
bool check_and_create_dir(const char *path);
bool open_dir(::SdFile &dir, const char *path);
std::vector<FileInfo> list_files_in_dir(::SdFile &dir);
bool create_directory(const char *path, const char *dirName);

#endif // SD_UTILS_H
