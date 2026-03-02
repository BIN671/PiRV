#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <string>
#include <vector>
#include <fstream>

#pragma pack(push, 1)
struct FileHeader {
    char signature[4];
    uint16_t version;
    uint32_t studentCount;
};
#pragma pack(pop)

class FileManager {
private:
    static constexpr const char* DEFAULT_SIGNATURE = "GRD";
    static constexpr uint16_t CURRENT_VERSION = 1;

public:
    static bool saveToFile(const std::string& filename,
        const std::vector<class Student*>& students,
        const std::vector<double>& allGrades);
    static bool loadFromFile(const std::string& filename,
        std::vector<class Student*>& students,
        std::vector<double>& allGrades);
    static bool checkSignature(const std::string& filename);
    static size_t getHeaderSize();
};

#endif
