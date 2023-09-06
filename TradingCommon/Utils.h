#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <string>

namespace Utils
{
	std::map<std::string, std::string> parseEnvFile(const std::string& path);
}

#endif // !UTILS_H
