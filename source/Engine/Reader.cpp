#define _CRT_SECURE_NO_WARNINGS
#include "Engine/Reader.hpp"
#include "Engine/HamburgerEngine.hpp"
#include <cstring>
#include <map>

std::map<std::basic_string<char>, uint64_t> binFilePositions;
std::map<std::basic_string<char>, uint32_t> binFileSizes;
FileInfo binFileInfo = {};

bool LoadFile(const char* filePath, FileInfo* info) 
{
    if (Engine.useBinFile) {
        auto posIt = binFilePositions.find(filePath);
        auto sizeIt = binFileSizes.find(filePath);
        
        if (posIt != binFilePositions.end() && sizeIt != binFileSizes.end()) {
            *info = binFileInfo;
            info->fileSize = sizeIt->second;
            SetFilePosition(info, posIt->second);
            return true;
        }
        printf("File not found in Data.bin: %s\n", filePath);
        return false;
    }
    
    info->file = fopen(filePath, "rb");
    if (!info->file)
        return false;

    fseek(info->file, 0, SEEK_END);
    info->fileSize = ftell(info->file);
    fseek(info->file, 0, SEEK_SET);
    info->position = 0;
    info->encrypted = false;
    
    return true;
}

void CloseFile(FileInfo* info)
{
    if (info->file)
        fclose(info->file);
    info->file = nullptr;
}

size_t ReadFileData(FileInfo* info, void* dest, size_t size)
{
    if (!info->file)
        return 0;

    size_t result = fread(dest, 1, size, info->file);
    info->position += result;
    return result;
}

void SetFilePosition(FileInfo* info, size_t pos)
{
    if (info->file) {
        fseek(info->file, pos, SEEK_SET);
        info->position = pos;
    }
}

size_t GetFilePosition(FileInfo* info)
{
    return info->position;
}

bool CheckBinFile(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");
    if (!file)
        return false;

    char buffer[6];
    fread(buffer, 1, 5, file);
    buffer[5] = 0;
    fclose(file);

    if (strcmp(buffer, "HBRGR") == 0) {
        Engine.useBinFile = true;
        return true;
    }

    Engine.useBinFile = false;
    return false;
}

bool LoadBinFile(const char* filePath)
{
    binFileInfo.file = fopen(filePath, "rb");
    if (!binFileInfo.file)
        return false;

    fseek(binFileInfo.file, 0, SEEK_END);
    binFileInfo.fileSize = ftell(binFileInfo.file);
    fseek(binFileInfo.file, 0, SEEK_SET);
    binFileInfo.position = 0;
    binFileInfo.encrypted = false;

    char buffer[6];
    ReadFileData(&binFileInfo, buffer, 5);
    buffer[5] = 0;
    
    if (strcmp(buffer, "HBRGR") != 0) {
        CloseFile(&binFileInfo);
        return false;
    }

    SetFilePosition(&binFileInfo, 7);

    uint32_t fileCount;
    ReadFileData(&binFileInfo, &fileCount, 4);

    for (uint32_t i = 0; i < fileCount; i++) {
        uint8_t nameLen;
        char fileName[256];
        ReadFileData(&binFileInfo, &nameLen, 1);
        ReadFileData(&binFileInfo, fileName, nameLen);
        fileName[nameLen] = '\0';

        uint32_t fileSize;
        ReadFileData(&binFileInfo, &fileSize, 4);

        binFilePositions[fileName] = GetFilePosition(&binFileInfo);
        binFileSizes[fileName] = fileSize;

        SetFilePosition(&binFileInfo, GetFilePosition(&binFileInfo) + fileSize);
    }

    return true;
} 