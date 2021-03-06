#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <errno.h>
#include <algorithm>

// search signature :   83 F8 3C 7E
// replace with     :   83 F8 3C EB

std::vector<unsigned char> load_file(std::string const& filepath)
{
    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

    if (!ifs)
        throw std::runtime_error(filepath + ": " + std::strerror(errno));

    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    auto size = std::size_t(end - ifs.tellg());

    if (size == 0)
        return {};

    std::vector<unsigned char> buffer(size);

    if (!ifs.read((char*)buffer.data(), buffer.size()))
        throw std::runtime_error(filepath + ": " + std::strerror(errno));

    return buffer;
}

void write_file(std::string const& filepath,
    const std::vector<unsigned char>& buffer,
    std::size_t offset) {

    std::fstream ifs;
    ifs.open(filepath, std::fstream::binary | std::fstream::in | std::fstream::out);

    if (!ifs)
        throw std::runtime_error(filepath + ": " + std::strerror(errno));

    ifs.seekp(offset);
    if (!ifs.write((char*)buffer.data(), buffer.size()))
        throw std::runtime_error(filepath + ": " + std::strerror(errno));
}

int main()
{
    std::string file_name{ "astra.dll" };
    std::vector<unsigned char> buffer;

    try {
        buffer = load_file(file_name);
    }
    catch (std::exception ex) {
        std::cout << ex.what() << std::endl;
        system("pause");
        return 200;
    }

    std::vector<unsigned char> signature{
        0x83, 0xF8, 0x3C, 0x7E
    };

    auto offset_it = std::search(buffer.begin(), buffer.end(),
        signature.begin(), signature.end());

    std::size_t offset = std::distance(buffer.begin(), offset_it);
    if (offset == 0 || offset == buffer.size()) {
        std::cout << "Patch failed - unknown version of astra.dll! :(\n";
        system("pause");
        return 100;
    }

    std::vector<unsigned char> patch{
        0x83, 0xF8, 0x3C, 0xEB
    };

    std::filesystem::copy_file(file_name, file_name + "_backup");

    try {
        write_file(file_name, patch, offset);
    }
    catch (std::exception ex) {
        std::cout << ex.what() << std::endl;
        system("pause");
        return 300;
    }

    std::cout << "Patched! :)\n";
    std::cout << "Backup file astra.dll_backup created.\n";

    system("pause");
    return 0;
}