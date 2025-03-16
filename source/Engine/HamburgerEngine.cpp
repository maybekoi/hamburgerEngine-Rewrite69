#include "Engine/HamburgerEngine.hpp"
#include "Engine/Reader.hpp"
#include <cstdio>

bool HamburgerEngine::InitDataFile()
{
    if (!CheckBinFile("Data.bin")) {
        printf("Data.bin not found or invalid\n");
        useBinFile = false;
        return false;
    }

    if (!LoadBinFile("Data.bin")) {
        printf("Failed to load Data.bin\n");
        useBinFile = false;
        return false;
    }

    return true;
}