#ifndef SHADEWATCHER_UTIL_FILE_H_
#define SHADEWATCHER_UTIL_FILE_H_

#include <vector>
#include <string>
#include <dirent.h>
#include <iostream>
#include <algorithm>
// new
#include <unordered_map>
#include <fstream>
// #include <filesystem>
#include <boost/filesystem.hpp>
// ------

std::vector<std::string> TraverseDir(std::string);
std::vector<std::string> TraverseFile(std::string);
std::vector<std::string> TraverseJsonFile(std::string);
std::vector<std::string> CollectJsonFile(std::string);
std::vector<std::string> CollectBeatFile(std::string);
std::vector<std::string> TraverseBeatDir(std::string);

// std::unordered_map<std::string, std::string> malicious_truth_files;

// new
// bool fileExists(const std::string &);
// std::unordered_map<std::string, std::string> CollectMaliciousTruthFiles(const std::vector<std::string> &, const std::string &);
// void CollectMaliciousTruthFiles(const std::vector<std::string> &, const std::string &);

#endif
