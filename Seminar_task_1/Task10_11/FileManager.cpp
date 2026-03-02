#include "FileManager.hpp"
#include "Student.hpp"
#include <iostream>
#include <cstring>

size_t FileManager::getHeaderSize() {
    return sizeof(FileHeader);
}

bool FileManager::saveToFile(const std::string& filename,
    const std::vector<Student*>& students,
    const std::vector<double>& allGrades) {

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file for writing: " << filename << std::endl;
        return false;
    }

    FileHeader header;
    strncpy(header.signature, DEFAULT_SIGNATURE, 4);
    header.signature[3] = '\0';
    header.version = CURRENT_VERSION;
    header.studentCount = static_cast<uint32_t>(students.size());

    std::cout << "Header structure size: " << getHeaderSize() << " bytes" << std::endl;

    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    uint32_t gradesCount = static_cast<uint32_t>(allGrades.size());
    file.write(reinterpret_cast<char*>(&gradesCount), sizeof(gradesCount));

    if (!allGrades.empty()) {
        file.write(reinterpret_cast<const char*>(allGrades.data()),
            allGrades.size() * sizeof(double));
    }

    file.close();
    std::cout << "Data successfully saved to file: " << filename << std::endl;
    return true;
}

bool FileManager::loadFromFile(const std::string& filename,
    std::vector<Student*>& students,
    std::vector<double>& allGrades) {

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file for reading: " << filename << std::endl;
        return false;
    }

    FileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (!checkSignature(filename)) {
        std::cerr << "Error: invalid file signature!" << std::endl;
        file.close();
        return false;
    }

    std::cout << "File header:" << std::endl;
    std::cout << "  Signature: " << header.signature << std::endl;
    std::cout << "  Version: " << header.version << std::endl;
    std::cout << "  Student count: " << header.studentCount << std::endl;

    if (header.version != CURRENT_VERSION) {
        std::cerr << "Warning: file version (" << header.version
            << ") differs from current (" << CURRENT_VERSION << ")" << std::endl;
    }

    uint32_t gradesCount;
    file.read(reinterpret_cast<char*>(&gradesCount), sizeof(gradesCount));
    std::cout << "  Grades count: " << gradesCount << std::endl;

    allGrades.clear();
    if (gradesCount > 0) {
        allGrades.resize(gradesCount);
        file.read(reinterpret_cast<char*>(allGrades.data()),
            gradesCount * sizeof(double));
    }

    file.close();
    std::cout << "Data successfully loaded from file: " << filename << std::endl;
    return true;
}

bool FileManager::checkSignature(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    char signature[4] = { 0 };
    file.read(signature, 4);
    file.close();

    return strcmp(signature, DEFAULT_SIGNATURE) == 0;
}