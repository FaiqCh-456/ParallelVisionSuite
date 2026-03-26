#pragma once
#include <string>

class AssetGenerator {
public:
    static void generateIfMissing();
    static void generateTestImage(int size, const std::string& path);
    static void generateCheckerboard(int size, const std::string& path);
    static void generateGradient(int size, const std::string& path);
};