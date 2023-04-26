#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include "../includes/ConfigFile.hpp"

bool isDigit(const std::string& a_attributes);
bool splitString(std::string aline, std::string character, std::vector<std::string>& asplit_line);
bool CheckExtension(const std::string& filename, std::string a_extension);
bool onlySpaces(const std::string&  aLine);
std::string getStatusMessage(int statusCode);
bool errorMessage(const std::string message);
void getLastLineofString(const std::string &string,  std::string& last_line);
std::string intToString(int num);

#endif