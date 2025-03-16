#define _CRT_SECURE_NO_WARNINGS
#include "Engine/Reader.hpp"
#include "Engine/HamburgerEngine.hpp"
#include <cstring>

bool LoadFile(const char* filePath, FileInfo* info) 
{
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
    FileInfo info = {};
    if (LoadFile(filePath, &info)) {
        char buffer[6];
        ReadFileData(&info, buffer, 5);
        buffer[5] = 0;

        if (strcmp(buffer, "HBRGR") == 0) {
            Engine.useBinFile = true;
            CloseFile(&info);
            return true;
        }
        CloseFile(&info);
    }
    Engine.useBinFile = false;
    return false;
}

bool LoadBinFile(const char* filePath)
{
    FileInfo info = {};
    if (!LoadFile(filePath, &info))
        return false;

    SetFilePosition(&info, 7);

    uint32_t fileCount;
    ReadFileData(&info, &fileCount, 4);

    for (uint32_t i = 0; i < fileCount; i++) {
        uint8_t nameLen;
        char fileName[256];
        
        ReadFileData(&info, &nameLen, 1);
        ReadFileData(&info, fileName, nameLen);
        fileName[nameLen] = '\0';

        uint32_t fileSize;
        ReadFileData(&info, &fileSize, 4);

        if (strcmp(fileName, "GameConfig.bin") == 0) {
            FILE* out = fopen("GameConfig.bin", "wb");
            if (!out) {
                CloseFile(&info);
                return false;
            }

            uint8_t buffer[4096];
            while (fileSize > 0) {
                size_t readSize = fileSize > 4096 ? 4096 : fileSize;
                ReadFileData(&info, buffer, readSize);
                fwrite(buffer, 1, readSize, out);
                fileSize -= readSize;
            }
            fclose(out);
            CloseFile(&info);
            return true;
        }

        SetFilePosition(&info, GetFilePosition(&info) + fileSize);
    }

    CloseFile(&info);
    return false;
} 