#pragma once
#include <string>
#include <vector>
#include <fstream>

class CSVWriter {
public:
    explicit CSVWriter(const std::string& path);
    ~CSVWriter();

    void writeHeader(const std::vector<std::string>& cols);
    void writeRow(const std::vector<std::string>& vals);

    static std::string quote(const std::string& s);

private:
    std::ofstream m_file;
};