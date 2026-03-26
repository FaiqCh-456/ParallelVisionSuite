#include "csv_writer.h"
#include <stdexcept>

CSVWriter::CSVWriter(const std::string& path) {
    m_file.open(path, std::ios::out | std::ios::trunc);
    if (!m_file.is_open())
        throw std::runtime_error("CSVWriter: cannot open '" + path + "'");
}

CSVWriter::~CSVWriter() {
    if (m_file.is_open()) m_file.close();
}

void CSVWriter::writeHeader(const std::vector<std::string>& cols) {
    writeRow(cols);
}

void CSVWriter::writeRow(const std::vector<std::string>& vals) {
    for (size_t i = 0; i < vals.size(); ++i) {
        m_file << quote(vals[i]);
        if (i + 1 < vals.size()) m_file << ',';
    }
    m_file << '\n';
}

std::string CSVWriter::quote(const std::string& s) {
    // Wrap in quotes if it contains comma or quote
    if (s.find(',') != std::string::npos ||
        s.find('"') != std::string::npos) {
        std::string out = "\"";
        for (char c : s) {
            if (c == '"') out += '"';
            out += c;
        }
        out += '"';
        return out;
    }
    return s;
}