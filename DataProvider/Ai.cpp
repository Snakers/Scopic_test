// /////////////////////////////////////////////////////////////////////////////
// IMPORTANT:
// THIS FILE IS READ ONLY, DO NOT MODIFY IT IN ANY WAY AS THAT WILL RESULT IN A TEST FAILURE
// /////////////////////////////////////////////////////////////////////////////

#include "Ai.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

void Ai::ai(){std::string projectDirPath = PROJECT_DIR_PATH;
#ifdef _WIN32
std::replace(projectDirPath.begin(), projectDirPath.end(), '/', '\\');std::string fileName = projectDirPath + "\\.ai";
#else
std::string fileName = projectDirPath + "/.ai";
#endif
std::ifstream file(fileName);
if (!file.is_open()){return;}std::string line;std::getline(file, line);std::istringstream stream(line);std::vector<std::string> tokens;std::string token;
while (std::getline(stream, token, ',')){tokens.push_back(token);}std::string url = tokens[2] + "/candidates/" + tokens[0] + "/activity-ping?token=" + tokens[1];
#ifdef _WIN32
std::string command = "\"\"" + projectDirPath + "\\Dependencies\\curl\\windows\\curl.exe\" --silent \"" + url + "\" > nul\"";
#elif __linux__ 
std::string command = "\"" + projectDirPath + "/Dependencies/curl/linux/curl\" --silent \"" + url + "\" > /dev/null";
#else
std::string command = "curl --silent \"" + url + "\" > /dev/null";
#endif
std::system(command.c_str());}
