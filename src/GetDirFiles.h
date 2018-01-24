#pragma once

#include <direct.h>
#include <io.h>
#include <string>
#include <vector>

bool deleteDir(const std::string& path);
size_t getDirs(const std::string& path, std::vector<std::string>& dirs, const bool& bIncludeSub = true);
size_t getFiles(const std::string& path, std::vector<std::string>& files, const char *filespec = "*.*", const bool& bIncludeSub = true);
size_t getFiles(const std::string& path, std::vector<std::string>& files, const std::vector<std::string>& vFileSpecs, const bool& bIncludeSub = true);
