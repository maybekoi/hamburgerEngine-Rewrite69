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

    WriteString(out, "Template Game"); // Title
    WriteString(out, "A simple template game for the Hamburger Engine"); // Desc

    // Window settings
    int windowScale = 2;
    int refreshRate = 60;
    bool startFullScreen = false;
    bool borderless = false;
    bool vsync = true;
    int screenWidth = 320;
    int screenHeight = 240;
    
    // Game settings
    int targetFPS = 60;
    bool showFPS = true;

    out.write(reinterpret_cast<char*>(&windowScale), sizeof(int));
    out.write(reinterpret_cast<char*>(&refreshRate), sizeof(int));
    out.write(reinterpret_cast<char*>(&startFullScreen), sizeof(bool));
    out.write(reinterpret_cast<char*>(&borderless), sizeof(bool));
    out.write(reinterpret_cast<char*>(&vsync), sizeof(bool));
    
    out.write(reinterpret_cast<char*>(&targetFPS), sizeof(int));
    out.write(reinterpret_cast<char*>(&screenWidth), sizeof(int));
    out.write(reinterpret_cast<char*>(&screenHeight), sizeof(int));
    out.write(reinterpret_cast<char*>(&showFPS), sizeof(bool));

    std::cout << "Successfully created GameConfig.bin\n";
    return 0;
}