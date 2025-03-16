#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void WriteString(std::ofstream& out, const std::string& str) {
    uint8_t len = static_cast<uint8_t>(str.length());
    out.write(reinterpret_cast<char*>(&len), 1);
    out.write(str.c_str(), len);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: SizzleHE <input_folder> <output_file>\n";
        return 1;
    }

    std::ofstream out(argv[2], std::ios::binary);
    if (!out) {
        std::cout << "Failed to create output file\n";
        return 1;
    }

    out.write("HBRGR", 5);

    uint16_t version = 1;
    out.write(reinterpret_cast<char*>(&version), 2);

    fs::path inputPath(argv[1]);
    std::vector<fs::path> files;

    for (const auto& entry : fs::recursive_directory_iterator(inputPath)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    uint32_t fileCount = static_cast<uint32_t>(files.size());
    out.write(reinterpret_cast<char*>(&fileCount), 4);

    for (const auto& file : files) {
        std::string relativePath = fs::relative(file, inputPath).string();
        WriteString(out, relativePath);

        uint32_t fileSize = static_cast<uint32_t>(fs::file_size(file));
        out.write(reinterpret_cast<char*>(&fileSize), 4);

        std::ifstream in(file, std::ios::binary);
        out << in.rdbuf();
    }

    std::cout << "Successfully compiled " << fileCount << " files into Data.bin\n";
    return 0;
}