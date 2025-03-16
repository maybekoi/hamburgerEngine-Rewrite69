#include <iostream>
#include <fstream>
#include <cstring>

void WriteString(std::ofstream& out, const char* str) {
    uint8_t len = strlen(str);
    out.write(reinterpret_cast<char*>(&len), 1);
    out.write(str, len);
}

int main() {
    std::ofstream out("GameConfig.bin", std::ios::binary);
    if (!out) {
        std::cout << "Failed to create GameConfig.bin\n";
        return 1;
    }

    WriteString(out, "Template Game");
    WriteString(out, "A simple template game for the Hamburger Engine");

    int windowScale = 2;
    int refreshRate = 60;
    bool startFullScreen = false;
    bool borderless = false;
    bool vsync = true;

    out.write(reinterpret_cast<char*>(&windowScale), sizeof(int));
    out.write(reinterpret_cast<char*>(&refreshRate), sizeof(int));
    out.write(reinterpret_cast<char*>(&startFullScreen), sizeof(bool));
    out.write(reinterpret_cast<char*>(&borderless), sizeof(bool));
    out.write(reinterpret_cast<char*>(&vsync), sizeof(bool));

    std::cout << "Successfully created GameConfig.bin\n";
    return 0;
}