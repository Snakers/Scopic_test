#include "Utils.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace Utils
{
    std::map<std::string, std::string> parseEnvFile(const std::string& path) {
        std::map<std::string, std::string> res;

        std::ifstream envFile(path);
        if (!envFile) {
            std::cout << "[parseEnvFile] can not open the file: " << path << std::endl;
            return res;
        }

        std::string line;
        while (std::getline(envFile, line)) {
            if (line[0] == '#' || line.empty()) {
                continue;
            }

            std::istringstream lineStream(line);
            std::string key;
            std::string value;

            if (std::getline(lineStream, key, '=') && std::getline(lineStream, value)) {
                res[key] = value;
            }
        }

        return res;
    }
}