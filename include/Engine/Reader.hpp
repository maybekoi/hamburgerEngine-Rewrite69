#ifndef HAMBURGER_READER_H
#define HAMBURGER_READER_H

#include <cstdio>
#include <string>
#include <map>

struct FileInfo {
    FILE* file;
    size_t fileSize;
    size_t position;
    bool encrypted;
};

bool LoadFile(const char* filePath, FileInfo* info);
void CloseFile(FileInfo* info);
size_t ReadFileData(FileInfo* info, void* dest, size_t size);
void SetFilePosition(FileInfo* info, size_t pos);
size_t GetFilePosition(FileInfo* info);

bool CheckBinFile(const char* filePath);
bool LoadBinFile(const char* filePath);

extern std::map<std::string, size_t> binFilePositions;
extern std::map<std::string, uint32_t> binFileSizes;
extern FileInfo binFileInfo;

#endif // HAMBURGER_READER_H